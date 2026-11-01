//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class AAResizeUI
{
public:
    struct SizeOption
    {
        juce::String name;
        float scale;
    };

    AAResizeUI (float& scaleToUse, const int defaultUIWidth, const int defaultUIHeight)
        : defaultWidth (defaultUIWidth), defaultHeight (defaultUIHeight), currentScale (scaleToUse)
    {
        if (currentScale <= 0.0f)
            currentScale = getDefaultScale();
    }

    const int defaultWidth;
    const int defaultHeight;

    float getCurrentScale() const { return currentScale; }
    void setCurrentScale (const float newScale) { currentScale = newScale; }

    juce::Rectangle<int> getEditorSize (const juce::Rectangle<int> editorScreenBounds, const float extraHeightRatio) const
    {
        const auto defaultEditorHeight = (float) defaultHeight * (1.0f + extraHeightRatio);
        auto effectiveScale = currentScale;

        if (const auto* display = juce::Desktop::getInstance().getDisplays().getDisplayForRect (editorScreenBounds))
        {
            const auto userBounds = display->userArea.toFloat();
            const auto usableBounds = userBounds.reduced (userBounds.getWidth() * 0.05f, userBounds.getHeight() * 0.05f);
            const auto widthScaleLimit = usableBounds.getWidth() / (float) defaultWidth;
            const auto heightScaleLimit = usableBounds.getHeight() / defaultEditorHeight;
            effectiveScale = juce::jmin (effectiveScale, widthScaleLimit, heightScaleLimit);
        }

        return {juce::roundToInt ((float) defaultWidth * effectiveScale), juce::roundToInt (defaultEditorHeight * effectiveScale)};
    }

    const juce::Array<SizeOption>& getSizeOptions() const { return sizeOptionsArray; }

    bool isCurrentScaleDefault() const { return juce::approximatelyEqual (currentScale, getDefaultScale()); }

    void makeCurrentSizeDefault() const
    {
        const auto settingsFile = getUserSettingsFile();
        settingsFile.getParentDirectory().createDirectory();
        juce::PropertiesFile userSettings (settingsFile, juce::PropertiesFile::Options());
        userSettings.setValue ("DefaultUIScale", currentScale);
        userSettings.saveIfNeeded();
    }

private:
    static juce::File getUserSettingsFile()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
              #if JUCE_MAC
               .getChildFile ("Application Support")
              #endif
               .getChildFile (JucePlugin_Manufacturer)
               .getChildFile (JucePlugin_Name)
               .getChildFile ("Settings.xml");
    }

    float getDefaultScale() const
    {
        const juce::PropertiesFile userSettings (getUserSettingsFile(), juce::PropertiesFile::Options());
        return static_cast<float> (userSettings.getDoubleValue ("DefaultUIScale", fallbackDefaultScale));
    }

    juce::Array<SizeOption> sizeOptionsArray {SizeOption {"X-Small", 0.5f}, SizeOption {"Small", 0.75f}, SizeOption {"Default", 1.0f}, SizeOption {"Large", 1.25f}, SizeOption {"X-Large", 1.5f}, SizeOption {"Huge", 1.75f}};
    const float fallbackDefaultScale {1.0f};
    float& currentScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAResizeUI)
};
