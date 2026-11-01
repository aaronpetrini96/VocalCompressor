//
// Created by Aaron Petrini on 31/10/2026.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class RotaryImageKnob : public juce::Slider
{
public:
    RotaryImageKnob(juce::RangedAudioParameter& parameter, const juce::Image& knobImageToUse, const juce::Image& pointerImageToUse = {})
    : knobImage(knobImageToUse),
      pointerImage(pointerImageToUse),
      attachment(parameter, *this)
    {
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
        setMouseDragSensitivity(300);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        setRotaryParameters(1.25f * pi, 2.75f * pi, true);

        setDoubleClickReturnValue(true, parameter.convertFrom0to1(parameter.getDefaultValue()));
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto knobSize = juce::jmin(boundsF.getWidth(), boundsF.getHeight());
        const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
        const auto centre = boundsF.getCentre();

        const auto rotaryParameters = getRotaryParameters();
        const auto sliderPos = static_cast<float>(valueToProportionOfLength(getValue()));
        const auto angle = rotaryParameters.startAngleRadians + sliderPos * (rotaryParameters.endAngleRadians - rotaryParameters.startAngleRadians);

        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        const auto hasPointer = pointerImage.isValid();

        if (hasPointer)
            g.drawImage(knobImage, knobBounds, juce::RectanglePlacement::centred);

        juce::Graphics::ScopedSaveState state(g);
        g.addTransform(juce::AffineTransform::rotation(angle, centre.x, centre.y));
        g.drawImage(hasPointer ? pointerImage : knobImage, knobBounds, juce::RectanglePlacement::centred);
    }

private:
    const float pi = juce::MathConstants<float>::pi;
    juce::Image knobImage;
    juce::Image pointerImage;
    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryImageKnob)
};
