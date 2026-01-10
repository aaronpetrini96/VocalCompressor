#include "1176Compressor.h"

void FET1176Compressor::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = static_cast<float>(newSampleRate);
    reset();
    
    // 1176-style fast meter ballistics
    grAttackCoeff  = std::exp(-1.f / (0.015f * sampleRate)); // ~15 ms
    grReleaseCoeff = std::exp(-1.f / (0.20f  * sampleRate)); // ~200 ms
    
    //4x oversampling, polyphase IIR
    oversampler.reset(new juce::dsp::Oversampling<float>(1, //num channels to be processed at once
                                                      2, //oversampling factor 2^2 = 4x
                                                      juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR));
    oversampler->initProcessing(samplesPerBlock);
                                                         
}
void FET1176Compressor::reset()
{
    env = 0.f;
    grEnv = 0.f;
}


float FET1176Compressor::getInputGainDB(float normValue)
{
    // reverse mapping: at 0 → -inf (or -40dB floor), at 1 → +24dB
    inGainNorm = normValue;
    if (normValue < 0.01f)
        return -40.0f;
    return juce::jmap(normValue, 0.0f, 1.0f, -20.0f, 24.0f);
}
float FET1176Compressor::getOutputGainDB(float normValue)
{
    outGainNorm = normValue;
    // more linear, from -20 to +20 dB
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

//void FET1176Compressor::process(juce::AudioBuffer<float>& buffer)
//{
//    const int numChannels = buffer.getNumChannels();
//    const int numSamples = buffer.getNumSamples();
//    juce::AudioBuffer<float> dryBuffer;
//    dryBuffer.makeCopyOf(buffer);
//    
//    for (int n = 0; n < numSamples; ++n)
//    {
//        float detector = 0.f;
//        for (int ch = 0; ch < numChannels; ++ch)
//        {
//            float x = buffer.getSample(ch, n) * inputGain; //input affects detection
////            buffer.setSample(ch, n, x);
//            detector += std::abs(x);
//        }
//        detector /= static_cast<float>(numChannels);
//        
////        Envelope Filter
////    y[n]=a⋅y[n−1]+(1−a)⋅x[n]   with  coeff a =e^(−1/(T⋅fs​))
//        if (detector > env)
//            env = attackCoeff * env + (1.0f - attackCoeff) * detector;
//        else
//            env = releaseCoeff * env + (1.0f - releaseCoeff) * detector;
//        
////        convert to dB
//        float levelDB = gainToDB(env);
//        float gainDB = 0.f;
//        
//        if (levelDB > -24.f)
//        {
//            float over = levelDB + 24.f;
//            gainDB = -over * (1.f - (1.f / ratio));
//        }
//        
//        float gain = dBToGain(gainDB);
//        
//        float gainReductionDB = -gainDB;
//        
////      Meter smoothing
//        if (gainReductionDB > grEnv)
//            grEnv = grAttackCoeff * grEnv + (1.f - grAttackCoeff) * gainReductionDB;
//        else
//            grEnv = grReleaseCoeff * grEnv + (1.f - grReleaseCoeff) * gainReductionDB;
//
//
//        for (int ch = 0; ch < numChannels; ++ch)
//        {
//            float x = buffer.getSample(ch, n) * inputGain;
//            float y = x * gain; // compressor gain - Dynamics First
//            float satDrive = 1.0f + grEnv * 0.06f; //Drive depende de GR
//            y = revASaturation(y, satDrive);
//            
//            y *= outputGain;
//            buffer.setSample(ch, n, y);
//        }
//    }
//    
////    Apply Mix
//    
//    if (mix < 1.f)
//    {
//        for (int ch = 0; ch < numChannels; ++ch)
//        {
//            const float* dry = dryBuffer.getReadPointer(ch);
//            float* wet = buffer.getWritePointer(ch);
//            
//            for (int i = 0; i < numSamples; ++i)
//                wet[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
//        }
//    }
//}

void FET1176Compressor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
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
            float x = buffer.getSample(ch, n) * inputGain;
            float y = x * gain;
            
            float satDrive = 1.0f + grEnv * 0.06f;
            if (allButtonsIn)
                satDrive *= 1.4f;
            
//            y = revASaturation(y, satDrive);
            y = saturateOS(y, satDrive);
            
            y *= outputGain;
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

float FET1176Compressor::saturateOS(float x, float drive)
{
    oversamplingBuffer.setSize(1, 1);
    oversamplingBuffer.setSample(0, 0, x);
    
    juce::dsp::AudioBlock<float> block (oversamplingBuffer);
    auto oversampledBlock = oversampler->processSamplesUp(block);
    
    float* samples = oversampledBlock.getChannelPointer(0);
    int numOs = (int) oversampledBlock.getNumSamples();
    
    for (int i = 0; i < numOs; ++i)
        samples [i] = revASaturation(samples[i], drive);
    
    oversampler->processSamplesDown(block);
    
    return oversamplingBuffer.getSample(0, 0);
}
