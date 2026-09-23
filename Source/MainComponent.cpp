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
    projectLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(projectLabel);

    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    addAndMakeVisible(statusLabel);

    aiSummaryLabel.setText("AI: waiting for analysis", juce::dontSendNotification);
    aiSummaryLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8be3a8));
    addAndMakeVisible(aiSummaryLabel);

    saveButton.onClick = [this] { saveCurrentProject(); };
    loadButton.onClick = [this] { loadDefaultProject(); };
    exportButton.onClick = [this] { statusLabel.setText("Export queued", juce::dontSendNotification); };

    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(exportButton);

    addAndMakeVisible(transportBar);
    addAndMakeVisible(trackList);
    addAndMakeVisible(tabs);
    tabs.addTab("Timeline", juce::Colour(0xff1f2128), &timeline, true);
    tabs.addTab("Mixer", juce::Colour(0xff1f2128), &mixerComponent, false);
    tabs.addTab("Piano Roll", juce::Colour(0xff1f2128), &pianoRollComponent, false);
    tabs.setTabBarDepth(30);

    transportBar.onPlay = [this] { togglePlay(); };
    transportBar.onStop = [this] { stopPlayback(); };
    transportBar.onReset = [this] { resetSession(); };

    trackList.onAddAudio = [this] {
        engine.addTrack(TrackInfo::Type::Audio, "Audio Track");
        trackList.refresh();
    };

    trackList.onAddMidi = [this] {
        engine.addTrack(TrackInfo::Type::Midi, "Midi Track");
        trackList.refresh();
    };

    tracktionBridge.initialise(currentSession.name);
    tracktionBridge.syncSession(currentSession.name, currentSession.tempo, currentSession.loop);

    startTimerHz(15);
    updateAiStatus();
}

MainComponent::~MainComponent() = default;

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
    projectLabel.setBounds(area.removeFromTop(22));
    statusLabel.setBounds(area.removeFromTop(20));
    aiSummaryLabel.setBounds(area.removeFromTop(22));

    auto controls = area.removeFromTop(34);
    saveButton.setBounds(controls.removeFromLeft(80).reduced(4));
    loadButton.setBounds(controls.removeFromLeft(80).reduced(4));
    exportButton.setBounds(controls.removeFromLeft(90).reduced(4));

    transportBar.setBounds(area.removeFromTop(52));
    auto content = area.reduced(4);
    trackList.setBounds(content.removeFromLeft(220));
    tabs.setBounds(content);
}

void MainComponent::timerCallback()
{
    if (engine.isPlaying())
        engine.setPosition(engine.getPosition() + 1.0 / 15.0);

    updateAiStatus();
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

void MainComponent::updateAiStatus()
{
    const float time = static_cast<float>(engine.getPosition());
    const float rms = 0.08f + 0.12f * std::sin(time * 0.25f);
    const float peak = 0.62f + 0.28f * std::sin(time * 0.7f);
    const float lufs = -20.0f + 7.0f * std::sin(time * 0.3f);

    const auto rec = aiAdvisor.analyse(rms, peak, lufs, 1500.0f, peak > 0.85f, engine.isPlaying());
    aiSummaryLabel.setText("AI: " + rec.summary, juce::dontSendNotification);
}

void MainComponent::saveCurrentProject()
{
    const auto savePath = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                              .getChildFile("foxstudio")
                              .getChildFile(currentSession.name + ".foxproj");

    if (projectManager.saveToFile(savePath, currentSession))
        statusLabel.setText("Project saved", juce::dontSendNotification);
    else
        statusLabel.setText("Save failed", juce::dontSendNotification);
}

void MainComponent::loadDefaultProject()
{
    const auto loadPath = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                              .getChildFile("foxstudio")
                              .getChildFile(currentSession.name + ".foxproj");

    currentSession = projectManager.loadFromFile(loadPath);
    projectLabel.setText(currentSession.name, juce::dontSendNotification);
    engine.setTempo(currentSession.tempo);
    statusLabel.setText("Project loaded", juce::dontSendNotification);
}
