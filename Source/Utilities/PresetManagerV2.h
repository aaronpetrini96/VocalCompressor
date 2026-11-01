#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../GUI/LAF/VCControlsLAF.h"

class PresetManagerV2 : public juce::Component
{
public:
    PresetManagerV2(VocalCompressorAudioProcessor& processor);
    void resized() override;
    
    void refreshPresetList();
    void applyPresetSelection(const juce::String&);
    
    int saveBtnY {0};
    int saveBtnHeight {0};
    
private:
    void savePreset();
    void presetSelected();
    
    int getItemIdForText(const juce::ComboBox&, const juce::String&);
    juce::File getPresetFolder();
    
    VocalCompressorAudioProcessor& audioProcessor;

    VCControlsLAF controlsLAF;
    juce::ComboBox presetBox;
    juce::TextButton savePresetButton {"SAVE"};
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    void setButtonStyle();
};
