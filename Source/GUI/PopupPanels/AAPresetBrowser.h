//
// Created by Aaron Petrini on 17/09/2026.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "../LAF/PresetBrowserLAF.h"
#include "../../Utilities/PresetManager.h"

class AAPresetBrowser : public juce::Component
{
public:
    explicit AAPresetBrowser (PresetManager& manager) : presetManager (manager)
    {
        setAlwaysOnTop (true);
        setLookAndFeel (&presetBrowserLAF);

        viewport.setViewedComponent (&rowsHolder, false);
        viewport.setScrollBarsShown (false, false);
        addAndMakeVisible (viewport);
        rowsHolder.addMouseListener (this, true);

        defaultTabButton.setToggleState (true, juce::dontSendNotification);
        defaultTabButton.setConnectedEdges (juce::Button::ConnectedOnRight);
        userTabButton.setConnectedEdges (juce::Button::ConnectedOnLeft);
        defaultTabButton.onClick = [this] { setPresetType (PresetManager::PresetType::factory); };
        userTabButton.onClick = [this] { setPresetType (PresetManager::PresetType::user); };

        for (auto* tabButton : {&defaultTabButton, &userTabButton})
        {
            tabButton->setRadioGroupId (7391);
            tabButton->setClickingTogglesState (true);
            addAndMakeVisible (tabButton);
        }
    }

    ~AAPresetBrowser() override
    {
        setLookAndFeel (nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.setColour (VCColors::popupBackground);
        g.fillRoundedRectangle (boundsF, boundsF.getHeight() * VCLook::popupCornerPOH);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto outlineThickness = boundsF.getHeight() / getNumRows() * VCLook::outlineThicknessPOH;
        const auto rowHeight = (boundsF.getHeight() - outlineThickness) / getNumRows();
        const auto tabHeight = rowHeight * VCLook::popupTitleRowRatio;
        const auto tabWidth = boundsF.getWidth() * 0.5f;

        defaultTabButton.setBounds (boundsF.withSize (tabWidth, tabHeight).toNearestInt());
        userTabButton.setBounds (boundsF.withHeight (tabHeight).withLeft (tabWidth).toNearestInt());

        const auto listBounds = boundsF.withTop (tabHeight).withTrimmedBottom (outlineThickness);
        viewport.setBounds (listBounds.toNearestInt());

        const auto rowWidth = boundsF.getWidth();
        rowsHolder.setSize (juce::roundToInt (rowWidth), juce::roundToInt (rowHeight * (float) rows.size()));

        for (int rowIndex = 0; rowIndex < rows.size(); ++rowIndex)
        {
            const auto rowBounds = juce::Rectangle<float> (0.0f, rowHeight * (float) rowIndex, rowWidth, rowHeight);
            rows[rowIndex]->setBounds (rowBounds.toNearestInt());
        }
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFHeight = boundsF.getHeight();
        const auto outlineThickness = boundsFHeight / getNumRows() * VCLook::outlineThicknessPOH;
        g.setColour (VCColors::popupOutline);
        g.drawRoundedRectangle (boundsF.reduced (outlineThickness * 0.5f), boundsFHeight * VCLook::popupCornerPOH, outlineThickness);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        isRowRightClicked = e.mods.isPopupMenu();
    }

    void showUserPresets()
    {
        setPresetType (PresetManager::PresetType::user);
    }

    float getNumRows() const { return (float) rows.size() + VCLook::popupTitleRowRatio; }



    void refresh()
    {
        const auto hasUserPresets = ! presetManager.getPresetFiles (PresetManager::PresetType::user).isEmpty();
        userTabButton.setEnabled (hasUserPresets);

        if (! hasUserPresets && selectedPresetType == PresetManager::PresetType::user)
        {
            setPresetType (PresetManager::PresetType::factory);
            return;
        }

        presetFiles = presetManager.getPresetFiles (selectedPresetType);
        const auto currentPresetName = presetManager.getCurrentPresetName();
        const auto isUserPresets = selectedPresetType == PresetManager::PresetType::user;

        rows.clear();

        for (const auto& presetFile : presetFiles)
        {
            auto* row = rows.add (new juce::TextButton (presetFile.getFileNameWithoutExtension()));
            row->setToggleState (presetFile.getFileNameWithoutExtension() == currentPresetName, juce::dontSendNotification);
            row->onClick = [this, presetFile, isUserPresets]
            {
                if (std::exchange (isRowRightClicked, false))
                {
                    if (isUserPresets)
                    {
                        juce::PopupMenu presetMenu;
                        presetMenu.setLookAndFeel (&presetBrowserLAF);
                        presetMenu.addItem ("Delete", [this, presetFile] { if (onDeleteRequested != nullptr) onDeleteRequested (presetFile); });
                        presetMenu.showMenuAsync (juce::PopupMenu::Options().withMousePosition().withDeletionCheck(*this));
                    }

                    return;
                }

                presetManager.loadPresetFile (presetFile);
                setVisible (false);
            };
            rowsHolder.addAndMakeVisible (row);
        }

        resized();

        if (onRowsChanged != nullptr)
            onRowsChanged();
    }


    std::function<void (const juce::File& presetFile)> onDeleteRequested;
    std::function<void()> onRowsChanged;

private:
    void setPresetType (const PresetManager::PresetType presetType)
    {
        selectedPresetType = presetType;
        defaultTabButton.setToggleState (presetType == PresetManager::PresetType::factory, juce::dontSendNotification);
        userTabButton.setToggleState (presetType == PresetManager::PresetType::user, juce::dontSendNotification);
        refresh();
    }

    PresetBrowserLAF presetBrowserLAF;
    PresetManager& presetManager;
    PresetManager::PresetType selectedPresetType {PresetManager::PresetType::factory};
    juce::Array<juce::File> presetFiles;

    juce::TextButton defaultTabButton {"Factory"}, userTabButton {"User"};
    juce::Component rowsHolder;
    juce::OwnedArray<juce::TextButton> rows;
    juce::Viewport viewport;
    bool isRowRightClicked {false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAPresetBrowser)
};
