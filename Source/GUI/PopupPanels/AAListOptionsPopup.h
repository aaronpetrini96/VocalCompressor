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
//        bool isCheckbox {false};
//        juce::String tooltip;
//        std::shared_ptr<juce::Drawable> icon;
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

        g.setColour (VCColors::popupText.withMultipliedBrightness (2.f));
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
//        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
//        {
//            const auto& icon = rows[rowIndex]->isOver() ? rowOverIcons[(size_t) rowIndex] : rowIcons[(size_t) rowIndex];
//            if (icon != nullptr)
//            {
//                const auto rowBounds = rows[rowIndex]->getBounds().toFloat();
//                const auto iconSize = rowBounds.getHeight() * 0.6f;
//                const auto iconWidth = iconSize * juce::jmax (1.0f, icon->getDrawableBounds().getWidth() / icon->getDrawableBounds().getHeight());
//                const auto iconBounds = rowBounds.withSizeKeepingCentre (iconWidth, iconSize).withRightX (rowBounds.getRight() - rowBounds.getHeight() * 0.45f);
//                icon->drawWithin (g, iconBounds, juce::RectanglePlacement::centred, rows[rowIndex]->isEnabled() ? 1.f : 0.3f);
//            }
//        }

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
//        rowIcons.clear();
//        rowOverIcons.clear();

        for (const auto& item : newItems)
        {
//            auto* row = rows.add (item.isCheckbox ? static_cast<juce::Button*> (new juce::ToggleButton (item.text)) : new juce::TextButton (item.text));
            auto* row = rows.add (new juce::TextButton (item.text));
            row->setEnabled (item.isEnabled);
            row->setToggleState (item.isSelected, juce::dontSendNotification);
//            row->setTooltip (item.tooltip);
//            row->onClick = [this, onSelected = item.onSelected, closesPopup = ! item.isCheckbox]
            row->onClick = [this, onSelected = item.onSelected]
            {
//                if (closesPopup)
                setVisible (false);

                if (onSelected != nullptr)
                    onSelected();
            };
            addAndMakeVisible (row);

//            rowIcons.push_back (item.icon);
//            rowOverIcons.push_back (item.icon != nullptr ? item.icon->createCopy() : nullptr);
//            if (rowOverIcons.back() != nullptr)
//                rowOverIcons.back()->replaceColour (VCColors::almostWhiteText, VCColors::iconOver);
        }

        resized();
    }

private:
    juce::String title;
    juce::Rectangle<float> titleBounds;
    juce::OwnedArray<juce::Button> rows;
//    std::vector<std::shared_ptr<juce::Drawable>> rowIcons, rowOverIcons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAListOptionsPopup)
};
