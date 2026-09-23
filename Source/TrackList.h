#pragma once

#include <functional>
#include <juce_gui_extra/juce_gui_extra.h>
#include "FoxStudioEngine.h"

class TrackList final : public juce::Component
{
public:
    explicit TrackList(FoxStudioEngine& engine);
    ~TrackList() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void refresh();

    std::function<void()> onAddAudio;
    std::function<void()> onAddMidi;

private:
    FoxStudioEngine& engine;
    juce::TextButton addAudioButton { "+ Audio" };
    juce::TextButton addMidiButton { "+ MIDI" };
    juce::Label titleLabel;
    juce::Label countLabel;
    juce::Viewport viewport;
    juce::Component rowsArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
