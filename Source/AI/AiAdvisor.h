#pragma once

#include <juce_core/juce_core.h>

struct AIRecommendation
{
    juce::String summary;
    double gainDb = 0.0;
    double thresholdDb = -18.0;
    double limiterDb = -1.0;
    bool clippingWarning = false;
};

class AIAdvisor
{
public:
    AIRecommendation analyse(float rms,
                             float peak,
                             float lufs,
                             float spectralCentroid,
                             bool clippingDetected,
                             bool isPlaying) const
    {
        AIRecommendation rec;
        rec.gainDb = 0.0;
        rec.thresholdDb = -18.0;
        rec.limiterDb = -1.0;
        rec.clippingWarning = clippingDetected;

        if (peak > 0.85f)
        {
            rec.gainDb = -2.5;
            rec.thresholdDb = -12.0;
            rec.limiterDb = -1.5;
            rec.summary = "Reduce peaks and use tighter limiting.";
        }
        else if (lufs < -18.0f)
        {
            rec.gainDb = 2.0;
            rec.thresholdDb = -20.0;
            rec.limiterDb = -1.0;
            rec.summary = "Signal is quiet; add gentle gain and air.";
        }
        else if (isPlaying)
        {
            rec.gainDb = 0.5;
            rec.thresholdDb = -18.0;
            rec.limiterDb = -1.0;
            rec.summary = "Balanced mix; maintain current master bus shape.";
        }
        else
        {
            rec.summary = "Idle state; AI waiting for audio input.";
        }

        if (clippingDetected)
            rec.summary = "Clipping detected; lower gain and compress faster.";

        return rec;
    }
};
