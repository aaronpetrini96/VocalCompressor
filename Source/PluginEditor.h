#pragma once

#include "PluginProcessor.h"
#include "GUI/Panels/HeaderPanel.h"
#include "GUI/Panels/Panel1176.h"
#include "GUI/Panels/PanelLA2A.h"
#include "GUI/PopupPanels/UISizeOptionsPanel.h"
#include "GUI/PopupPanels/AAPresetBrowser.h"
#include "GUI/PopupPanels/AAPresetPopups.h"
#include "Utilities/AAUITools.h"

class VocalCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::ComponentListener
{
public:
    VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor&);

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void paintOverChildren (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& event) override;
    void componentVisibilityChanged (juce::Component& component) override;

private:
    VocalCompressorAudioProcessor& audioProcessor;
    void togglePopup (juce::Component& popup, const std::function<void()>& prepareContent);
    bool isAnyPopupVisible() const;
    void updateEditorSize();

    //  EDITOR CONSTANTS
    const int guiWidth = 900;
    const int guiHeight = 600;

//    PANELS
    HeaderPanel headerPanel {audioProcessor};
    Panel1176 panel1176 {audioProcessor};
    PanelLA2A panelLA2A {audioProcessor};
    juce::Rectangle<float> topPanelBounds, bottomPanelBounds;
    const juce::Image rackEarsImage {juce::ImageCache::getFromMemory(BinaryData::rackEarsImage_png, BinaryData::rackEarsImage_pngSize)};

    float uiScale {0.0f};
    AAResizeUI uiResize {uiScale, guiWidth, guiHeight};
    UISizeOptionsPanel resizeOptionsPopup {uiResize};

    AAPresetBrowser presetBrowser {audioProcessor.presetManager};
    AAPresetPopup presetPopup;

    const std::array<juce::Component*, 3> popups {&resizeOptionsPopup, &presetBrowser, &presetPopup};
    juce::Component* popupClosedOnMouseDown {nullptr};
    juce::Component modalBlocker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalCompressorAudioProcessorEditor)
};
