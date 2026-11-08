#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../VCLook.h"

class GRDistressorMeter : public juce::Component, private juce::Timer
{
public:
    explicit GRDistressorMeter (std::atomic<float>& source, juce::Colour labelTextColour) : gainReductionSource (source), labelColour (labelTextColour)
    {
        smoothingCoefficient = 1.f - std::exp (-1.f / (static_cast<float> (refreshRateHz) * smoothingTimeSeconds));
        startTimerHz (refreshRateHz);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto columnWidth = boundsF.getWidth() / static_cast<float> (numberOfLights);
        const auto halfHeight = boundsF.getHeight() * 0.5f;
        const auto labelFont = VCFonts::barlowCondensedSemiBold (halfHeight * 0.925f);

        for (const auto isGlowPass : { true, false })
        {
            for (size_t index {0}; index < numberOfLights; ++index)
            {
                const auto columnX = boundsF.getRight() - columnWidth * static_cast<float> (index + 1);
                paintLight (g, {columnX, halfHeight, columnWidth, halfHeight}, index, isGlowPass);

                if (! isGlowPass)
                {
                    g.setColour (labelColour);
                    g.setFont (labelFont);
                    g.drawText (juce::String (juce::roundToInt (lightThresholdsDb[index])), juce::Rectangle<float> (columnX, 0.f, columnWidth, halfHeight), juce::Justification::centred, false);
                }
            }
        }
    }

private:
    void timerCallback() override
    {
        const auto targetDb = gainReductionSource.load();
        const auto previousDb = gainReductionDb;
        gainReductionDb = juce::jmax (targetDb, gainReductionDb + (targetDb - gainReductionDb) * smoothingCoefficient);

        if (std::abs (gainReductionDb - previousDb) > repaintThresholdDb)
            repaint();
    }

    void paintLight (juce::Graphics& g, juce::Rectangle<float> columnBounds, size_t index, bool isGlowPass)
    {
        const auto lightDiameter = columnBounds.getHeight() * 0.75f;
        const auto ledCentre = columnBounds.getCentre();
        const auto ledDiameter = lightDiameter * 0.8f;
        const auto previousThresholdDb = index == 0 ? 0.f : lightThresholdsDb[index - 1];
        const auto litAmount = juce::jlimit (0.f, 1.f, juce::jmap (gainReductionDb, previousThresholdDb, lightThresholdsDb[index], 0.f, 1.f));
        const auto ledColour = index >= 13 ? meterRed : index >= 10 ? meterOrange : index >= 6 ? meterYellow : meterGreen;
        const auto ledBounds = juce::Rectangle<float> (ledDiameter, ledDiameter).withCentre (ledCentre);

        if (isGlowPass)
        {
            if (litAmount > 0.f)
            {
                const auto glowRadius = ledDiameter * 0.9f;
                g.setGradientFill (juce::ColourGradient (ledColour.withAlpha (0.2f * litAmount), ledCentre, ledColour.withAlpha (0.f), ledCentre.translated (glowRadius, 0.f), true));
                g.fillEllipse (ledBounds.withSizeKeepingCentre (glowRadius * 2.f, glowRadius * 2.f));
            }
            return;
        }

        g.setColour (VCColors::almostBlack);
        g.fillEllipse (ledBounds.withSizeKeepingCentre (lightDiameter, lightDiameter));

        const auto centreColour = ledColour.withMultipliedBrightness (0.4f).interpolatedWith (ledColour.interpolatedWith (juce::Colours::white, 0.5f), litAmount);
        const auto edgeColour = ledColour.withMultipliedBrightness (0.15f).interpolatedWith (ledColour, litAmount);
        g.setGradientFill (juce::ColourGradient (centreColour, ledCentre.translated (0.f, ledDiameter * 0.3f), edgeColour, { ledCentre.x, ledBounds.getY() }, true));
        g.fillEllipse (ledBounds);

        g.setColour (juce::Colours::white.withAlpha (0.55f));
        g.fillEllipse (ledBounds.withSizeKeepingCentre (ledDiameter * 0.15f, ledDiameter * 0.12f).translated (0.f, -ledDiameter * 0.22f));
    }

    const int refreshRateHz {30};
    const float smoothingTimeSeconds {0.04f};
    const float repaintThresholdDb {0.01f};
    float smoothingCoefficient {0.f};

    std::atomic<float>& gainReductionSource;
    const juce::Colour labelColour;
    float gainReductionDb {0.f};

    static constexpr size_t numberOfLights {16};
    const std::array<float, numberOfLights> lightThresholdsDb {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 12.f, 14.f, 17.f, 20.f, 23.f, 26.f};

    const juce::Colour meterGreen   {60, 200, 80};
    const juce::Colour meterYellow  {235, 200, 40};
    const juce::Colour meterOrange  {250, 95, 25};
    const juce::Colour meterRed     {235, 50, 40};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GRDistressorMeter)
};
