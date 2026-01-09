
#include "BypassButton.h"

BypassButton::BypassButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID, const int& size)
: attachment(apvts, parameterID.getParamID(), button)
{
    auto bypassBtnImg = juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize);
    button.setClickingTogglesState(true);
    button.setImages(false, true, true,
                     bypassBtnImg, 0.8f, juce::Colours::white,
                     bypassBtnImg, 0.8f, juce::Colours::white.withAlpha(0.8f),
                     bypassBtnImg, 0.8f, juce::Colours::red.withAlpha(0.7f),
                           0.f);

    button.setBounds(0, 0, size, size);
    addAndMakeVisible(button);
    
}

BypassButton::~BypassButton()
{
}

void BypassButton::paint (juce::Graphics& g)
{
}

void BypassButton::resized()
{
}

