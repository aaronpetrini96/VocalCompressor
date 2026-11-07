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

        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(getFontForTextHeight(button.getButtonText(), boundsF.getHeight()), button.getButtonText(), 0.f, 0.f);
        juce::Path textPath;
        glyphs.createPath(textPath);

        g.setColour(textColour);
        g.fillPath(textPath, juce::AffineTransform::translation(boundsF.getCentre() - textPath.getBounds().getCentre()));
    }

    static juce::Font getFontForTextHeight(const juce::String& text, const float textHeight)
    {
        juce::GlyphArrangement referenceGlyphs;
        referenceGlyphs.addLineOfText(VCFonts::barlowCondensedSemiBoldItalic(textHeight), text, 0.f, 0.f);
        juce::Path referencePath;
        referenceGlyphs.createPath(referencePath);
        const auto referenceTextHeight = referencePath.getBounds().getHeight();
        return VCFonts::barlowCondensedSemiBoldItalic(textHeight * textHeight / referenceTextHeight);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTitleButtonLAF)
};
