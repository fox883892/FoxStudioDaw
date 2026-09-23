#include "TrackList.h"

TrackList::TrackList(FoxStudioEngine& e)
    : engine(e)
{
    titleLabel.setText("Tracks", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    trackCountLabel.setText("0", juce::dontSendNotification);
    trackCountLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(trackCountLabel);

    addAudioButton.onClick = [this] { if (onAddAudio) onAddAudio(); };
    addMidiButton.onClick = [this] { if (onAddMidi) onAddMidi(); };
    addAndMakeVisible(addAudioButton);
    addAndMakeVisible(addMidiButton);
}

void TrackList::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff181b21));
    g.setColour(juce::Colour(0xff2d3139));
    g.drawRect(getLocalBounds(), 1);
}

void TrackList::resized()
{
    auto area = getLocalBounds().reduced(6);
    titleLabel.setBounds(area.removeFromTop(24));
    trackCountLabel.setBounds(area.removeFromTop(20));
    auto buttonRow = area.removeFromTop(34);
    addAudioButton.setBounds(buttonRow.removeFromLeft(90).reduced(2));
    addMidiButton.setBounds(buttonRow.removeFromLeft(90).reduced(2));
}
