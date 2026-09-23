#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

class FoxStudioEngine
{
public:
    FoxStudioEngine();
    ~FoxStudioEngine();

    juce::AudioDeviceManager& getAudioDeviceManager() noexcept { return deviceManager; }
    bool initialiseAudio();
    void shutdownAudio();
    void startPlayback();
    void stopPlayback();
    bool isPlaying() const noexcept { return playing; }
    void setPosition(double seconds);
    double getPosition() const noexcept { return positionSeconds; }
    void setTempo(double bpm);
    double getTempo() const noexcept { return tempoBpm; }
    void setLoopEnabled(bool enabled) noexcept { loopEnabled = enabled; }
    bool isLoopEnabled() const noexcept { return loopEnabled; }

private:
    juce::AudioDeviceManager deviceManager;
    bool playing = false;
    bool loopEnabled = false;
    double positionSeconds = 0.0;
    double tempoBpm = 120.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FoxStudioEngine)
};
