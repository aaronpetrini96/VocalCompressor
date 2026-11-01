#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../VCLook.h"

class VCControlsLAF : public juce::LookAndFeel_V4
{
public:
    VCControlsLAF() = default;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(juce::FontOptions(static_cast<float>(buttonHeight) * VCLook::buttonFontPOH));
    }

    juce::Font getComboBoxFont(juce::ComboBox& box) override
    {
        return juce::Font(juce::FontOptions(static_cast<float>(box.getHeight()) * VCLook::comboBoxFontPOH));
    }

    juce::Font getPopupMenuFont() override
    {
        return juce::Font(juce::FontOptions(popupMenuMaxFontHeight));
    }

    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label& label) override
    {
        const auto itemHeight = juce::roundToInt(static_cast<float>(box.getHeight()) * VCLook::comboBoxMenuItemPOH);
        return LookAndFeel_V4::getOptionsForComboBoxPopupMenu(box, label).withStandardItemHeight(itemHeight);
    }

private:
    const float popupMenuMaxFontHeight {100.f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VCControlsLAF)
};
