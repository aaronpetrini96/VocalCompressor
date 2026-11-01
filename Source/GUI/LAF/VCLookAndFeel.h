#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class VCLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VCLookAndFeel() = default;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(juce::FontOptions(juce::jmin(16.f * uiScale, static_cast<float>(buttonHeight) * 0.6f)));
    }

    juce::Font getComboBoxFont(juce::ComboBox& box) override
    {
        return juce::Font(juce::FontOptions(juce::jmin(16.f * uiScale, static_cast<float>(box.getHeight()) * 0.85f)));
    }

    juce::Font getPopupMenuFont() override
    {
        return juce::Font(juce::FontOptions(17.f * uiScale));
    }

    void setUIScale(float newScale) { uiScale = newScale; }

private:
    float uiScale {1.f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VCLookAndFeel)
};
