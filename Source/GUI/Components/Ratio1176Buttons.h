#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../VCLook.h"

class Ratio1176Buttons : public juce::Component
{
public:
    explicit Ratio1176Buttons(juce::RangedAudioParameter& parameter)
    : attachment(parameter, [this](const float newValue) { setSelectedIndex(juce::roundToInt(newValue)); }, nullptr)
    {
        attachment.sendInitialUpdate();

        if (!unpushedButtonImage.isValid() || !pushedButtonImage.isValid())
            return;
        const auto buttonImageRatio = static_cast<float>(unpushedButtonImage.getHeight()) / static_cast<float>(unpushedButtonImage.getWidth());
        heightToWidthRatio = buttonImageRatio * static_cast<float>(buttonLabels.size());
    }

    void paint(juce::Graphics& g) override
    {
        for (int index {0}; index < buttonLabels.size(); ++index)
        {
            const auto isPushed = index == selectedIndex;
            const auto buttonBounds = getButtonBounds(index);
            g.drawImage(isPushed ? pushedButtonImage : unpushedButtonImage, buttonBounds, juce::RectanglePlacement::centred);

            const auto labelCentreY = buttonBounds.getY() + buttonBounds.getHeight() * (isPushed ? 0.5f : 0.42f);
            const auto labelBounds = buttonBounds.withSize(buttonBounds.getWidth(), buttonBounds.getHeight() * 0.5f)
                                                 .withCentre({buttonBounds.getCentreX(), labelCentreY});
            g.setColour(VCColors::almostWhiteText);
            g.setFont(VCFonts::barlowCondensedSemiBold(labelBounds.getHeight()));
            g.drawText(buttonLabels[index], labelBounds, juce::Justification::centred, false);
        }
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        const auto buttonHeight = static_cast<float>(getHeight()) / static_cast<float>(buttonLabels.size());
        const auto clickedIndex = juce::jlimit(0, buttonLabels.size() - 1, static_cast<int>(event.position.y / buttonHeight));
        attachment.setValueAsCompleteGesture(static_cast<float>(clickedIndex));
    }

    float heightToWidthRatio {1.f};

private:
    void setSelectedIndex(const int newIndex)
    {
        selectedIndex = newIndex;
        repaint();
    }

    juce::Rectangle<float> getButtonBounds(const int index) const
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto buttonHeight = boundsF.getHeight() / static_cast<float>(buttonLabels.size());
        return boundsF.withHeight(buttonHeight).withY(boundsF.getY() + buttonHeight * static_cast<float>(index));
    }

    const juce::StringArray buttonLabels {"4", "8", "12", "20", "ALL"};
    int selectedIndex {0};
    const juce::Image pushedButtonImage {juce::ImageCache::getFromMemory(BinaryData::pushedButton_png, BinaryData::pushedButton_pngSize)};
    const juce::Image unpushedButtonImage {juce::ImageCache::getFromMemory(BinaryData::unpushedButton_png, BinaryData::unpushedButton_pngSize)};
    juce::ParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ratio1176Buttons)
};
