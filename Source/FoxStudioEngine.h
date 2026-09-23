#pragma once

#include <vector>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

class TrackInfo
{
public:
    enum class Type { Audio, Midi };
    TrackInfo() = default;
    TrackInfo(Type t, const juce::String& n) : type(t), name(n) {}

    Type type = Type::Audio;
    juce::String name = "Track";
    double gainDb = 0.0;
    double pan = 0.0;
    bool muted = false;
    bool solo = false;
    bool armed = false;
};

struct MidiNoteData
{
    int midiNote = 60;
    double startBeat = 0.0;
    double lengthBeats = 1.0;
};

struct TrackDefinition
{
    TrackInfo info;
    std::vector<MidiNoteData> notes;
};

class FoxStudioEngine
{
public:
    FoxStudioEngine();
    ~FoxStudioEngine();

    bool initialiseAudio();
    void shutdownAudio();
    void prepareToPlay(double newSampleRate) noexcept;

    void startPlayback() noexcept { playing = true; }
    void stopPlayback() noexcept { playing = false; positionSeconds = 0.0; }
    bool isPlaying() const noexcept { return playing; }

    void setPosition(double seconds) noexcept;
    double getPosition() const noexcept { return positionSeconds; }
    void setTempo(double bpm) noexcept;
    double getTempo() const noexcept { return tempoBpm; }
    void setLoopEnabled(bool enabled) noexcept { loopEnabled = enabled; }
    bool isLoopEnabled() const noexcept { return loopEnabled; }

    void addTrack(TrackInfo::Type type, const juce::String& name = "Track");
    void addMidiNoteToTrack(const juce::String& trackName, int midiNote, double startBeat, double lengthBeats);
    void processAudio(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) noexcept;
    bool exportProjectAsWav(const juce::File& targetFile, double durationSeconds = 8.0) const;

    std::vector<TrackInfo>& getTracks() noexcept { return tracks; }
    const std::vector<TrackInfo>& getTracks() const noexcept { return tracks; }
    std::vector<TrackDefinition>& getTrackDefinitions() noexcept { return trackDefinitions; }
    const std::vector<TrackDefinition>& getTrackDefinitions() const noexcept { return trackDefinitions; }
    void updateTrack(size_t index, const TrackInfo& track);

private:
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, double timeSeconds) const noexcept;

    juce::AudioDeviceManager deviceManager;
    std::vector<TrackInfo> tracks;
    std::vector<TrackDefinition> trackDefinitions;
    bool playing = false;
    bool loopEnabled = false;
    double positionSeconds = 0.0;
    double tempoBpm = 120.0;
    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FoxStudioEngine)
};
