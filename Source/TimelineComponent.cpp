#include "TimelineComponent.h"

TimelineComponent::TimelineComponent(FoxStudioEngine& e)
    : engine(e)
{
    timeRuler.setText("Timeline", juce::dontSendNotification);
    timeRuler.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(timeRuler);

    viewport.setViewedComponent(&timelineArea, false);
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

    const auto pos = (int)(engine.getPosition() * 100.0f);
    g.setColour(juce::Colours::red);
    g.drawVerticalLine(pos + 20, 0.0f, (float)getHeight(), 2.0f);
}

void TimelineComponent::resized()
{
    auto area = getLocalBounds();
    timeRuler.setBounds(area.removeFromTop(24));
    viewport.setBounds(area);
    timelineArea.setSize(1400, 400);
}

void TimelineComponent::timerCallback()
{
    repaint();
}
