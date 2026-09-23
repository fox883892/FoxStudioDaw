#pragma once

#include <juce_core/juce_core.h>
#include <vector>

struct ProjectSession
{
    juce::String name = "untitled-session";
    int tempo = 120;
    bool loop = false;
    bool metronome = false;
    bool autoSave = true;
    double sampleRate = 44100.0;
    std::vector<juce::String> tracks;
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
};
