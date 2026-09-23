#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"

class TransportBar final : public juce::Component, private juce::Timer
{
public:
    explicit TransportBar(FoxStudioEngine& engine);
    ~TransportBar() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onPlay;
    std::function<void()> onStop;
    std::function<void()> onReset;

private:
    void timerCallback() override;
    juce::String formatTime(double seconds) const;

    FoxStudioEngine& engine;

    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton resetButton { "Reset" };
    juce::Label clockLabel;
    juce::Slider tempoSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportBar)
};
