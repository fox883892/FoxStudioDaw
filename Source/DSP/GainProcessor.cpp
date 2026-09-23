#include "GainProcessor.h"

void GainProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    (void)spec;
}

void GainProcessor::reset()
{
    gainDb = 0.0f;
}

void GainProcessor::process(juce::AudioBuffer<float>& buffer)
{
    const float gain = juce::Decibels::decibelsToGain(gainDb);
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] *= gain;
    }
}
