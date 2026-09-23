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

void FoxStudioEngine::prepare(double newSampleRate, int maximumBlockSize) noexcept
{
    sampleRate = std::max(1.0, newSampleRate);
    blockSize = std::max(1, maximumBlockSize);
}

void FoxStudioEngine::release() noexcept
{
    playing = false;
}

void FoxStudioEngine::stop() noexcept
{
    playing = false;
    positionSeconds = 0.0;
}

void FoxStudioEngine::setPositionSeconds(double seconds) noexcept
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

void FoxStudioEngine::render(juce::AudioBuffer<float>& output, int startSample, int numSamples) noexcept
{
    if (output.getNumChannels() == 0 || numSamples <= 0)
        return;

    if (!playing)
        return;

    const auto secondsPerBeat = 60.0 / tempoBpm;
    const auto beginTime = positionSeconds;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto time = beginTime + static_cast<double>(sample) / sampleRate;
        double left = 0.0;
        double right = 0.0;

        for (const auto& track : tracks)
        {
            if (track.muted)
                continue;

            const auto gain = juce::Decibels::decibelsToGain(track.gainDb);
            for (const auto& note : track.notes)
            {
                const auto noteStart = note.startBeat * secondsPerBeat;
                const auto noteEnd = noteStart + note.lengthBeats * secondsPerBeat;
                if (time < noteStart || time >= noteEnd)
                    continue;

                const auto phase = (time - noteStart) * midiToHz(note.pitch);
                const auto noteProgress = (time - noteStart) / (noteEnd - noteStart);
                const auto envelope = std::sin(juce::MathConstants<double>::pi * noteProgress);
                const auto value = std::sin(twoPi * phase) * envelope * note.velocity * gain * 0.12;
                left += value * std::max(0.0f, 1.0f - track.pan);
                right += value * std::max(0.0f, 1.0f + track.pan);
            }
        }

        output.addSample(0, startSample + sample, static_cast<float>(left));
        if (output.getNumChannels() > 1)
            output.addSample(1, startSample + sample, static_cast<float>(right));
    }

    positionSeconds += static_cast<double>(numSamples) / sampleRate;
    if (loopEnabled && positionSeconds >= 8.0)
        positionSeconds = std::fmod(positionSeconds, 8.0);
}
