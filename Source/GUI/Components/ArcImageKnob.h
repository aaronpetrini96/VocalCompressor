//
// Created by Aaron Petrini on 06/11/2026.
//

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class ArcImageKnob : public juce::Slider
{
public:
    ArcImageKnob(juce::RangedAudioParameter& parameter, const juce::Image& imageToUse)
    : image(imageToUse), attachment(parameter, *this)
    {
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
        setMouseDragSensitivity(250);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        setRotaryParameters(1.25f * pi, 2.75f * pi, true);
        setDoubleClickReturnValue(true, parameter.convertFrom0to1(parameter.getDefaultValue()));
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto knobSize = juce::jmin(boundsF.getWidth(), boundsF.getHeight());
        const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
        const auto centre = knobBounds.getCentre();

        const auto rotaryParameters = getRotaryParameters();
        const auto sliderPos = static_cast<float>(valueToProportionOfLength(getValue()));
        const auto angle = rotaryParameters.startAngleRadians + sliderPos * (rotaryParameters.endAngleRadians - rotaryParameters.startAngleRadians);

        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(image, knobBounds, juce::RectanglePlacement::centred);

        const auto arcRadius = knobSize * 0.4535f;
        const juce::PathStrokeType arcStroke(knobSize * 0.0427f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Path trackArc;
        trackArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.f, rotaryParameters.startAngleRadians, rotaryParameters.endAngleRadians, true);
        g.setColour(trackColour);
        g.strokePath(trackArc, arcStroke);

        if (angle > rotaryParameters.startAngleRadians)
        {
            juce::Path fillArc;
            fillArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.f, rotaryParameters.startAngleRadians, angle, true);
            g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(fillArc, arcStroke);
        }

        const auto tickWidth = knobSize * 0.036f;
        const auto tickLength = knobSize * 0.072f;
        const auto tickTopY = centre.y - knobSize * 0.353f;
        juce::Path tick;
        tick.addRoundedRectangle(centre.x - tickWidth * 0.5f, tickTopY, tickWidth, tickLength, tickWidth * 0.5f);
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillPath(tick, juce::AffineTransform::rotation(angle, centre.x, centre.y));
    }

private:
    const float pi = juce::MathConstants<float>::pi;
    const juce::Colour trackColour {juce::Colour(41, 41, 41).withAlpha(0.48f)};
    juce::Image image;
    juce::SliderParameterAttachment attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArcImageKnob)
};
