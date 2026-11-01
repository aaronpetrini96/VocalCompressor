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
#include "GUI/PopupPanels/UISizeOptionsPanel.h"
#include "Utilities/AAUITools.h"

//==============================================================================
/**
*/
class VocalCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer, private juce::ComponentListener
{
public:
    VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor&);
    ~VocalCompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseUp (const juce::MouseEvent& event) override;
    void componentVisibilityChanged (juce::Component& component) override;

private:
    VocalCompressorAudioProcessor& audioProcessor;
    void showUISizeOptions();
    void showPopup (juce::Component& popup);
    void updateEditorSize();
    //  EDITOR CONSTANTS
    const int guiWidth = 650;
    const int guiHeight = 450;
    const int guiHeightExtended = 480;

    VCLookAndFeel vcLookAndFeel;

//    PANELS
    HeaderPanel headerPanel {audioProcessor};
    Panel1176 panel1176 {audioProcessor};
    PanelLA2A panelLA2A {audioProcessor};

    float uiScale {0.0f};
    AAResizeUI uiResize {uiScale, guiWidth, guiHeightExtended};
    UISizeOptionsPanel resizeOptionsPopup {uiResize};
    juce::DrawableButton resizeButton {"UI Size", juce::DrawableButton::ImageFitted};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessorEditor)
};
