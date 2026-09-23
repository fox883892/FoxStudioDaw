#include "TrackList.h"

TrackList::TrackList(FoxStudioEngine& e)
    : engine(e)
{
    titleLabel.setText("Tracks", juce::dontSendNotification);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    countLabel.setText("0", juce::dontSendNotification);
    countLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(countLabel);

    addAudioButton.onClick = [this] { if (onAddAudio) onAddAudio(); };
    addMidiButton.onClick = [this] { if (onAddMidi) onAddMidi(); };
    addAndMakeVisible(addAudioButton);
    addAndMakeVisible(addMidiButton);

    viewport.setViewedComponent(&rowsArea, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);

    refresh();
}

void TrackList::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff181b21));
    g.setColour(juce::Colour(0xff2d3139));
    g.drawRect(getLocalBounds(), 1);
}

void TrackList::refresh()
{
    countLabel.setText(juce::String(engine.getTracks().size()), juce::dontSendNotification);
    rowsArea.removeAllChildren();

    const auto tracks = engine.getTracks();
    for (size_t i = 0; i < tracks.size(); ++i)
    {
        auto* row = new juce::Component();
        row->setOpaque(true);
        row->setName("TrackRow");
        row->setSize(200, 52);

        auto* name = new juce::Label();
        name->setText(tracks[i].name, juce::dontSendNotification);
        name->setColour(juce::Label::textColourId, juce::Colours::white);
        name->setBounds(8, 8, 150, 18);
        row->addAndMakeVisible(name);

        auto* gain = new juce::Slider();
        gain->setRange(-60.0, 12.0, 0.1);
        gain->setValue(tracks[i].gainDb);
        gain->setBounds(8, 28, 150, 16);
        row->addAndMakeVisible(gain);

        row->setBounds(0, static_cast<int>(i) * 52, 200, 52);
        rowsArea.addAndMakeVisible(row);
    }

    rowsArea.setSize(200, static_cast<int>(tracks.size()) * 52);
    resized();
}

void TrackList::resized()
{
    auto area = getLocalBounds().reduced(6);
    titleLabel.setBounds(area.removeFromTop(24));
    countLabel.setBounds(area.removeFromTop(18));

    auto buttons = area.removeFromTop(42);
    addAudioButton.setBounds(buttons.removeFromLeft(90).reduced(2));
    addMidiButton.setBounds(buttons.removeFromLeft(90).reduced(2));

    viewport.setBounds(area);
    rowsArea.setBounds(0, 0, viewport.getWidth(), rowsArea.getHeight());
}
