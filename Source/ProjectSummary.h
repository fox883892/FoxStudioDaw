#pragma once

#include <juce_core/juce_core.h>

class ProjectSummary final : public juce::Component
{
public:
    ProjectSummary() = default;
    ~ProjectSummary() override = default;

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff1b1d24));
        g.setColour(juce::Colours::white);
        g.setFont(18.0f);
        g.drawText("Project Overview", juce::Rectangle<float>(10.0f, 10.0f, 200.0f, 30.0f), juce::Justification::left);
    }
};
