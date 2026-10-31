#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class RotaryImageKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    explicit RotaryImageKnobLookAndFeel(const juce::Image& image)
    : knobImage(image)
    {
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override
    {
        const auto boundsF = juce::Rectangle<int>(x, y, width, height).toFloat();
        const float knobSize = juce::jmin(boundsF.getWidth(), boundsF.getHeight());
        const auto centre = boundsF.getCentre();
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        const float imageScale = knobSize / static_cast<float>(knobImage.getWidth());
        const auto transform = juce::AffineTransform::translation(-knobImage.getWidth() * 0.5f, -knobImage.getHeight() * 0.5f)
                                   .scaled(imageScale)
                                   .rotated(angle)
                                   .translated(centre.x, centre.y);

        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImageTransformed(knobImage, transform);
    }

private:
    juce::Image knobImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryImageKnobLookAndFeel)
};
