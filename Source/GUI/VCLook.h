#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utilities/AAUITools.h"

namespace VCColors
{
    inline const juce::Colour background {juce::Colour(41, 42, 48).darker(1.f)};
    inline const juce::Colour separator {juce::Colour(245, 245, 245).withAlpha(0.5f)};
    inline const juce::Colour text {juce::Colour(255, 255, 255)};
    inline const juce::Colour displayText {juce::Colour(255, 255, 255).withAlpha(0.9f)};

    inline const juce::Colour buttonBackground {juce::Colour(100, 100, 110).darker(1.f)};
    inline const juce::Colour ratioButtonOff {juce::Colour(100, 100, 110).darker(0.5f)};
    inline const juce::Colour ratioButtonOn {juce::Colour(255, 0, 0).withAlpha(0.7f).darker(0.15f)};
    inline const juce::Colour presetBoxBackground {juce::Colour(85, 85, 85).darker(0.5f)};

    inline const juce::Colour bypassNormal {juce::Colour(255, 255, 255)};
    inline const juce::Colour bypassOver {juce::Colour(255, 255, 255).withAlpha(0.8f)};
    inline const juce::Colour bypassOn {juce::Colour(255, 0, 0).withAlpha(0.7f)};

    inline const juce::Colour ledOff {juce::Colour(100, 100, 110).darker(1.f)};

    inline const juce::Colour iconSVG {juce::Colour(255, 255, 255)};
    inline const juce::Colour iconNormal {juce::Colour(200, 200, 200).withAlpha(0.9f)};
    inline const juce::Colour iconOver {juce::Colour(120, 120, 120)};
    inline const juce::Colour iconActive {juce::Colour(64, 196, 255)};

    inline const juce::Colour popupBackground {juce::Colour(5, 7, 14)};
    inline const juce::Colour popupOutline {juce::Colour(40, 50, 70)};
    inline const juce::Colour popupText {juce::Colour(215, 215, 215)};
    inline const juce::Colour popupSelectedText {juce::Colour(64, 196, 255)};
    inline const juce::Colour popupRowHighlight {juce::Colour(35, 42, 60)};
    inline const juce::Colour popupIconSVG {juce::Colour(100, 100, 100)};
}

namespace VCFonts
{
    inline juce::Font barlowCondensedRegular(const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::BarlowCondensedRegular_ttf, BinaryData::BarlowCondensedRegular_ttfSize);
        return AAUITools::makeFont(typeface, fontHeight);
    }

    inline juce::Font barlowCondensedMedium(const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::BarlowCondensedMedium_ttf, BinaryData::BarlowCondensedMedium_ttfSize);
        return AAUITools::makeFont(typeface, fontHeight);
    }

    inline juce::Font barlowCondensedSemiBold(const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::BarlowCondensedSemiBold_ttf, BinaryData::BarlowCondensedSemiBold_ttfSize);
        return AAUITools::makeFont(typeface, fontHeight);
    }

    inline juce::Font barlowCondensedSemiBoldItalic(const float fontHeight)
    {
        static const juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::BarlowCondensedSemiBoldItalic_ttf, BinaryData::BarlowCondensedSemiBoldItalic_ttfSize);
        return AAUITools::makeFont(typeface, fontHeight);
    }
}

namespace VCLook
{
    inline constexpr float popupCornerPOH {0.0f};
    inline constexpr float outlineThicknessPOH {0.06f};
    inline constexpr float fontHeightPOH {0.625f};
    inline constexpr float popupTitleRowRatio {1.1f};

    inline constexpr float buttonFontPOH {0.5f};
    inline constexpr float comboBoxFontPOH {0.5f};
    inline constexpr float comboBoxMenuItemPOH {0.66f};

    inline void setupLabel(juce::Label& label)
    {
        label.setText(label.getName(), juce::dontSendNotification);
        label.setColour(juce::Label::ColourIds::textColourId, VCColors::text);
        label.setJustificationType(juce::Justification::centred);
    }

    inline void drawUnitTitle(juce::Graphics& g, const juce::String& title, juce::Rectangle<float> boundsF)
    {
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto lineThickness = boundsFWidth * 0.0033f;
        g.setColour(VCColors::separator);
        g.fillRect(boundsF.withHeight(lineThickness));

        g.setColour(VCColors::text);
        g.setFont(VCFonts::barlowCondensedSemiBoldItalic(boundsFWidth * 0.0333f));
        g.drawText(title, boundsF.withTrimmedTop(boundsFHeight * 0.0444f), juce::Justification::centredTop);
    }
}
