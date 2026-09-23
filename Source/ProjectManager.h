#pragma once

#include <juce_core/juce_core.h>

struct ProjectTrack
{
    juce::String name = "Track";
    juce::String type = "audio";
    double gainDb = 0.0;
    double pan = 0.0;
    std::vector<MidiNoteData> notes;
};

struct ProjectSession
{
    juce::String name = "untitled-session";
    int tempo = 120;
    bool loop = false;
    bool metronome = false;
    bool autoSave = true;
    double sampleRate = 44100.0;
    std::vector<juce::String> tracks;
    std::vector<ProjectTrack> trackData;
};

class ProjectManager
{
public:
    ProjectManager() = default;
    ~ProjectManager() = default;

    ProjectSession createDefaultSession() const;
    bool saveToFile(const juce::File& file, const ProjectSession& session);
    ProjectSession loadFromFile(const juce::File& file);

private:
    static juce::ValueTree createTrackTree(const juce::String& trackName);
    static juce::ValueTree createNoteTree(const MidiNoteData& note);
    static MidiNoteData readNoteTree(const juce::ValueTree& tree);
};
