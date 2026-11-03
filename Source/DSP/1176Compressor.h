#pragma once
#include <JuceHeader.h>

class FET1176Compressor
{
public:
    void prepare(double newSampleRate);
    void reset();

    float getInputGainDB(float) const;
    float getOutputGainDB(float) const;
    void setParameters(float inputGain, float outputGain, float attack, float release, float ratio, float mix);
    void process(juce::AudioBuffer<float>& buffer);
    float revASaturation(float x, float drive);

    float getGainReductionDB() const {return grEnv;}


    private:
    float sampleRate = 44100.f;

//    Gain Reduction Meter
    float grEnv = 0.0f;
    float grAttackCoeff = 0.0f;
    float grReleaseCoeff = 0.0f;

//    Params
    float inputGain = 1.f;
    float outputGain = 1.f;
    float attackCoeff = 0.f;
    float releaseCoeff = 0.f;
    float ratio = 4.f;
    float mix = 1.f;
//    float gainReduction = 0.f;
    
//    Enveloper Follower
    float env = 0.f;
    
//    Helper Funcs
    inline float dBToGain(float dB) {return std::pow(10.0f, dB/20.0f);}
    inline float gainToDB(float g) {return 20.f * std::log10(g);}
};
