#include "1176Compressor.h"

void FET1176Compressor::prepare(double newSampleRate)
{
    sampleRate = static_cast<float>(newSampleRate);
    reset();
    // control envelope ballistics
    controlAttackCoeff  = std::exp(-1.f / (0.015f * sampleRate)); // ~15 ms
    controlReleaseCoeff = std::exp(-1.f / (0.20f  * sampleRate)); // ~200 ms
}
void FET1176Compressor::reset()
{
    env = 0.f;
    controlEnvDb = 0.f;
    gainReductionDb = 0.f;
}

float FET1176Compressor::taperedKnobToDB (float normValue, float minDB, float maxDB, float taper)
{
    const auto curvedPosition = 1.f - std::pow (1.f - normValue, taper);
    return juce::jmap (curvedPosition, minDB, maxDB);
}

float FET1176Compressor::knobToTimeMs (float normValue, float slowestMs, float fastestMs)
{
    return slowestMs * std::pow (fastestMs / slowestMs, normValue);
}

void FET1176Compressor::setParameters(float inputNormValue, float outputNormValue, float attackNormValue, float releaseNormValue, float newRatio, float newMix)
{
    for ([[maybe_unused]] const auto normValue : {inputNormValue, outputNormValue, attackNormValue, releaseNormValue})
        jassert (juce::isPositiveAndNotGreaterThan (normValue, 1.f));

    inputGain = dBToGain(taperedKnobToDB (inputNormValue, -22.f, 32.f, 1.6f));
    outputGain = dBToGain(taperedKnobToDB (outputNormValue, -84.f, 16.f, 2.4f));
    ratio = newRatio;
    mix = newMix;

    const auto attackMs = knobToTimeMs (attackNormValue, 0.8f, 0.02f);
    const auto releaseMs = knobToTimeMs (releaseNormValue, 1100.f, 50.f);
//  coeff a =e^(−1/(T⋅fs​))
    attackCoeff = std::exp(-1.f / (attackMs * 0.001f * sampleRate));
    releaseCoeff = std::exp(-1.f / (releaseMs * 0.001f  * sampleRate));
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
            const auto speedUpAmountDb = juce::jlimit(0.f, 20.f, controlEnvDb);
            const auto attackSpeedUp = juce::jmap(speedUpAmountDb, 0.f, 20.f, 1.f, 2.86f);
            attack = std::pow(attackCoeff, attackSpeedUp);
        }
        
        if (detector > env)
            env = attack * env + (1.0f - attack) * detector;
        else
            env = releaseCoeff * env + (1.0f - releaseCoeff) * detector;
        
        //dB Level
        float levelDb = gainToDB(env);
        
        //Threshold
        float effectiveThreshold = -12.f;
        if (allButtonsIn)
            effectiveThreshold -= controlEnvDb * 0.25f;
        
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
        
        //Control envelope
        gainReductionDb = -gainDb;

        if (gainReductionDb > controlEnvDb)
            controlEnvDb = controlAttackCoeff * controlEnvDb + (1.f - controlAttackCoeff) * gainReductionDb;
        else
            controlEnvDb = controlReleaseCoeff * controlEnvDb + (1.f - controlReleaseCoeff) * gainReductionDb;

        float satDrive = 1.0f + controlEnvDb * 0.06f;
        if (allButtonsIn)
            satDrive *= 1.4f;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float dry = buffer.getSample(ch, n);
            float x = dry * inputGain;
            float y = x * gain;

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
    float y = std::tanh(k * x) / k;
    return y;
}

