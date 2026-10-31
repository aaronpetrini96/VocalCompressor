#include "LA2ACompressor.h"

void LA2ACompressor::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = static_cast<float>(newSampleRate);
    attackCoeff  = std::exp(-1.0f / (0.01f * sampleRate));
    relFastCoeff = std::exp(-1.0f / (0.06f * sampleRate));
    relSlowCoeff = std::exp(-1.0f / (2.0f  * sampleRate));
    reset();
}

void LA2ACompressor::reset()
{
    env = 0.f;
    gainReduction = 0.f;
}

void LA2ACompressor::setParameters(float normGain, float normPeakReduction, bool limitMode)
{
    peakReduction = juce::jmap(normPeakReduction, 0.f, 1.f, 0.f, 40.f);
    
    outputGain = dBToGain(juce::jmap(normGain, 0.f, 1.f, -10.f, 40.f));
    
    isLimit = limitMode;
    ratio = isLimit ? 10.f : 3.f;
    
}

void LA2ACompressor::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int n = 0; n < numSamples; ++n)
    {
        //Level detection
        float detector = 0.f;
        for (int ch = 0; ch < numChannels; ++ch)
            detector += std::abs(buffer.getSample(ch, n));
        detector /= static_cast<float>(numChannels);
        
        // --- T4 Optical Cell Envelope ---

        // Program-dependent blend
        float releaseCoeff = juce::jmap(env, 0.0f, 1.0f, relFastCoeff, relSlowCoeff);
        if (detector > env)
            env = attackCoeff * env + (1.0f - attackCoeff) * detector;
        else
            env = releaseCoeff * env + (1.0f - releaseCoeff) * detector;
        
        // --- Gain Reduction ---
        float envDB = gainToDB(env + 1.0e-9f);
        float over = envDB + peakReduction - (-24.0f);
        float gainDB = 0.0f;
        
        if (over > 0.0f)
        {
            float knee = isLimit ? 2.0f : 6.0f;
            float compAmount = over / (over + knee);
            gainDB = -compAmount * (1.0f - 1.0f / ratio) * over;
        }
        
        float gain = dBToGain(gainDB);
        
        // --- Apply Gain & Tube Character ---
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float x = buffer.getSample(ch, n);
            float y = x * gain;
            y = tubeSaturation(y, 0.4f);
            buffer.setSample(ch, n, y * outputGain);
        }
        
        float targetGR =-gainDB;
        gainReduction = 0.9f * gainReduction + 0.1f * targetGR;
    }
}

float LA2ACompressor::tubeSaturation(float x, float drive)
{
    float k = juce::jmap(drive, 0.0f, 1.0f, 1.0f, 2.5f);
    return std::tanh(k * x);
}
