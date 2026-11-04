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
    linkParameters();
    undoManager.trackParameters(*this);
    PresetManager::installFactoryPresets();
    presetManager.setCurrentPresetName("Default");
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


void VocalCompressorAudioProcessor::linkParameters()
{
    auto floatParams = std::array
    {
        &input1176Param, &output1176Param, &attack1176Param, &release1176Param, &mix1176Param,
        &gainLA2AParam, &peakReductionLA2AParam
    };

    auto floatNameFuncs = std::array
    {
        &ParamNames::get1176InputName, &ParamNames::get1176OutputName, &ParamNames::get1176AttackName,
        &ParamNames::get1176ReleaseName, &ParamNames::get1176MixName,
        &ParamNames::getLA2AGainName, &ParamNames::getLA2APeakReductionName
    };

    initCachedParams<juce::AudioParameterFloat*>(floatParams, floatNameFuncs);

    auto choiceParams = std::array { &ratio1176Param };
    auto choiceNameFuncs = std::array { &ParamNames::get1176RatioName };

    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceNameFuncs);

    auto boolParams = std::array
    {
        &bypass1176Param, &modeLA2AParam, &bypassLA2AParam, &pluginBypassParam, &reverseSignalOrderParam
    };

    auto boolNameFuncs = std::array
    {
        &ParamNames::get1176BypassName, &ParamNames::getLA2ALimitCompressModeName, &ParamNames::getLA2ABypassName,
        &ParamNames::getGeneralBypassName, &ParamNames::getReverseSignalOrderName
    };

    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);

    smootherBindings =
    {
        {input1176Param, &input1176ParamSmoother},
        {output1176Param, &output1176ParamSmoother},
        {attack1176Param, &attack1176ParamSmoother},
        {release1176Param, &release1176ParamSmoother},
        {mix1176Param, &mix1176ParamSmoother},
        {gainLA2AParam, &gainLA2AParamSmoother},
        {peakReductionLA2AParam, &peakReductionLA2AParamSmoother}
    };
}

void VocalCompressorAudioProcessor::updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init)
{
    for (const auto& [param, smoother] : smootherBindings)
    {
        if (init == SmootherUpdateMode::initialize)
            smoother->setCurrentAndTargetValue(param->get());
        else
            smoother->setTargetValue(param->get());
        
        smoother->skip(numSamplesToSkip);
    }
}

//==============================================================================
void VocalCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fet1176.prepare(sampleRate);
    la2aComp.prepare(sampleRate);

    const auto numChannels = std::max (getTotalNumInputChannels(), getTotalNumOutputChannels());
    bypass1176.prepare (sampleRate, samplesPerBlock, numChannels, bypass1176Param->get());
    bypassLA2A.prepare (sampleRate, samplesPerBlock, numChannels, bypassLA2AParam->get());
    pluginBypass.prepare (sampleRate, samplesPerBlock, numChannels, pluginBypassParam->get());
    isOrderReversed = reverseSignalOrderParam->get();
    orderSwitchSamplesLeft = 0;

    //Reset all smoothers
    for (const auto& [param, smoother] : smootherBindings)
        smoother->reset(sampleRate, 0.05);
    
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
    const auto isBypassed = bypass1176Param->get();

    bypass1176.setBypassed(isBypassed);
    if (bypass1176.isFullyBypassed())
    {
        fet1176.reset();
        gR1176 = 0.f;
        return;
    }

    const float inDB = fet1176.getInputGainDB(input1176ParamSmoother.getCurrentValue());
    const float outDB = fet1176.getOutputGainDB(output1176ParamSmoother.getCurrentValue());
    const float attack = attack1176ParamSmoother.getCurrentValue();
    const float release = release1176ParamSmoother.getCurrentValue();
    const float mix = mix1176ParamSmoother.getCurrentValue();
    const int ratioIndex = ratio1176Param->getIndex();
    const float ratio = (ratioIndex == 0) ? 4.f :
                  (ratioIndex == 1) ? 8.f :
                  (ratioIndex == 2) ? 12.f :
                  (ratioIndex == 3) ? 20.f : 100.f;

    fet1176.setParameters(inDB, outDB, attack, release, ratio, mix);
    bypass1176.process(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(),
                       [this] (float* const* chunkData, int chunkChannels, int chunkSamples)
                       {
                           juce::AudioBuffer<float> chunkBuffer (chunkData, chunkChannels, chunkSamples);
                           fet1176.process(chunkBuffer);
                       });

    gR1176 = isBypassed ? 0.f : fet1176.getGainReductionDB();
}

void VocalCompressorAudioProcessor::processBlockLA2A(juce::AudioBuffer<float>& buffer)
{
    const auto isBypassed = bypassLA2AParam->get();

    bypassLA2A.setBypassed(isBypassed);
    if (bypassLA2A.isFullyBypassed())
    {
        la2aComp.reset();
        grLA2A = 0.f;
        return;
    }

    float normGain = gainLA2AParamSmoother.getCurrentValue();
    float normPeakReduction = peakReductionLA2AParamSmoother.getCurrentValue();
    bool limitMode = modeLA2AParam->get();
    la2aComp.setParameters(normGain, normPeakReduction, limitMode);
    bypassLA2A.process(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(),
                       [this] (float* const* chunkData, int chunkChannels, int chunkSamples)
                       {
                           juce::AudioBuffer<float> chunkBuffer (chunkData, chunkChannels, chunkSamples);
                           la2aComp.process(chunkBuffer);
                       });

    grLA2A = isBypassed ? 0.f : la2aComp.getGainReduction();
}


void VocalCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    [[maybe_unused]] auto totalNumInputChannels  = getTotalNumInputChannels();
    [[maybe_unused]] auto totalNumOutputChannels = getTotalNumOutputChannels();

    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime); //Update Smoothers in Real Time

    const auto shouldReverseOrder = reverseSignalOrderParam->get();
    if (shouldReverseOrder != isOrderReversed && orderSwitchSamplesLeft == 0)
        orderSwitchSamplesLeft = static_cast<int>(getSampleRate() * 0.05);

    const auto isSwitchingOrder = orderSwitchSamplesLeft > 0;
    pluginBypass.setBypassed(pluginBypassParam->get() || isSwitchingOrder);
    pluginBypass.process(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(),
                         [this] (float* const* chunkData, int chunkChannels, int chunkSamples)
                         {
                             juce::AudioBuffer<float> chunkBuffer (chunkData, chunkChannels, chunkSamples);

                             if (isOrderReversed)
                             {
                                 processBlockLA2A(chunkBuffer);
                                 processBlock1176(chunkBuffer);
                             }
                             else
                             {
                                 processBlock1176(chunkBuffer);
                                 processBlockLA2A(chunkBuffer);
                             }
                         });

    if (isSwitchingOrder)
    {
        orderSwitchSamplesLeft -= buffer.getNumSamples();
        if (orderSwitchSamplesLeft <= 0)
        {
            orderSwitchSamplesLeft = 0;
            isOrderReversed = shouldReverseOrder;
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
    state.setProperty("CurrentPresetName", presetManager.getCurrentPresetName(), nullptr);
    state.setProperty("PresetModified", presetManager.isPresetModified(), nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        const juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        const auto restoredPresetName = state.getProperty("CurrentPresetName").toString();
        const auto restoredPresetModified = static_cast<bool>(state.getProperty("PresetModified", false));
        apvts.replaceState(state);
        presetManager.setCurrentPresetName(restoredPresetName);
        presetManager.setPresetModified(restoredPresetModified);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalCompressorAudioProcessor();
}
