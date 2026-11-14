//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <functional>
#include "../LAF/ListOptionsPopupLAF.h"

class AAListOptionsPopup : public juce::Component
{
public:
    struct Item
    {
        juce::String text;
        std::function<void()> onSelected;
        bool isEnabled {true};
        bool isSelected {false};
    };

    explicit AAListOptionsPopup (const juce::String& titleToUse) : title (titleToUse)
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&listOptionsLAF);
    }

    ~AAListOptionsPopup() override
    {
        setLookAndFeel (nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto cornerSize = boundsFHeight * VCLook::popupCornerPOH;
        g.setColour (VCColors::popupBackground);
        g.fillRoundedRectangle (boundsF, cornerSize);

        juce::Path titleShape;
        titleShape.addRoundedRectangle (titleBounds.getX(), titleBounds.getY(), titleBounds.getWidth(), titleBounds.getHeight(), cornerSize, cornerSize,
                                        true, true, false, false);
        g.setColour (VCColors::popupRowHighlight);
        g.fillPath (titleShape);

        const auto titleDividerThickness = boundsFHeight / getNumRows() * VCLook::outlineThicknessPOH * 0.5f;
        g.setColour (VCColors::popupOutline);
        g.fillRect (titleBounds.withTop (titleBounds.getBottom() - titleDividerThickness));

        g.setColour (VCColors::popupTitleText);
        g.setFont (VCFonts::barlowCondensedSemiBold (titleBounds.getHeight() * VCLook::fontHeightPOH));
        g.drawText (title, titleBounds, juce::Justification::centred, true);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto outlineThickness = boundsF.getHeight() / getNumRows() * VCLook::outlineThicknessPOH;
        const auto rowHeight = (boundsF.getHeight() - outlineThickness) / getNumRows();
        titleBounds = boundsF.withHeight (rowHeight * VCLook::popupTitleRowRatio);

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
            rows[rowIndex]->setBounds (boundsF.withHeight (rowHeight).withY (titleBounds.getBottom() + rowHeight * (float) rowIndex).toNearestInt());
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto outlineThickness = boundsFHeight / getNumRows() * VCLook::outlineThicknessPOH;
        g.setColour (VCColors::popupOutline);
        g.drawRoundedRectangle (boundsF.reduced (outlineThickness * 0.5f), boundsFHeight * VCLook::popupCornerPOH, outlineThickness);
    }

    float getNumRows() const { return (float) rows.size() + VCLook::popupTitleRowRatio; }

protected:
    ListOptionsPopupLAF listOptionsLAF;

    void setItems (const juce::Array<Item>& newItems)
    {
        rows.clear();

        for (const auto& item : newItems)
        {
            auto* row = rows.add (new juce::TextButton (item.text));
            row->setEnabled (item.isEnabled);
            row->setToggleState (item.isSelected, juce::dontSendNotification);
            row->onClick = [this, onSelected = item.onSelected]
            {
                setVisible (false);

                if (onSelected != nullptr)
                    onSelected();
            };
            addAndMakeVisible (row);
        }

        resized();
    }

private:
    juce::String title;
    juce::Rectangle<float> titleBounds;
    juce::OwnedArray<juce::Button> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
