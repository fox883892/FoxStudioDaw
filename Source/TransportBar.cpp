#include "TransportBar.h"

TransportBar::TransportBar(FoxStudioEngine& e)
    : engine(e)
{
    playButton.onClick = [this] { if (onPlay) onPlay(); };
    stopButton.onClick = [this] { if (onStop) onStop(); };
    resetButton.onClick = [this] { if (onReset) onReset(); };

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(resetButton);

    clockLabel.setText("00:00.000", juce::dontSendNotification);
    clockLabel.setJustificationType(juce::Justification::centred);
    clockLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(clockLabel);

    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(engine.getTempo());
    tempoSlider.onValueChange = [this] { engine.setTempo(tempoSlider.getValue()); };
    addAndMakeVisible(tempoSlider);

    startTimerHz(30);
}

TransportBar::~TransportBar()
{
    stopTimer();
}

void TransportBar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1d1f25));
    g.setColour(juce::Colour(0xff3b3d46));
    g.drawRect(getLocalBounds(), 1);
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced(4);
    playButton.setBounds(area.removeFromLeft(80).reduced(2));
    stopButton.setBounds(area.removeFromLeft(80).reduced(2));
    resetButton.setBounds(area.removeFromLeft(80).reduced(2));
    clockLabel.setBounds(area.removeFromLeft(150));
    tempoSlider.setBounds(area);
}

void TransportBar::timerCallback()
{
    if (engine.isPlaying())
    {
        engine.setPosition(engine.getPosition() + 1.0 / 30.0);
    }

    clockLabel.setText(formatTime(engine.getPosition()), juce::dontSendNotification);
}

juce::String TransportBar::formatTime(double seconds) const
{
    const auto totalMs = static_cast<int>(std::round(seconds * 1000.0));
    const auto minutes = totalMs / 60000;
    const auto secs = (totalMs / 1000) % 60;
    const auto ms = totalMs % 1000;

    return juce::String(minutes).paddedLeft('0', 2) + ":" +
           juce::String(secs).paddedLeft('0', 2) + "." +
           juce::String(ms).paddedLeft('0', 3);
}
