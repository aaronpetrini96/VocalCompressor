//
// Created by Aaron Petrini on 31/10/2026.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class RotaryImageKnob : public juce::Slider
{
public:
    RotaryImageKnob(juce::RangedAudioParameter& parameter, const juce::Image& knobImageToUse, const juce::Image& rotorImageToUse = {}, const int mouseSensitivity = 250)
    : knobImage(knobImageToUse), rotorImage(rotorImageToUse), attachment(parameter, *this)
    {
        if (!knobImage.isValid())
            return;

        setSliderStyle(juce::Slider::RotaryVerticalDrag);
        setMouseDragSensitivity(mouseSensitivity);
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

        g.setOpacity(0.9f);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        const auto hasRotor = rotorImage.isValid();

        if (hasRotor)
            g.drawImage(knobImage, knobBounds, juce::RectanglePlacement::centred);

        juce::Graphics::ScopedSaveState state(g);
        g.addTransform(juce::AffineTransform::rotation(angle, centre.x, centre.y));
        g.drawImage(hasRotor ? rotorImage : knobImage, knobBounds, juce::RectanglePlacement::centred);
    }


private:
    const float pi = juce::MathConstants<float>::pi;
    juce::Image knobImage;
    juce::Image rotorImage;
    juce::SliderParameterAttachment attachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryImageKnob)
};
