#pragma once

#include <juce_core/juce_core.h>

class ExportManager
{
public:
    ExportManager() = default;
    ~ExportManager() = default;

    juce::String exportToWav(const juce::File& destination, const juce::String& projectName)
    {
        const juce::String finalPath = destination.getFullPathName() + "/" + projectName + ".wav";
        return finalPath;
    }
};
