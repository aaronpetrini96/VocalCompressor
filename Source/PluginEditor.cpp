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

    addAndMakeVisible(resizeButton);
    AAUITools::setIconButtonImages(resizeButton, BinaryData::resizeLines_svg, BinaryData::resizeLines_svgSize,
                                   juce::Colours::white, VCColors::iconNormal, VCColors::iconOver, VCColors::iconActive);
    resizeButton.setToggleable(true);
    resizeButton.setClickingTogglesState(false);
    resizeButton.onClick = [this] { showUISizeOptions(); };

    addChildComponent(resizeOptionsPopup);
    resizeOptionsPopup.addComponentListener(this);
    resizeOptionsPopup.onSizeChanged = [this]
    {
        updateEditorSize();
        uiResize.makeCurrentSizeDefault();
    };

    addMouseListener(this, true);

    setResizable(false, false);
    updateEditorSize();
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

    const auto headerHeight = boundsFHeight * 0.07f;
    const auto unitPanelHeight = (boundsFHeight - headerHeight) * 0.5f;

    const auto headerBounds = boundsF.withHeight(headerHeight);
    const auto panel1176Bounds = boundsF.withHeight(unitPanelHeight).withY(headerBounds.getBottom());
    const auto panelLA2ABounds = panel1176Bounds.withY(panel1176Bounds.getBottom());

    headerPanel.setBounds(headerBounds.toNearestInt());
    panel1176.setBounds(panel1176Bounds.toNearestInt());
    panelLA2A.setBounds(panelLA2ABounds.toNearestInt());

    const auto resizeButtonSize = boundsFHeight * 0.057f;
    const auto resizeBtnGap = resizeButtonSize * 0.5f;
    const auto resizeBtnBounds = boundsF.withSize(resizeButtonSize, resizeButtonSize)
                                        .withCentre({boundsF.getWidth() - resizeBtnGap, boundsFHeight - resizeBtnGap});
    resizeButton.setBounds(resizeBtnBounds.toNearestInt());

    const auto rowH = headerHeight * 0.6f;
    const auto sizeOptionsWidth = boundsF.getWidth() * 0.2f;
    const auto sizeOptionsHeight = rowH * resizeOptionsPopup.getNumRows();
    resizeOptionsPopup.setBounds(juce::Rectangle<float>(sizeOptionsWidth, sizeOptionsHeight)
                                     .withRightX(resizeBtnBounds.getRight())
                                     .withBottomY(resizeBtnBounds.getCentreY())
                                     .toNearestInt());
}

void VocalCompressorAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    g.setColour(VCColors::separator);
    for (auto* panel : std::initializer_list<juce::Component*> {&panel1176, &panelLA2A})
        g.fillRect(panel->getBounds().toFloat().withHeight(static_cast<float>(getWidth()) * 0.0033f));
}

void VocalCompressorAudioProcessorEditor::timerCallback()
{
    panel1176.updateGainReduction();
    panelLA2A.updateGainReduction();
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

    hidePopupUnlessClickedIn(resizeOptionsPopup, resizeButton);
}

void VocalCompressorAudioProcessorEditor::componentVisibilityChanged(juce::Component& component)
{
    if (&component == &resizeOptionsPopup)
        resizeButton.setToggleState(resizeOptionsPopup.isVisible(), juce::dontSendNotification);
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
