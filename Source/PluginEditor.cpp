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

    headerPanel.getPresetBox().onNameClicked = [this]
    {
        togglePopup(presetBrowser, [this] { presetBrowser.refresh(); });
    };
    headerPanel.getSavePresetButton().onClick = [this]
    {
        togglePopup(presetPopup, [this]
        {
            presetPopup.prepare(audioProcessor.presetManager.getCurrentPresetName());
        });
    };

    for (auto* bypassButton : std::initializer_list<juce::Button*> {&headerPanel.getBypassButton(), &headerPanel.get1176BypassButton(), &headerPanel.getLA2ABypassButton()})
        bypassButton->onClick = [this] { repaint(); };

    headerPanel.getSignalOrderButton().onClick = [this] { resized(); };
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
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.drawImage(rackEarsImage, topPanelBounds, juce::RectanglePlacement::fillDestination);
    g.drawImage(rackEarsImage, bottomPanelBounds, juce::RectanglePlacement::fillDestination);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFHeight = boundsF.getHeight();
    const auto boundsFWidth = boundsF.getWidth();
    const auto headerHeight = boundsFHeight * 0.065f;
    const auto unitPanelHeight = (boundsFHeight - headerHeight) * 0.5f;

    const auto headerBounds = boundsF.withHeight(headerHeight);
    topPanelBounds = boundsF.withHeight(unitPanelHeight).withY(headerBounds.getBottom());
    bottomPanelBounds = topPanelBounds.withY(topPanelBounds.getBottom());

    headerPanel.setBounds(headerBounds.toNearestInt());
    const auto isSignalOrderReversed = headerPanel.getSignalOrderButton().getToggleState();
    panel1176.setBounds((isSignalOrderReversed ? bottomPanelBounds : topPanelBounds).toNearestInt());
    panelLA2A.setBounds((isSignalOrderReversed ? topPanelBounds : bottomPanelBounds).toNearestInt());

    modalBlocker.setBounds(boundsF.withTop(headerBounds.getBottom()).toNearestInt());
    const auto resizeBtnBounds = headerPanel.getResizeButton().getBounds().toFloat();

    const auto rowH = boundsFHeight * VCLook::rowPOH;

    const auto sizeOptionsWidth = boundsFWidth * 0.2f;
    const auto sizeOptionsHeight = rowH * resizeOptionsPopup.getNumRows();
    resizeOptionsPopup.setBounds(juce::Rectangle<float>(sizeOptionsWidth, sizeOptionsHeight).withRightX(resizeBtnBounds.getRight()).withY(headerBounds.getBottom()).toNearestInt());

    const auto presetBrowserBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.3f, presetBrowser.getNumRows() * rowH).withY(headerBounds.getBottom());
    presetBrowser.setBounds(presetBrowserBounds.toNearestInt());
    presetPopup.setBounds(presetBrowserBounds.withHeight(rowH * presetPopup.numRows).toNearestInt());
}

void VocalCompressorAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    const auto isPopupOpen = isAnyPopupVisible();
    const auto isPluginBypassed = headerPanel.getBypassButton().getToggleState();
    const auto is1176Bypassed = headerPanel.get1176BypassButton().getToggleState();
    const auto isLA2ABypassed = headerPanel.getLA2ABypassButton().getToggleState();
    const auto isAnyBypassed = isPluginBypassed || is1176Bypassed || isLA2ABypassed;

    if (! isPopupOpen && ! isAnyBypassed)
        return;

    for (const auto* popup : popups)
    {
        if (popup->isVisible())
            g.excludeClipRegion(popup->getBounds());
    }

    const auto fillDim = [&] (const juce::Component& panel, bool isUnitBypassed)
    {
        const auto isBypassed = isPluginBypassed || isUnitBypassed;
        const auto dimAmount = isBypassed ? 0.5f : (isPopupOpen ? 0.3f : 0.f);

        if (dimAmount == 0.0f)
            return;

        g.setColour(VCColors::almostBlack.withAlpha(dimAmount));
        g.fillRect(panel.getBounds());
    };

    fillDim(panel1176, is1176Bypassed);
    fillDim(panelLA2A, isLA2ABypassed);
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
