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

    headerPanel.getResizeButton().onClick = [this] { showUISizeOptions(); };

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
    addChildComponent(modalBlocker);

    headerPanel.onPresetNameClicked = [this] { showPresetBrowser(); };
    headerPanel.onSaveClicked = [this]
    {
        showPresetPopup(AAPresetPopup::Action::save, audioProcessor.presetManager.getCurrentPresetName());
    };
    headerPanel.onBypassChanged = [this] { updatePanelsDimming(); };

    presetBrowser.onRowsChanged = [this] { resized(); };
    presetBrowser.onDeleteRequested = [this] (const juce::File& presetFile)
    {
        presetFileToEdit = presetFile;
        showPresetPopup(AAPresetPopup::Action::remove, presetFile.getFileNameWithoutExtension());
    };

    presetPopup.presetExists = [this] (const juce::String& presetName)
    {
        return audioProcessor.presetManager.getUserPresetFile(presetName).existsAsFile();
    };
    presetPopup.isNameReserved = [this] (const juce::String& presetName)
    {
        return audioProcessor.presetManager.isFactoryPresetName(presetName);
    };
    presetPopup.onConfirm = [this] (AAPresetPopup::Action action, const juce::String& presetName)
    {
        auto& presetManager = audioProcessor.presetManager;

        presetPopup.setVisible(false);

        if (action == AAPresetPopup::Action::save)
        {
            presetManager.savePreset(presetName);
            presetBrowser.showUserPresets();
            return;
        }

        presetManager.deletePreset(presetFileToEdit);
        presetBrowser.refresh();
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


    const auto resizeBtnBounds = headerPanel.getResizeButton().getBounds().toFloat();

    const auto rowH = boundsFHeight * VCLook::rowPOH;
    const auto sizeOptionsWidth = boundsFWidth * 0.2f;
    const auto sizeOptionsHeight = rowH * resizeOptionsPopup.getNumRows();
    resizeOptionsPopup.setBounds(juce::Rectangle<float>(sizeOptionsWidth, sizeOptionsHeight)
                                     .withRightX(resizeBtnBounds.getRight())
                                     .withY(resizeBtnBounds.getBottom())
                                     .toNearestInt());

    const auto presetBrowserBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.3f, presetBrowser.getNumRows() * rowH);
    presetBrowser.setBounds(presetBrowserBounds.withY(headerBounds.getBottom()).toNearestInt());
    presetPopup.setBounds(boundsF.withSizeKeepingCentre(presetBrowserBounds.getWidth(), boundsFHeight * 0.3f).toNearestInt());
    modalBlocker.setBounds(getLocalBounds());
}

void VocalCompressorAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    const auto isPopupOpen = resizeOptionsPopup.isVisible() || presetBrowser.isVisible() || presetPopup.isVisible();

    for (auto* popup : std::initializer_list<juce::Component*> {&resizeOptionsPopup, &presetBrowser, &presetPopup})
        if (popup->isVisible())
            g.excludeClipRegion(popup->getBounds());

    const auto fillDim = [&] (const juce::Component& panel, bool isUnitBypassed)
    {
        const auto isBypassed = headerPanel.isBypassed() || isUnitBypassed;
        const auto dimAmount = isBypassed ? 0.5f : (isPopupOpen ? 0.3f : 0.f);

        g.setColour(VCColors::almostBlack.withAlpha(dimAmount));
        g.fillRect(panel.getBounds());
    };

    fillDim(panel1176, headerPanel.is1176Bypassed());
    fillDim(panelLA2A, headerPanel.isLA2ABypassed());
}

void VocalCompressorAudioProcessorEditor::timerCallback()
{
    panel1176.updateGainReduction();
    panelLA2A.updateGainReduction();
}

void VocalCompressorAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &modalBlocker)
        presetPopup.setVisible(false);
}

void VocalCompressorAudioProcessorEditor::mouseUp(const juce::MouseEvent& event)
{
    const auto isClickInside = [&event] (const juce::Component& component)
    {
        return event.eventComponent == &component || component.isParentOf(event.eventComponent);
    };

    const auto hidePopupUnlessClickedIn = [&isClickInside] (juce::Component& popup, const auto&... keepOpenFor)
    {
        if (popup.isVisible() && ! isClickInside(popup) && ! (isClickInside(keepOpenFor) || ...))
            popup.setVisible(false);
    };

    hidePopupUnlessClickedIn(resizeOptionsPopup, headerPanel.getResizeButton());
    hidePopupUnlessClickedIn(presetBrowser, headerPanel.getPresetBox(), presetPopup, modalBlocker);
}

void VocalCompressorAudioProcessorEditor::componentVisibilityChanged(juce::Component& component)
{
    if (&component == &resizeOptionsPopup)
        headerPanel.getResizeButton().setToggleState(resizeOptionsPopup.isVisible(), juce::dontSendNotification);
    else if (&component == &presetPopup)
    {
        modalBlocker.setVisible(presetPopup.isVisible());
        modalBlocker.toBehind(&component);
    }

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

void VocalCompressorAudioProcessorEditor::showUISizeOptions()
{
    if (resizeOptionsPopup.isVisible())
    {
        resizeOptionsPopup.setVisible(false);
        return;
    }
    resizeOptionsPopup.updateItems();
    showPopup(resizeOptionsPopup);
}

void VocalCompressorAudioProcessorEditor::showPopup(juce::Component& popup)
{
    resized();
    popup.toFront(false);
    popup.setVisible(true);
}

void VocalCompressorAudioProcessorEditor::showPresetBrowser()
{
    if (presetBrowser.isVisible())
    {
        presetBrowser.setVisible(false);
        return;
    }
    presetBrowser.refresh();
    showPopup(presetBrowser);
}

void VocalCompressorAudioProcessorEditor::showPresetPopup(AAPresetPopup::Action action, const juce::String& presetName)
{
    showPopup(presetPopup);
    presetPopup.prepare(action, presetName);
}
