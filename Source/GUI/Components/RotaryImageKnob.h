//
// Created by Aaron Petrini on 31/10/2026.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../LAF/RotaryImageKnobLookAndFeel.h"

class RotaryImageKnob : public juce::Component
{
public:
    RotaryImageKnob(juce::RangedAudioParameter& parameter, const juce::Image& knobImage)
    : knobLookAndFeel(knobImage),
      attachment(parameter, slider)
    {

        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);
        slider.setLookAndFeel(&knobLookAndFeel);

        slider.setDoubleClickReturnValue(true, parameter.convertFrom0to1(parameter.getDefaultValue()));

        addAndMakeVisible(slider);
    }

    void resized() override
    {
        slider.setBounds(getLocalBounds());
    }

private:
    const float pi = juce::MathConstants<float>::pi;
    RotaryImageKnobLookAndFeel knobLookAndFeel;
    juce::Slider slider;
    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryImageKnob)
};