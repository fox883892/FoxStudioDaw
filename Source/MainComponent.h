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
    void getNextAudioBlock(const juce::AudioSourceChannelInfo&) override;
    void releaseResources() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateStatus();
    juce::String formatTime(double seconds) const;

    FoxStudioEngine engine;
    juce::Label titleLabel, timeLabel, statusLabel, tracksLabel;
    juce::TextButton playButton { "Play" }, stopButton { "Stop" }, resetButton { "New" };
    juce::TextButton audioButton { "+ Audio" }, midiButton { "+ MIDI" };
    juce::ToggleButton loopButton { "Loop" };
    juce::Slider tempoSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
