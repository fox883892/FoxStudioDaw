#include "MainComponent.h"
#include "AI/AiAdvisor.h"

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

    aiSummaryLabel.setText("AI: waiting for analysis", juce::dontSendNotification);
    aiSummaryLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8be3a8));
    addAndMakeVisible(aiSummaryLabel);

    addAndMakeVisible(transportBar);
    addAndMakeVisible(trackList);
    addAndMakeVisible(tabs);
    tabs.addTab("Timeline", juce::Colour(0xff1f2128), &timeline, true);
    tabs.addTab("Mixer", juce::Colour(0xff1f2128), &mixerComponent, false);
    tabs.setTabBarDepth(30);

    transportBar.onPlay = [this] { togglePlay(); };
    transportBar.onStop = [this] { stopPlayback(); };
    transportBar.onReset = [this] { resetSession(); };

    trackList.onAddAudio = [this] { engine.addTrack(TrackInfo::Type::Audio, "Audio Track"); trackList.refresh(); };
    trackList.onAddMidi = [this] { engine.addTrack(TrackInfo::Type::Midi, "Midi Track"); trackList.refresh(); };

    aiAdvisor = new AIAdvisor();
    startTimerHz(15);
}

MainComponent::~MainComponent()
{
    stopTimer();
    delete aiAdvisor;
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
    aiSummaryLabel.setBounds(area.removeFromTop(26));
    transportBar.setBounds(area.removeFromTop(54));

    auto content = area.reduced(4);
    trackList.setBounds(content.removeFromLeft(220));
    tabs.setBounds(content);
}

void MainComponent::timerCallback()
{
    if (engine.isPlaying())
        engine.setPosition(engine.getPosition() + 1.0 / 15.0);

    const float rms = 0.08f + 0.12f * std::sin((float)engine.getPosition() * 0.25f);
    const float peak = 0.6f + 0.3f * std::sin((float)engine.getPosition() * 0.7f);
    const float lufs = -18.0f + 6.0f * std::sin((float)engine.getPosition() * 0.3f);
    const auto rec = aiAdvisor->analyse(rms, peak, lufs, 1500.0f, peak > 0.85f, engine.isPlaying());

    aiSummaryLabel.setText("AI: " + rec.summary, juce::dontSendNotification);
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
    aiSummaryLabel.setText("AI: waiting for analysis", juce::dontSendNotification);
}
