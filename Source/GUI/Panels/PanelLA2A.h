#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/ImageKnob.h"
#include "../Components/GainReductionMeter.h"
#include "../Components/GRTextDisplay.h"
#include "../Components/BypassButton.h"

class PanelLA2A : public juce::Component
{
public:
    explicit PanelLA2A(VocalCompressorAudioProcessor& processor)
    : audioProcessor(processor)
    {
        for (auto* imageKnob : {&la2aGainDial, &la2aPeakReductionDial})
        {
            addAndMakeVisible(*imageKnob);
            imageKnob->setAlwaysOnTop(true);
        }

        la2aModeButton.setImages(false, true, true,
                                 la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                                 la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                                 la2aModeImageDOWN, 0.90f, juce::Colours::transparentBlack);
        la2aModeButton.setClickingTogglesState(true);
        la2aModeButton.setAlwaysOnTop(true);
        la2aModeButton.onStateChange = [this] { updateModeLabels(); };
        addAndMakeVisible(la2aModeButton);

        for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel, &grLA2ALabel})
        {
            VCLook::setupLabel(*label);
            addAndMakeVisible(*label);
        }
        for (auto* label : {&la2aCompModeLabel, &la2aLimitModeLabel})
        {
            VCLook::setupLabel(*label, 14.f);
            addChildComponent(*label);
        }
        updateModeLabels();

        addAndMakeVisible(grMeterLA2ALeft);
        addAndMakeVisible(grMeterLA2ARight);
        addAndMakeVisible(grTextDisplayLA2A);
        addAndMakeVisible(bypassButtonLA2A);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.drawImage(clearBackground, boundsF, juce::RectanglePlacement::stretchToFit);
        VCLook::drawUnitTitle(g, "OPT-2A", boundsF);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto knobSizeRef = boundsFWidth * 0.25f;
        const auto smallControlSize = knobSizeRef * 0.1333f;

        const auto knobBounds = boundsF.withSize(knobSizeRef, knobSizeRef);
        const auto labelBounds = boundsF.withSize(knobSizeRef, knobSizeRef * 0.3f);
        const auto grMeterBounds = boundsF.withSize(smallControlSize, knobSizeRef * 0.8f);
        const auto grTextBounds = boundsF.withSize(smallControlSize * 2.5f, smallControlSize * 2.f);
        const auto bypassBounds = boundsF.withSize(smallControlSize, smallControlSize);

        const auto labelFontHeight = labelBounds.getHeight() * 0.3556f;
        const auto smallLabelFontHeight = labelBounds.getHeight() * 0.3111f;
        for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel, &grLA2ALabel})
            label->setFont(label->getFont().withHeight(labelFontHeight));
        for (auto* label : {&la2aCompModeLabel, &la2aLimitModeLabel})
            label->setFont(label->getFont().withHeight(smallLabelFontHeight));

        const auto knobsCY = boundsFHeight * 0.5334f;
        const auto labelsCY = knobsCY + knobSizeRef * 0.5f;
        const auto la2aGainCX = boundsFWidth * 0.245f;
        const auto la2aPeakReductionCX = boundsFWidth * 0.745f;

        la2aGainDial.setBounds(knobBounds.withCentre({la2aGainCX, knobsCY}).toNearestInt());
        la2aGainLabel.setBounds(labelBounds.withCentre({la2aGainCX, labelsCY}).toNearestInt());
        la2aPeakReductionDial.setBounds(knobBounds.withCentre({la2aPeakReductionCX, knobsCY}).toNearestInt());
        la2aPeakReductionLabel.setBounds(labelBounds.withCentre({la2aPeakReductionCX, labelsCY}).toNearestInt());

        const auto modeButtonSize = knobSizeRef * 0.4f;
        const juce::Point<float> modeButtonCentre {boundsFWidth * 0.0417f, boundsFHeight * 0.8222f};
        la2aModeButton.setBounds(boundsF.withSize(modeButtonSize, modeButtonSize).withCentre(modeButtonCentre).toNearestInt());
        la2aCompModeLabel.setBounds(labelBounds.withCentre(modeButtonCentre.translated(0.f, modeButtonSize * 0.4167f)).toNearestInt());
        la2aLimitModeLabel.setBounds(labelBounds.withCentre(modeButtonCentre.translated(0.f, -modeButtonSize * 0.45f)).toNearestInt());

        const auto grMetersCX = boundsF.getCentreX();
        const auto grMetersTopY = boundsFHeight * 0.2712f;
        grMeterLA2ALeft.setBounds(grMeterBounds.withPosition(grMetersCX - smallControlSize, grMetersTopY).toNearestInt());
        grMeterLA2ARight.setBounds(grMeterBounds.withPosition(grMetersCX, grMetersTopY).toNearestInt());
        grLA2ALabel.setBounds(labelBounds.withCentre({grMetersCX, labelsCY}).toNearestInt());
        grTextDisplayLA2A.setBounds(grTextBounds.withCentre({grMetersCX, boundsFHeight * 0.2178f}).toNearestInt());

        bypassButtonLA2A.setBounds(bypassBounds.withPosition(smallControlSize * 0.5f, boundsFHeight * 0.0444f).toNearestInt());
    }

    void updateGainReduction()
    {
        const float grLA2A = audioProcessor.grLA2A;
        grMeterLA2ALeft.setReduction(grLA2A);
        grMeterLA2ARight.setReduction(grLA2A);
        grTextDisplayLA2A.setValue(grLA2A);
    }

