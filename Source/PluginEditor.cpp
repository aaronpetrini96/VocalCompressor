/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessorEditor::VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&vcLookAndFeel);
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(panel1176);
    addAndMakeVisible(panelLA2A);

    setResizable(false, true);
    setResizeLimits(juce::roundToInt(guiWidth * 0.6f), juce::roundToInt(guiHeightExtended * 0.6f),
                    juce::roundToInt(guiWidth * 2.f), juce::roundToInt(guiHeightExtended * 2.f));
    getConstrainer()->setFixedAspectRatio(static_cast<double>(guiWidth) / static_cast<double>(guiHeightExtended));
    setSize (guiWidth, guiHeightExtended);
    startTimerHz(30);
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(VCColors::background);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();
    vcLookAndFeel.setUIScale(boundsF.getWidth() / static_cast<float>(guiWidth));

    const auto headerHeight = boundsFHeight * static_cast<float>(guiHeightExtended - guiHeight) / static_cast<float>(guiHeightExtended);
    const auto unitPanelHeight = (boundsFHeight - headerHeight) * 0.5f;

    const auto headerBounds = boundsF.withHeight(headerHeight);
    const auto panel1176Bounds = boundsF.withHeight(unitPanelHeight).withY(headerBounds.getBottom());
    const auto panelLA2ABounds = panel1176Bounds.withY(panel1176Bounds.getBottom());

    headerPanel.setBounds(headerBounds.toNearestInt());
    panel1176.setBounds(panel1176Bounds.toNearestInt());
    panelLA2A.setBounds(panelLA2ABounds.toNearestInt());
}

void VocalCompressorAudioProcessorEditor::timerCallback()
{
    panel1176.updateGainReduction();
    panelLA2A.updateGainReduction();
}
