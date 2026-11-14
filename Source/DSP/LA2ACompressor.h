#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include "../Utilities/AADspTools.h"

class LA2ACompressor
{
public:
    LA2ACompressor() = default;
    
    void prepare(double sampleRate);
    void reset();
    void setParameters(float normGain, float normPeakReduction, bool limitMode, float newMix);
    void process(juce::AudioBuffer<float>& buffer);
    float getGainReductionDb() const {return gainReduction;}
    
private:
//    SAT
    AADspTools::AsymmetricSoftClipper asymClipper;
    std::array<AADspTools::DCBlocker, 2> dcBlockers;

//    params
    float attackCoeff  = 0.f;
    float relFastCoeff = 0.f;
    float relSlowCoeff = 0.f;
    float peakReduction = 0.f;
    float outputGain = 1.f;
    float ratio = 5.f;
    bool isLimit = false;
    float mix = 1.f;
    
//    Envelope/Detector
    float env = 0.f;
    float gainReduction = 0.f;
    
};
