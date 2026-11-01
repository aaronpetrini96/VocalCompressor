/*
  ==============================================================================

    PresetManagerV2.cpp
    Created: 5 Jan 2026 2:33:52pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "PresetManagerV2.h"
#include "../GUI/VCLook.h"

PresetManagerV2::PresetManagerV2(VocalCompressorAudioProcessor& processor) : audioProcessor(processor)
{
//    ComboBox
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.setColour(juce::ComboBox::backgroundColourId, VCColors::presetBoxBackground);
    presetBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    
    presetBox.onChange = [this] {presetSelected();};
    addAndMakeVisible(presetBox);
    
//    SaveButton
    setButtonStyle();
    savePresetButton.onClick = [this] {savePreset();};
    addAndMakeVisible(savePresetButton);
    
    refreshPresetList();
    
    auto presetName = audioProcessor.getCurrentPresetName();
    if (presetName.isNotEmpty())
        applyPresetSelection(presetName);
}



void PresetManagerV2::savePreset()
{
    auto presetFolder = getPresetFolder();
    if (!presetFolder.exists())
        presetFolder.createDirectory();
    
    fileChooser = std::make_unique<juce::FileChooser> ("Save Preset", presetFolder, "*.xml");
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync (fileChooserFlags, [this] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();
        
        if (selectedFile.existsAsFile() || selectedFile.create())
        {
            // Save the preset to the selected file
            audioProcessor.savePreset(selectedFile);
            DBG("Preset saved to: " + selectedFile.getFullPathName());
            
            // Optionally, refresh the preset list in the UI or do other actions
            refreshPresetList();

            applyPresetSelection(selectedFile.getFileNameWithoutExtension());
        }
        else
        {
            DBG("Error: Failed to save preset. File creation failed.");
        }
        
    });
}

void PresetManagerV2::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto area = boundsF.reduced(boundsFHeight * 0.1429f);
    const auto areaCentreX = area.getWidth() * 0.5f;
    const auto comboWidth = area.getWidth() * 0.2f;

    presetBox.setBounds(area.withWidth(comboWidth).withRightX(areaCentreX).toNearestInt());
    saveBtnY = juce::roundToInt(area.getY());
    saveBtnHeight = juce::roundToInt(area.getHeight());
    savePresetButton.setBounds(area.withWidth(comboWidth * 0.75f).withX(areaCentreX + boundsFWidth * 0.0167f).toNearestInt());
}

void PresetManagerV2::presetSelected()
{
    auto selectedId = presetBox.getSelectedId();
    if (selectedId > 0)
    {
        auto presetName = presetBox.getItemText(selectedId - 1);
        auto presetFile = getPresetFolder().getChildFile(presetName + ".xml");
        audioProcessor.loadPreset(presetFile);
        applyPresetSelection(presetName);
    }
}

void PresetManagerV2::refreshPresetList()
{
    presetBox.clear();
    auto presetFolder = getPresetFolder();
    presetFolder.createDirectory();
    
    auto files = presetFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    
    int index = 1;
    for (auto& file : files)
    {
        presetBox.addItem(file.getFileNameWithoutExtension(), index);
        ++index;
    }
}

juce::File PresetManagerV2::getPresetFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("IRFx/Presets");
}


int PresetManagerV2::getItemIdForText(const juce::ComboBox& box, const juce::String& textToFind)
{
    for (int i = 0; i < box.getNumItems(); ++i)
    {
        if (box.getItemText(i) == textToFind)
            return box.getItemId(i);
    }
    return 0; // 0 means "not found"
}

void PresetManagerV2::applyPresetSelection(const juce::String& presetName)
{
    audioProcessor.setCurrentPresetName(presetName);
    presetBox.setSelectedId(getItemIdForText(presetBox, presetName));
}

void PresetManagerV2::setButtonStyle()
{
    savePresetButton.setClickingTogglesState(false);
    savePresetButton.setColour(juce::TextButton::buttonColourId, VCColors::buttonBackground);
    savePresetButton.setColour(juce::TextButton::textColourOffId, VCColors::text);
    savePresetButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
}
