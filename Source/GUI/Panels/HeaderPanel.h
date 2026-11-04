#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../LAF/UnitTitleButtonLAF.h"
#include "../Components/AAPresetBox.h"

class HeaderPanel : public juce::Component, private juce::ChangeListener
{
public:
    explicit HeaderPanel(VocalCompressorAudioProcessor& processor)
    : presetManager(processor.presetManager),
      undoManager(processor.undoManager),
      presetBox(processor.presetManager),
      bypassPluginButtonAttachment(processor.apvts, ParamNames::getGeneralBypassName(), bypassPluginButton),
      unitTitle1176ButtonAttachment(processor.apvts, ParamNames::get1176BypassName(), unitTitle1176Button),
      unitTitleLA2AButtonAttachment(processor.apvts, ParamNames::getLA2ABypassName(), unitTitleLA2AButton),
      reverseSignalOrderButtonAttachment(processor.apvts, ParamNames::getReverseSignalOrderName(), signalOrderButton)
    {
        const auto iconNormal = juce::Drawable::createFromImageData (BinaryData::arrowsIcon_svg,  BinaryData::arrowsIcon_svgSize);
        const auto iconOver = iconNormal->createCopy();
        const auto iconDown = iconNormal->createCopy();
        iconOver->replaceColour (VCColors::almostWhiteText, VCColors::iconOver);
        iconDown->replaceColour (VCColors::almostWhiteText,VCColors::iconActive);
        signalOrderButton.setImages (iconNormal.get(), iconOver.get(), iconDown.get(), nullptr, iconNormal.get(), iconOver.get(), iconDown.get());
        signalOrderButton.setColour(juce::DrawableButton::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        signalOrderButton.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, juce::Colours::transparentBlack);
        signalOrderButton.setClickingTogglesState(true);
        signalOrderButton.onStateChange = [this] { resized(); };

        for (auto* unitTitleButton : {&unitTitle1176Button, &unitTitleLA2AButton})
        {
            unitTitleButton->setLookAndFeel(&unitTitleLAF);
            unitTitleButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
            unitTitleButton->setClickingTogglesState(true);
            unitTitleButton->onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };
            addAndMakeVisible(*unitTitleButton);
        }

        presetBox.onNameClicked = [this] { if (onPresetNameClicked != nullptr) onPresetNameClicked(); };

        AAUITools::setIconButtonImages(saveButton, BinaryData::saveIcon2_svg, BinaryData::saveIcon2_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        saveButton.onClick = [this] { if (onSaveClicked != nullptr) onSaveClicked(); };

        AAUITools::setIconButtonImages(bypassPluginButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);

        bypassPluginButton.setClickingTogglesState(true);
        bypassPluginButton.onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };

        AAUITools::setIconButtonImages(resizeButton, BinaryData::resizeSquare2_svg, BinaryData::resizeSquare2_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        resizeButton.setToggleable(true);
        resizeButton.setClickingTogglesState(false);

        AAUITools::setIconButtonImages(undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        AAUITools::setIconButtonImages(redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
        undoButton.onClick = [this] { undoManager.undo(); };
        redoButton.onClick = [this] { undoManager.redo(); };

        presetManager.addChangeListener(this);
        undoManager.addChangeListener(this);
        updatePresetName();
        updateUndoRedoButtons();

        addAndMakeVisible(presetBox);
        addAndMakeVisible(saveButton);
        addAndMakeVisible(bypassPluginButton);
        addAndMakeVisible(signalOrderButton);
        addAndMakeVisible(resizeButton);
        addAndMakeVisible(undoButton);
        addAndMakeVisible(redoButton);
    }

    ~HeaderPanel() override
    {
        presetManager.removeChangeListener(this);
        undoManager.removeChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();

        g.fillAll(VCColors::background);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();



        const auto presetBoxBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.33f, boundsFHeight * 0.7f);
        presetBox.setBounds(presetBoxBounds.toNearestInt());

        const auto iconSize = boundsFHeight * 0.65f;
        const auto iconButtonBounds = boundsF.withSizeKeepingCentre(iconSize, iconSize);
        saveButton.setBounds(iconButtonBounds.withX(presetBoxBounds.getRight() + iconSize * 0.5f).toNearestInt());
        bypassPluginButton.setBounds(iconButtonBounds.withRightX(boundsFWidth - iconSize * 0.5f).toNearestInt());
        resizeButton.setBounds(iconButtonBounds.withRightX(boundsFWidth - iconSize * 2.f).toNearestInt());

        const auto iconGap = iconSize * 0.5f;
        const auto unitTitleFont = VCFonts::barlowCondensedSemiBoldItalic(iconSize);
        const auto isOrderReversed = signalOrderButton.getToggleState();
        auto& firstTitleButton = isOrderReversed ? unitTitleLA2AButton : unitTitle1176Button;
        auto& secondTitleButton = isOrderReversed ? unitTitle1176Button : unitTitleLA2AButton;
        const auto titleWidth = juce::jmax(juce::GlyphArrangement::getStringWidth(unitTitleFont, unitTitle1176Button.getButtonText()),
                                           juce::GlyphArrangement::getStringWidth(unitTitleFont, unitTitleLA2AButton.getButtonText())) + iconGap;
        const auto titlesGroupX = (presetBoxBounds.getX() - (titleWidth * 2.f + iconSize)) * 0.5f;
        firstTitleButton.setBounds(juce::Rectangle<float>(titlesGroupX, iconButtonBounds.getY(), titleWidth, iconSize).toNearestInt());
        signalOrderButton.setBounds(iconButtonBounds.withX(titlesGroupX + titleWidth).toNearestInt());
        secondTitleButton.setBounds(juce::Rectangle<float>(titlesGroupX + titleWidth + iconSize, iconButtonBounds.getY(), titleWidth, iconSize).toNearestInt());

        const auto saveButtonRight = presetBoxBounds.getRight() + iconGap + iconSize;
        const auto resizeButtonX = boundsFWidth - iconSize * 3.f;
        const auto undoRedoCentreX = (saveButtonRight + resizeButtonX) * 0.5f;
        const auto undoRedoOffset = (iconSize + iconGap) * 0.5f;
        undoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX - undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
        redoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX + undoRedoOffset, boundsF.getCentreY()}).toNearestInt());
    }

    AAPresetBox& getPresetBox() { return presetBox; }
    juce::DrawableButton& getResizeButton() { return resizeButton; }

    std::function<void()> onPresetNameClicked;
    std::function<void()> onSaveClicked;
    std::function<void()> onBypassChanged;

    bool isBypassed() const { return bypassPluginButton.getToggleState(); }
    bool is1176Bypassed() const { return unitTitle1176Button.getToggleState(); }
    bool isLA2ABypassed() const { return unitTitleLA2AButton.getToggleState(); }


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

    // const juce::Image screwImage {juce::ImageCache::getFromMemory(BinaryData::screw_png, BinaryData::screw_pngSize)};

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
