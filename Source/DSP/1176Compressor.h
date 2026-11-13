#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class FET1176Compressor
{
public:
    void prepare(double newSampleRate);
    void reset();

    // Input, output, attack and release are expected normalised 0-1
    void setParameters(float inputNormValue, float outputNormValue, float attackNormValue, float releaseNormValue, float ratio, float mix);
    void process(juce::AudioBuffer<float>& buffer);

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
    
//    Envelope Follower
    float env = 0.f;
    
//    Helper Funcs
    float revASaturation(float x, float drive);
    float dBToGain(float dB) {return std::pow(10.0f, dB/20.0f);}
    float gainToDB(float g) {return 20.f * std::log10(std::max(g, 1.0e-6f));}
    static float taperedKnobToDB (float normValue, float minDB, float maxDB, float taper);
    static float knobToTimeMs (float normValue, float slowestMs, float fastestMs);
};
