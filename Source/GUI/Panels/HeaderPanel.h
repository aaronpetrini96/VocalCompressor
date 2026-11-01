#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../../Utilities/PresetManagerV2.h"
#include "../VCLook.h"
#include "../LAF/VCControlsLAF.h"
#include "../Components/BypassButton.h"

class HeaderPanel : public juce::Component
{
public:
    explicit HeaderPanel(VocalCompressorAudioProcessor& processor)
    : presetManager(processor),
      bypassPluginButton("", processor.apvts, ParamNames::getGeneralBypassName(), 20),
      reverseSignalOrderButtonAttachment(processor.apvts, ParamNames::getReverseSignalOrderName(), reverseSignalOrderButton)
    {
        reverseSignalOrderButton.setLookAndFeel(&controlsLAF);
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

        const auto screwSize = boundsF.getHeight() * 0.6f;
        const auto screwGap = screwSize * 0.5f;
        const auto screwBounds = boundsF.withSizeKeepingCentre(screwSize, screwSize);
        g.drawImage(screwImage, screwBounds.withX(screwGap), juce::RectanglePlacement::stretchToFit);
        g.drawImage(screwImage, screwBounds.withRightX(boundsFWidth - screwGap), juce::RectanglePlacement::stretchToFit);
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

    const juce::Image screwImage {juce::ImageCache::getFromMemory(BinaryData::screw_png, BinaryData::screw_pngSize)};

    VCControlsLAF controlsLAF;
    PresetManagerV2 presetManager;
    BypassButton bypassPluginButton;
    juce::TextButton reverseSignalOrderButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseSignalOrderButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
