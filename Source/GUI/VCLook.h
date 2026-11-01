#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace VCLook
{
    inline const juce::String labelFont {"Basic Comical NC"};
    inline const juce::String titleFont {"Barber Chop"};
    inline const juce::Colour separatorColour {juce::Colours::whitesmoke.withAlpha(0.5f)};

    inline void setupLabel(juce::Label& label, float fontHeight = 16.f)
    {
        label.setText(label.getName(), juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions(labelFont, fontHeight, juce::Font::FontStyleFlags::plain)));
        label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
        label.setJustificationType(juce::Justification::centred);
    }

    inline void drawUnitTitle(juce::Graphics& g, const juce::String& title, juce::Rectangle<float> boundsF)
    {
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto lineThickness = boundsFWidth * 0.0033f;
        g.setColour(separatorColour);
        g.fillRect(boundsF.withHeight(lineThickness));

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(juce::FontOptions(titleFont, boundsFWidth * 0.0333f, juce::Font::FontStyleFlags::italic)));
        g.drawText(title, boundsF.withTrimmedTop(boundsFHeight * 0.0444f), juce::Justification::centredTop);
    }
}
