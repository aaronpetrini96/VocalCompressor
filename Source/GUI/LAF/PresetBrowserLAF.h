//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../VCLook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF() = default;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& , bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto cornerSize = boundsF.getHeight() * VCLook::popupCornerPOH;
        const auto outlineThickness = boundsF.getHeight() * VCLook::outlineThicknessPOH;

        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();

        if (isTabButton)
        {
            const auto tabLineThickness = boundsF.getHeight() / VCLook::popupTitleRowRatio * VCLook::outlineThicknessPOH * 0.5f;
            juce::Path tabShape;
            tabShape.addRoundedRectangle (boundsF.getX(), boundsF.getY(), boundsF.getWidth(), boundsF.getHeight(), cornerSize, cornerSize,
                                          button.isConnectedOnRight(), button.isConnectedOnLeft(), false, false);

            g.setColour (isSelected ? VCColors::popupOutline : VCColors::popupBackground);
            g.fillPath (tabShape);

            g.setColour (VCColors::popupOutline);
            g.fillRect (boundsF.withTop (boundsF.getBottom() - tabLineThickness));

            if (button.isConnectedOnRight())
                g.fillRect (boundsF.withLeft (boundsF.getRight() - tabLineThickness * 0.5f));

            if (button.isConnectedOnLeft())
                g.fillRect (boundsF.withRight (boundsF.getX() + tabLineThickness * 0.5f));
        }

        if (!isTabButton && isHighlighted)
        {
            g.setColour (VCColors::popupRowHighlight);
            g.fillRoundedRectangle (boundsF.reduced (outlineThickness, 0.f), cornerSize);
        }
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isHighlighted, bool) override
    {
        //bounds
        const auto boundsF = button.getLocalBounds().toFloat();
        //bool checkers
        const auto isTabButton = button.getConnectedEdgeFlags() != 0;
        const auto isSelected = button.getToggleState();

        //Colors
        const auto tabButtonTextColour = isSelected ? VCColors::popupSelectedText : VCColors::fadedWhiteText.withMultipliedAlpha (isHighlighted ? 0.8f : 0.4f);
        const auto presetTextColour = isHighlighted ? VCColors::fadedWhiteText : VCColors::almostWhiteText;
        const auto buttonTextColour = isTabButton ? tabButtonTextColour : presetTextColour;

        g.setColour (buttonTextColour);
        const auto fontHeight = boundsF.getHeight() * VCLook::fontHeightPOH;
        g.setFont (isTabButton ?VCFonts::barlowCondensedSemiBold (fontHeight) : VCFonts::barlowCondensedRegular (fontHeight));
        g.drawText (isTabButton ? button.getButtonText().toUpperCase() : button.getButtonText(), boundsF, juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
