#include "FoxStudioEngine.h"
#include <algorithm>
#include <cmath>

FoxStudioEngine::FoxStudioEngine()
{
    addTrack(TrackInfo::Type::Audio, "Drums");
    addTrack(TrackInfo::Type::Audio, "Bass");
    addTrack(TrackInfo::Type::Midi, "Lead");
    addMidiNoteToTrack("Lead", 60, 0.0, 1.0);
    addMidiNoteToTrack("Lead", 64, 1.0, 1.0);
    addMidiNoteToTrack("Lead", 67, 2.0, 1.0);
    addMidiNoteToTrack("Lead", 72, 3.0, 2.0);
}

FoxStudioEngine::~FoxStudioEngine() { shutdownAudio(); }

bool FoxStudioEngine::initialiseAudio() { return true; }
void FoxStudioEngine::shutdownAudio() { playing = false; }
void FoxStudioEngine::prepareToPlay(double newSampleRate) noexcept { sampleRate = juce::jmax(1.0, newSampleRate); }

void FoxStudioEngine::setPosition(double seconds) noexcept
{
    positionSeconds = juce::jlimit(0.0, 3600.0, seconds);
}

void FoxStudioEngine::setTempo(double bpm) noexcept
{
    tempoBpm = juce::jlimit(40.0, 240.0, bpm);
}

void FoxStudioEngine::addTrack(TrackInfo::Type type, const juce::String& name)
{
    tracks.emplace_back(type, name);
    trackDefinitions.push_back({ tracks.back(), {} });
}

void FoxStudioEngine::addMidiNoteToTrack(const juce::String& trackName, int midiNote, double startBeat, double lengthBeats)
{
    for (auto& track : trackDefinitions)
        if (track.info.name == trackName)
            track.notes.push_back({ juce::jlimit(0, 127, midiNote), juce::jmax(0.0, startBeat), juce::jmax(0.01, lengthBeats) });
}

void FoxStudioEngine::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, double timeSeconds) const noexcept
{
    const double secondsPerBeat = 60.0 / tempoBpm;
    for (int i = 0; i < numSamples; ++i)
    {
        const double time = timeSeconds + static_cast<double>(i) / sampleRate;
        double left = 0.0, right = 0.0;
        for (const auto& track : trackDefinitions)
        {
            if (track.info.muted) continue;
            const auto gain = juce::Decibels::decibelsToGain(static_cast<float>(track.info.gainDb));
            const auto pan = juce::jlimit(-1.0, 1.0, track.info.pan);
            for (const auto& note : track.notes)
            {
                const double begin = note.startBeat * secondsPerBeat;
                const double end = begin + note.lengthBeats * secondsPerBeat;
                if (time < begin || time >= end) continue;
                const double frequency = 440.0 * std::pow(2.0, (note.midiNote - 69.0) / 12.0);
                const double phase = (time - begin) * frequency;
                const double attackRelease = std::sin(juce::MathConstants<double>::pi * (time - begin) / (end - begin));
                const double value = std::sin(juce::MathConstants<double>::twoPi * phase) * attackRelease * gain * 0.10;
                left += value * std::max(0.0, 1.0 - pan);
                right += value * std::max(0.0, 1.0 + pan);
            }
        }
        buffer.addSample(0, startSample + i, static_cast<float>(left));
        if (buffer.getNumChannels() > 1) buffer.addSample(1, startSample + i, static_cast<float>(right));
    }
}

void FoxStudioEngine::processAudio(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) noexcept
{
    if (!playing || buffer.getNumChannels() == 0) return;
    renderBlock(buffer, startSample, numSamples, positionSeconds);
    positionSeconds += static_cast<double>(numSamples) / sampleRate;
    if (loopEnabled && positionSeconds >= 8.0) positionSeconds = 0.0;
}

bool FoxStudioEngine::exportProjectAsWav(const juce::File& targetFile, double durationSeconds) const
{
    juce::WavAudioFormat format;
    auto output = targetFile.getParentDirectory();
    output.createDirectory();
    auto writer = format.createWriterFor(targetFile, sampleRate, 2, 24, {}, 0);
    if (writer == nullptr) return false;

    const int blockSize = 4096;
    const int totalSamples = static_cast<int>(std::ceil(durationSeconds * sampleRate));
    juce::AudioBuffer<float> block(2, blockSize);
    double time = 0.0;
    for (int offset = 0; offset < totalSamples; offset += blockSize)
    {
        const int count = std::min(blockSize, totalSamples - offset);
        block.clear();
        renderBlock(block, 0, count, time);
        writer->writeFromAudioSampleData(block.getArrayOfReadPointers(), 2, count, 0);
        time += static_cast<double>(count) / sampleRate;
    }
    writer.reset();
    return targetFile.existsAsFile();
}

void FoxStudioEngine::updateTrack(size_t index, const TrackInfo& track)
{
    if (index < tracks.size()) tracks[index] = track;
    if (index < trackDefinitions.size()) trackDefinitions[index].info = track;
}
