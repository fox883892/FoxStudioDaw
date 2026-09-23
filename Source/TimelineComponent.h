#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"

class TimelineComponent final : public juce::Component, private juce::Timer
{
public:
    explicit TimelineComponent(FoxStudioEngine& engine);
    ~TimelineComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    FoxStudioEngine& engine;
    juce::Label timeRuler;
    juce::Viewport viewport;
    juce::Component timelineArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimelineComponent)
};
