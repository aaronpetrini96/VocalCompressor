#pragma once
#include <JuceHeader.h>

namespace ParamPointers
{
    //======================= PARAMETERS PTRS =======================
    inline juce::AudioParameterFloat*  input1176Param      = nullptr;
    inline juce::AudioParameterFloat*  output1176Param     = nullptr;
    inline juce::AudioParameterFloat*  attack1176Param     = nullptr;
    inline juce::AudioParameterFloat*  release1176Param    = nullptr;
    inline juce::AudioParameterFloat*  mix1176Param        = nullptr;
    inline juce::AudioParameterChoice* ratio1176Param      = nullptr;
    inline juce::AudioParameterBool*   bypass1176Param     = nullptr;

    // LA2A PARAMETERS
    inline juce::AudioParameterFloat*  gainLA2AParam           = nullptr;
    inline juce::AudioParameterFloat*  peakReductionLA2AParam  = nullptr;
    inline juce::AudioParameterBool*   modeLA2AParam           = nullptr;
    inline juce::AudioParameterBool*   bypassLA2AParam         = nullptr;

    inline juce::AudioParameterBool*   bypassPluginParam         = nullptr;
    inline juce::AudioParameterBool*   reverseSignalOrder         = nullptr;


    //======================= SMOOTHED PARAMS =======================
    inline juce::SmoothedValue<float> input1176ParamSmoother;
    inline juce::SmoothedValue<float> output1176ParamSmoother;
    inline juce::SmoothedValue<float> attack1176ParamSmoother;
    inline juce::SmoothedValue<float> release1176ParamSmoother;
    inline juce::SmoothedValue<float> mix1176ParamSmoother;
    inline juce::SmoothedValue<float> gainLA2AParamSmoother;
    inline juce::SmoothedValue<float> peakReductionLA2AParamSmoother;
}


