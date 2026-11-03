#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../LAF/VCControlsLAF.h"
#include "../Components/BypassButton.h"
#include "../Components/AAPresetBox.h"

class HeaderPanel : public juce::Component, private juce::ChangeListener
{
public:
    explicit HeaderPanel(VocalCompressorAudioProcessor& processor)
    : presetManager(processor.presetManager),
      undoManager(processor.undoManager),
      presetBox(processor.presetManager),
      bypassPluginButton("", processor.apvts, ParamNames::getGeneralBypassName(), 20),
      reverseSignalOrderButtonAttachment(processor.apvts, ParamNames::getReverseSignalOrderName(), reverseSignalOrderButton)
    {
        reverseSignalOrderButton.setLookAndFeel(&controlsLAF);
        reverseSignalOrderButton.setColour(juce::TextButton::buttonColourId, VCColors::buttonBackground);
        reverseSignalOrderButton.setColour(juce::TextButton::buttonOnColourId, VCColors::buttonBackground);
        reverseSignalOrderButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        reverseSignalOrderButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        reverseSignalOrderButton.setClickingTogglesState(true);
        reverseSignalOrderButton.onStateChange = [this] { updateReverseSignalButtonText(); };
        updateReverseSignalButtonText();

        presetBox.onNameClicked = [this] { if (onPresetNameClicked != nullptr) onPresetNameClicked(); };

        AAUITools::setIconButtonImages(saveButton, BinaryData::saveIcon2_svg, BinaryData::saveIcon2_svgSize,
                                       VCColors::popupIconSVG, VCColors::iconNormal, VCColors::iconOver, VCColors::iconActive);
        saveButton.onClick = [this] { if (onSaveClicked != nullptr) onSaveClicked(); };

        AAUITools::setIconButtonImages(undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize, juce::Colours::white, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        AAUITools::setIconButtonImages(redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize, juce::Colours::white, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        undoButton.onClick = [this] { undoManager.undo(); };
        redoButton.onClick = [this] { undoManager.redo(); };

        presetManager.addChangeListener(this);
        undoManager.addChangeListener(this);
        updatePresetName();

        addAndMakeVisible(presetBox);
        addAndMakeVisible(saveButton);
        addAndMakeVisible(bypassPluginButton);
        addAndMakeVisible(reverseSignalOrderButton);
        addAndMakeVisible(undoButton);
        addAndMakeVisible(redoButton);
    }

    ~HeaderPanel() override
    {
        presetManager.removeChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();

        g.fillAll(VCColors::background);

        const auto screwSize = boundsF.getHeight() * 0.6f;
        const auto screwGap = screwSize * 0.5f;
        const auto screwBounds = boundsF.withSizeKeepingCentre(screwSize, screwSize);
        g.drawImage(screwImage, screwBounds.withX(screwGap), juce::RectanglePlacement::stretchToFit);
        g.drawImage(screwImage, screwBounds.withRightX(boundsFWidth - screwGap), juce::RectanglePlacement::stretchToFit);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto bypassSize = boundsFWidth * 0.0333f;
        bypassPluginButton.setBounds(boundsF.withSize(bypassSize, bypassSize)
                                            .withPosition(boundsFWidth * 0.14f, boundsFHeight * 0.2f)
                                            .toNearestInt());

        const auto controlY = boundsFHeight * 0.1429f;
        const auto controlHeight = boundsFHeight * 0.7142f;

        const auto presetBoxBounds = boundsF.withX(boundsFWidth * 0.22f)
                                            .withWidth(boundsFWidth * 0.32f)
                                            .withY(controlY)
                                            .withHeight(controlHeight);
        presetBox.setBounds(presetBoxBounds.toNearestInt());

        const auto saveButtonSize = controlHeight * 0.8f;
        saveButton.setBounds(juce::Rectangle<float>(saveButtonSize, saveButtonSize)
                                 .withX(presetBoxBounds.getRight() + saveButtonSize * 0.3f)
                                 .withCentre({presetBoxBounds.getRight() + saveButtonSize * 0.8f, presetBoxBounds.getCentreY()})
                                 .toNearestInt());

        reverseSignalOrderButton.setBounds(boundsF.withX(boundsFWidth * 0.7085f)
                                                  .withWidth(boundsFWidth * 0.2083f)
                                                  .withY(controlY)
                                                  .withHeight(controlHeight)
                                                  .toNearestInt());
        const auto undoRedoCentreX = (saveButtonRight + resizeButtonX) * 0.5f;
        const auto undoRedoOffset = (iconSize + iconGap) * 0.5f;
        undoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
        redoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    }

    AAPresetBox& getPresetBox() { return presetBox; }

    std::function<void()> onPresetNameClicked;
    std::function<void()> onSaveClicked;

private:
    void changeListenerCallback(juce::ChangeBroadcaster*) override
    {
        updatePresetName();
        updateUndoRedoButtons();
    }

    void updateUndoRedoButtons()
    {
        undoButton.setEnabled(undoManager.canUndo());
        redoButton.setEnabled(undoManager.canRedo());
    }

    void updatePresetName()
    {
        const auto presetName = presetManager.getCurrentPresetName();
        const auto displayName = presetName.isEmpty() ? juce::String("Untitled") : presetName;
        presetBox.setPresetName(presetManager.isPresetModified() ? displayName + "*" : displayName);
    }

    void updateReverseSignalButtonText()
    {
        reverseSignalOrderButton.setButtonText(reverseSignalOrderButton.getToggleState() ? "OPT-2A -> FET-76"
                                                                                         : "FET-76 -> OPT-2A");
    }

    const juce::Image screwImage {juce::ImageCache::getFromMemory(BinaryData::screw_png, BinaryData::screw_pngSize)};

    VCControlsLAF controlsLAF;
    PresetManager& presetManager;
    juce::UndoManager& undoManager;
    AAPresetBox presetBox;
    juce::DrawableButton saveButton {"Save preset", juce::DrawableButton::ImageFitted};
    BypassButton bypassPluginButton;
    juce::TextButton reverseSignalOrderButton;
    juce::DrawableButton undoButton {"Undo", juce::DrawableButton::ImageFitted};
    juce::DrawableButton redoButton {"Redo", juce::DrawableButton::ImageFitted};
    juce::AudioProcessorValueTreeState::ButtonAttachment reverseSignalOrderButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderPanel)
};
