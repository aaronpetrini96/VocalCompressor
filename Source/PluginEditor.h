/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/Panels/Panel1176.h"
#include "GUI/Panels/PanelLA2A.h"
#include "GUI/LAF/VCLookAndFeel.h"

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

private:
    VocalCompressorAudioProcessor& audioProcessor;
    //  EDITOR CONSTANTS
    const int guiWidth = 600;
    const int guiHeight = 450;
    const int guiHeightExtended = 485;

    VCLookAndFeel vcLookAndFeel;

//    PANELS
    HeaderPanel headerPanel {audioProcessor};
    Panel1176 panel1176 {audioProcessor};
    PanelLA2A panelLA2A {audioProcessor};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessorEditor)
};
