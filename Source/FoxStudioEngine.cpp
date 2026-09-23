#include "FoxStudioEngine.h"

FoxStudioEngine::FoxStudioEngine()
{
    initialiseAudio();
    addTrack(TrackInfo::Type::Audio, "Drums");
    addTrack(TrackInfo::Type::Audio, "Bass");
    addTrack(TrackInfo::Type::Midi, "Lead");
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
}

void FoxStudioEngine::updateTrack(size_t index, const TrackInfo& track)
{
    if (index < tracks.size())
        tracks[index] = track;
}
