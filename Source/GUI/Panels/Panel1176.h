#pragma once

#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/ArcImageKnob.h"
#include "../Components/GRDistressorMeter.h"
#include "../Components/Ratio1176Buttons.h"

class Panel1176 : public juce::Component
{
public:
    explicit Panel1176(VocalCompressorAudioProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    VocalCompressorAudioProcessor& audioProcessor;
    float knobSize {0.f};
    float smallKnobsSize{0.f};

//   IMAGE LOADING
    const juce::Image background1176 {juce::ImageCache::getFromMemory(BinaryData::rack1176BackgroundEarsLight_png, BinaryData::rack1176BackgroundEarsLight_pngSize)};
    const juce::Image knobBaseImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Base_png, BinaryData::knob1176Base_pngSize)};
    const juce::Image knobRotorImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Rotor_png, BinaryData::knob1176Rotor_pngSize)};
    const juce::Image smallKnobImage1176 {juce::ImageCache::getFromMemory(BinaryData::small1176Base_png, BinaryData::small1176Base_pngSize)};
    const juce::Image smallKnobRotorImage1176 {juce::ImageCache::getFromMemory(BinaryData::small1176Rotor_png, BinaryData::small1176Rotor_pngSize)};
    const juce::Image mixKnobImage1176 {juce::ImageCache::getFromMemory(BinaryData::mixKnobLa2aBase_png, BinaryData::mixKnobLa2aBase_pngSize)};

//   KNOBS
    RotaryImageKnob input1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176InputName()), knobBaseImage1176, knobRotorImage1176, 300};
    RotaryImageKnob output1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176OutputName()), knobBaseImage1176, knobRotorImage1176, 300};
    RotaryImageKnob attack1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176AttackName()), smallKnobImage1176, smallKnobRotorImage1176, 250};
    RotaryImageKnob release1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176ReleaseName()), smallKnobImage1176, smallKnobRotorImage1176, 250};
    ArcImageKnob mix1176Knob {*audioProcessor.apvts.getParameter(ParamNames::get1176MixName()), mixKnobImage1176};

//    KNOB SCALES
    const juce::StringArray bigScaleLabels {juce::String(juce::CharPointer_UTF8("\xe2\x88\x9e")), "48", "36", "24", "12", "0"};
    juce::Font bigScaleFont {juce::FontOptions{}};
    float bigScaleInset {0.f}, bigScaleLabelGap {0.f}, bigScaleDotSize {0.f};
    const juce::StringArray smallScaleLabels {"1", "3", "5", "7"};
    juce::Font smallScaleFont {juce::FontOptions{}};
    float smallScaleLabelGap {0.f}, smallScaleDotSize {0.f};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"},
                attack1176Label {"ATTACK"}, release1176Label {"RELEASE"},
                mix1176Label {"MIX"}, ratioLabel {"RATIO"};

//    RATIO BUTTONS
    Ratio1176Buttons ratioButtons1176 {*audioProcessor.apvts.getParameter(ParamNames::get1176RatioName())};

//    GAIN REDUCTION
    GRDistressorMeter grMeter1176 {audioProcessor.gR1176, VCColors::almostWhiteText};
    juce::Label gainReductionLabel {"GAIN REDUCTION"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Panel1176)
};
