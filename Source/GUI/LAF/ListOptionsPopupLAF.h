//
// Created by Aaron Petrini on 19/09/2026.
//

#pragma once
#include "../VCLook.h"

class ListOptionsPopupLAF : public juce::LookAndFeel_V4
{
public:
    ListOptionsPopupLAF () = default;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, const bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * VCLook::popupCornerPOH;
        const auto textColour = button.getToggleState() ? VCColors::darkRed : (shouldDrawButtonAsHighlighted ? VCColors::fadedWhiteText : VCColors::almostWhiteText);
        const auto fontText = VCFonts::barlowCondensedMedium (boundsF.getHeight() * VCLook::fontHeightPOH);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (VCColors::popupRowHighlight);
            g.fillRoundedRectangle (boundsF.reduced (boundsF.getHeight() * VCLook::outlineThicknessPOH, 0.f), cornerSize);
        }

        g.setColour (textColour);
        if (!button.isEnabled())
            g.setColour (textColour.withAlpha (0.3f));
        g.setFont (fontText);
        const auto textBounds = leftAlignText ? boundsF.withLeft (boundsF.getHeight() * 0.45f) : boundsF;
        g.drawText (button.getButtonText(), textBounds, leftAlignText ? juce::Justification::centredLeft : juce::Justification::centred, true);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour (VCColors::popupRowHighlight);
            g.fillRoundedRectangle (boundsF.reduced (boundsFHeight * VCLook::outlineThicknessPOH, 0.f), boundsFHeight * 0.15f);
        }

        const auto tickBoxSize = boundsFHeight * 0.55f;
        const auto tickBoxBounds = boundsF.withSizeKeepingCentre (tickBoxSize, tickBoxSize).withRightX (boundsF.getRight() - boundsFHeight * 0.45f);
        const auto tickBoxLineThickness = tickBoxSize * 0.08f;
        g.setColour (button.getToggleState() ? VCColors::popupText.withMultipliedBrightness(2.f)  : (shouldDrawButtonAsHighlighted ? VCColors::iconOver : VCColors::popupText));
        g.drawRoundedRectangle (tickBoxBounds, tickBoxSize * 0.2f, tickBoxLineThickness);

        if (button.getToggleState())
        {
            juce::Path tick;
            tick.startNewSubPath (tickBoxBounds.getRelativePoint (0.25f, 0.52f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.43f, 0.7f));
            tick.lineTo (tickBoxBounds.getRelativePoint (0.75f, 0.32f));
            g.setColour (VCColors::popupText.withMultipliedBrightness(2.f));
            g.strokePath (tick, juce::PathStrokeType (tickBoxLineThickness * 1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        g.setColour (shouldDrawButtonAsHighlighted ? VCColors::fadedWhiteText : VCColors::almostWhiteText);
        g.setFont (VCFonts::barlowCondensedMedium (boundsFHeight * VCLook::fontHeightPOH));
        g.drawText (button.getButtonText(), boundsF.withLeft (boundsFHeight * 0.45f).withRight (tickBoxBounds.getX()), juce::Justification::centredLeft, true);
    }

    bool leftAlignText {false};
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
