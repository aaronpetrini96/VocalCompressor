/*
  ==============================================================================

    GRTextDisplay.h
    Created: 5 Jan 2026 11:47:55pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class GRTextDisplay : public juce::Component
{
public:
    void setValue(float gr)
    {
        value = gr;
        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::Font(juce::FontOptions("Basic Comical NC", 13.f, juce::Font::plain)));
        g.drawText(juce::String(value, 1) + " dB", getLocalBounds(), juce::Justification::centred);
    }
    
private:
    float value = 0.f;
};
