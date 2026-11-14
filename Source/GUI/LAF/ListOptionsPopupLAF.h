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
        const auto textColour = button.getToggleState() ? VCColors::popupTextSelected : (shouldDrawButtonAsHighlighted ? VCColors::popupTextHover : VCColors::popupText);
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
        g.drawText (button.getButtonText(), boundsF, juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListOptionsPopupLAF)
};
