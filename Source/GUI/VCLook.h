#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utilities/AAUITools.h"

namespace VCColors
{
    inline const juce::Colour background {20, 21, 24};
    inline const juce::Colour almostBlack {5, 5, 5};
    inline const juce::Colour darkRed {200, 30, 30};
    inline const juce::Colour fadedWhiteText {200, 200, 200};
    inline const juce::Colour almostWhite {240, 240, 240};

    inline const juce::Colour buttonBackground {50, 50, 55};
    inline const juce::Colour ratioButtonOff {66, 66, 73};
    inline const juce::Colour presetBoxBackground {56, 56, 56};
    inline const juce::Colour presetBoxOutline {40, 50, 70};

    inline const juce::Colour ledOff {50, 50, 55};
    inline const juce::Colour iconOver {180, 180, 180};
    inline const juce::Colour iconActive = darkRed;
    inline const juce::Colour iconActiveOver {140, 0, 0};

    inline const juce::Colour popupBackground {5, 7, 14};
    inline const juce::Colour popupOutline {40, 50, 70};
    inline const juce::Colour popupText = fadedWhiteText;
    inline const juce::Colour popupRowHighlight {35, 42, 60};
    inline const juce::Colour popupSelectedText = almostWhite;
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
    inline constexpr float fontHeightPOH {0.7f};
    inline constexpr float popupTitleRowRatio {1.1f};
    inline constexpr float rowPOH {0.045f};
    inline constexpr int presetNameMaxLength {40};

    inline constexpr float buttonFontPOH {0.5f};
    inline constexpr float comboBoxFontPOH {0.5f};
    inline constexpr float comboBoxMenuItemPOH {0.66f};

    inline void setupLabel(juce::Label& label, const juce::Colour& labelColour = juce::Colours::white, const juce::Justification justification = juce::Justification::centred)
    {
        label.setText(label.getName(), juce::dontSendNotification);
        label.setColour(juce::Label::ColourIds::textColourId, labelColour);
        label.setJustificationType(justification);
    }
}
