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

    juce::DrawableButton& getResizeButton() { return resizeButton; }

    std::function<void()> onPresetNameClicked;
    std::function<void()> onSaveClicked;
    std::function<void()> onBypassChanged;

    bool isBypassed() const { return bypassPluginButton.getToggleState(); }
    bool is1176Bypassed() const { return unitTitle1176Button.getToggleState(); }
    bool isLA2ABypassed() const { return unitTitleLA2AButton.getToggleState(); }

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override;
    void setupArrowIconButtons();
    void updateUndoRedoButtons();
    void updatePresetName();

    UnitTitleButtonLAF unitTitleLAF;
    PresetManager& presetManager;
    juce::UndoManager& undoManager;
    AAPresetBox presetBox;
    juce::DrawableButton saveButton {"Save preset", juce::DrawableButton::ImageFitted};
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
