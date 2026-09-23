#include "FoxStudioEngine.h"

FoxStudioEngine::FoxStudioEngine()
{
    initialiseAudio();
}

FoxStudioEngine::~FoxStudioEngine()
{
    shutdownAudio();
}

bool FoxStudioEngine::initialiseAudio()
{
    const auto result = deviceManager.initialiseWithDefaultDevices(2, 2);
    if (result == juce::AudioDeviceManager::InitialisationResult::success)
        return true;

    return false;
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
