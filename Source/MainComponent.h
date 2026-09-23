#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

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
    void toggleTransport();
    void resetSession();
    juce::String formatTime(double) const;

    juce::TextButton newButton{"New"}, openButton{"Open"}, saveButton{"Save"};
    juce::TextButton playButton{"Play"}, stopButton{"Stop"}, recordButton{"Record"};
    juce::TextButton addAudioButton{"+ Audio"}, addMidiButton{"+ MIDI"};
    juce::ToggleButton loopButton{"Loop"}, metronomeButton{"Metronome"};
    juce::Label titleLabel, transportLabel, statusLabel, trackLabel;
    juce::Label timelineLabel, mixerLabel, inspectorLabel;
    juce::Slider tempoSlider, masterSlider;
    juce::AudioBuffer<float> scratch;
    double sampleRate = 44100.0;
    double positionSeconds = 0.0;
    bool playing = false;
    int trackCount = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
