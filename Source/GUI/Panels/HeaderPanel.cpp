#include "HeaderPanel.h"

HeaderPanel::HeaderPanel(VocalCompressorAudioProcessor& processor)
: presetManager(processor.presetManager),
  undoManager(processor.undoManager),
  presetBox(processor.presetManager),
  bypassPluginButtonAttachment(processor.apvts,       ParamNames::getGeneralBypassName(), bypassPluginButton),
  unitTitle1176ButtonAttachment(processor.apvts,      ParamNames::get1176BypassName(), unitTitle1176Button),
  unitTitleLA2AButtonAttachment(processor.apvts,      ParamNames::getLA2ABypassName(), unitTitleLA2AButton),
  reverseSignalOrderButtonAttachment(processor.apvts, ParamNames::getReverseSignalOrderName(), signalOrderButton)
{
    setupArrowIconButtons();

    for (auto* unitTitleButton : {&unitTitle1176Button, &unitTitleLA2AButton})
    {
        unitTitleButton->setLookAndFeel(&unitTitleLAF);
        unitTitleButton->setClickingTogglesState(true);
        unitTitleButton->onStateChange = [this] { if (onBypassChanged != nullptr) onBypassChanged(); };
        addAndMakeVisible(*unitTitleButton);
    }

    presetBox.onNameClicked = [this] { if (onPresetNameClicked != nullptr) onPresetNameClicked(); };

    AAUITools::setIconButtonImages(saveButton, BinaryData::saveIcon2_svg, BinaryData::saveIcon2_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
    saveButton.onClick = [this] { if (onSaveClicked != nullptr) onSaveClicked(); };

    AAUITools::setIconButtonImages(bypassPluginButton, BinaryData::bypassIcon_svg, BinaryData::bypassIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);

    bypassPluginButton.setClickingTogglesState(true);
    bypassPluginButton.onStateChange = [this] {if (onBypassChanged != nullptr) onBypassChanged();};

    AAUITools::setIconButtonImages(resizeButton, BinaryData::resizeSquare2_svg, BinaryData::resizeSquare2_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
    resizeButton.setToggleable(true);
    resizeButton.setClickingTogglesState(false);

    AAUITools::setIconButtonImages(undoButton, BinaryData::undoIcon_svg, BinaryData::undoIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
    AAUITools::setIconButtonImages(redoButton, BinaryData::redoIcon_svg, BinaryData::redoIcon_svgSize, VCColors::almostWhiteText, VCColors::iconOver, VCColors::iconActive, VCColors::iconActiveOver);
    undoButton.onClick = [this] { undoManager.undo(); };
    redoButton.onClick = [this] { undoManager.redo(); };

    presetManager.addChangeListener(this);
    undoManager.addChangeListener(this);
    updatePresetName();
    updateUndoRedoButtons();

    addAndMakeVisible(presetBox);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(bypassPluginButton);
    addAndMakeVisible(signalOrderButton);
    addAndMakeVisible(resizeButton);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
}

HeaderPanel::~HeaderPanel()
{
    presetManager.removeChangeListener(this);
    undoManager.removeChangeListener(this);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(VCColors::almostBlack);
}

void HeaderPanel::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();

    const auto presetBoxBounds = boundsF.withSizeKeepingCentre(boundsFWidth * 0.3f, boundsFHeight * 0.7f);
    presetBox.setBounds(presetBoxBounds.toNearestInt());

    const auto iconSize = boundsFHeight * 0.5f;
    const auto iconGap = iconSize;
    const auto iconButtonBounds = boundsF.withSizeKeepingCentre(iconSize, iconSize);
    saveButton.setBounds(iconButtonBounds.withX(presetBoxBounds.getRight() + iconGap).toNearestInt());
    bypassPluginButton.setBounds(iconButtonBounds.withRightX(boundsFWidth  - iconGap * 0.5f).toNearestInt());
    resizeButton.setBounds(iconButtonBounds.withRightX(bypassPluginButton.getBounds().toFloat().getX() - iconGap).toNearestInt());

    const auto signalOrderButtonBounds = iconButtonBounds.withCentre({presetBoxBounds.getX() * 0.5f, iconButtonBounds.getCentreY()});
    signalOrderButton.setBounds(signalOrderButtonBounds.toNearestInt());

    const auto titleButtonBounds = iconButtonBounds.withSizeKeepingCentre(iconSize * 3.5f, iconSize);
    unitTitle1176Button.setBounds(titleButtonBounds.withRightX(signalOrderButtonBounds.getX() - iconGap * 0.5f).toNearestInt());
    unitTitleLA2AButton.setBounds(titleButtonBounds.withX(signalOrderButtonBounds.getRight() + iconGap * 0.5f).toNearestInt());
    const auto saveButtonRight = presetBoxBounds.getRight() + iconGap + iconSize;
    const auto resizeButtonX = boundsFWidth - iconSize * 3.f;
    const auto undoRedoCentreX = (saveButtonRight + resizeButtonX) * 0.5f;
    undoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX - iconGap, boundsF.getCentreY()}).toNearestInt());
    redoButton.setBounds(iconButtonBounds.withCentre({undoRedoCentreX + iconGap, boundsF.getCentreY()}).toNearestInt());
}

void HeaderPanel::changeListenerCallback(juce::ChangeBroadcaster*)
{
    updatePresetName();
    updateUndoRedoButtons();
}

void HeaderPanel::setupArrowIconButtons()
{
    const auto forwardIcon = juce::Drawable::createFromImageData (BinaryData::arrowRightIcon_svg, BinaryData::arrowRightIcon_svgSize);
    const auto forwardIconOver = forwardIcon->createCopy();
    const auto forwardIconDown = forwardIcon->createCopy();
    forwardIconOver->replaceColour (VCColors::almostWhiteText, VCColors::iconOver);
    forwardIconDown->replaceColour (VCColors::almostWhiteText, VCColors::iconActive);
    const auto reverseIcon = juce::Drawable::createFromImageData (BinaryData::arrowLeftIcon_svg, BinaryData::arrowLeftIcon_svgSize);
    const auto reverseIconOver = reverseIcon->createCopy();
    const auto reverseIconDown = reverseIcon->createCopy();
    reverseIconOver->replaceColour (VCColors::almostWhiteText, VCColors::iconOver);
    reverseIconDown->replaceColour (VCColors::almostWhiteText, VCColors::iconActive);
    signalOrderButton.setImages (forwardIcon.get(), forwardIconOver.get(), forwardIconDown.get(), nullptr,
                                 reverseIcon.get(), reverseIconOver.get(), reverseIconDown.get());
    signalOrderButton.setEdgeIndent (0);
    signalOrderButton.setColour(juce::DrawableButton::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
    signalOrderButton.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, juce::Colours::transparentBlack);
    signalOrderButton.setClickingTogglesState(true);
}

void HeaderPanel::updateUndoRedoButtons()
{
    undoButton.setEnabled(undoManager.canUndo());
    redoButton.setEnabled(undoManager.canRedo());
}

void HeaderPanel::updatePresetName()
{
    const auto presetName = presetManager.getCurrentPresetName();
    const auto displayName = presetName.isEmpty() ? juce::String("Untitled") : presetName;
    presetBox.setPresetName(presetManager.isPresetModified() ? displayName + "*" : displayName);
}
