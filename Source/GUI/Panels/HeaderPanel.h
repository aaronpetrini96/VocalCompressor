#pragma once
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../LAF/UnitTitleButtonLAF.h"
#include "../Components/AAPresetBox.h"

class HeaderPanel : public juce::Component, private juce::ChangeListener
{
public:
    explicit HeaderPanel(VocalCompressorAudioProcessor& processor);
    ~HeaderPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    AAPresetBox& getPresetBox() { return presetBox; }
    juce::DrawableButton& getSavePresetButton() { return savePresetButton; }
    juce::DrawableButton& getBypassButton() { return bypassPluginButton; }
    juce::DrawableButton& getResizeButton() { return resizeButton; }
    juce::DrawableButton& getSignalOrderButton() { return signalOrderButton; }
    juce::TextButton& get1176BypassButton() { return unitTitle1176Button; }
    juce::TextButton& getLA2ABypassButton() { return unitTitleLA2AButton; }

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override;
    void setupArrowIconButtons();
    void updateUndoRedoButtons();
    void updatePresetName();

    UnitTitleButtonLAF unitTitleLAF;
    PresetManager& presetManager;
    juce::UndoManager& undoManager;
    AAPresetBox presetBox;
    juce::DrawableButton savePresetButton {"Save preset", juce::DrawableButton::ImageFitted};
    juce::DrawableButton bypassPluginButton {"Bypass", juce::DrawableButton::ImageFitted};
    juce::DrawableButton resizeButton {"resizeUIButton", juce::DrawableButton::ImageFitted};
    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassPluginButtonAttachment;
    juce::TextButton unitTitle1176Button {"FET-76"}, unitTitleLA2AButton {"OPT-2A"};
    juce::AudioProcessorValueTreeState::ButtonAttachment unitTitle1176ButtonAttachment, unitTitleLA2AButtonAttachment;
    juce::DrawableButton signalOrderButton {"Signal order", juce::DrawableButton::ImageFitted};
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseSignalOrderButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
