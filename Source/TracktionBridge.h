#pragma once

#include <juce_core/juce_core.h>

class TracktionBridge
{
public:
    TracktionBridge() = default;
    ~TracktionBridge() = default;

    void initialise(const juce::String& projectName)
    {
        currentProjectName = projectName;
        connected = true;
        status = "Tracktion bridge ready";
    }

    void syncSession(const juce::String& projectName, int tempo, bool loopEnabled)
    {
        currentProjectName = projectName;
        currentTempo = tempo;
        loop = loopEnabled;
        status = "Tracktion synced: " + projectName;
    }

    bool isConnected() const noexcept { return connected; }
    juce::String getStatus() const noexcept { return status; }

private:
    bool connected = false;
    bool loop = false;
    int currentTempo = 120;
    juce::String currentProjectName = "untitled";
    juce::String status = "Tracktion bridge inactive";
};
