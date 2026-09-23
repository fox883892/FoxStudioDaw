#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"
#include "ProjectManager.h"
#include "TracktionBridge.h"
#include "TransportBar.h"
#include "TrackList.h"
#include "TimelineComponent.h"
#include "MixerComponent.h"
#include "PianoRollComponent.h"
#include "AI/AiAdvisor.h"

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
    void updateAiStatus();
    void saveCurrentProject();
    void loadDefaultProject();

    FoxStudioEngine engine;
    ProjectManager projectManager;
    TracktionBridge tracktionBridge;
    TransportBar transportBar{ engine };
    TrackList trackList{ engine };
    TimelineComponent timeline{ engine };
    MixerComponent mixerComponent{ engine };
    PianoRollComponent pianoRollComponent;

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    juce::Label titleLabel;
    juce::Label projectLabel;
    juce::Label statusLabel;
    juce::Label aiSummaryLabel;
    juce::TextButton saveButton { "Save" };
    juce::TextButton loadButton { "Load" };
    juce::TextButton exportButton { "Export" };

    ProjectSession currentSession;
    AIAdvisor aiAdvisor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