private:
    void updateModeLabels()
    {
        const bool isLimitMode = la2aModeButton.getToggleState();
        la2aLimitModeLabel.setVisible(isLimitMode);
        la2aCompModeLabel.setVisible(! isLimitMode);
    }

    VocalCompressorAudioProcessor& audioProcessor;
    const int knobSize = 150;
    const int versionHint = 1;

//   IMAGE LOADING
    const juce::Image clearBackground {juce::ImageCache::getFromMemory(BinaryData::clearSilverBackground2_png, BinaryData::clearSilverBackground2_pngSize)};
    const juce::Image la2aKnobImage {juce::ImageCache::getFromMemory(BinaryData::spriteSheetLa2a_png, BinaryData::spriteSheetLa2a_pngSize)};
    const juce::Image la2aModeImageUP {juce::ImageCache::getFromMemory(BinaryData::switchUP_png, BinaryData::switchUP_pngSize)};
    const juce::Image la2aModeImageDOWN {juce::ImageCache::getFromMemory(BinaryData::switchDOWN_png, BinaryData::switchDOWN_pngSize)};

//   KNOBS
    ImageKnob la2aGainDial {"LA2A Gain Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::getLA2AGainName(), versionHint), la2aKnobImage, "", knobSize, false, false};
    ImageKnob la2aPeakReductionDial {"LA2A Peak Reduction Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::getLA2APeakReductionName(), versionHint), la2aKnobImage, "", knobSize, false, false};

//   COMP / LIMIT SWITCH
    juce::ImageButton la2aModeButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment la2aModeButtonAttachment {audioProcessor.apvts, ParamNames::getLA2ALimitCompressModeName(), la2aModeButton};

//    LABELS
    juce::Label la2aGainLabel {"GAIN"}, la2aPeakReductionLabel {"PEAK REDUCTION"},
                la2aCompModeLabel {"COMP"}, la2aLimitModeLabel {"LIMIT"}, grLA2ALabel {"GR"};

//    GAIN REDUCTION + BYPASS
    GainReductionMeter grMeterLA2ALeft, grMeterLA2ARight;
    GRTextDisplay grTextDisplayLA2A;
    BypassButton bypassButtonLA2A {"", audioProcessor.apvts, ParamNames::getLA2ABypassName(), 20};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelLA2A)
};
