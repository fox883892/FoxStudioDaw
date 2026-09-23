#include "EqProcessor.h"

void EqProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    (void)spec;
}

void EqProcessor::reset()
{
    lowShelfDb = 0.0f;
    midDb = 0.0f;
    highShelfDb = 0.0f;
}

void EqProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const float low = juce::Decibels::decibelsToGain(lowShelfDb);
    const float mid = juce::Decibels::decibelsToGain(midDb);
    const float high = juce::Decibels::decibelsToGain(highShelfDb);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float x = data[i];
            const float s = (float)i / std::max(1, buffer.getNumSamples());
            const float h = 1.0f - s;
            data[i] = x * ((s * low) + (0.5f * mid) + (h * high));
        }
    }
}
