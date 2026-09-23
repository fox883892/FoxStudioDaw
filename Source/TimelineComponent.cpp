#include "TimelineComponent.h"

TimelineComponent::TimelineComponent(FoxStudioEngine& e)
    : engine(e)
{
    rulerLabel.setText("Timeline", juce::dontSendNotification);
    rulerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(rulerLabel);

    viewport.setViewedComponent(&content, false);
    viewport.setScrollBarsShown(true, true);
    addAndMakeVisible(viewport);

    startTimerHz(20);
}

TimelineComponent::~TimelineComponent()
{
    stopTimer();
}

void TimelineComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1d24));
    g.setColour(juce::Colour(0xff2e3340));
    g.drawRect(getLocalBounds(), 1);

    for (int x = 20; x < getWidth(); x += 80)
        g.drawVerticalLine(x, 0.0f, (float)getHeight(), 1.0f);

    const int playheadX = (int)(engine.getPosition() * 100.0f);
    g.setColour(juce::Colours::red);
    g.drawVerticalLine(playheadX, 0.0f, (float)getHeight(), 2.0f);
}

void TimelineComponent::resized()
{
    auto area = getLocalBounds();
    rulerLabel.setBounds(area.removeFromTop(24));
    viewport.setBounds(area);
    content.setSize(1400, 400);
}

void TimelineComponent::timerCallback()
{
    repaint();
}
