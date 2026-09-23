#pragma once

#include <vector>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

class TrackInfo
{
public:
    enum class Type
    {
        Audio,
        Midi
    };

    TrackInfo() = default;
    TrackInfo(Type t, const juce::String& n)
        : type(t), name(n)
    {
    }

    Type type = Type::Audio;
    juce::String name = "Track";
    double gainDb = 0.0;
    double pan = 0.0;
    bool muted = false;
    bool solo = false;
    bool armed = false;
};

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

    void addTrack(TrackInfo::Type type, const juce::String& name = "Track");
    std::vector<TrackInfo>& getTracks() noexcept { return tracks; }
    const std::vector<TrackInfo>& getTracks() const noexcept { return tracks; }
    void updateTrack(size_t index, const TrackInfo& track);

private:
    juce::AudioDeviceManager deviceManager;
    std::vector<TrackInfo> tracks;
    bool playing = false;
    bool loopEnabled = false;
    double positionSeconds = 0.0;
    double tempoBpm = 120.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FoxStudioEngine)
};
