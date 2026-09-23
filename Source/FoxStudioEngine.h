#pragma once

#include <vector>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

struct MidiNote
{
    int pitch = 60;
    double startBeat = 0.0;
    double lengthBeats = 1.0;
    float velocity = 0.8f;
};

struct TrackState
{
    enum class Type { audio, midi };
    Type type = Type::midi;
    juce::String name { "Track" };
    float gainDb = 0.0f;
    float pan = 0.0f;
    bool muted = false;
    bool solo = false;
    bool armed = false;
    std::vector<MidiNote> notes;
};

class FoxStudioEngine
{
public:
    FoxStudioEngine();
    ~FoxStudioEngine();

    bool initialiseAudio();
    void shutdownAudio();
    void prepareToPlay(double newSampleRate, int maximumBlockSize) noexcept;
    void releaseResources() noexcept;
    void render(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) noexcept;

    void startPlayback() noexcept { playing = true; }
    void stopPlayback() noexcept;
    bool isPlaying() const noexcept { return playing; }
    void setPosition(double seconds) noexcept;
    double getPosition() const noexcept { return positionSeconds; }
    void setTempo(double bpm) noexcept;
    double getTempo() const noexcept { return tempoBpm; }
    void setLoopEnabled(bool enabled) noexcept { loopEnabled = enabled; }
    bool isLoopEnabled() const noexcept { return loopEnabled; }

    void addTrack(TrackState::Type type, const juce::String& name = "Track");
    void addNote(size_t trackIndex, MidiNote note);
    std::vector<TrackState>& getTracks() noexcept { return tracks; }
    const std::vector<TrackState>& getTracks() const noexcept { return tracks; }
    void updateTrack(size_t index, const TrackState& track);

private:
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample,
                     int numSamples, double startTime) const noexcept;

    juce::AudioDeviceManager deviceManager;
    std::vector<TrackState> tracks;
    double sampleRate = 44100.0;
    double positionSeconds = 0.0;
    double tempoBpm = 120.0;
    bool playing = false;
    bool loopEnabled = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FoxStudioEngine)
};
