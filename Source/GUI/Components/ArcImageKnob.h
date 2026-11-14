//
// Created by Aaron Petrini on 06/11/2026.
//

#pragma once
#include <numbers>
#include <juce_audio_processors/juce_audio_processors.h>

class ArcImageKnob : public juce::Slider
{
public:
    ArcImageKnob(juce::RangedAudioParameter& parameter, const juce::Image& imageToUse)
    : image(imageToUse), attachment(parameter, *this)
    {
        constexpr auto pi = std::numbers::pi_v<float>;
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
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

        const auto arcRadius = knobSize * 0.425f;
        const juce::PathStrokeType arcStroke(knobSize * 0.0427f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt);

        juce::Path trackArc;
        trackArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.f, rotaryParameters.startAngleRadians, rotaryParameters.endAngleRadians, true);
        g.setColour(findColour(juce::Slider::rotarySliderFillColourId).withAlpha(0.275f));
        g.strokePath(trackArc, arcStroke);

        if (angle > rotaryParameters.startAngleRadians)
        {
            juce::Path fillArc;
            fillArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.f, rotaryParameters.startAngleRadians, angle, true);
            g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(fillArc, arcStroke);
        }

        const auto tickWidth = knobSize * 0.05f;
        const auto tickLength = knobSize * 0.1f;
        const auto tickTopY = centre.y - knobSize * 0.353f;
        juce::Path tick;
        tick.addRoundedRectangle(centre.x - tickWidth * 0.5f, tickTopY, tickWidth, tickLength, tickWidth * 0.5f);
        g.setColour(findColour(juce::Slider::thumbColourId));
        g.fillPath(tick, juce::AffineTransform::rotation(angle, centre.x, centre.y));
    }

private:
    juce::Image image;
    juce::SliderParameterAttachment attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArcImageKnob)
};
