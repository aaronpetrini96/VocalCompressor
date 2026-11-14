#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessor::VocalCompressorAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    linkParameters();
    undoManager.trackParameters(*this);
    PresetManager::installFactoryPresets();
    presetManager.setCurrentPresetName("Default");
}


juce::AudioProcessorValueTreeState::ParameterLayout VocalCompressorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    constexpr int versionHint = 1;
    juce::NormalisableRange<float> normalizedRange {0.f, 1.f, 0.01f, 1.f};
    juce::NormalisableRange<float> timeKnobRange {1.f, 7.f, 0.01f};
    
//    1176
    auto name = ParamNames::get1176InputName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.3f));
    name = ParamNames::get1176OutputName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 0.5f));
    
    name = ParamNames::get1176AttackName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, timeKnobRange, 4.f));
    name = ParamNames::get1176ReleaseName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, timeKnobRange, 5.f));
    
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
    name = ParamNames::getLA2AMixName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, normalizedRange, 1.0f));
    name = ParamNames::getLA2ABypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
//    GENERAL PLUGIN PARAMETERS
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
    return false;
}

bool VocalCompressorAudioProcessor::producesMidi() const
{
    return false;
}

bool VocalCompressorAudioProcessor::isMidiEffect() const
{
    return false;
}

double VocalCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalCompressorAudioProcessor::getNumPrograms()
{
    return 1;
}

int VocalCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalCompressorAudioProcessor::setCurrentProgram (int)
{
}

const juce::String VocalCompressorAudioProcessor::getProgramName (int)
{
    return {};
}

void VocalCompressorAudioProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================

