#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

class ExportManager
{
public:
    ExportManager() = default;
    ~ExportManager() = default;

    juce::String exportToWav(const juce::File& destination, const juce::String& projectName)
    {
        const juce::String finalPath = destination.getFullPathName() + "/" + projectName + ".wav";
        const juce::File outFile(finalPath);
        return outFile.exists() ? finalPath : finalPath;
    }

    bool writeWavFromBuffer(const juce::File& file, const juce::AudioBuffer<float>& buffer, double sampleRate)
    {
        juce::WavAudioFormat wavFormat;
        auto writer = wavFormat.createWriterFor(file, sampleRate, static_cast<int>(buffer.getNumChannels()), 16, {}, 0);
        if (writer == nullptr)
            return false;

        writer->writeFromAudioSampleData(buffer.getArrayOfWritePointers(), static_cast<int>(buffer.getNumChannels()), static_cast<int>(buffer.getNumSamples()), 0);
        return true;
    }
};
