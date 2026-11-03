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
#include "GUI/PopupPanels/UISizeOptionsPanel.h"
#include "GUI/PopupPanels/AAPresetBrowser.h"
#include "GUI/PopupPanels/AAPresetPopups.h"
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
    void paintOverChildren (juce::Graphics& g) override;
    void timerCallback() override;
    void mouseUp (const juce::MouseEvent& event) override;
    void componentVisibilityChanged (juce::Component& component) override;

private:
    VocalCompressorAudioProcessor& audioProcessor;
    void showUISizeOptions();
    void showPopup (juce::Component& popup);
    void showPresetBrowser();
    void showPresetPopup (AAPresetPopup::Action action, const juce::String& presetName);
    void updatePanelsDimming();
    void updateEditorSize();

    //  EDITOR CONSTANTS
    const int guiWidth = 780;
    const int guiHeight = 520;

//    PANELS
    HeaderPanel headerPanel {audioProcessor};
    Panel1176 panel1176 {audioProcessor};
    PanelLA2A panelLA2A {audioProcessor};

    float uiScale {0.0f};
    AAResizeUI uiResize {uiScale, guiWidth, guiHeight};
    UISizeOptionsPanel resizeOptionsPopup {uiResize};

    AAPresetBrowser presetBrowser {audioProcessor.presetManager};
    AAPresetPopup presetPopup;
    juce::File presetFileToEdit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessorEditor)
};
