#include "MainComponent.h"
#include <cmath>

MainComponent::MainComponent()
{
    setOpaque(true);
    setAudioChannels(0, 2);
    titleLabel.setText("FOX STUDIO DAW — REAL ENGINE", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff9b54));
    addAndMakeVisible(titleLabel);

    for (auto* button : { &newButton, &playButton, &stopButton, &exportButton, &addAudioButton, &addMidiButton, &loopButton })
        addAndMakeVisible(button);

    playButton.onClick = [this] { toggleTransport(); };
    stopButton.onClick = [this] { engine.stopPlayback(); statusLabel.setText("Stopped", juce::dontSendNotification); };
    newButton.onClick = [this] { resetSession(); };
    addAudioButton.onClick = [this] { engine.addTrack(TrackInfo::Type::Audio, "Audio " + juce::String(engine.getTracks().size() + 1)); trackLabel.setText(juce::String(engine.getTracks().size()) + " tracks", juce::dontSendNotification); };
    addMidiButton.onClick = [this] { engine.addTrack(TrackInfo::Type::Midi, "MIDI " + juce::String(engine.getTracks().size() + 1)); trackLabel.setText(juce::String(engine.getTracks().size()) + " tracks", juce::dontSendNotification); };
    loopButton.onClick = [this] { engine.setLoopEnabled(loopButton.getToggleState()); };
    exportButton.onClick = [this]
    {
        const auto file = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("FOX_STUDIO_Render.wav");
        statusLabel.setText(engine.exportProjectAsWav(file) ? "WAV exported" : "Export failed", juce::dontSendNotification);
    };

    transportLabel.setJustificationType(juce::Justification::centred);
    transportLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    addAndMakeVisible(transportLabel);
    statusLabel.setText("Audio engine ready", juce::dontSendNotification);
    addAndMakeVisible(statusLabel);
    trackLabel.setText(juce::String(engine.getTracks().size()) + " tracks", juce::dontSendNotification);
    addAndMakeVisible(trackLabel);

    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(engine.getTempo());
    tempoSlider.setTextValueSuffix(" BPM");
    tempoSlider.onValueChange = [this] { engine.setTempo(tempoSlider.getValue()); };
    addAndMakeVisible(tempoSlider);
    startTimerHz(30);
}

MainComponent::~MainComponent() { stopTimer(); shutdownAudio(); }
void MainComponent::prepareToPlay(int block, double rate) { sampleRate = rate; engine.prepareToPlay(rate); (void) block; }
void MainComponent::releaseResources() {}
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    info.clearActiveBufferRegion();
    engine.processAudio(*info.buffer, info.startSample, info.numSamples);
}
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121318));
    g.setColour(juce::Colour(0xff20232b));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 8.0f);
    auto area = getLocalBounds().reduced(22); area.removeFromTop(48);
    g.setColour(juce::Colour(0xff343946));
    for (int x = area.getX(); x < area.getRight(); x += 80) g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());
    const auto x = area.getX() + (int)(std::fmod(engine.getPosition(), 8.0) / 8.0 * area.getWidth());
    g.setColour(juce::Colours::red); g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom(), 2.0f);
}
void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(22); auto bar = area.removeFromTop(48);
    titleLabel.setBounds(bar.removeFromLeft(300));
    for (auto* button : { &newButton, &playButton, &stopButton, &exportButton }) button->setBounds(bar.removeFromLeft(90).reduced(2));
    transportLabel.setBounds(bar.removeFromLeft(130)); statusLabel.setBounds(bar);
    auto controls = area.removeFromTop(42);
    addAudioButton.setBounds(controls.removeFromLeft(100).reduced(2)); addMidiButton.setBounds(controls.removeFromLeft(100).reduced(2));
    trackLabel.setBounds(controls.removeFromLeft(100)); loopButton.setBounds(controls.removeFromLeft(80)); tempoSlider.setBounds(controls);
}
void MainComponent::timerCallback() { transportLabel.setText(formatTime(engine.getPosition()), juce::dontSendNotification); repaint(); }
void MainComponent::toggleTransport() { if (engine.isPlaying()) engine.stopPlayback(); else engine.startPlayback(); playButton.setButtonText(engine.isPlaying() ? "Pause" : "Play"); statusLabel.setText(engine.isPlaying() ? "Playing" : "Paused", juce::dontSendNotification); }
void MainComponent::resetSession() { engine.stopPlayback(); engine.setPosition(0.0); statusLabel.setText("New session", juce::dontSendNotification); }
juce::String MainComponent::formatTime(double seconds) const { const auto ms = static_cast<int>(std::round(seconds * 1000.0)); return juce::String::formatted("%02d:%02d.%03d", ms / 60000, (ms / 1000) % 60, ms % 1000); }
