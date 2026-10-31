/*
  ==============================================================================

    ImgButton.h
    Created: 30 Apr 2025 3:18:17am
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================
/*
*/
class BypassButton  : public juce::Component
{
public:
    BypassButton(const juce::String& name, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const int& size)
: attachment(apvts, parameterID.getParamID(), button)
    {
        if (!bypassBtnImg.isValid())
            return;
        button.setClickingTogglesState(true);
        button.setImages(false, true, true,
                         bypassBtnImg, 0.8f, juce::Colours::white,
                         bypassBtnImg, 0.8f, juce::Colours::white.withAlpha(0.8f),
                         bypassBtnImg, 0.8f, juce::Colours::red.withAlpha(0.7f),
                               0.f);

        button.setBounds(0, 0, size, size);
        addAndMakeVisible(button);
    }
    ~BypassButton() override = default;

private:
    const juce::Image bypassBtnImg {juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize)};
    juce::ImageButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BypassButton)
};
