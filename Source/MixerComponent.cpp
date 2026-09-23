#include "MixerComponent.h"

MixerComponent::MixerComponent(FoxStudioEngine& e)
    : engine(e)
{
    viewport.setViewedComponent(&stripsArea, false);
    viewport.setScrollBarsShown(false, true);
    addAndMakeVisible(viewport);
    refresh();
}

MixerComponent::~MixerComponent() = default;

void MixerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff131720));
}

void MixerComponent::refresh()
{
    stripsArea.removeAllChildren();

    const auto& tracks = engine.getTracks();
    for (size_t i = 0; i < tracks.size(); ++i)
    {
        auto* strip = new juce::Component();
        strip->setOpaque(true);
        strip->setSize(150, 220);

        auto* name = new juce::Label();
        name->setText(tracks[i].name, juce::dontSendNotification);
        name->setJustificationType(juce::Justification::centred);
        name->setColour(juce::Label::textColourId, juce::Colours::white);
        name->setBounds(10, 8, 130, 20);
        strip->addAndMakeVisible(name);

        auto* gain = new juce::Slider();
        gain->setRange(-60.0, 12.0, 0.1);
        gain->setValue(tracks[i].gainDb);
        gain->setSliderStyle(juce::Slider::LinearVertical);
        gain->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        gain->setBounds(10, 36, 40, 130);
        strip->addAndMakeVisible(gain);

        auto* pan = new juce::Slider();
        pan->setRange(-1.0, 1.0, 0.01);
        pan->setValue(tracks[i].pan);
        pan->setSliderStyle(juce::Slider::LinearHorizontal);
        pan->setBounds(60, 60, 70, 20);
        strip->addAndMakeVisible(pan);

        strip->setBounds(static_cast<int>(i) * 160, 0, 150, 220);
        stripsArea.addAndMakeVisible(strip);
    }

    stripsArea.setSize(static_cast<int>(tracks.size()) * 160, 220);
    resized();
}

void MixerComponent::resized()
{
    viewport.setBounds(getLocalBounds());
    stripsArea.setBounds(0, 0, viewport.getWidth(), stripsArea.getHeight());
}
