#include "LimiterProcessor.h"

void LimiterProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    (void)spec;
}

void LimiterProcessor::reset()
{
    limitDb = -1.0f;
}

void LimiterProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const float limit = juce::Decibels::decibelsToGain(limitDb);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float x = data[i];
            const float absX = std::abs(x);
            if (absX > limit)
                data[i] = x * (limit / absX);
        }
    }
}
