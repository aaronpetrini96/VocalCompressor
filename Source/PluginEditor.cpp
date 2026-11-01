/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessorEditor::VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
presetManager(p)
{
    
    for (const auto& imageKnob : imageKnobs)
    {
        addAndMakeVisible(*imageKnob);
        imageKnob->setAlwaysOnTop(true);
    }
    addAndMakeVisible(mix1176Knob);
    mix1176Knob.setAlwaysOnTop(true);
    
    la2aModeButton.setImages(false, true, true,
                             la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                             la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                             la2aModeImageDOWN, 0.90f, juce::Colours::transparentBlack);
    la2aModeButton.setClickingTogglesState(true);
    la2aModeButton.setSize(knobSize / 2.5, knobSize / 2.5);
    la2aModeButton.setAlwaysOnTop(true);
    addAndMakeVisible(la2aModeButton);
    
    for (const auto& label : knobLabels)
    {
        labelsGeneralSetup(label);
        addAndMakeVisible(*label);
    }
    attack1176Label.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    release1176Label.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    mix1176Label.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    la2aCompModeLabel.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    la2aLimitModeLabel.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    
    for (const auto& ratioBtn : ratioButtons)
    {
        setupRatioButtons(ratioBtn);
        addAndMakeVisible(*ratioBtn);
    }
    
    ratio4Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 0);};
    ratio8Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 1);};
    ratio16Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 2);};
    ratio20Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 3);};
    ratioAllBtn.onClick = [this]() {setRatioIndex(ParamNames::get1176RatioName(), 4);};
    const int ratioIndex = static_cast<int>(audioProcessor.apvts.getRawParameterValue(ParamNames::get1176RatioName())->load());
    updateRatioButtons(ratioIndex);
    
    for (const auto& grMeter : grMeters)
    {
        addAndMakeVisible(*grMeter);
    }
    addAndMakeVisible(grTextDisplay1176);
    addAndMakeVisible(grTextDisplayLA2A);
    
    addAndMakeVisible(bypassButton1176);
    addAndMakeVisible(bypassButtonLA2A);
    
    addAndMakeVisible(presetManager);
    addAndMakeVisible(bypassPluginButton);
    
    setupReverseSignalButton(reverseSignalOrderButton);
    reverseSignalOrderButton.onClick = [this]
    {
        updateReverseSignalButtonText(reverseSignalOrderButton.getToggleState());
    };
    addAndMakeVisible(reverseSignalOrderButton);
    updateReverseSignalButtonText(reverseSignalOrderButton.getToggleState());
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setResizable(false, true);
    setResizeLimits(juce::roundToInt(guiWidth * 0.6f), juce::roundToInt(guiHeightExtended * 0.6f),
                    juce::roundToInt(guiWidth * 2.f), juce::roundToInt(guiHeightExtended * 2.f));
    getConstrainer()->setFixedAspectRatio(static_cast<double>(guiWidth) / static_cast<double>(guiHeightExtended));
    setSize (guiWidth, guiHeightExtended);
    startTimerHz(30);
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto mainHeight = boundsFHeight * static_cast<float>(guiHeight) / static_cast<float>(guiHeightExtended);
    const auto halfHeight = mainHeight * 0.5f;

    g.fillAll(juce::Colour(41, 42, 48).darker(1));
    const auto topHalf = boundsF.withHeight(halfHeight);
    const auto bottomHalf = topHalf.withY(halfHeight);
    g.drawImage(darkBackground, topHalf, juce::RectanglePlacement::stretchToFit);
    g.drawImage(clearBackground, bottomHalf, juce::RectanglePlacement::stretchToFit);

    const auto screwSize = boundsFWidth * 0.0417f;
    const auto screwBounds = boundsF.withSize(screwSize, screwSize).withY(mainHeight + screwSize * 0.25f);
    g.drawImage(screwImage, screwBounds, juce::RectanglePlacement::centred);
    g.drawImage(screwImage, screwBounds.withRightX(boundsFWidth), juce::RectanglePlacement::centred);
    
    // Horizontal line separating 1176 and LA2A
    const auto lineThickness = boundsFWidth * 0.0033f;
    g.setColour(juce::Colours::whitesmoke.withAlpha(0.5f));
    g.drawLine(0.f, halfHeight, boundsFWidth, halfHeight, lineThickness);
    g.drawLine(0.f, mainHeight, boundsFWidth, mainHeight, lineThickness);
    
    // Unit titles
    const auto titleOffset = mainHeight * 0.0222f;
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(titleFont, boundsFWidth * 0.0333f, juce::Font::FontStyleFlags::italic)));
    g.drawText("FET-76", topHalf.withTrimmedTop(titleOffset), juce::Justification::centredTop);
    g.drawText("OPT-2A", bottomHalf.withTrimmedTop(titleOffset), juce::Justification::centredTop);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    const auto boundsF = getLocalBounds().toFloat();
    const auto boundsFWidth = boundsF.getWidth();
    const auto boundsFHeight = boundsF.getHeight();
    const auto mainHeight = boundsFHeight * static_cast<float>(guiHeight) / static_cast<float>(guiHeightExtended);

    const auto knobSizeRef = boundsFWidth * 0.25f;
    const auto smallKnobSize = knobSizeRef * 0.5f;
    const auto smallControlSize = knobSizeRef * 0.1333f;
    const auto knobGap = boundsFWidth * 0.0483f;

    const auto knobBounds = boundsF.withSize(knobSizeRef, knobSizeRef);
    const auto smallKnobBounds = boundsF.withSize(smallKnobSize, smallKnobSize);
    const auto labelBounds = boundsF.withSize(knobSizeRef, knobSizeRef * 0.3f);
    const auto grMeterBounds = boundsF.withSize(smallControlSize, knobSizeRef * 0.8f);
    const auto grTextBounds = boundsF.withSize(smallControlSize * 2.5f, smallControlSize * 2.f);
    const auto bypassBounds = boundsF.withSize(smallControlSize, smallControlSize);

    // Top Half - 1176
    const auto topKnobsCY = mainHeight * 0.2556f;
    const auto topLabelsCY = topKnobsCY + knobSizeRef * 0.51f;
    const auto inputCX = knobGap + knobSizeRef * 0.5f;
    const auto outputCX = inputCX + knobSizeRef + knobGap;

    input1176Dial.setBounds(knobBounds.withCentre({inputCX, topKnobsCY}).toNearestInt());
    input1176Label.setBounds(labelBounds.withCentre({inputCX, topLabelsCY}).toNearestInt());
    output1176Dial.setBounds(knobBounds.withCentre({outputCX, topKnobsCY}).toNearestInt());
    output1176Label.setBounds(labelBounds.withCentre({outputCX, topLabelsCY}).toNearestInt());

    const auto mixScrewCX = inputCX + (knobSizeRef + knobGap) * 0.5f;
    const auto mixScrewCY = topKnobsCY + knobSizeRef * 0.5f - smallControlSize;
    const auto mixScrewBounds = bypassBounds.withCentre({mixScrewCX, mixScrewCY});
    mix1176Knob.setBounds(mixScrewBounds.toNearestInt());
    mix1176Label.setBounds(labelBounds.withCentre({mixScrewCX, mixScrewBounds.getBottom() + knobSizeRef * 0.053f}).toNearestInt());

    const auto attackCX = outputCX + (knobSizeRef + smallKnobSize) * 0.5f + knobGap;
    const auto attackCY = mainHeight * 0.1233f;
    const auto releaseCY = attackCY + smallKnobSize * 1.3067f;
    attack1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, attackCY}).toNearestInt());
    attack1176Label.setBounds(labelBounds.withCentre({attackCX, attackCY + smallKnobSize * 0.5933f}).toNearestInt());
    release1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, releaseCY}).toNearestInt());
    release1176Label.setBounds(labelBounds.withCentre({attackCX, releaseCY + smallKnobSize * 0.62f}).toNearestInt());

    const auto ratioColumnCX = boundsFWidth * 0.834f;
    const auto ratioColumnTopY = mainHeight * 0.0467f;
    const auto ratioBtnBounds = boundsF.withSize(knobSizeRef * 0.25f, knobSizeRef * 0.2f).withCentre({ratioColumnCX, 0.f});
    auto ratioBtnY = ratioColumnTopY;
    for (auto* ratioBtn : {&ratioAllBtn, &ratio20Btn, &ratio16Btn, &ratio8Btn, &ratio4Btn})
    {
        ratioBtn->setBounds(ratioBtnBounds.withY(ratioBtnY).toNearestInt());
        ratioBtnY += ratioBtnBounds.getHeight();
    }
    ratioLabel.setBounds(labelBounds.withCentre({ratioColumnCX, topLabelsCY}).toNearestInt());

    const auto grMeters1176CX = boundsFWidth * 0.9267f;
    const auto grMeters1176TopY = ratioColumnTopY + ratioBtnBounds.getHeight();
    grMeter1176Left.setBounds(grMeterBounds.withPosition(grMeters1176CX - smallControlSize, grMeters1176TopY).toNearestInt());
    grMeter1176Right.setBounds(grMeterBounds.withPosition(grMeters1176CX, grMeters1176TopY).toNearestInt());
    gr1176Label.setBounds(labelBounds.withCentre({grMeters1176CX, topLabelsCY}).toNearestInt());
    grTextDisplay1176.setBounds(grTextBounds.withCentre({grMeters1176CX, mainHeight * 0.0889f}).toNearestInt());

    bypassButton1176.setBounds(bypassBounds.withPosition(smallControlSize * 0.5f, smallControlSize * 0.5f).toNearestInt());

    // Bottom Half - LA2A
    const auto bottomKnobsCY = mainHeight * 0.7667f;
    const auto bottomLabelsCY = bottomKnobsCY + knobSizeRef * 0.5f;
    const auto la2aGainCX = boundsFWidth * 0.245f;
    const auto la2aPeakReductionCX = boundsFWidth * 0.745f;

    la2aGainDial.setBounds(knobBounds.withCentre({la2aGainCX, bottomKnobsCY}).toNearestInt());
    la2aGainLabel.setBounds(labelBounds.withCentre({la2aGainCX, bottomLabelsCY}).toNearestInt());
    la2aPeakReductionDial.setBounds(knobBounds.withCentre({la2aPeakReductionCX, bottomKnobsCY}).toNearestInt());
    la2aPeakReductionLabel.setBounds(labelBounds.withCentre({la2aPeakReductionCX, bottomLabelsCY}).toNearestInt());

    const auto modeButtonSize = knobSizeRef * 0.4f;
    const juce::Point<float> modeButtonCentre {boundsFWidth * 0.0417f, mainHeight * 0.9111f};
    la2aModeButton.setBounds(boundsF.withSize(modeButtonSize, modeButtonSize).withCentre(modeButtonCentre).toNearestInt());
    la2aCompModeLabel.setBounds(labelBounds.withCentre(modeButtonCentre.translated(0.f, modeButtonSize * 0.4167f)).toNearestInt());
    la2aLimitModeLabel.setBounds(labelBounds.withCentre(modeButtonCentre.translated(0.f, -modeButtonSize * 0.45f)).toNearestInt());

    const auto grMetersLA2ACX = boundsF.getCentreX();
    const auto grMetersLA2ATopY = mainHeight * 0.6356f;
    grMeterLA2ALeft.setBounds(grMeterBounds.withPosition(grMetersLA2ACX - smallControlSize, grMetersLA2ATopY).toNearestInt());
    grMeterLA2ARight.setBounds(grMeterBounds.withPosition(grMetersLA2ACX, grMetersLA2ATopY).toNearestInt());
    grLA2ALabel.setBounds(labelBounds.withCentre({grMetersLA2ACX, bottomLabelsCY}).toNearestInt());
    grTextDisplayLA2A.setBounds(grTextBounds.withCentre({grMetersLA2ACX, mainHeight * 0.6089f}).toNearestInt());

    bypassButtonLA2A.setBounds(bypassBounds.withPosition(smallControlSize * 0.5f, mainHeight * 0.5222f).toNearestInt());

    // PresetManager + Utilities
    const auto presetArea = boundsF.withTrimmedTop(mainHeight);
    presetManager.setBounds(presetArea.toNearestInt());
    bypassPluginButton.setBounds(bypassBounds.withPosition(boundsFWidth * 0.14f, presetArea.getY() + smallControlSize * 0.35f).toNearestInt());
    reverseSignalOrderButton.setBounds(presetArea.withX(boundsFWidth * 0.7085f)
                                                 .withWidth(boundsFWidth * 0.2083f)
                                                 .withY(presetArea.getY() + presetManager.saveBtnY)
                                                 .withHeight(static_cast<float>(presetManager.saveBtnHeight))
                                                 .toNearestInt());
}


