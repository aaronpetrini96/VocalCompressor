/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessor::VocalCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
//      FLOAT PARAMETERS
    auto floatParams = std::array
    {
//        1176
        &ParamPointers::input1176Param,
        &ParamPointers::output1176Param,
        &ParamPointers::attack1176Param,
        &ParamPointers::release1176Param,
        &ParamPointers::mix1176Param,
        
//        LA2A
        &ParamPointers::gainLA2AParam,
        &ParamPointers::peakReductionLA2AParam,
    };
    
    auto floatNameFuncs = std::array
    {
//      1176
        &ParamNames::get1176InputName,
        &ParamNames::get1176OutputName,
        &ParamNames::get1176AttackName,
        &ParamNames::get1176ReleaseName,
        &ParamNames::get1176MixName,
        
//        LA2A
        &ParamNames::getLA2AGainName,
        &ParamNames::getLA2APeakReductionName,
    };
    
    initCachedParams<juce::AudioParameterFloat*>(floatParams, floatNameFuncs);
    
//    ADD TO REGISTRY
    for (auto param : floatParams)
    {
        if (*param != nullptr)
        {
            juce::String name = (*param)->paramID;
            smoothedParamsRegistry[name] = SmoothedParam{*param, juce::SmoothedValue<float>()};
        }
    }
    
//    CHOICE PARAMS
    auto choiceParams = std::array
    {
        &ParamPointers::ratio1176Param,
        
        
    };
    
    auto choiceNameFuncs = std::array
    {
        &ParamNames::get1176RatioName,
        
        
    };
    
    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceNameFuncs);
    
//    BOOL PARAMS
    auto boolParams = std::array
    {
        &ParamPointers::bypass1176Param,
        
        &ParamPointers::modeLA2AParam,
        &ParamPointers::bypassLA2AParam,
        
        &ParamPointers::bypassPluginParam,
        &ParamPointers::reverseSignalOrder,
    };
    
    auto boolNameFuncs = std::array
    {
        &ParamNames::get1176BypassName,
        
        &ParamNames::getLA2ALimitCompressModeName,
        &ParamNames::getLA2ABypassName,
        
        &ParamNames::getGeneralBypassName,
        &ParamNames::getReverseSignalOrderName,
    };
    
    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);
}


VocalCompressorAudioProcessor::~VocalCompressorAudioProcessor()
{
}


juce::AudioProcessorValueTreeState::ParameterLayout VocalCompressorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    const int versionHint = 1;

    juce::NormalisableRange<float> normalizedRange {0.f, 1.f, 0.01f, 1.f};
    
//    1176
    auto name = ParamNames::get1176InputName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.3f));
    name = ParamNames::get1176OutputName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.7f));
    
    // Attack: 0.02 ms – 0.8 ms  (20 µs – 800 µs)
    name = ParamNames::get1176AttackName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(0.02f, 0.8f, 0.0001f, 0.3f), 0.574f));
    // Release: 50 ms – 1100 ms
    name = ParamNames::get1176ReleaseName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(50.f, 1100.0f, 0.1f, 0.45f), 369.f));
    
    name = ParamNames::get1176RatioName();
    const juce::StringArray ratioArray_1176 {"4", "8", "12", "20", "ALL"};
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, ratioArray_1176, 0));
    
    name = ParamNames::get1176MixName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 1.0f));
    name = ParamNames::get1176BypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
    //    LA2A
    name = ParamNames::getLA2AGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.3f));
    name = ParamNames::getLA2APeakReductionName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.3f));
    name = ParamNames::getLA2ALimitCompressModeName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getLA2ABypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
//    GENERAL PUGIN PARAMETERS
    name = ParamNames::getGeneralBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
    name = ParamNames::getReverseSignalOrderName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
    return {params.begin(), params.end()};
}

