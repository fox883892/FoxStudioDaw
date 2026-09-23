#pragma once

#include "DspChain.h"

class EqProcessor final : public DspProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(juce::AudioBuffer<float>& buffer) override;

    void setLowShelfDb(float db) noexcept { lowShelfDb = db; }
    void setMidDb(float db) noexcept { midDb = db; }
    void setHighShelfDb(float db) noexcept { highShelfDb = db; }

private:
    float lowShelfDb = 0.0f;
    float midDb = 0.0f;
    float highShelfDb = 0.0f;
};
