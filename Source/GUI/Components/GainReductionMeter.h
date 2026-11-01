#pragma once
#include <JuceHeader.h>
#include "../VCLook.h"

class GainReductionMeter : public juce::Component
{
public:
    GainReductionMeter()
    {
        numLEDs = 8;
        maxReduction = 20.f;
        currentReduction = 0.f;
    }

    void setReduction(float gr)
    {
        currentReduction = juce::jlimit(0.f, maxReduction, gr);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Layout
        float ledSpacing = bounds.getHeight() / (float) numLEDs;
        float ledHeight  = ledSpacing * 0.7f;
        float ledWidth   = ledHeight * 1.5f;

        float x = bounds.getCentreX() - ledWidth * 0.5f;

        int numLit = (int) juce::jmap(currentReduction,
                                      0.f, maxReduction,
                                      0.f, (float) numLEDs);

        for (int i = 0; i < numLEDs; ++i)
        {
            // Bottom → top
            float y = bounds.getBottom()
                      - (i + 1) * ledSpacing
                      + (ledSpacing - ledHeight) * 0.5f;

            juce::Colour ledColour = VCColors::ledOff;

            if (i < numLit)
            {
                // 0 = green, 1 = red
                float t = (float) i / (float) (numLEDs - 1);

                // Hue: 0.33 (green) → 0.0 (red)
                float hue = juce::jmap(t, 0.33f, 0.0f);

                ledColour = juce::Colour::fromHSV(
                    hue,
                    0.9f,   // saturation
                    0.8f,   // brightness
                    1.0f
                );
            }

            g.setColour(ledColour);
            g.fillRoundedRectangle(x, y, ledWidth, ledHeight, ledHeight * 0.25f);
        }
    }


private:
    int numLEDs;
    float maxReduction;
    float currentReduction;
};

