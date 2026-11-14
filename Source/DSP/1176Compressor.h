#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include "../Utilities/AADspTools.h"

class FET1176Compressor
{
public:
    void prepare(double newSampleRate);
    void reset();

    // Input, output, attack and release are expected normalised 0-1
    void setParameters(float inputNormValue, float outputNormValue, float attackNormValue, float releaseNormValue, float ratio, float mix);
    void process(juce::AudioBuffer<float>& buffer);
    float getGainReductionDb() const {return peakGainReductionDb;}

private:
//    Set in prepare
    float sampleRate = 44100.f;
    float controlAttackCoeff = 0.0f;
    float controlReleaseCoeff = 0.0f;

//    Set in setParameters
    float inputGain = 1.f;
    float outputGain = 1.f;
    float attackCoeff = 0.f;
    float releaseCoeff = 0.f;
    float ratio = 4.f;
    float mix = 1.f;

//    State (changes while processing)
    float env = 0.f;
//    Control Envelope (drives ALL-mode threshold and attack speed-up)
    float controlEnvDb = 0.0f;
    float gainReductionDb = 0.0f;
    float peakGainReductionDb = 0.0f;
    AADspTools::AsymmetricSoftClipper asymClipper;
    std::array<AADspTools::DCBlocker, 2> dcBlockers;

//    Helper Funcs
    static float taperedKnobToDB (float normValue, float minDB, float maxDB, float taper);
    static float knobToTimeMs (float normValue, float slowestMs, float fastestMs);
};
