//
// Created by Aaron Petrini on 18/09/2026.
//

#pragma once
#include "../VCLook.h"

class PresetPopupLAF : public juce::LookAndFeel_V4
{
public:
    PresetPopupLAF()
    {
        setColour (juce::TextEditor::backgroundColourId, VCColors::presetBoxBackground);
        setColour (juce::TextEditor::textColourId, VCColors::popupText);
    }

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool , bool) override { }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto fontHeight = boundsF.getHeight() * VCLook::fontHeightPOH;
        const auto cornerSize = boundsF.getHeight() * VCLook::popupCornerPOH;
        const auto outlineThickness = boundsF.getHeight() * VCLook::outlineThicknessPOH;

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (VCColors::presetBoxBackground);
            g.fillRoundedRectangle (boundsF, cornerSize);
        }

        g.setColour (VCColors::presetBoxOutline.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.drawRoundedRectangle (boundsF.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);

        g.setColour ((shouldDrawButtonAsHighlighted ? VCColors::popupText.withMultipliedBrightness (0.6f) : VCColors::popupText).withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
        g.setFont (VCFonts::barlowCondensedMedium (fontHeight));
        g.drawFittedText (button.getButtonText(), boundsF.reduced (boundsF.getWidth() * 0.05f, boundsF.getHeight() * 0.04f).toNearestInt(), juce::Justification::centred, 1);
    }

    void fillTextEditorBackground (juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override
    {
        const auto boundsF = juce::Rectangle<int> (width, height).toFloat();
        const auto cornerSize = boundsF.getHeight() * VCLook::popupCornerPOH;
        AAUITools::drawOutlinedRoundedRectangle (g, boundsF, cornerSize, boundsF.getHeight() * VCLook::outlineThicknessPOH,
                                                 textEditor.findColour (juce::TextEditor::backgroundColourId), VCColors::presetBoxOutline);
    }

    void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override { }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPopupLAF)
};
