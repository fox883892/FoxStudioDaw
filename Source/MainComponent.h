#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"
#include "ProjectManager.h"
#include "TracktionBridge.h"

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
    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label aiSummaryLabel;
    juce::Label projectLabel;
    juce::TextButton saveButton { "Save" };
    juce::TextButton loadButton { "Load" };
    juce::TextButton exportButton { "Export" };
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    juce::Component contentArea;

    ProjectSession currentSession;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
