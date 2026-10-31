#pragma once

#include <JuceHeader.h>

class LA2ACompressor
{
public:
    LA2ACompressor() = default;
    
    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void setParameters(float normGain, float normPeakReduction, bool limitMode);
    void process(juce::AudioBuffer<float>& buffer);
    
    float getGainReduction() const {return gainReduction;}
    
    
    private:
//    UTILITIES
    float dBToGain(float dB) const { return std::pow(10.0f, dB / 20.0f); }
    float gainToDB(float g) const { return 20.0f * std::log10(std::max(g, 1.0e-6f)); }
//    SAT
    float tubeSaturation(float x, float drive);
    
//    params
    float sampleRate = 44100.f;
    float attackCoeff  = 0.f;
    float relFastCoeff = 0.f;
    float relSlowCoeff = 0.f;
    float peakReduction = 0.f;
    float outputGain = 1.f;
    float ratio = 3.f;
    bool isLimit = false;
    
//    Envelope/Detector
    float env = 0.f;
    float gainReduction = 0.f;
    
};
