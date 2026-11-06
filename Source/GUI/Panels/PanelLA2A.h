#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/GRDistressorMeter.h"

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

        for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel, &la2aCompModeLabel, &la2aLimitModeLabel})
        {
            VCLook::setupLabel(*label, juce::Colours::black);
            addAndMakeVisible(*label);
        }
        updateModeLabels();

        addAndMakeVisible(grMeterLA2A);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.drawImage(clearBackground, boundsF, juce::RectanglePlacement::stretchToFit);

        const auto scaleFont = VCFonts::barlowCondensedMedium(boundsF.getHeight() * 0.07f);
        const auto scaleInset = boundsF.getHeight() * 0.07f;
        for (auto* dial : {&la2aGainDial, &la2aPeakReductionDial})
            AAUITools::drawKnobArcScale(g, dial->getBounds().toFloat().reduced(scaleInset), dial->getRotaryParameters().startAngleRadians, dial->getRotaryParameters().endAngleRadians,
                                        11, boundsF.getHeight() * 0.03f, boundsF.getHeight() * 0.006f, {"0", "20", "40", "60", "80", "100"}, scaleFont, juce::Colours::black);
    }

    void resized() override
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto grMeterWidth = bounds.getWidth() * 0.6f;
        const auto grMeterHeight = bounds.getHeight() * 0.14f;
        const auto grMeterCentre = juce::Point<float> (bounds.getCentreX(), grMeterHeight * 0.5f);
        grMeterLA2A.setBounds(bounds.withSize(grMeterWidth, grMeterHeight).withCentre(grMeterCentre).toNearestInt());

        const auto boundsF = bounds.withTrimmedTop(grMeterHeight);
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        const auto knobSize = boundsFHeight * 0.65f;
        const auto knobBounds = boundsF.withSize(knobSize, knobSize);
        const auto labelBounds = boundsF.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.22f);

        for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel})
            label->setFont(VCFonts::barlowCondensedMedium(boundsFHeight * 0.1f));
        for (auto* label : {&la2aCompModeLabel, &la2aLimitModeLabel})
            label->setFont(VCFonts::barlowCondensedRegular(boundsFHeight * 0.08f));

        const auto labelsCY = boundsF.getCentreY() + knobSize * 0.5f + labelBounds.getHeight() * 0.5f;
        la2aGainDial.setBounds(knobBounds.withCentre({boundsFWidth * 0.245f, boundsF.getCentreY()}).toNearestInt());
        la2aGainLabel.setBounds(labelBounds.withCentre({boundsFWidth * 0.245f, labelsCY}).toNearestInt());
        la2aPeakReductionDial.setBounds(knobBounds.withCentre({boundsFWidth * 0.745f, boundsF.getCentreY()}).toNearestInt());
        la2aPeakReductionLabel.setBounds(labelBounds.withCentre({boundsFWidth * 0.745f, labelsCY}).toNearestInt());

        const auto modeCX = boundsFWidth * 0.0417f;
        la2aModeButton.setBounds(boundsF.withSize(boundsFHeight * 0.2912f, boundsFHeight * 0.2912f).withCentre({modeCX, boundsFHeight * 0.8222f}).toNearestInt());
        la2aCompModeLabel.setBounds(labelBounds.withCentre({modeCX, boundsFHeight * 0.9435f}).toNearestInt());
        la2aLimitModeLabel.setBounds(labelBounds.withCentre({modeCX, boundsFHeight * 0.6911f}).toNearestInt());
    }

    void paintOverChildren(juce::Graphics& g) override
    {
        if (shouldDim)
            g.fillAll(VCColors::almostBlack.withAlpha(0.3f));
    }

    bool shouldDim {false};

private:
    void updateModeLabels()
    {
        const bool isLimitMode = la2aModeButton.getToggleState();
        la2aLimitModeLabel.setVisible(isLimitMode);
        la2aCompModeLabel.setVisible(! isLimitMode);
    }

    VocalCompressorAudioProcessor& audioProcessor;

//   IMAGE LOADING
    const juce::Image clearBackground {juce::ImageCache::getFromMemory(BinaryData::clearSilverBackground2_png, BinaryData::clearSilverBackground2_pngSize)};
    const juce::Image la2aKnobImage {juce::ImageCache::getFromMemory(BinaryData::knobLa2aBase_png, BinaryData::knobLa2aBase_pngSize)};
    const juce::Image la2aKnobRotorImage {juce::ImageCache::getFromMemory(BinaryData::knobLa2aRotor_png, BinaryData::knobLa2aRotor_pngSize)};
    const juce::Image la2aModeImageUP {juce::ImageCache::getFromMemory(BinaryData::switchUP_png, BinaryData::switchUP_pngSize)};
    const juce::Image la2aModeImageDOWN {juce::ImageCache::getFromMemory(BinaryData::switchDOWN_png, BinaryData::switchDOWN_pngSize)};

//   KNOBS
    RotaryImageKnob la2aGainDial {*audioProcessor.apvts.getParameter(ParamNames::getLA2AGainName()), la2aKnobImage, la2aKnobRotorImage};
    RotaryImageKnob la2aPeakReductionDial {*audioProcessor.apvts.getParameter(ParamNames::getLA2APeakReductionName()), la2aKnobImage, la2aKnobRotorImage};

//   COMP / LIMIT SWITCH
    juce::ImageButton la2aModeButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment la2aModeButtonAttachment {audioProcessor.apvts, ParamNames::getLA2ALimitCompressModeName(), la2aModeButton};

//    LABELS
    juce::Label la2aGainLabel {"GAIN"}, la2aPeakReductionLabel {"PEAK REDUCTION"},
                la2aCompModeLabel {"COMP"}, la2aLimitModeLabel {"LIMIT"};

//    GAIN REDUCTION
    GRDistressorMeter grMeterLA2A {audioProcessor.grLA2A, juce::Colours::black};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelLA2A)
};
