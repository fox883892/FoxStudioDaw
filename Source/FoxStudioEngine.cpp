#include "FoxStudioEngine.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr double twoPi = 6.28318530717958647692;
    double midiToHz(int pitch) noexcept
    {
        return 440.0 * std::pow(2.0, (static_cast<double>(pitch) - 69.0) / 12.0);
    }
}

FoxStudioEngine::FoxStudioEngine()
{
    addTrack(TrackState::Type::midi, "Lead");
    addNote(0, { 60, 0.0, 1.0, 0.80f });
    addNote(0, { 64, 1.0, 1.0, 0.75f });
    addNote(0, { 67, 2.0, 1.0, 0.75f });
    addNote(0, { 72, 3.0, 2.0, 0.70f });
}

FoxStudioEngine::~FoxStudioEngine() { shutdownAudio(); }

bool FoxStudioEngine::initialiseAudio()
{
    return deviceManager.initialise(0, 2, nullptr, true)
        == juce::AudioDeviceManager::InitialisationResult::success;
}

void FoxStudioEngine::shutdownAudio()
{
    deviceManager.closeAudioDevice();
    playing = false;
}

void FoxStudioEngine::prepareToPlay(double newSampleRate, int) noexcept
{
    sampleRate = std::max(1.0, newSampleRate);
}

void FoxStudioEngine::releaseResources() noexcept { playing = false; }

void FoxStudioEngine::stopPlayback() noexcept
{
    playing = false;
    positionSeconds = 0.0;
}

void FoxStudioEngine::setPosition(double seconds) noexcept
{
    positionSeconds = juce::jlimit(0.0, 3600.0, seconds);
}

void FoxStudioEngine::setTempo(double bpm) noexcept
{
    tempoBpm = juce::jlimit(40.0, 240.0, bpm);
}

void FoxStudioEngine::addTrack(TrackState::Type type, const juce::String& name)
{
    TrackState track;
    track.type = type;
    track.name = name;
    tracks.push_back(std::move(track));
}

void FoxStudioEngine::addNote(size_t trackIndex, MidiNote note)
{
    if (trackIndex < tracks.size())
        tracks[trackIndex].notes.push_back(note);
}

void FoxStudioEngine::updateTrack(size_t index, const TrackState& track)
{
    if (index < tracks.size())
        tracks[index] = track;
}

void FoxStudioEngine::renderBlock(juce::AudioBuffer<float>& buffer, int startSample,
                                  int numSamples, double startTime) const noexcept
{
    const double secondsPerBeat = 60.0 / tempoBpm;
    for (int i = 0; i < numSamples; ++i)
    {
        const double time = startTime + static_cast<double>(i) / sampleRate;
        double left = 0.0, right = 0.0;
        for (const auto& track : tracks)
        {
            if (track.muted)
                continue;
            const auto gain = juce::Decibels::decibelsToGain(track.gainDb);
            for (const auto& note : track.notes)
            {
                const double begin = note.startBeat * secondsPerBeat;
                const double end = begin + note.lengthBeats * secondsPerBeat;
                if (time < begin || time >= end)
                    continue;
                const double frequency = midiToHz(note.pitch);
                const double progress = (time - begin) / (end - begin);
                const double envelope = std::sin(juce::MathConstants<double>::pi * progress);
                const double sample = std::sin(twoPi * frequency * (time - begin))
                                     * envelope * note.velocity * gain * 0.12;
                left += sample * std::max(0.0f, 1.0f - track.pan);
                right += sample * std::max(0.0f, 1.0f + track.pan);
            }
        }
        buffer.addSample(0, startSample + i, static_cast<float>(left));
        if (buffer.getNumChannels() > 1)
            buffer.addSample(1, startSample + i, static_cast<float>(right));
    }
}

void FoxStudioEngine::render(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) noexcept
{
    if (!playing || numSamples <= 0 || buffer.getNumChannels() == 0)
        return;
    renderBlock(buffer, startSample, numSamples, positionSeconds);
    positionSeconds += static_cast<double>(numSamples) / sampleRate;
    if (loopEnabled && positionSeconds >= 8.0)
        positionSeconds = std::fmod(positionSeconds, 8.0);
}