void VocalCompressorAudioProcessorEditor::timerCallback()
{
    if (la2aModeButton.getToggleState() == false)
    {
        la2aCompModeLabel.setVisible(true);
        la2aLimitModeLabel.setVisible(false);
    }
    else
    {
        la2aLimitModeLabel.setVisible(true);
        la2aCompModeLabel.setVisible(false);
    }
    
    updateReverseSignalButtonText(reverseSignalOrderButton.getToggleState());
    
    float gr1176 = audioProcessor.gR1176;
    float grLA2A = audioProcessor.grLA2A;
    
    grMeter1176Left.setReduction(gr1176);
    grMeter1176Right.setReduction(gr1176);
    grMeterLA2ALeft.setReduction(grLA2A);
    grMeterLA2ARight.setReduction(grLA2A);
    grTextDisplay1176.setValue(gr1176);
    grTextDisplayLA2A.setValue(grLA2A);
}


void VocalCompressorAudioProcessorEditor::labelsGeneralSetup (juce::Label* label)
{
    label->setText(label->getName(), juce::dontSendNotification);
    label->setFont(juce::Font(juce::FontOptions(labelFont, 16.f, juce::Font::FontStyleFlags::plain)));
    label->setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label->setJustificationType(juce::Justification::centred);
    label->setSize(knobSize, knobSize * 0.3);
}

