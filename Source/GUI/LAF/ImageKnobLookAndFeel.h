#pragma once
#include <JuceHeader.h>

class ImageKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ImageKnobLookAndFeel();
    ImageKnobLookAndFeel(const juce::Image& img);
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
    
    
    static ImageKnobLookAndFeel* get()
    {
        static ImageKnobLookAndFeel instance;
        return &instance;
    }

private:
    juce::Image knobImage;
    juce::Slider slider;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageKnobLookAndFeel)
};

