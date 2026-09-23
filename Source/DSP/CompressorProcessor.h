#pragma once

#include "DspChain.h"

class CompressorProcessor final : public DspProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

    void setThresholdDb(float db) noexcept { thresholdDb = db; }
    void setRatio(float r) noexcept { ratio = r; }

private:
    float thresholdDb = -18.0f;
    float ratio = 3.0f;
};
