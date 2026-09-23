#pragma once

#include "DspChain.h"

class LimiterProcessor final : public DspProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

    void setLimitDb(float db) noexcept { limitDb = db; }

private:
    float limitDb = -1.0f;
};
