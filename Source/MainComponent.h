#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"

class MainComponent final : public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void toggleTransport();
    void resetSession();
    juce::String formatTime(double) const;

    FoxStudioEngine engine;
    juce::TextButton newButton{"New"}, playButton{"Play"}, stopButton{"Stop"}, exportButton{"Export WAV"};
    juce::TextButton addAudioButton{"+ Audio"}, addMidiButton{"+ MIDI"};
    juce::ToggleButton loopButton{"Loop"};
    juce::Label titleLabel, transportLabel, statusLabel, trackLabel;
    juce::Slider tempoSlider;
    double sampleRate = 44100.0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
