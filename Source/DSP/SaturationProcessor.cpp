#include "SaturationProcessor.h"

void SaturationProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    (void)spec;
}

void SaturationProcessor::reset()
{
    drive = 1.4f;
    mix = 0.45f;
}

void SaturationProcessor::process(juce::AudioBuffer<float>& buffer)
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float wet = std::tanh(data[i] * drive * 2.0f);
            data[i] = (data[i] * (1.0f - mix)) + (wet * mix);
        }
    }
}
