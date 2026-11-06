#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../VCLook.h"

class Ratio1176Buttons : public juce::Component
{
public:
    Ratio1176Buttons()
    {
        for (int index {0}; index < capLabels.size(); ++index)
        {
            auto* cap = caps.add(new Cap(capLabels[index], capOnImage, capOffImage));
            cap->onClick = [this, index]
            {
                const auto isShiftClick = juce::ModifierKeys::currentModifiers.isShiftDown();
                const auto newIndex = isShiftClick && selectedIndex != allIndex ? allIndex : index;
                setSelectedIndex(newIndex);

                if (onIndexChosen != nullptr)
                    onIndexChosen(newIndex);
            };
            addAndMakeVisible(cap);
        }
        const auto capImageRatio = static_cast<float>(capOffImage.getHeight()) / static_cast<float>(capOffImage.getWidth());
        heightToWidthRatio = capImageRatio * getCapHeightsInColumn();
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto capHeight = boundsF.getHeight() / getCapHeightsInColumn();
        const auto capStep = capHeight * (1.f + capGapRatio);

        for (int index {0}; index < caps.size(); ++index)
            caps[index]->setBounds(boundsF.withHeight(capHeight).withY(boundsF.getY() + capStep * static_cast<float>(index)).toNearestInt());
    }

    void setSelectedIndex(const int newIndex)
    {
        selectedIndex = newIndex;
        for (int index {0}; index < caps.size(); ++index)
            caps[index]->setToggleState(newIndex == allIndex || index == newIndex, juce::dontSendNotification);
    }

    std::function<void(int)> onIndexChosen;
    float heightToWidthRatio {1.f};

private:
    float getCapHeightsInColumn() const
    {
        const auto numberOfCaps = static_cast<float>(caps.size());
        return numberOfCaps + (numberOfCaps - 1.f) * capGapRatio;
    }

    class Cap : public juce::Button
    {
    public:
        Cap(const juce::String& labelText, const juce::Image& onImage, const juce::Image& offImage)
        : juce::Button(labelText), imageOn(onImage), imageOff(offImage)
        {
        }

        void paintButton(juce::Graphics& g, bool, bool) override
        {
            const auto isPressed = getToggleState();
            const auto imageBounds = juce::RectanglePlacement(juce::RectanglePlacement::centred).appliedTo(imageOff.getBounds().toFloat(), getLocalBounds().toFloat());
            g.drawImage(isPressed ? imageOn : imageOff, imageBounds, juce::RectanglePlacement::centred);

            const auto labelCentreY = imageBounds.getY() + imageBounds.getHeight() * (isPressed ? 0.5f : 0.42f);
            const auto labelBounds = imageBounds.withSizeKeepingCentre(imageBounds.getWidth(), imageBounds.getHeight() * 0.5f)
                                                .withCentre({imageBounds.getCentreX(), labelCentreY});
            g.setColour(VCColors::almostBlack);
            g.setFont(VCFonts::barlowCondensedSemiBold(labelBounds.getHeight() * (isPressed ? 1.f : 1.1f)));
            g.drawText(getButtonText(), labelBounds, juce::Justification::centred, false);
        }

    private:
        juce::Image imageOn, imageOff;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Cap)
    };

    const juce::StringArray capLabels {"4", "8", "12", "20"};
    static constexpr int allIndex {4};
    const float capGapRatio {0.15f};
    int selectedIndex {0};
    const juce::Image capOnImage {juce::ImageCache::getFromMemory(BinaryData::ratioButtonOn_png, BinaryData::ratioButtonOn_pngSize)};
    const juce::Image capOffImage {juce::ImageCache::getFromMemory(BinaryData::ratioButtonOff_png, BinaryData::ratioButtonOff_pngSize)};
    juce::OwnedArray<Cap> caps;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ratio1176Buttons)
};
