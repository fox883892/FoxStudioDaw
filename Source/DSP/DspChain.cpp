#include "DspChain.h"

DspChain::DspChain() = default;
DspChain::~DspChain() = default;

void DspChain::prepare(const juce::dsp::ProcessSpec& s)
{
    spec = s;
    for (auto& p : processors)
        p->prepare(s);
    prepared = true;
}

void DspChain::reset()
{
    for (auto& p : processors)
        p->reset();
}

void DspChain::addProcessor(std::unique_ptr<DspProcessor> processor)
{
    if (processor != nullptr)
        processors.push_back(std::move(processor));
}

void DspChain::process(juce::AudioBuffer<float>& buffer)
{
    if (!prepared)
        return;

    for (auto& p : processors)
    {
        if (p != nullptr && p->isEnabled())
            p->process(buffer);
    }
}
