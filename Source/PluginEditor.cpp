/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessorEditor::VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(headerPanel);
    addAndMakeVisible(panel1176);
    addAndMakeVisible(panelLA2A);
    addChildComponent(modalBlocker);

    headerPanel.getResizeButton().onClick = [this]
    {
        togglePopup(resizeOptionsPopup, [this] { resizeOptionsPopup.updateItems(); });
    };

    addChildComponent(resizeOptionsPopup);
    resizeOptionsPopup.addComponentListener(this);
    resizeOptionsPopup.onSizeChanged = [this]
    {
        updateEditorSize();
        uiResize.makeCurrentSizeDefault();
    };

    addChildComponent(presetBrowser);
    presetBrowser.addComponentListener(this);
    addChildComponent(presetPopup);
    presetPopup.addComponentListener(this);

    headerPanel.onPresetNameClicked = [this]
    {
        togglePopup(presetBrowser, [this] { presetBrowser.refresh(); });
    };
    headerPanel.onSaveClicked = [this]
    {
        togglePopup(presetPopup, [this]
        {
            presetPopup.prepare(audioProcessor.presetManager.getCurrentPresetName());
        });
    };
    headerPanel.onBypassChanged = [this] { updatePanelsDimming(); };

    presetBrowser.onRowsChanged = [this] { resized(); };

    presetPopup.presetExists = [this] (const juce::String& presetName)
    {
        return audioProcessor.presetManager.getUserPresetFile(presetName).existsAsFile();
    };

    presetPopup.isNameReserved = [this] (const juce::String& presetName)
    {
        return audioProcessor.presetManager.isFactoryPresetName(presetName);
    };
    
    presetPopup.onConfirm = [this] (const juce::String& presetName)
    {
        presetPopup.setVisible(false);
        audioProcessor.presetManager.savePreset(presetName);
    };

    addMouseListener(this, true);

    setResizable(false, false);
    updateEditorSize();
    updatePanelsDimming();
    startTimerHz(30);
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(VCColors::background);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();
    const auto boundsFWidth = boundsF.getWidth();

    const auto headerHeight = boundsFHeight * 0.0725f;
    const auto unitPanelHeight = (boundsFHeight - headerHeight) * 0.5f;

    const auto headerBounds = boundsF.withHeight(headerHeight);
    const auto panel1176Bounds = boundsF.withHeight(unitPanelHeight).withY(headerBounds.getBottom());
    const auto panelLA2ABounds = panel1176Bounds.withY(panel1176Bounds.getBottom());

    headerPanel.setBounds(headerBounds.toNearestInt());
    panel1176.setBounds(panel1176Bounds.toNearestInt());
    panelLA2A.setBounds(panelLA2ABounds.toNearestInt());
    modalBlocker.setBounds(boundsF.withTop(headerBounds.getBottom()).toNearestInt());
    const auto resizeBtnBounds = headerPanel.getResizeButton().getBounds().toFloat();

    const auto rowH = boundsFHeight * VCLook::rowPOH;
    const auto sizeOptionsWidth = boundsFWidth * 0.2f;
    const auto sizeOptionsHeight = rowH * resizeOptionsPopup.getNumRows();
    resizeOptionsPopup.setBounds(juce::Rectangle<float>(sizeOptionsWidth, sizeOptionsHeight)
                                     .withRightX(resizeBtnBounds.getRight())
                                     .withY(resizeBtnBounds.getBottom())
                                     .toNearestInt());

    const auto presetBrowserBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.3f, presetBrowser.getNumRows() * rowH).withY(headerBounds.getBottom());
    presetBrowser.setBounds(presetBrowserBounds.toNearestInt());
    presetPopup.setBounds(presetBrowserBounds.withHeight(boundsFHeight * 0.3f).toNearestInt());
}

void VocalCompressorAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    const auto isPopupOpen = isAnyPopupVisible();

    for (const auto* popup : popups)
    {
        if (popup->isVisible())
            g.excludeClipRegion(popup->getBounds());
    }

    const auto fillDim = [&] (const juce::Component& panel, bool isUnitBypassed)
    {
        const auto isBypassed = headerPanel.isBypassed() || isUnitBypassed;
        const auto dimAmount = isBypassed ? 0.55f : (isPopupOpen ? 0.4f : 0.f);

        g.setColour(VCColors::almostBlack.withAlpha(dimAmount));
        g.fillRect(panel.getBounds());
    };

    fillDim(panel1176, headerPanel.is1176Bypassed());
    fillDim(panelLA2A, headerPanel.isLA2ABypassed());
}

void VocalCompressorAudioProcessorEditor::timerCallback()
{
    panelLA2A.updateGainReduction();
}

void VocalCompressorAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    popupClosedOnMouseDown = nullptr;

    for (auto* popup : popups)
    {
        const auto isClickInside = event.eventComponent == popup || popup->isParentOf(event.eventComponent);

        if (popup->isVisible() && ! isClickInside)
        {
            popup->setVisible(false);
            popupClosedOnMouseDown = popup;
        }
    }
}

void VocalCompressorAudioProcessorEditor::componentVisibilityChanged(juce::Component& component)
{
    if (&component == &resizeOptionsPopup)
        headerPanel.getResizeButton().setToggleState(resizeOptionsPopup.isVisible(), juce::dontSendNotification);

    modalBlocker.setVisible(isAnyPopupVisible());
    updatePanelsDimming();
}

void VocalCompressorAudioProcessorEditor::updatePanelsDimming()
{
    repaint();
}

void VocalCompressorAudioProcessorEditor::updateEditorSize()
{
    const auto editorSize = uiResize.getEditorSize(getScreenBounds(), 0.0f);
    setSize(editorSize.getWidth(), editorSize.getHeight());
}

void VocalCompressorAudioProcessorEditor::togglePopup(juce::Component& popup, const std::function<void()>& prepareContent)
{
    if (std::exchange(popupClosedOnMouseDown, nullptr) == &popup)
        return;

    popup.toFront(false);
    popup.setVisible(true);
    prepareContent();
    resized();
}

bool VocalCompressorAudioProcessorEditor::isAnyPopupVisible() const
{
    return std::any_of(popups.begin(), popups.end(), [] (const auto* popup) { return popup->isVisible(); });
}
