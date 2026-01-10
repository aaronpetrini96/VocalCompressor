#pragma once

#include <JuceHeader.h>
#include <unordered_map>
#include <vector>
#include "ParamNames.h"
#include "ParamPointers.h"
#include "DSP/1176Compressor.h"
#include "DSP/LA2ACompressor.h"

struct SmoothedParam
{
    juce::AudioParameterFloat* param = nullptr;
    juce::SmoothedValue<float> smoother;
};

//==============================================================================
/**
*/
class VocalCompressorAudioProcessor  : public juce::AudioProcessor
{
    public:
    //==============================================================================
    VocalCompressorAudioProcessor();
    ~VocalCompressorAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock1176(juce::AudioBuffer<float>&);
    void processBlockLA2A(juce::AudioBuffer<float>&);
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //======================= PRESET MANAGER FUNCTIONS =======================
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void setCurrentPresetName(const juce::String& name) {currentPresetName = name;}
    juce::String getCurrentPresetName() const {return currentPresetName;}
    juce::String currentPresetName {""};
    
    //======================= PARAMETER LAYOUT =======================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    

    
    
    float gR1176{0.f}, grLA2A {0.f};
    bool allButtonsIn {false};
    
private:
    
    FET1176Compressor fet1176;
    LA2ACompressor la2aComp;
    
    
    
    
    
//  ======== PARAMETERS FUNCTIONS ========
    template<typename ParamType, typename Params, typename Funcs>
    void initCachedParams(Params paramsArray, Funcs funcsArray)
    {
        for (size_t i = 0 ; i < paramsArray.size(); ++i)
        {
            auto ptrToParamPtr = paramsArray[i];
            auto* found = dynamic_cast<ParamType>(apvts.getParameter(funcsArray[i]()));
            if (! found)
            {
                DBG("initCachedParams: failed to find param ID '" << funcsArray[i]() << "'");
                jassertfalse; // debug-only
                *ptrToParamPtr = nullptr;
            }
            else
            {
                *ptrToParamPtr = found;
            }
        }
    }

    enum class SmootherUpdateMode
    {
        initialize,
        liveInRealTime
    };

//    std::vector<juce::SmoothedValue<float>*> getSmoothers();
    void updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init);
    
    float getSmoothedValue(const juce::String& paramName) const
    {
        auto it = smoothedParamsRegistry.find(paramName);
        return it != smoothedParamsRegistry.end() ? it->second.smoother.getCurrentValue() : 0.f;
    }
    
    std::unordered_map<juce::String, SmoothedParam> smoothedParamsRegistry;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessor)
};
