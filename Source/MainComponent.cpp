#include "MainComponent.h"
#include <cmath>

MainComponent::MainComponent()
{
    setOpaque(true);
    addAndMakeVisible(titleLabel);
    titleLabel.setText("FOX STUDIO DAW", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff9b54));

    for (auto* control : { &playButton, &stopButton, &resetButton, &audioButton, &midiButton, &loopButton })
        addAndMakeVisible(control);

    addAndMakeVisible(timeLabel);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    addAndMakeVisible(statusLabel);
    addAndMakeVisible(tracksLabel);

    playButton.onClick = [this]
    {
        if (engine.isPlaying()) engine.pause(); else engine.play();
        updateStatus();
    };
    stopButton.onClick = [this] { engine.stop(); updateStatus(); };
    resetButton.onClick = [this]
    {
        engine.stop();
        engine.getTracks().clear();
        engine.addTrack(TrackState::Type::midi, "Lead");
        updateStatus();
    };
    audioButton.onClick = [this] { engine.addTrack(TrackState::Type::audio, "Audio " + juce::String(engine.getTracks().size() + 1)); updateStatus(); };
    midiButton.onClick = [this] { engine.addTrack(TrackState::Type::midi, "MIDI " + juce::String(engine.getTracks().size() + 1)); updateStatus(); };
    loopButton.onClick = [this] { engine.setLoop(loopButton.getToggleState()); };

    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(engine.getTempo());
    tempoSlider.setTextValueSuffix(" BPM");
    tempoSlider.onValueChange = [this] { engine.setTempo(tempoSlider.getValue()); };
    addAndMakeVisible(tempoSlider);

    startTimerHz(30);
    updateStatus();
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    engine.prepare(sampleRate, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (info.buffer == nullptr)
        return;
    info.clearActiveBufferRegion();
    engine.render(*info.buffer, info.startSample, info.numSamples);
}

void MainComponent::releaseResources()
{
    engine.release();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111318));
    auto area = getLocalBounds().reduced(18);
    area.removeFromTop(76);
    g.setColour(juce::Colour(0xff242832));
    g.fillRoundedRectangle(area.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff3a404d));
    for (int x = area.getX(); x < area.getRight(); x += 80)
        g.drawVerticalLine(x, static_cast<float>(area.getY()), static_cast<float>(area.getBottom()));

    const auto playhead = area.getX() + static_cast<int>(std::fmod(engine.getPositionSeconds(), 8.0) / 8.0 * area.getWidth());
    g.setColour(juce::Colours::orange);
    g.drawVerticalLine(playhead, static_cast<float>(area.getY()), static_cast<float>(area.getBottom()), 2.0f);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(18);
    auto header = area.removeFromTop(40);
    titleLabel.setBounds(header.removeFromLeft(300));
    timeLabel.setBounds(header.removeFromLeft(150));
    statusLabel.setBounds(header);

    auto controls = area.removeFromTop(42);
    for (auto* control : { &resetButton, &playButton, &stopButton, &audioButton, &midiButton, &loopButton })
        control->setBounds(controls.removeFromLeft(84).reduced(2));
    tempoSlider.setBounds(controls.reduced(4));
    tracksLabel.setBounds(area.removeFromBottom(28));
}

void MainComponent::timerCallback()
{
    timeLabel.setText(formatTime(engine.getPositionSeconds()), juce::dontSendNotification);
    repaint();
}

void MainComponent::updateStatus()
{
    playButton.setButtonText(engine.isPlaying() ? "Pause" : "Play");
    statusLabel.setText(engine.isPlaying() ? "Playing" : "Ready", juce::dontSendNotification);
    tracksLabel.setText("Tracks: " + juce::String(engine.getTracks().size()), juce::dontSendNotification);
}

juce::String MainComponent::formatTime(double seconds) const
{
    const auto milliseconds = static_cast<int>(std::round(seconds * 1000.0));
    return juce::String::formatted("%02d:%02d.%03d", milliseconds / 60000,
                                   (milliseconds / 1000) % 60, milliseconds % 1000);
}
