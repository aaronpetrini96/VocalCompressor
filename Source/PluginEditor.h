/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/ImageKnob.h"
#include "GUI/GainReductionMeter.h"
#include "GUI/BypassButton.h"
#include "Utilities/PresetManagerV2.h"
#include "GUI/GRTextDisplay.h"

//==============================================================================
/**
*/
class VocalCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor&);
    ~VocalCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void setRatioIndex(const juce::String& parameterName, const int& index);
    void updateRatioButtons(const int& selectedIndex);

private:
    VocalCompressorAudioProcessor& audioProcessor;
    //  EDITOR CONSTANTS
    const int guiWidth = 600;
    const int guiHeight = 450;
    const int guiHeightExtended = 485;
    const int knobSize = 150;
    const int versionHint = 1;
    const juce::String labelFont = "Basic Comical NC";
    const juce::String titleFont = "Barber Chop";

    
//   IMAGE LOADING
    const juce::Image darkBackground {juce::ImageCache::getFromMemory(BinaryData::darkSilverBackground_png, BinaryData::darkSilverBackground_pngSize)};
    const juce::Image clearBackground {juce::ImageCache::getFromMemory(BinaryData::clearSilverBackground_png, BinaryData::clearSilverBackground_pngSize)};

    const juce::Image screwImage {juce::ImageCache::getFromMemory(BinaryData::screw25x25_png, BinaryData::screw25x25_pngSize)};
    
    const juce::Image knobImage1176 {juce::ImageCache::getFromMemory(BinaryData::spriteSheet1176_png, BinaryData::spriteSheet1176_pngSize)};
    const juce::Image la2aKnobImage {juce::ImageCache::getFromMemory(BinaryData::spriteSheetLa2a_png, BinaryData::spriteSheetLa2a_pngSize)};
    
    const juce::Image la2aModeImageUP {juce::ImageCache::getFromMemory(BinaryData::switchUP_png, BinaryData::switchUP_pngSize)};
    const juce::Image la2aModeImageDOWN {juce::ImageCache::getFromMemory(BinaryData::switchDOWN_png, BinaryData::switchDOWN_pngSize)};
    

    
//   IMAGE KNOBS
    ImageKnob input1176Dial {"1176 Input Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176InputName(), versionHint), knobImage1176, "", knobSize, false, false};
    ImageKnob output1176Dial {"1176 Output Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176OutputName(), versionHint), knobImage1176, "", knobSize, false, false};
    ImageKnob attack1176Dial {"1176 Attack Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176AttackName(), versionHint), knobImage1176, "", 75, false, false};
    ImageKnob release1176Dial {"1176 Release Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176ReleaseName(), versionHint), knobImage1176, "", 75, false, false};
    ImageKnob la2aGainDial {"LA2A Gain Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::getLA2AGainName(), versionHint), la2aKnobImage, "", knobSize, false, false};
    ImageKnob la2aPeakReductionDial {"LA2A Peak Reduction Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::getLA2APeakReductionName(), versionHint), la2aKnobImage, "", knobSize, false, false};
    
    std::vector <ImageKnob*> imageKnobs
    {
        &input1176Dial, &output1176Dial, &attack1176Dial, &release1176Dial,
        &la2aGainDial, &la2aPeakReductionDial,
    };
    
    
    juce::ImageButton la2aModeButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment la2aModeButtonAttachment {audioProcessor.apvts, ParamNames::getLA2ALimitCompressModeName(), la2aModeButton};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"}, attack1176Label {"ATTACK"}, release1176Label{"RELEASE"}, ratioLabel{"RATIO"}, gr1176Label{"GR"};
    juce::Label la2aGainLabel {"GAIN"}, la2aPeakReductionLabel {"PEAK REDUCTION"}, la2aCompModeLabel {"COMP"}, la2aLimitModeLabel {"LIMIT"}, grLA2ALabel{"GR"};
    
    std::vector <juce::Label*> knobLabels
    {
        &input1176Label, &output1176Label, &attack1176Label, &release1176Label, &ratioLabel, &gr1176Label,
        &la2aGainLabel, &la2aPeakReductionLabel, &la2aCompModeLabel, &la2aLimitModeLabel, &grLA2ALabel,
    };
    
    void labelsGeneralSetup (juce::Label* label);
    
//    BYPASS BUTTONS
    BypassButton bypassButton1176 {"", audioProcessor.apvts, ParamNames::get1176BypassName(), 20};
    BypassButton bypassButtonLA2A {"", audioProcessor.apvts, ParamNames::getLA2ABypassName(), 20};
    BypassButton bypassPluginButton {"", audioProcessor.apvts, ParamNames::getGeneralBypassName(), 20};
    
//    RATIO BUTTONS
    juce::TextButton ratio4Btn{"4"}, ratio8Btn{"8"}, ratio16Btn{"16"}, ratio20Btn{"20"}, ratioAllBtn{"ALL"};
    std::vector<juce::TextButton*> ratioButtons
    {
        &ratio4Btn, &ratio8Btn, &ratio16Btn, &ratio20Btn, &ratioAllBtn,
    };
    
    void setupRatioButtons (juce::TextButton* ratioBtn);
    
//    GAIN REDUCTION TEXT & METERS
    GainReductionMeter grMeter1176Left, grMeter1176Right, grMeterLA2ALeft, grMeterLA2ARight;
    std::vector<GainReductionMeter*> grMeters
    {
        &grMeter1176Left, &grMeter1176Right,
        &grMeterLA2ALeft, &grMeterLA2ARight
    };
    
    GRTextDisplay grTextDisplay1176, grTextDisplayLA2A;
    
//    PRESET MANAGER
    PresetManagerV2 presetManager;

//    SIGNAL ORDER
    juce::TextButton reverseSignalOrderButton;
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseSignalOrderButtonAttachment {audioProcessor.apvts, ParamNames::getReverseSignalOrderName(), reverseSignalOrderButton};
    void updateReverseSignalButtonText (bool state);
    void setupReverseSignalButton (juce::TextButton&);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessorEditor)
};
