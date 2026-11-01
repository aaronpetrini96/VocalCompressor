#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace VCColors
{
    inline const juce::Colour background {juce::Colour(41, 42, 48).darker(1.f)};
    inline const juce::Colour separator {juce::Colour(245, 245, 245).withAlpha(0.5f)};
    inline const juce::Colour text {juce::Colour(255, 255, 255)};
    inline const juce::Colour displayText {juce::Colour(255, 255, 255).withAlpha(0.9f)};
    inline const juce::Colour valueText {juce::Colour(255, 255, 255).darker(0.2f)};

    inline const juce::Colour buttonBackground {juce::Colour(100, 100, 110).darker(1.f)};
    inline const juce::Colour ratioButtonOff {juce::Colour(100, 100, 110).darker(0.5f)};
    inline const juce::Colour ratioButtonOn {juce::Colour(255, 0, 0).withAlpha(0.7f).darker(0.15f)};
    inline const juce::Colour presetBoxBackground {juce::Colour(85, 85, 85).darker(0.5f)};

    inline const juce::Colour bypassNormal {juce::Colour(255, 255, 255)};
    inline const juce::Colour bypassOver {juce::Colour(255, 255, 255).withAlpha(0.8f)};
    inline const juce::Colour bypassOn {juce::Colour(255, 0, 0).withAlpha(0.7f)};

    inline const juce::Colour knobTick {juce::Colour(255, 255, 255)};
    inline const juce::Colour knobActiveTick {juce::Colour(255, 0, 0).withAlpha(0.9f)};
    inline const juce::Colour missingImage {juce::Colour(255, 0, 0)};
    inline const juce::Colour ledOff {juce::Colour(100, 100, 110).darker(1.f)};
}

namespace VCLook
{
    inline const juce::String labelFont {"Basic Comical NC"};
    inline const juce::String titleFont {"Barber Chop"};

    inline void setupLabel(juce::Label& label, float fontHeight = 16.f)
    {
        label.setText(label.getName(), juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions(labelFont, fontHeight, juce::Font::FontStyleFlags::plain)));
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
        g.setFont(juce::Font(juce::FontOptions(titleFont, boundsFWidth * 0.0333f, juce::Font::FontStyleFlags::italic)));
        g.drawText(title, boundsF.withTrimmedTop(boundsFHeight * 0.0444f), juce::Justification::centredTop);
    }
}