void VocalCompressorAudioProcessor::linkParameters()
{
    input1176Param = getParam<juce::AudioParameterFloat>(ParamNames::get1176InputName());
    output1176Param = getParam<juce::AudioParameterFloat>(ParamNames::get1176OutputName());
    attack1176Param = getParam<juce::AudioParameterFloat>(ParamNames::get1176AttackName());
    release1176Param = getParam<juce::AudioParameterFloat>(ParamNames::get1176ReleaseName());
    mix1176Param = getParam<juce::AudioParameterFloat>(ParamNames::get1176MixName());
    ratio1176Param = getParam<juce::AudioParameterChoice>(ParamNames::get1176RatioName());
    bypass1176Param = getParam<juce::AudioParameterBool>(ParamNames::get1176BypassName());

    gainLA2AParam = getParam<juce::AudioParameterFloat>(ParamNames::getLA2AGainName());
    peakReductionLA2AParam = getParam<juce::AudioParameterFloat>(ParamNames::getLA2APeakReductionName());
    modeLA2AParam = getParam<juce::AudioParameterBool>(ParamNames::getLA2ALimitCompressModeName());
    mixLA2AParam = getParam<juce::AudioParameterFloat>(ParamNames::getLA2AMixName());
    bypassLA2AParam = getParam<juce::AudioParameterBool>(ParamNames::getLA2ABypassName());

    pluginBypassParam = getParam<juce::AudioParameterBool>(ParamNames::getGeneralBypassName());
    reverseSignalOrderParam = getParam<juce::AudioParameterBool>(ParamNames::getReverseSignalOrderName());

    smootherBindings =
    {
        {input1176Param, &input1176ParamSmoother},
        {output1176Param, &output1176ParamSmoother},
        {attack1176Param, &attack1176ParamSmoother},
        {release1176Param, &release1176ParamSmoother},
        {mix1176Param, &mix1176ParamSmoother},
        {gainLA2AParam, &gainLA2AParamSmoother},
        {peakReductionLA2AParam, &peakReductionLA2AParamSmoother},
        {mixLA2AParam, &mixLA2AParamSmoother}
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
    const auto processingRate = sampleRate * AADspTools::Oversampler2x::factor;
    const auto processingBlockSize = samplesPerBlock * AADspTools::Oversampler2x::factor;

    fet1176.prepare(processingRate);
    la2aComp.prepare(processingRate);

    const auto numChannels = std::max (getTotalNumInputChannels(), getTotalNumOutputChannels());
    bypass1176.prepare (processingRate, processingBlockSize, numChannels, bypass1176Param->get());
    bypassLA2A.prepare (processingRate, processingBlockSize, numChannels, bypassLA2AParam->get());
    pluginBypass.prepare (processingRate, processingBlockSize, numChannels, pluginBypassParam->get());
    isOrderReversed = reverseSignalOrderParam->get();
    orderFadeLength = std::max(1, static_cast<int>(processingRate * 0.02));
    orderFadeSamplesLeft = 0;
    orderFadeBuffer.setSize(numChannels, processingBlockSize);
    oversampler.prepare(sampleRate, samplesPerBlock, numChannels);
    setLatencySamples(oversampler.getLatencySamples());

    //Reset all smoothers
    for (const auto& [param, smoother] : smootherBindings)
        smoother->reset(sampleRate, 0.05);
    
    //Initial sync
    updateSmootherFromParams(1, SmootherUpdateMode::initialize);
}

void VocalCompressorAudioProcessor::releaseResources()
{
}

bool VocalCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void VocalCompressorAudioProcessor::processBlock1176(juce::AudioBuffer<float>& buffer, FET1176Compressor& compressor)
{
    const auto isBypassed = bypass1176Param->get();

    bypass1176.setBypassed(isBypassed);
    if (bypass1176.isFullyBypassed())
    {
        compressor.reset();
        gR1176.store (0.f);
        return;
    }

    const float inputNormValue = input1176ParamSmoother.getCurrentValue();
    const float outputNormValue = output1176ParamSmoother.getCurrentValue();
    const float attackNormValue = attack1176Param->convertTo0to1(attack1176ParamSmoother.getCurrentValue());
    const float releaseNormValue = release1176Param->convertTo0to1(release1176ParamSmoother.getCurrentValue());
    const float mix = mix1176ParamSmoother.getCurrentValue();
    static constexpr std::array ratios1176 {4.f, 8.f, 12.f, 20.f, 100.f};
    const float ratio = ratios1176[static_cast<size_t>(ratio1176Param->getIndex())];

    compressor.setParameters(inputNormValue, outputNormValue, attackNormValue, releaseNormValue, ratio, mix);
    bypass1176.process(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(),
                       [&compressor] (float* const* chunkData, int chunkChannels, int chunkSamples)
                       {
                           juce::AudioBuffer<float> chunkBuffer (chunkData, chunkChannels, chunkSamples);
                           compressor.process(chunkBuffer);
                       });

    gR1176.store (isBypassed ? 0.f : compressor.getGainReductionDB());
}

void VocalCompressorAudioProcessor::processBlockLA2A(juce::AudioBuffer<float>& buffer, LA2ACompressor& compressor)
{
    const auto isBypassed = bypassLA2AParam->get();

    bypassLA2A.setBypassed(isBypassed);
    if (bypassLA2A.isFullyBypassed())
    {
        compressor.reset();
        grLA2A.store (0.f);
        return;
    }

    float normGain = gainLA2AParamSmoother.getCurrentValue();
    float normPeakReduction = peakReductionLA2AParamSmoother.getCurrentValue();
    bool limitMode = modeLA2AParam->get();
    float mix = mixLA2AParamSmoother.getCurrentValue();
    compressor.setParameters(normGain, normPeakReduction, limitMode, mix);
    bypassLA2A.process(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples(),
                       [&compressor] (float* const* chunkData, int chunkChannels, int chunkSamples)
                       {
                           juce::AudioBuffer<float> chunkBuffer (chunkData, chunkChannels, chunkSamples);
                           compressor.process(chunkBuffer);
                       });

    grLA2A.store (isBypassed ? 0.f : compressor.getGainReduction());
}

void VocalCompressorAudioProcessor::processChain(juce::AudioBuffer<float>& buffer, FET1176Compressor& compressor1176, LA2ACompressor& compressorLA2A, const bool isReversed)
{
    if (isReversed)
    {
        processBlockLA2A(buffer, compressorLA2A);
        processBlock1176(buffer, compressor1176);
        return;
    }

    processBlock1176(buffer, compressor1176);
    processBlockLA2A(buffer, compressorLA2A);
}

void VocalCompressorAudioProcessor::processOrderCrossfade(juce::AudioBuffer<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    juce::AudioBuffer<float> newOrderBuffer(orderFadeBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (int ch {0}; ch < numChannels; ++ch)
        newOrderBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    processChain(buffer, fet1176, la2aComp, isOrderReversed);
    processChain(newOrderBuffer, fet1176Next, la2aNext, ! isOrderReversed);

    const auto fadedSamples = orderFadeLength - orderFadeSamplesLeft;
    for (int n {0}; n < numSamples; ++n)
    {
        const auto newOrderGain = std::min(1.f, static_cast<float>(fadedSamples + n + 1) / static_cast<float>(orderFadeLength));
        for (int ch {0}; ch < numChannels; ++ch)
        {
            const auto oldOrderSample = buffer.getSample(ch, n);
            buffer.setSample(ch, n, oldOrderSample + (newOrderBuffer.getSample(ch, n) - oldOrderSample) * newOrderGain);
        }
    }

    orderFadeSamplesLeft -= numSamples;
    if (orderFadeSamplesLeft <= 0)
    {
        orderFadeSamplesLeft = 0;
        fet1176 = fet1176Next;
        la2aComp = la2aNext;
        isOrderReversed = ! isOrderReversed;
    }
}


void VocalCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    constexpr int subBlockSize = 32; // Parameters update once per sub-block, so smoothing stays inaudible at any host buffer size
    for (int blockStart {0}; blockStart < buffer.getNumSamples(); blockStart += subBlockSize)
    {
        const auto blockSize = std::min (subBlockSize, buffer.getNumSamples() - blockStart);
        juce::AudioBuffer<float> subBlock (buffer.getArrayOfWritePointers(), buffer.getNumChannels(), blockStart, blockSize);
        updateSmootherFromParams (blockSize, SmootherUpdateMode::liveInRealTime);

        const auto shouldReverseOrder = reverseSignalOrderParam->get();
        if (shouldReverseOrder != isOrderReversed && orderFadeSamplesLeft == 0)
        {
            fet1176Next = fet1176;
            la2aNext = la2aComp;
            orderFadeSamplesLeft = orderFadeLength;
        }

        pluginBypass.setBypassed(pluginBypassParam->get());

        const auto upsampledData = oversampler.upsample(subBlock.getArrayOfReadPointers(), subBlock.getNumChannels(), blockSize);
        juce::AudioBuffer<float> upsampledBlock(upsampledData, subBlock.getNumChannels(), blockSize * AADspTools::Oversampler2x::factor);

        if (pluginBypass.isFullyBypassed())
        {
            fet1176.reset();
            la2aComp.reset();
            gR1176.store(0.f);
            grLA2A.store(0.f);
            isOrderReversed = shouldReverseOrder;
            orderFadeSamplesLeft = 0;
        }
        else
        {
            pluginBypass.process(upsampledBlock.getArrayOfWritePointers(),
                                 upsampledBlock.getNumChannels(),
                                 upsampledBlock.getNumSamples(),
                                 [this](float* const* chunkData, int chunkChannels, int chunkSamples)
                                 {
                                     juce::AudioBuffer<float> chunkBuffer(chunkData, chunkChannels, chunkSamples);

                                     if (orderFadeSamplesLeft == 0)
                                         processChain(chunkBuffer, fet1176, la2aComp, isOrderReversed);
                                     else
                                         processOrderCrossfade(chunkBuffer);
                                 });
        }

        oversampler.downsample(subBlock.getArrayOfWritePointers(), subBlock.getNumChannels(), blockSize);
    }
}

juce::AudioProcessorParameter* VocalCompressorAudioProcessor::getBypassParameter() const
{
    return pluginBypassParam;
}


//==============================================================================
bool VocalCompressorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VocalCompressorAudioProcessor::createEditor()
{
    return new VocalCompressorAudioProcessorEditor (*this);
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
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        const auto restoredPresetName = state.getProperty("CurrentPresetName").toString();
        const auto restoredPresetModified = static_cast<bool>(state.getProperty("PresetModified", false));
        state.removeAllProperties(nullptr);
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
