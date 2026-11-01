/*
  ==============================================================================

    ImgButton.h
    Created: 30 Apr 2025 3:18:17am
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../VCLook.h"

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
                         bypassBtnImg, 0.8f, VCColors::bypassNormal,
                         bypassBtnImg, 0.8f, VCColors::bypassOver,
                         bypassBtnImg, 0.8f, VCColors::bypassOn,
                               0.f);

        button.setBounds(0, 0, size, size);
        addAndMakeVisible(button);
    }
    ~BypassButton() override = default;

    void resized() override
    {
        button.setBounds(getLocalBounds());
    }

private:
    const juce::Image bypassBtnImg {juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize)};
    juce::ImageButton button;
    juce::AudioProcessorValueTreeState::ButtonAttachment attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BypassButton)
};
