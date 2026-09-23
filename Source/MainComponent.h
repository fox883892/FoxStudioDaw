#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"
#include "TransportBar.h"
#include "TrackList.h"
#include "TimelineComponent.h"
#include "MixerComponent.h"
#
class AIAdvisor;

class MainComponent final : public juce::Component, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void togglePlay();
    void stopPlayback();
    void resetSession();

    FoxStudioEngine engine;
    TransportBar transportBar{ engine };
    TrackList trackList{ engine };
    TimelineComponent timeline{ engine };
    MixerComponent mixerComponent{ engine };
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label aiSummaryLabel;
    AIAdvisor* aiAdvisor = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
