#include "MainComponent.h"

MainComponent::MainComponent()
{
    setOpaque(true);

    titleLabel.setText("FOX STUDIO DAW", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff9b54));
    addAndMakeVisible(titleLabel);

    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(statusLabel);

    addAndMakeVisible(transportBar);
    addAndMakeVisible(trackList);
    addAndMakeVisible(tabs);
    tabs.addTab("Timeline", juce::Colour(0xff1f2128), &timeline, true);
    tabs.setTabBarDepth(30);

    transportBar.onPlay = [this] { togglePlay(); };
    transportBar.onStop = [this] { stopPlayback(); };
    transportBar.onReset = [this] { resetSession(); };

    trackList.onAddAudio = [] { };
    trackList.onAddMidi = [] { };

    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    stopTimer();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121318));
    g.setColour(juce::Colour(0xff20232b));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 8.0f);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(14);

    titleLabel.setBounds(area.removeFromTop(32));
    statusLabel.setBounds(area.removeFromTop(22));
    transportBar.setBounds(area.removeFromTop(54));

    auto content = area.reduced(4);
    trackList.setBounds(content.removeFromLeft(220));
    tabs.setBounds(content);
}

void MainComponent::timerCallback()
{
    if (engine.isPlaying())
    {
        const double next = engine.getPosition() + 1.0 / 30.0;
        engine.setPosition(next);
    }
    repaint();
}

void MainComponent::togglePlay()
{
    if (engine.isPlaying())
        engine.stopPlayback();
    else
        engine.startPlayback();

    statusLabel.setText(engine.isPlaying() ? "Playing" : "Stopped", juce::dontSendNotification);
}

void MainComponent::stopPlayback()
{
    engine.stopPlayback();
    statusLabel.setText("Stopped", juce::dontSendNotification);
}

void MainComponent::resetSession()
{
    engine.stopPlayback();
    engine.setPosition(0.0);
    statusLabel.setText("Ready", juce::dontSendNotification);
}
