#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"

class MixerComponent final : public juce::Component
{
public:
    explicit MixerComponent(FoxStudioEngine& engine);
    ~MixerComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void refresh();

private:
    FoxStudioEngine& engine;
    juce::Viewport viewport;
    juce::Component stripsArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
