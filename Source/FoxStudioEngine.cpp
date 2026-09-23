#include "FoxStudioEngine.h"

FoxStudioEngine::FoxStudioEngine()
{
    initialiseAudio();
    addTrack(TrackInfo::Type::Audio, "Drums");
    addTrack(TrackInfo::Type::Audio, "Bass");
    addTrack(TrackInfo::Type::Midi, "Lead");

    addMidiNoteToTrack("Lead", 60, 0.0, 1.0);
    addMidiNoteToTrack("Lead", 64, 1.0, 1.0);
    addMidiNoteToTrack("Lead", 67, 2.0, 1.0);
    addMidiNoteToTrack("Lead", 72, 3.0, 2.0);
}

FoxStudioEngine::~FoxStudioEngine()
{
    shutdownAudio();
}

bool FoxStudioEngine::initialiseAudio()
{
    const auto result = deviceManager.initialise(2, 2, nullptr, true);
    return result == juce::AudioDeviceManager::InitialisationResult::success;
}

void FoxStudioEngine::shutdownAudio()
{
    deviceManager.closeAudioDevice();
    playing = false;
}

void FoxStudioEngine::startPlayback()
{
    playing = true;
}

void FoxStudioEngine::stopPlayback()
{
    playing = false;
    positionSeconds = 0.0;
}

void FoxStudioEngine::setPosition(double seconds)
{
    positionSeconds = juce::jlimit(0.0, 3600.0, seconds);
}

void FoxStudioEngine::setTempo(double bpm)
{
    tempoBpm = juce::jlimit(40.0, 240.0, bpm);
}

void FoxStudioEngine::addTrack(TrackInfo::Type type, const juce::String& name)
{
    tracks.emplace_back(type, name);
    TrackDefinition definition;
    definition.info = tracks.back();
    trackDefinitions.push_back(definition);
}

void FoxStudioEngine::addMidiNoteToTrack(const juce::String& trackName, int midiNote, double startBeat, double lengthBeats)
{
    for (auto& definition : trackDefinitions)
    {
        if (definition.info.name == trackName)
        {
            MidiNoteData note;
            note.midiNote = midiNote;
            note.startBeat = startBeat;
            note.lengthBeats = lengthBeats;
            definition.notes.push_back(note);
            return;
        }
    }
}

void FoxStudioEngine::processAudio(juce::AudioBuffer<float>& buffer, int numSamples)
{
    buffer.clear();
    buffer.setSize(2, numSamples);

    const double secondsPerBeat = 60.0 / tempoBpm;
    const double globalTime = positionSeconds;

    for (auto& definition : trackDefinitions)
    {
        if (definition.info.muted)
            continue;

        const auto gain = juce::Decibels::decibelsToGain(static_cast<float>(definition.info.gainDb));
        const auto pan = juce::jlimit(-1.0, 1.0, definition.info.pan);

        for (int i = 0; i < numSamples; ++i)
        {
            const double sampleTime = globalTime + (double)i / sampleRate;
            double sampleValue = 0.0;

            for (const auto& note : definition.notes)
            {
                const auto noteStart = note.startBeat * secondsPerBeat;
                const auto noteEnd = noteStart + note.lengthBeats * secondsPerBeat;
                if (sampleTime < noteStart || sampleTime > noteEnd)
                    continue;

                const double notePhase = (sampleTime - noteStart) / std::max(0.0001, noteEnd - noteStart);
                const double frequency = 440.0 * std::pow(2.0, (note.midiNote - 69.0) / 12.0);
                const double envelope = std::sin(juce::MathConstants<double>::pi * notePhase);
                sampleValue += (std::sin(2.0 * juce::MathConstants<double>::pi * frequency * sampleTime) * envelope);
            }

            const float leftGain = static_cast<float>(std::max(0.0, 1.0 - pan) * gain);
            const float rightGain = static_cast<float>(std::max(0.0, 1.0 + pan) * gain);
            buffer.addSample(0, i, sampleValue * leftGain * 0.12f);
            buffer.addSample(1, i, sampleValue * rightGain * 0.12f);
        }
    }
}

bool FoxStudioEngine::exportProjectAsWav(const juce::File& targetFile) const
{
    juce::WavAudioFormat wavFormat;
    auto writer = wavFormat.createWriterFor(new juce::File(targetFile), sampleRate, 2, 16, {}, 0);
    if (writer == nullptr)
        return false;

    juce::AudioBuffer<float> buffer;
    const int totalSamples = static_cast<int>(std::ceil(30.0 * sampleRate));
    buffer.setSize(2, totalSamples);
    buffer.clear();

    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.setSize(2, totalSamples);
    tempBuffer.clear();

    auto mutableThis = const_cast<FoxStudioEngine*>(this);
    mutableThis->positionSeconds = 0.0;

    for (int i = 0; i < totalSamples; ++i)
    {
        const double t = static_cast<double>(i) / sampleRate;
        double sampleValueL = 0.0;
        double sampleValueR = 0.0;

        for (const auto& definition : trackDefinitions)
        {
            if (definition.info.muted)
                continue;

            const auto gain = juce::Decibels::decibelsToGain(static_cast<float>(definition.info.gainDb));
            const auto pan = juce::jlimit(-1.0, 1.0, definition.info.pan);
            for (const auto& note : definition.notes)
            {
                const double noteStart = note.startBeat * (60.0 / tempoBpm);
                const double noteEnd = noteStart + note.lengthBeats * (60.0 / tempoBpm);
                if (t < noteStart || t > noteEnd)
                    continue;

                const double notePhase = (t - noteStart) / std::max(0.0001, noteEnd - noteStart);
                const double frequency = 440.0 * std::pow(2.0, (note.midiNote - 69.0) / 12.0);
                const double envelope = std::sin(juce::MathConstants<double>::pi * notePhase);
                const double sine = std::sin(2.0 * juce::MathConstants<double>::pi * frequency * t) * envelope;
                const double channelScaleL = std::max(0.0, 1.0 - pan) * gain * 0.12;
                const double channelScaleR = std::max(0.0, 1.0 + pan) * gain * 0.12;
                sampleValueL += sine * channelScaleL;
                sampleValueR += sine * channelScaleR;
            }
        }

        tempBuffer.setSample(0, i, static_cast<float>(sampleValueL));
        tempBuffer.setSample(1, i, static_cast<float>(sampleValueR));
    }

    writer->writeFromAudioSampleData(tempBuffer.getArrayOfWritePointers(), 2, totalSamples, 0);
    writer.reset();
    return targetFile.existsAsFile();
}

void FoxStudioEngine::updateTrack(size_t index, const TrackInfo& track)
{
    if (index < tracks.size())
        tracks[index] = track;

    if (index < trackDefinitions.size())
        trackDefinitions[index].info = track;
}
