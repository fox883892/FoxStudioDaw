#pragma once

#include "DspChain.h"

class SaturationProcessor final : public DspProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

    void setDrive(float d) noexcept { drive = d; }
    void setMix(float m) noexcept { mix = m; }

private:
    float drive = 1.4f;
    float mix = 0.45f;
};
