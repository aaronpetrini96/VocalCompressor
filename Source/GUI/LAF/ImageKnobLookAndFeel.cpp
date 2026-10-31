
#include "ImageKnobLookAndFeel.h"

ImageKnobLookAndFeel::ImageKnobLookAndFeel() {}

ImageKnobLookAndFeel::ImageKnobLookAndFeel(const juce::Image& img)
{
    knobImage = img;
}


void ImageKnobLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                            float sliderPosProportional, float rotaryStartAngle,
                                            float rotaryEndAngle, juce::Slider &slider)
{
    if (!knobImage.isValid())
    {
        g.setColour(juce::Colours::red);
        g.drawFittedText("No Image", x, y, width, height, juce::Justification::centred, 1);
        return;
    }

    // === Common values ===
    const float usableRadius = juce::jmin(width, height) * 0.5f;
    const juce::Point<float> center(x + width * 0.5f, y + height * 0.5f);

    // === Slider normalized value ===
    const double sliderRange = slider.getMaximum() - slider.getMinimum();
    const double rawValue = (slider.getValue() - slider.getMinimum()) / sliderRange;
    const double normalizedValue = juce::jlimit(0.0, 1.0, rawValue);

    const int frames = knobImage.getHeight() / knobImage.getWidth();
    const int frameId = (int)std::round(normalizedValue * (frames - 1));

    // === Arc / knob angles ===
    const float startAngle = juce::MathConstants<float>::pi * 1.25f;
    const float endAngle   = juce::MathConstants<float>::pi * 2.75f;

    // === Tick parameters (proportional to usable radius) ===
    const int numTicks = 11;
    const float tickLength = usableRadius * 0.1f;      // 10% of radius
    const float tickThickness = juce::jmax(1.0f, usableRadius * 0.02f);
    const float tickMargin = usableRadius * 0.08f;    // space between knob and ticks
    const juce::Colour tickColor = juce::Colours::white;

    // === Arc radius for ticks ===
    const float tickRadius = usableRadius;

    // === Draw ticks around knob ===
    g.setColour(tickColor);
    g.saveState();
    for (int i = 0; i < numTicks; ++i)
    {
        float proportion = (float)i / (numTicks - 1);
        float angle = startAngle + proportion * (endAngle - startAngle);

        juce::Point<float> inner = center.getPointOnCircumference(tickRadius - tickLength, angle);
        juce::Point<float> outer = center.getPointOnCircumference(tickRadius, angle);

        g.drawLine({inner, outer}, tickThickness);
    }

    // Highlight active tick
    int activeTick = (int)std::round(normalizedValue * (numTicks - 1));
    float activeAngle = startAngle + (float)activeTick / (numTicks - 1) * (endAngle - startAngle);
    juce::Point<float> innerActive = center.getPointOnCircumference(tickRadius - tickLength, activeAngle);
    juce::Point<float> outerActive = center.getPointOnCircumference(tickRadius, activeAngle);
//    g.setColour(juce::Colour::fromRGB(200, 30, 100).withAlpha(0.8f));
    g.setColour(juce::Colours::red.withAlpha(0.9f));
    g.drawLine({innerActive, outerActive}, tickThickness + 1.0f);
    g.restoreState();

    // === Draw knob image (scaled to leave room for ticks) ===
    const float imageRadius = tickRadius - tickLength - tickMargin;
    const float imageScale = (imageRadius * 2.0f) / (float)knobImage.getWidth();
    const int imageSize = knobImage.getWidth();

    g.setOpacity(0.8f);
    g.drawImageTransformed(
        knobImage.getClippedImage(juce::Rectangle<int>(0, frameId * imageSize, imageSize, imageSize)),
        juce::AffineTransform::scale(imageScale)
                              .translated(center.x - imageRadius, center.y - imageRadius)
    );
    g.setOpacity(1.f);
}


