#include "CompressorProcessor.h"

void CompressorProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    (void)spec;
}

void CompressorProcessor::reset()
{
    thresholdDb = -18.0f;
    ratio = 3.0f;
}

void CompressorProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const float threshold = juce::Decibels::decibelsToGain(thresholdDb);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float absX = std::abs(data[i]);
            const float over = std::max(0.0f, absX - threshold);
            const float reduction = std::min(1.0f, over / std::max(0.0001f, threshold));
            data[i] *= 1.0f - (reduction / std::max(1.0f, ratio));
        }
    }
}
