#pragma once
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/ArcImageKnob.h"
#include "../Components/GRDistressorMeter.h"

class PanelLA2A : public juce::Component
{
public:
    explicit PanelLA2A(VocalCompressorAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void updateModeLabels();

    VocalCompressorAudioProcessor& audioProcessor;

//   IMAGE LOADING
    const juce::Image clearBackground {juce::ImageCache::getFromMemory(BinaryData::rackLA2ABackgroundEarsLight_png, BinaryData::rackLA2ABackgroundEarsLight_pngSize)};
    const juce::Image la2aKnobImage {juce::ImageCache::getFromMemory(BinaryData::knobLa2aBase_png, BinaryData::knobLa2aBase_pngSize)};
    const juce::Image la2aKnobRotorImage {juce::ImageCache::getFromMemory(BinaryData::knobLa2aRotor_png, BinaryData::knobLa2aRotor_pngSize)};
    const juce::Image la2aMixKnobImage {juce::ImageCache::getFromMemory(BinaryData::mixKnob1176Base_png, BinaryData::mixKnob1176Base_pngSize)};
    const juce::Image la2aModeImageUP {juce::ImageCache::getFromMemory(BinaryData::switchUP_png, BinaryData::switchUP_pngSize)};
    const juce::Image la2aModeImageDOWN {juce::ImageCache::getFromMemory(BinaryData::switchDOWN_png, BinaryData::switchDOWN_pngSize)};

//   KNOBS
    RotaryImageKnob la2aGainDial {*audioProcessor.apvts.getParameter(ParamNames::getLA2AGainName()), la2aKnobImage, la2aKnobRotorImage, 300};
    RotaryImageKnob la2aPeakReductionDial {*audioProcessor.apvts.getParameter(ParamNames::getLA2APeakReductionName()), la2aKnobImage, la2aKnobRotorImage, 300};
    ArcImageKnob la2aMixKnob {*audioProcessor.apvts.getParameter(ParamNames::getLA2AMixName()), la2aMixKnobImage};

//    KNOB SCALES
    const juce::StringArray scaleLabels {"0", "20", "40", "60", "80", "100"};
    juce::Font scaleFont {juce::FontOptions{}};
    float scaleInset {0.f}, scaleTickLength {0.f}, scaleLineThickness {0.f};

//   COMP / LIMIT SWITCH
    juce::ImageButton la2aModeButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment la2aModeButtonAttachment {audioProcessor.apvts, ParamNames::getLA2ALimitCompressModeName(), la2aModeButton};

//    LABELS
    juce::Label la2aGainLabel {"GAIN"}, la2aPeakReductionLabel {"PEAK REDUCTION"},
                la2aCompModeLabel {"COMP"}, la2aLimitModeLabel {"LIMIT"}, la2aMixLabel {"MIX"};

//    GAIN REDUCTION
    GRDistressorMeter grMeterLA2A {audioProcessor.grLA2A, juce::Colours::black};
    juce::Label gainReductionLabel {"GAIN REDUCTION"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelLA2A)
};
