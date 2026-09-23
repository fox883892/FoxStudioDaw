#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"
#include "ProjectManager.h"
#include "TransportBar.h"
#include "TrackList.h"
#include "TimelineComponent.h"
#include "MixerComponent.h"
#include "PianoRollComponent.h"
#include "AI/AiAdvisor.h"

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
    void togglePlay();
    void stopPlayback();
    void resetSession();
    void updateAiStatus();
    void saveCurrentProject();
    void loadDefaultProject();

    FoxStudioEngine engine;
    ProjectManager projectManager;
    ProjectSession currentSession;
    TransportBar transportBar { engine };
    TrackList trackList { engine };
    TimelineComponent timeline { engine };
    MixerComponent mixerComponent { engine };
    PianoRollComponent pianoRollComponent;
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    juce::Label titleLabel, projectLabel, statusLabel, aiSummaryLabel;
    juce::TextButton saveButton { "Save" }, loadButton { "Load" }, exportButton { "Export" };
    AIAdvisor aiAdvisor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
