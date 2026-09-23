#pragma once

#include <memory>
#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class DspProcessor
{
public:
    virtual ~DspProcessor() = default;
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void reset() = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void setEnabled(bool enabled) { this->enabled = enabled; }
    virtual bool isEnabled() const noexcept { return enabled; }

protected:
    bool enabled = true;
};

class DspChain
{
public:
    DspChain();
    ~DspChain();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void addProcessor(std::unique_ptr<DspProcessor> processor);
    void process(juce::AudioBuffer<float>& buffer);

private:
    std::vector<std::unique_ptr<DspProcessor>> processors;
    juce::dsp::ProcessSpec spec;
    bool prepared = false;
};
