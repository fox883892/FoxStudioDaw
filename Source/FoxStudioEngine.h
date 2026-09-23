#pragma once

#include <vector>
#include <juce_audio_basics/juce_audio_basics.h>

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
    std::vector<MidiNote> notes;
};

class FoxStudioEngine
{
public:
    FoxStudioEngine();
    ~FoxStudioEngine() = default;

    void prepare(double newSampleRate, int maximumBlockSize) noexcept;
    void release() noexcept;
    void render(juce::AudioBuffer<float>& output, int startSample, int numSamples) noexcept;

    void play() noexcept { playing = true; }
    void pause() noexcept { playing = false; }
    void stop() noexcept;
    bool isPlaying() const noexcept { return playing; }

    void setPositionSeconds(double seconds) noexcept;
    double getPositionSeconds() const noexcept { return positionSeconds; }
    void setTempo(double bpm) noexcept;
    double getTempo() const noexcept { return tempoBpm; }
    void setLoop(bool enabled) noexcept { loopEnabled = enabled; }
    bool isLooping() const noexcept { return loopEnabled; }

    void addTrack(TrackState::Type type, const juce::String& name);
    void addNote(size_t trackIndex, MidiNote note);
    const std::vector<TrackState>& getTracks() const noexcept { return tracks; }
    std::vector<TrackState>& getTracks() noexcept { return tracks; }

private:
    double sampleRate = 44100.0;
    int blockSize = 512;
    double positionSeconds = 0.0;
    double tempoBpm = 120.0;
    bool playing = false;
    bool loopEnabled = false;
    std::vector<TrackState> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FoxStudioEngine)
};
