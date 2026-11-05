#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

#include "Utilities/ParamNames.h"
#include "Utilities/AADspTools.h"
#include "Utilities/PresetManager.h"
#include "Utilities/AAUndoManager.h"
#include "DSP/1176Compressor.h"
#include "DSP/LA2ACompressor.h"

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
    
    //======================= PARAMETER LAYOUT =======================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

    //======================= PRESETS =======================
    AAUndoManager undoManager;
    PresetManager presetManager {apvts, undoManager};


    
    
    std::atomic<float> gR1176 {0.f}, grLA2A {0.f};
    
private:
    FET1176Compressor fet1176;
    LA2ACompressor la2aComp;

    AADspTools::SmoothedBypass bypass1176, bypassLA2A, pluginBypass;

    bool isOrderReversed {false};
    int orderSwitchSamplesLeft {0};


    juce::AudioParameterFloat* input1176Param {nullptr};
    juce::AudioParameterFloat* output1176Param {nullptr};
    juce::AudioParameterFloat* attack1176Param {nullptr};
    juce::AudioParameterFloat* release1176Param {nullptr};
    juce::AudioParameterFloat* mix1176Param {nullptr};
    juce::AudioParameterChoice* ratio1176Param {nullptr};
    juce::AudioParameterBool* bypass1176Param {nullptr};
    juce::AudioParameterFloat* gainLA2AParam {nullptr};
    juce::AudioParameterFloat* peakReductionLA2AParam {nullptr};
    juce::AudioParameterBool* modeLA2AParam {nullptr};
    juce::AudioParameterBool* bypassLA2AParam {nullptr};
    juce::AudioParameterBool* pluginBypassParam {nullptr};
    juce::AudioParameterBool* reverseSignalOrderParam {nullptr};

    juce::SmoothedValue<float>
    input1176ParamSmoother,
    output1176ParamSmoother,
    attack1176ParamSmoother,
    release1176ParamSmoother,
    mix1176ParamSmoother,
    gainLA2AParamSmoother,
    peakReductionLA2AParamSmoother;

    std::vector<std::pair<juce::AudioParameterFloat*, juce::SmoothedValue<float>*>> smootherBindings;
    
    
    
    
    
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

    void linkParameters();

    enum class SmootherUpdateMode
    {
        initialize,
        liveInRealTime
    };

    void updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessor)
};
