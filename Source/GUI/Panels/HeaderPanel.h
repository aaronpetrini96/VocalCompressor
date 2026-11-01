#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../../Utilities/PresetManagerV2.h"
#include "../VCLook.h"
#include "../Components/BypassButton.h"

class HeaderPanel : public juce::Component
{
public:
    explicit HeaderPanel(VocalCompressorAudioProcessor& processor)
    : presetManager(processor),
      bypassPluginButton("", processor.apvts, ParamNames::getGeneralBypassName(), 20),
      reverseSignalOrderButtonAttachment(processor.apvts, ParamNames::getReverseSignalOrderName(), reverseSignalOrderButton)
    {
        reverseSignalOrderButton.setColour(juce::TextButton::buttonColourId, VCColors::buttonBackground);
        reverseSignalOrderButton.setColour(juce::TextButton::buttonOnColourId, VCColors::buttonBackground);
        reverseSignalOrderButton.setColour(juce::TextButton::textColourOffId, VCColors::text);
        reverseSignalOrderButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        reverseSignalOrderButton.setClickingTogglesState(true);
        reverseSignalOrderButton.onStateChange = [this] { updateReverseSignalButtonText(); };
        updateReverseSignalButtonText();

        addAndMakeVisible(presetManager);
        addAndMakeVisible(bypassPluginButton);
        addAndMakeVisible(reverseSignalOrderButton);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();

        g.fillAll(VCColors::background);

        const auto screwSize = boundsFWidth * 0.0417f;
        const auto screwBounds = boundsF.withSize(screwSize, screwSize).withCentre({screwSize * 0.5f, boundsF.getCentreY()});
        g.drawImage(screwImage, screwBounds, juce::RectanglePlacement::centred);
        g.drawImage(screwImage, screwBounds.withRightX(boundsFWidth), juce::RectanglePlacement::centred);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        presetManager.setBounds(getLocalBounds());

        const auto bypassSize = boundsFWidth * 0.0333f;
        bypassPluginButton.setBounds(boundsF.withSize(bypassSize, bypassSize)
                                            .withPosition(boundsFWidth * 0.14f, boundsFHeight * 0.2f)
                                            .toNearestInt());

        reverseSignalOrderButton.setBounds(boundsF.withX(boundsFWidth * 0.7085f)
                                                  .withWidth(boundsFWidth * 0.2083f)
                                                  .withY(static_cast<float>(presetManager.saveBtnY))
                                                  .withHeight(static_cast<float>(presetManager.saveBtnHeight))
                                                  .toNearestInt());
    }

private:
    void updateReverseSignalButtonText()
    {
        reverseSignalOrderButton.setButtonText(reverseSignalOrderButton.getToggleState() ? "OPT-2A -> FET-76"
                                                                                         : "FET-76 -> OPT-2A");
    }

    const juce::Image screwImage {juce::ImageCache::getFromMemory(BinaryData::screw25x25_png, BinaryData::screw25x25_pngSize)};

    PresetManagerV2 presetManager;
    BypassButton bypassPluginButton;
    juce::TextButton reverseSignalOrderButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseSignalOrderButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