//==============================================================================
const juce::String VocalCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocalCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocalCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocalCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocalCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocalCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VocalCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocalCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void VocalCompressorAudioProcessor::updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init)
{
    for (auto& [name, smoothedParam] : smoothedParamsRegistry)
    {
        if (init == SmootherUpdateMode::initialize)
            smoothedParam.smoother.setCurrentAndTargetValue(smoothedParam.param->get());
        else
            smoothedParam.smoother.setTargetValue(smoothedParam.param->get());
        
        smoothedParam.smoother.skip(numSamplesToSkip);
    }
}

//==============================================================================
void VocalCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fet1176.prepare(sampleRate, samplesPerBlock);
    la2aComp.prepare(sampleRate, samplesPerBlock);
//    for(auto smoother : getSmoothers())
//        smoother->reset(sampleRate, 0.05);
    //Reset all smoothers
    for (auto& pair : smoothedParamsRegistry)
        pair.second.smoother.reset(sampleRate, 0.05);
    
    //Initial sync
    updateSmootherFromParams(1, SmootherUpdateMode::initialize);
}

void VocalCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocalCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VocalCompressorAudioProcessor::processBlock1176(juce::AudioBuffer<float>& buffer)
{
    if (ParamPointers::bypass1176Param->get() == false)
    {
        float inDB = fet1176.getInputGainDB(getSmoothedValue(ParamNames::get1176InputName()));
        float outDB = fet1176.getOutputGainDB(getSmoothedValue(ParamNames::get1176OutputName()));
        float attack = getSmoothedValue(ParamNames::get1176AttackName());
        float release = getSmoothedValue(ParamNames::get1176ReleaseName());
        float mix = getSmoothedValue(ParamNames::get1176MixName());
        int ratioIndex = ParamPointers::ratio1176Param->getIndex();
        float ratio = (ratioIndex == 0) ? 4.f :
                      (ratioIndex == 1) ? 8.f :
                      (ratioIndex == 2) ? 12.f :
                      (ratioIndex == 3) ? 20.f : 100.f;

        fet1176.setParameters(inDB, outDB, attack, release, ratio, mix);
        fet1176.process(buffer);
        gR1176 = fet1176.getGainReductionDB();
    }
    else
    {
        gR1176 = 0.f;
    }
}

void VocalCompressorAudioProcessor::processBlockLA2A(juce::AudioBuffer<float>& buffer)
{
    if (ParamPointers::bypassLA2AParam->get() == false)
    {
        float normGain = getSmoothedValue(ParamNames::getLA2AGainName());
        float normPeakReduction = getSmoothedValue(ParamNames::getLA2APeakReductionName());
        bool limitMode = ParamPointers::modeLA2AParam->get();
        la2aComp.setParameters(normGain, normPeakReduction, limitMode);
        la2aComp.process(buffer);
        grLA2A = la2aComp.getGainReduction();
    }
    else
    {
        grLA2A = 0.f;
    }
}


void VocalCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    [[maybe_unused]] auto totalNumInputChannels  = getTotalNumInputChannels();
    [[maybe_unused]] auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    //Update Smoothers in Real Time
    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    
    if (ParamPointers::bypassPluginParam->get() == false)
    {
        if (ParamPointers::reverseSignalOrder->get() == false)
        {
            processBlock1176(buffer);
            processBlockLA2A(buffer);
        }
        else
        {
            processBlockLA2A(buffer);
            processBlock1176(buffer);
        }
    }
}


//==============================================================================
bool VocalCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VocalCompressorAudioProcessor::createEditor()
{
    return new VocalCompressorAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void VocalCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(state);
    }
}

void VocalCompressorAudioProcessor::savePreset(const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml) { xml->writeTo(file);}
}

void VocalCompressorAudioProcessor::loadPreset(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        if (auto* presetNameProp = state.getPropertyPointer("CurrentPresetName"))
            currentPresetName = presetNameProp->toString(); // restore preset name
        apvts.replaceState(state);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalCompressorAudioProcessor();
}
