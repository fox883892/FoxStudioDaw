#include "MainComponent.h"

#include <cmath>

MainComponent::MainComponent()
{
    setOpaque(true);
    setAudioChannels(0, 2);
    titleLabel.setText("FOX STUDIO DAW", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff9b54));
    addAndMakeVisible(titleLabel);

    for (auto* b : {&newButton, &openButton, &saveButton, &playButton, &stopButton,
                    &recordButton, &addAudioButton, &addMidiButton, &loopButton, &metronomeButton})
        addAndMakeVisible(b);

    newButton.onClick = [this] { resetSession(); };
    playButton.onClick = [this] { toggleTransport(); };
    stopButton.onClick = [this] { playing = false; positionSeconds = 0.0; playButton.setButtonText("Play"); };
    addAudioButton.onClick = [this] { ++trackCount; trackLabel.setText("Audio track " + juce::String(trackCount), juce::dontSendNotification); };
    addMidiButton.onClick = [this] { ++trackCount; trackLabel.setText("MIDI track " + juce::String(trackCount), juce::dontSendNotification); };

    transportLabel.setJustificationType(juce::Justification::centred);
    transportLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    addAndMakeVisible(transportLabel);
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(statusLabel);
    trackLabel.setText("No tracks", juce::dontSendNotification);
    addAndMakeVisible(trackLabel);

    for (auto* label : {&timelineLabel, &mixerLabel, &inspectorLabel})
    {
        label->setColour(juce::Label::textColourId, juce::Colour(0xffaeb4c0));
        label->setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(label);
    }
    timelineLabel.setText("ARRANGEMENT / TIMELINE", juce::dontSendNotification);
    mixerLabel.setText("MIXER", juce::dontSendNotification);
    inspectorLabel.setText("INSPECTOR", juce::dontSendNotification);

    tempoSlider.setRange(40.0, 240.0, 1.0); tempoSlider.setValue(120.0); addAndMakeVisible(tempoSlider);
    masterSlider.setRange(-60.0, 6.0, 0.1); masterSlider.setValue(0.0); addAndMakeVisible(masterSlider);
    startTimerHz(30);
}

MainComponent::~MainComponent() { stopTimer(); shutdownAudio(); }
void MainComponent::prepareToPlay(int block, double rate) { sampleRate = rate; scratch.setSize(2, block); }
void MainComponent::releaseResources() { scratch.setSize(0, 0); }
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    info.clearActiveBufferRegion();
    if (!playing || info.buffer == nullptr) return;
    static double phase = 0.0;
    const auto step = 2.0 * juce::MathConstants<double>::pi * 220.0 / sampleRate;
    for (int i = 0; i < info.numSamples; ++i)
    {
        const auto value = 0.08f * std::sin(phase); phase += step;
        for (int ch = 0; ch < info.buffer->getNumChannels(); ++ch)
            info.buffer->setSample(ch, info.startSample + i, value);
    }
}
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121318));
    auto area = getLocalBounds().reduced(22); area.removeFromTop(46);
    auto body = area.reduced(8); auto left = body.removeFromLeft(250); auto right = body.removeFromRight(230); auto centre = body;
    g.setColour(juce::Colour(0xff292d37));
    g.fillRoundedRectangle(left.toFloat(), 6.0f); g.fillRoundedRectangle(centre.toFloat(), 6.0f); g.fillRoundedRectangle(right.toFloat(), 6.0f);
    g.setColour(juce::Colour(0xff343946));
    for (int y = centre.getY() + 45; y < centre.getBottom(); y += 44) g.drawHorizontalLine(y, (float)centre.getX(), (float)centre.getRight());
    for (int x = centre.getX() + 70; x < centre.getRight(); x += 70) g.drawVerticalLine(x, (float)centre.getY(), (float)centre.getBottom());
    g.setColour(juce::Colours::red); g.drawVerticalLine(centre.getX() + (int)(std::fmod(positionSeconds, 32.0) / 32.0 * centre.getWidth()), (float)centre.getY(), (float)centre.getBottom(), 2.0f);
}
void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(22); auto bar = area.removeFromTop(46);
    titleLabel.setBounds(bar.removeFromLeft(230));
    for (auto* b : {&newButton, &openButton, &saveButton, &playButton, &stopButton, &recordButton}) b->setBounds(bar.removeFromLeft(70).reduced(2));
    transportLabel.setBounds(bar.removeFromLeft(125)); statusLabel.setBounds(bar);
    auto body = area.reduced(8); auto left = body.removeFromLeft(250); auto right = body.removeFromRight(230); auto centre = body;
    addAudioButton.setBounds(left.removeFromTop(32).reduced(6)); addMidiButton.setBounds(left.removeFromTop(32).reduced(6)); trackLabel.setBounds(left.removeFromTop(32).reduced(10, 4));
    timelineLabel.setBounds(centre.removeFromTop(34).reduced(10, 4)); mixerLabel.setBounds(centre.removeFromBottom(34).reduced(10, 4));
    inspectorLabel.setBounds(right.removeFromTop(34).reduced(10, 4)); tempoSlider.setBounds(right.removeFromTop(48).reduced(10)); masterSlider.setBounds(right.removeFromTop(48).reduced(10));
    loopButton.setBounds(right.removeFromTop(30).reduced(10)); metronomeButton.setBounds(right.removeFromTop(30).reduced(10));
}
void MainComponent::timerCallback() { if (playing) positionSeconds += 1.0 / 30.0; transportLabel.setText(formatTime(positionSeconds), juce::dontSendNotification); repaint(); }
void MainComponent::toggleTransport() { playing = !playing; playButton.setButtonText(playing ? "Pause" : "Play"); statusLabel.setText(playing ? "Playing" : "Paused", juce::dontSendNotification); }
void MainComponent::resetSession() { playing = false; positionSeconds = 0.0; trackCount = 0; playButton.setButtonText("Play"); trackLabel.setText("No tracks", juce::dontSendNotification); statusLabel.setText("Ready", juce::dontSendNotification); }
juce::String MainComponent::formatTime(double seconds) const {
    const auto ms = static_cast<int>(std::round(seconds * 1000.0));
    return juce::String(ms / 60000) + ":" + juce::String((ms / 1000) % 60).formatted("%02d") + "." + juce::String(ms % 1000).formatted("%03d");
}
