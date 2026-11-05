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

    float getGainReductionDB() const {return gainReductionDb;}


    private:
    float sampleRate = 44100.f;

//    Control Envelope (drives ALL-mode threshold, attack speed-up and saturation drive)
    float controlEnvDb = 0.0f;
    float controlAttackCoeff = 0.0f;
    float controlReleaseCoeff = 0.0f;
    float gainReductionDb = 0.0f;

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
    inline float gainToDB(float g) {return 20.f * std::log10(std::max(g, 1.0e-6f));}
    static float taperedKnobToDB (float normValue, float minDB, float maxDB, float taper);
};
