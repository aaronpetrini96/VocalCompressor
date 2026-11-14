//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../LAF/PresetPopupLAF.h"

class AAPresetPopup : public juce::Component
{
public:
    AAPresetPopup()
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&presetPopupLAF);

        nameEditor.setJustification (juce::Justification::centred);
        nameEditor.setIndents(0, 0);
        nameEditor.setInputRestrictions (maxNameLength);
        nameEditor.setEscapeAndReturnKeysConsumed (false);
        setWantsKeyboardFocus (true);
        nameEditor.onTextChange = [this]
        {
            updateConfirmEnabled (nameEditor.getText());
            isConfirmingOverwrite = false;
            confirmButton.setButtonText ("Save");
        };
        addChildComponent (nameEditor);

        messageLabel.setJustificationType (juce::Justification::centred);
        messageLabel.setColour (juce::Label::textColourId, VCColors::popupLabelText);
        messageLabel.setInterceptsMouseClicks (false, false);
        addChildComponent (messageLabel);

        nameLabel.setJustificationType (juce::Justification::centred);
        nameLabel.setColour (juce::Label::textColourId, VCColors::popupLabelText);
        nameLabel.setInterceptsMouseClicks (false, false);
        addChildComponent (nameLabel);

        confirmButton.onClick = [this]
        {
            const auto presetName = nameEditor.getText();

            if (! isConfirmingOverwrite && presetExists != nullptr && presetExists (presetName))
            {
                isConfirmingOverwrite = true;
                confirmButton.setButtonText ("Overwrite");
                nameEditor.setVisible (false);
                nameLabel.setVisible (false);
                messageLabel.setText ("Overwrite \"" + presetName + "\"?", juce::dontSendNotification);
                messageLabel.setVisible (true);
                grabKeyboardFocus();
                return;
            }

            if (onConfirm != nullptr)
                onConfirm (presetName);
        };
        cancelButton.onClick = [this] { setVisible (false); };

        for (auto* button : {&confirmButton, &cancelButton})
            addAndMakeVisible (button);
    }

    ~AAPresetPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * VCLook::popupCornerPOH;
        const auto outlineThickness = boundsF.getHeight() / numRows * VCLook::outlineThicknessPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, cornerSize, outlineThickness, VCColors::popupBackground, VCColors::popupOutline);

        juce::Path titleShape;
        titleShape.addRoundedRectangle (titleBounds.getX(), titleBounds.getY(), titleBounds.getWidth(), titleBounds.getHeight(), cornerSize, cornerSize,
                                        true, true, false, false);
        g.setColour (VCColors::popupRowHighlight);
        g.fillPath (titleShape);

        g.setColour (VCColors::popupOutline);
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - outlineThickness * 0.5f));

        g.setColour (VCColors::popupTitleText);
        g.setFont (VCFonts::barlowCondensedSemiBold (titleBounds.getHeight() * VCLook::fontHeightPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();
        const auto lineHeight = boundsFHeight / numRows;
        const auto fontHeight = lineHeight * VCLook::fontHeightPOH;
        titleBounds = boundsF.withHeight (boundsFHeight / numRows * VCLook::popupTitleRowRatio);

        const auto nameEditorWidth = boundsFWidth * 0.85f;
        const auto nameEditorHeight = boundsFHeight * 0.2f;
        const auto nameEditorBounds = boundsF.withSizeKeepingCentre (nameEditorWidth, nameEditorHeight);
        nameEditor.applyFontToAllText (VCFonts::barlowCondensedRegular (fontHeight));
        nameEditor.setBounds (nameEditorBounds.toNearestInt());

        nameLabel.setFont (VCFonts::barlowCondensedRegular (fontHeight));
        nameLabel.setBounds (nameEditorBounds.withBottomY (nameEditorBounds.getY()).toNearestInt());

        messageLabel.setFont (VCFonts::barlowCondensedRegular (fontHeight));
        messageLabel.setBounds (nameEditorBounds.toNearestInt());

        const auto buttonWidth = boundsFWidth * 0.35f;
        const auto buttonsBounds = boundsF.withSizeKeepingCentre (buttonWidth, lineHeight).withY (boundsFHeight * 0.72f);
        confirmButton.setBounds (buttonsBounds.withCentre({ boundsFWidth * 0.3f, buttonsBounds.getCentreY()}).toNearestInt());
        cancelButton.setBounds (buttonsBounds.withCentre({boundsFWidth * 0.7f, buttonsBounds.getCentreY()}).toNearestInt());
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::returnKey)
        {
            confirmButton.triggerClick();
            return true;
        }

        if (key == juce::KeyPress::escapeKey)
        {
            cancelButton.triggerClick();
            return true;
        }

        return false;
    }

    void prepare (const juce::String& presetName)
    {
        isConfirmingOverwrite = false;
        confirmButton.setButtonText ("Save");
        nameEditor.setVisible (true);
        nameLabel.setVisible (true);
        messageLabel.setVisible (false);

        nameEditor.setText (presetName, juce::dontSendNotification);
        updateConfirmEnabled (presetName);
        nameEditor.grabKeyboardFocus();
        nameEditor.selectAll();
    }

    std::function<void (const juce::String& presetName)> onConfirm;
    std::function<bool (const juce::String& presetName)> presetExists;
    std::function<bool (const juce::String& presetName)> isNameReserved;
    float numRows {6.f};

private:
    void updateConfirmEnabled (const juce::String& presetName)
    {
        const auto isLegalName = juce::File::createLegalFileName (presetName.trim()).isNotEmpty();
        const auto isReserved = isNameReserved != nullptr && isNameReserved (presetName);
        confirmButton.setEnabled (isLegalName && ! isReserved);
    }

    PresetPopupLAF presetPopupLAF;
    const juce::String title {"SAVE PRESET"};
    static constexpr int maxNameLength {35};
    juce::Rectangle<float> titleBounds;
    juce::Label messageLabel {"messageLabel", ""};
    juce::Label nameLabel {"nameLabel", "New preset name"};
    juce::TextEditor nameEditor;
    juce::TextButton confirmButton, cancelButton {"Cancel"};
    bool isConfirmingOverwrite {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetPopup)
};