void VocalCompressorAudioProcessorEditor::setRatioIndex(const juce::String& parameterName, const int& index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter(parameterName)))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(static_cast<float>(index) / (static_cast<float>(param->choices.size() - 1)));
        param->endChangeGesture();
        updateRatioButtons(index);
    }
}

void VocalCompressorAudioProcessorEditor::updateRatioButtons(const int& selectedIndex)
{
    ratio4Btn.setToggleState(selectedIndex == 0, juce::dontSendNotification);
    ratio8Btn.setToggleState(selectedIndex == 1, juce::dontSendNotification);
    ratio16Btn.setToggleState(selectedIndex == 2, juce::dontSendNotification);
    ratio20Btn.setToggleState(selectedIndex == 3, juce::dontSendNotification);
}

void VocalCompressorAudioProcessorEditor::setupRatioButtons(juce::TextButton* ratioBtn)
{
    ratioBtn->setRadioGroupId(1515);
    ratioBtn->setSize(knobSize * 0.225, knobSize * 0.225);
    ratioBtn->setClickingTogglesState(true);
    ratioBtn->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
    ratioBtn->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red.withAlpha(0.7f).darker(0.15f));
    ratioBtn->setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    ratioBtn->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    ratioBtn->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
}

void VocalCompressorAudioProcessorEditor::updateReverseSignalButtonText (bool state)
{
    if (state)
        reverseSignalOrderButton.setButtonText("OPT-2A -> FET-76");
    else
        reverseSignalOrderButton.setButtonText("FET-76 -> OPT-2A");
}

void VocalCompressorAudioProcessorEditor::setupReverseSignalButton (juce::TextButton& btn)
{
    reverseSignalOrderButton.setColour(juce::TextButton::buttonColourId, juce::Colour(100, 100, 110).darker(1.f));
    reverseSignalOrderButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(100, 100, 110).darker(1.f));
    reverseSignalOrderButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    reverseSignalOrderButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    reverseSignalOrderButton.setClickingTogglesState(true);
}
