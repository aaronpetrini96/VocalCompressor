//
// Created by Aaron Petrini on 03/09/2026.
//

#pragma once
#include "../VCLook.h"

class PresetBrowserLAF : public juce::LookAndFeel_V4
{
public:
    PresetBrowserLAF()
    {
        setColour (juce::PopupMenu::textColourId, VCColors::popupText);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, VCColors::presetBoxBackground);
        setColour (juce::PopupMenu::highlightedTextColourId, VCColors::popupText);
    }

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override
    {
        g.fillAll (VCColors::popupBackground);
        g.setColour (VCColors::popupOutline);
        g.drawRect (juce::Rectangle<int> (width, height).toFloat(), 1.0f);
    }

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

            g.setColour (isSelected ? VCColors::presetBoxBackground : VCColors::popupBackground);
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
            g.setColour (VCColors::presetBoxBackground);
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
        const auto tabButtonTextColour = isSelected || isHighlighted ? VCColors::popupText.withBrightness (2.f) : VCColors::popupText.withMultipliedAlpha (0.4f);
        const auto presetTextColour = isSelected ? VCColors::popupText.withMultipliedBrightness (2.f) : VCColors::popupText.withAlpha (0.8f);
        const auto buttonTextColour = isTabButton ? tabButtonTextColour : presetTextColour;

        g.setColour (buttonTextColour);
        const auto fontHeight = boundsF.getHeight() * VCLook::fontHeightPOH;
        g.setFont (! isTabButton && isSelected ? VCFonts::barlowCondensedSemiBold (fontHeight) : VCFonts::barlowCondensedMedium (fontHeight));
        g.drawText (isTabButton ? button.getButtonText().toUpperCase() : button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserLAF)
};
