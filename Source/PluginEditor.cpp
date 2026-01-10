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
    
    for (auto& imageKnob : imageKnobs)
    {
        addAndMakeVisible(*imageKnob);
        imageKnob->setAlwaysOnTop(true);
    }
    
    la2aModeButton.setImages(false, true, true,
                             la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                             la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                             la2aModeImageDOWN, 0.90f, juce::Colours::transparentBlack);
    la2aModeButton.setClickingTogglesState(true);
    la2aModeButton.setSize(knobSize / 2.5, knobSize / 2.5);
    la2aModeButton.setAlwaysOnTop(true);
    addAndMakeVisible(la2aModeButton);
    
    for (auto& label : knobLabels)
    {
        labelsGeneralSetup(label);
        addAndMakeVisible(*label);
    }
    attack1176Label.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    release1176Label.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    la2aCompModeLabel.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    la2aLimitModeLabel.setFont(juce::Font(juce::FontOptions(labelFont, 14.f, juce::Font::FontStyleFlags::plain)));
    
    for (auto& ratioBtn : ratioButtons)
    {
        setupRatioButtons(ratioBtn);
        addAndMakeVisible(*ratioBtn);
    }
    
    ratio4Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 0);};
    ratio8Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 1);};
    ratio16Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 2);};
    ratio20Btn.onClick = [this] () {setRatioIndex(ParamNames::get1176RatioName(), 3);};
    ratioAllBtn.onClick = [this]() {setRatioIndex(ParamNames::get1176RatioName(), 4);};
    int ratioIndex = static_cast<int>(audioProcessor.apvts.getRawParameterValue(ParamNames::get1176RatioName())->load());
    updateRatioButtons(ratioIndex);
    
    for (auto& grMeter : grMeters)
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
    startTimerHz(30);
    setSize (guiWidth, guiHeightExtended);
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(41, 42, 48).darker(1));
    auto halfHeight = guiHeight / 2;
    g.drawImageWithin(darkBackground, 0, 0, guiWidth, halfHeight, juce::RectanglePlacement::stretchToFit);
    g.drawImageWithin(clearBackground, 0, halfHeight, guiWidth, halfHeight, juce::RectanglePlacement::stretchToFit);
    g.drawImageWithin(screwImage, 0, guiHeight + 6.25 , 25, 25, juce::RectanglePlacement::stretchToFit);
    g.drawImageWithin(screwImage, guiWidth - 25, guiHeight + 6.25 , 25, 25, juce::RectanglePlacement::stretchToFit);
    
    // Horizontal line separating 1176 and LA2A
    g.setColour(juce::Colours::whitesmoke.withAlpha(0.5f));
    g.drawLine(0.f, (float)halfHeight, (float)guiWidth, (float)halfHeight, 2.f);
    g.drawLine(0.f, (float)guiHeight, (float)guiWidth, (float)guiHeight, 2.f);
    
    // Unit titles
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(titleFont, 20.0f, juce::Font::FontStyleFlags::italic)));
    g.drawText("FET-76", 0, 10, guiWidth, halfHeight, juce::Justification::centredTop);
    g.drawText("OPT-2A", 0, halfHeight + 10, guiWidth, halfHeight, juce::Justification::centredTop);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    auto boundsBox1 = juce::Rectangle<int>(guiWidth, guiHeight);
    auto bounds = boundsBox1;
    const auto halfHeight = bounds.getHeight() * 0.5;
    const auto grMeterWidth = 20;
    const auto grMeterHeight = knobSize * 0.8;
    const auto bypassBtnSize = 20;
    const auto ratioBtnWidth = knobSize * 0.25;
    const auto ratioBtnHeight = knobSize * 0.2;

    // Top Half - 1176
    const auto topArea = bounds.removeFromTop(halfHeight).reduced(10);
    const auto knobSpacing = topArea.getWidth() * 0.1;
    
    input1176Dial.setBounds(knobSpacing * 0.5, topArea.getY() * 4, knobSize, knobSize);
    input1176Label.setCentrePosition(input1176Dial.getX() + knobSize * 0.5, input1176Dial.getBottom() * 1.01);
    output1176Dial.setBounds(input1176Dial.getRight() + knobSpacing * 0.5, input1176Dial.getY(), knobSize, knobSize);
    output1176Label.setCentrePosition(output1176Dial.getX() + knobSize * 0.5, output1176Dial.getBottom() * 1.01);
    
    attack1176Dial.setBounds(output1176Dial.getRight() + knobSpacing * 0.5, output1176Dial.getY() - 22, knobSize * 0.5, knobSize * 0.5);
    attack1176Label.setCentrePosition(attack1176Dial.getRight() - knobSize * 0.25, attack1176Dial.getBottom() * 1.08);
    release1176Dial.setBounds(attack1176Dial.getX(), attack1176Label.getBottom() * 0.95, knobSize * 0.5, knobSize * 0.5);
    release1176Label.setCentrePosition(release1176Dial.getRight() - knobSize * 0.25, release1176Dial.getBottom() * 1.05);
    
    ratio20Btn.setBounds(attack1176Dial.getRight() * 1.045, attack1176Dial.getY() * 2.85, ratioBtnWidth, ratioBtnHeight);
    ratio16Btn.setBounds(ratio20Btn.getX(), ratio20Btn.getBottom(), ratioBtnWidth, ratioBtnHeight);
    ratio8Btn.setBounds(ratio16Btn.getX(), ratio16Btn.getBottom(), ratioBtnWidth, ratioBtnHeight);
    ratio4Btn.setBounds(ratio8Btn.getX(), ratio8Btn.getBottom(), ratioBtnWidth, ratioBtnHeight);
    ratioLabel.setCentrePosition(ratio4Btn.getRight() - ratioBtnWidth * 0.5, ratio4Btn.getBottom() * 1.12);
    ratioAllBtn.setBounds(ratio20Btn.getX(), ratio20Btn.getY() - ratio20Btn.getHeight(), ratioBtnWidth, ratioBtnHeight);
    
    grMeter1176Left.setBounds(ratio20Btn.getRight() * 1.034, attack1176Dial.getY() * 2.84, grMeterWidth, grMeterHeight);
    grMeter1176Right.setBounds(grMeter1176Left.getRight(), grMeter1176Left.getY(), grMeterWidth, grMeterHeight);
    gr1176Label.setCentrePosition(grMeter1176Left.getRight(), grMeter1176Left.getBottom() * 1.12);
    grTextDisplay1176.setBounds(grMeter1176Left.getX() * 0.992, grMeter1176Left.getY() * 0.4, grMeterWidth * 2.5, grMeterWidth * 2);
    
    bypassButton1176.setBounds(bypassBtnSize / 2, bypassBtnSize / 2, bypassBtnSize, bypassBtnSize);
    
    // Bottom Half - LA2A
    const auto bottomArea = bounds.reduced(10);

    la2aGainDial.setBounds(knobSpacing * 1.25, bottomArea.getY() * 1.15, knobSize, knobSize);
    la2aGainLabel.setCentrePosition(la2aGainDial.getX() + knobSize * 0.5, la2aGainDial.getBottom());
    la2aPeakReductionDial.setBounds(bottomArea.getWidth() - knobSize - knobSpacing, la2aGainDial.getY(), knobSize, knobSize);
    la2aPeakReductionLabel.setCentrePosition(la2aPeakReductionDial.getX() + knobSize * 0.5, la2aPeakReductionDial.getBottom());
    
    la2aModeButton.setBounds(-5, bottomArea.getY() + knobSize * 0.97, knobSize / 2.5, knobSize / 2.5);
    la2aCompModeLabel.setCentrePosition(la2aModeButton.getRight() * 0.49, la2aModeButton.getBottom() * 0.99);
    la2aLimitModeLabel.setCentrePosition(la2aModeButton.getRight() * 0.49, la2aModeButton.getY() * 1.01);
 
    grMeterLA2ALeft.setBounds((bottomArea.getWidth() - grMeterWidth) * 0.5, la2aGainDial.getY() * 1.06, grMeterWidth, grMeterHeight);
    grMeterLA2ARight.setBounds(grMeterLA2ALeft.getRight(), grMeterLA2ALeft.getY(), grMeterWidth, grMeterHeight);
    grLA2ALabel.setCentrePosition(grMeterLA2ALeft.getRight(), grMeterLA2ALeft.getBottom() * 1.035);
    grTextDisplayLA2A.setBounds(grMeterLA2ALeft.getX() * 0.992, grMeterLA2ALeft.getY() * 0.89, grMeterWidth * 2.5, grMeterWidth * 2);
    
    bypassButtonLA2A.setBounds(bypassButton1176.getX(), bottomArea.getY(), bypassBtnSize, bypassBtnSize);
    
    // PresetManager + Utilities
    presetManager.setBounds(0, guiHeightExtended - (guiHeightExtended - guiHeight), guiWidth, (guiHeightExtended - guiHeight));
    bypassPluginButton.setBounds(guiWidth * 0.14, guiHeight + 7, 20, 20);
    reverseSignalOrderButton.setBounds(guiWidth * 0.7085, guiHeight + presetManager.saveBtnY, 125, presetManager.saveBtnHeight);
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
