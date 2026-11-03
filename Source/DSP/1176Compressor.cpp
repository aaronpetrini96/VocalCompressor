#include "1176Compressor.h"

void FET1176Compressor::prepare(double newSampleRate)
{
    sampleRate = static_cast<float>(newSampleRate);
    reset();
    // 1176-style fast meter ballistics
    grAttackCoeff  = std::exp(-1.f / (0.015f * sampleRate)); // ~15 ms
    grReleaseCoeff = std::exp(-1.f / (0.20f  * sampleRate)); // ~200 ms
}
void FET1176Compressor::reset()
{
    env = 0.f;
    grEnv = 0.f;
}


float FET1176Compressor::getInputGainDB(float normValue) const
{
    // reverse mapping: at 0 → -inf (or -40dB floor), at 1 → +24dB
    if (normValue < 0.01f)
        return -40.0f;
    return juce::jmap(normValue, 0.0f, 1.0f, -20.0f, 24.0f);
}
float FET1176Compressor::getOutputGainDB(float normValue) const
{
    // more linear, from -60 to +20 dB
    return juce::jmap(normValue, 0.0f, 1.0f, -60.0f, 20.0f);
}

void FET1176Compressor::setParameters(float inDB, float outDB, float attackTime, float releaseTime, float newRatio, float newMix)
{
    inputGain = dBToGain(inDB);
    outputGain = dBToGain(outDB);
    ratio = newRatio;
    mix = newMix;
    
//  coeff a =e^(−1/(T⋅fs​))
    attackCoeff = std::exp(-1.f / (attackTime * 0.001f * sampleRate));
    releaseCoeff = std::exp(-1.f / (releaseTime * 0.001f  * sampleRate));
}

void FET1176Compressor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const bool allButtonsIn = ratio > 20.f;
    
    for (int n = 0; n < numSamples; ++n)
    {
        float detector = 0.f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float x = buffer.getSample(ch, n) * inputGain;
            detector += std::abs(x);
        }
        detector /= static_cast<float>(numChannels);
        
        //Envelope Follower
        float attack = attackCoeff;
        
        if (allButtonsIn)
        {
            float speedUp = juce::jmap(grEnv, 0.f, 20.f, 1.f, 0.35f);
            attack = std::pow(attackCoeff, speedUp);
        }
        
        if (detector > env)
            env = attack * env + (1.0f - attack) * detector;
        else
            env = releaseCoeff * env + (1.0f - releaseCoeff) * detector;
        
        //dB Level
        float levelDb = gainToDB(env);
        
        //Threshold
        float effectiveThreshold = -24.f;
        if (allButtonsIn)
            effectiveThreshold -= grEnv * 0.25f;
        
        float gainDb = 0.f;
        
        if (levelDb > effectiveThreshold)
        {
            float over = levelDb - effectiveThreshold;
            
            if (!allButtonsIn)
                gainDb = -over * (1.f - (1.f / ratio));
            else
            {
                float dynamicRatio = juce::jmap(juce::jlimit(0.f, 20.f, over), 0.f, 20.f, 4.f, 20.f);
                
                gainDb = -over * (1.f - (1.f / dynamicRatio));
            }
        }
        
        float gain = dBToGain(gainDb);
        
        //GR meter
        float gainReductionDb = -gainDb;
        
        if (gainReductionDb > grEnv)
            grEnv = grAttackCoeff * grEnv + (1.f - grAttackCoeff) * gainReductionDb;
        else
            grEnv = grReleaseCoeff * grEnv + (1.f - grReleaseCoeff) * gainReductionDb;
        
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float dry = buffer.getSample(ch, n);
            float x = dry * inputGain;
            float y = x * gain;
            
            float satDrive = 1.0f + grEnv * 0.06f;
            if (allButtonsIn)
                satDrive *= 1.4f;
            
            y = revASaturation(y, satDrive);
            
            y *= outputGain;
            y = dry * (1.f - mix) + y * mix;
            buffer.setSample(ch, n, y);
        }
    }
}

float FET1176Compressor::revASaturation(float x, float drive)
{
    float k = juce::jlimit(1.0f, 6.0f, drive);
    float y = std::tanh(k * x) / std::tanh(k);
    return y;
}

