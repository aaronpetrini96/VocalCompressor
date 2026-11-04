#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../VCLook.h"

class UnitTitleButtonLAF : public juce::LookAndFeel_V4
{
public:
    UnitTitleButtonLAF() = default;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override {}

    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isHighlighted, bool) override
    {
        const auto boundsF = button.getLocalBounds().toFloat();
        const auto activeColour = isHighlighted ? VCColors::iconActiveOver : VCColors::iconActive;
        const auto inactiveColour = isHighlighted ? VCColors::iconOver : button.findColour(juce::TextButton::textColourOffId);
        const auto textColour = button.getToggleState() ? activeColour : inactiveColour;

        g.setColour(textColour);
        g.setFont(VCFonts::barlowCondensedSemiBoldItalic(boundsF.getHeight()));
        g.drawText(button.getButtonText(), boundsF, juce::Justification::centred, true);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTitleButtonLAF)
};
