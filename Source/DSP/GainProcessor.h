#pragma once

#include "DspChain.h"

class GainProcessor final : public DspProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

    void setGainDb(float db) noexcept { gainDb = db; }
    float getGainDb() const noexcept { return gainDb; }

private:
    float gainDb = 0.0f;
};
