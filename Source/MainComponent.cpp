#include "MainComponent.h"
#include <cmath>

MainComponent::MainComponent()
{
    setOpaque(true);
    currentSession = projectManager.createDefaultSession();

    titleLabel.setText("FOX STUDIO DAW", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff9b54));
    addAndMakeVisible(titleLabel);
    projectLabel.setText(currentSession.name, juce::dontSendNotification);
    addAndMakeVisible(projectLabel);
    statusLabel.setText("Audio engine ready", juce::dontSendNotification);
    addAndMakeVisible(statusLabel);
    aiSummaryLabel.setText("AI: waiting for audio", juce::dontSendNotification);
    aiSummaryLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8be3a8));
    addAndMakeVisible(aiSummaryLabel);

    for (auto* button : { &saveButton, &loadButton, &exportButton })
        addAndMakeVisible(button);
    saveButton.onClick = [this] { saveCurrentProject(); };
    loadButton.onClick = [this] { loadDefaultProject(); };
    exportButton.onClick = [this] { statusLabel.setText("Render integration pending", juce::dontSendNotification); };

    addAndMakeVisible(transportBar);
    transportBar.onPlay = [this] { togglePlay(); };
    transportBar.onStop = [this] { stopPlayback(); };
    transportBar.onReset = [this] { resetSession(); };

    addAndMakeVisible(trackList);
    trackList.onAddAudio = [this] { engine.addTrack(TrackState::Type::audio, "Audio Track"); trackList.refresh(); mixerComponent.refresh(); };
    trackList.onAddMidi = [this] { engine.addTrack(TrackState::Type::midi, "MIDI Track"); trackList.refresh(); mixerComponent.refresh(); };

    addAndMakeVisible(tabs);
    tabs.addTab("Timeline", juce::Colour(0xff1f2128), &timeline, false);
    tabs.addTab("Mixer", juce::Colour(0xff1f2128), &mixerComponent, false);
    tabs.addTab("Piano Roll", juce::Colour(0xff1f2128), &pianoRollComponent, false);
    tabs.setTabBarDepth(30);

    setAudioChannels(0, 2);
    startTimerHz(15);
    updateAiStatus();
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    engine.prepareToPlay(sampleRate, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (info.buffer == nullptr)
        return;
    info.clearActiveBufferRegion();
    engine.render(*info.buffer, info.startSample, info.numSamples);
}

void MainComponent::releaseResources() { engine.releaseResources(); }

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff121318));
    g.setColour(juce::Colour(0xff20232b));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 8.0f);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(14);
    titleLabel.setBounds(area.removeFromTop(28));
    projectLabel.setBounds(area.removeFromTop(20));
    statusLabel.setBounds(area.removeFromTop(20));
    aiSummaryLabel.setBounds(area.removeFromTop(22));
    auto buttons = area.removeFromTop(32);
    saveButton.setBounds(buttons.removeFromLeft(80).reduced(2));
    loadButton.setBounds(buttons.removeFromLeft(80).reduced(2));
    exportButton.setBounds(buttons.removeFromLeft(90).reduced(2));
    transportBar.setBounds(area.removeFromTop(52));
    auto body = area.reduced(4);
    trackList.setBounds(body.removeFromLeft(220));
    tabs.setBounds(body);
}

void MainComponent::timerCallback()
{
    updateAiStatus();
    repaint();
}

void MainComponent::togglePlay()
{
    if (engine.isPlaying()) engine.pause(); else engine.startPlayback();
    statusLabel.setText(engine.isPlaying() ? "Playing" : "Paused", juce::dontSendNotification);
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
    statusLabel.setText("Session reset", juce::dontSendNotification);
}

void MainComponent::updateAiStatus()
{
    const float t = static_cast<float>(engine.getPosition());
    const float peak = 0.55f + 0.25f * std::sin(t * 0.7f);
    const float lufs = -20.0f + 6.0f * std::sin(t * 0.3f);
    const auto result = aiAdvisor.analyse(0.1f, peak, lufs, 1200.0f, peak > 0.95f, engine.isPlaying());
    aiSummaryLabel.setText("AI: " + result.summary, juce::dontSendNotification);
}

void MainComponent::saveCurrentProject()
{
    auto directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("foxstudio");
    directory.createDirectory();
    const auto file = directory.getChildFile(currentSession.name + ".foxproj");
    currentSession.tempo = static_cast<int>(engine.getTempo());
    currentSession.loop = engine.isLoopEnabled();
    currentSession.tracks.clear();
    for (const auto& track : engine.getTracks()) currentSession.tracks.push_back(track.name);
    statusLabel.setText(projectManager.saveToFile(file, currentSession) ? "Project saved" : "Save failed", juce::dontSendNotification);
}

void MainComponent::loadDefaultProject()
{
    auto directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("foxstudio");
    const auto file = directory.getChildFile(currentSession.name + ".foxproj");
    currentSession = projectManager.loadFromFile(file);
    engine.setTempo(currentSession.tempo);
    engine.setLoopEnabled(currentSession.loop);
    projectLabel.setText(currentSession.name, juce::dontSendNotification);
    statusLabel.setText("Project loaded", juce::dontSendNotification);
}
