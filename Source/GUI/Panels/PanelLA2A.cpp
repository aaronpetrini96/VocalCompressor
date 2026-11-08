#include "PanelLA2A.h"

PanelLA2A::PanelLA2A(VocalCompressorAudioProcessor& processor) : audioProcessor(processor)
{
    for (auto* imageKnob : {&la2aGainDial, &la2aPeakReductionDial})
    {
        addAndMakeVisible(*imageKnob);
        imageKnob->setAlwaysOnTop(true);
    }

    la2aMixKnob.setColour(juce::Slider::rotarySliderFillColourId, VCColors::almostBlack);
    la2aMixKnob.setColour(juce::Slider::thumbColourId, VCColors::almostWhiteText);
    la2aMixKnob.setAlwaysOnTop(true);
    addAndMakeVisible(la2aMixKnob);

    la2aModeButton.setImages(false, true, true,
                             la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                               la2aModeImageUP, 0.90f, juce::Colours::transparentBlack,
                              la2aModeImageDOWN, 0.90f, juce::Colours::transparentBlack);
    la2aModeButton.setClickingTogglesState(true);
    la2aModeButton.setAlwaysOnTop(true);
    la2aModeButton.onStateChange = [this] { updateModeLabels(); };
    addAndMakeVisible(la2aModeButton);

    for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel, &la2aCompModeLabel, &la2aLimitModeLabel})
    {
        VCLook::setupLabel(*label, juce::Colours::black);
        addAndMakeVisible(*label);
    }
    for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel})
        label->setJustificationType(juce::Justification::centredTop);
    updateModeLabels();

    VCLook::setupLabel(la2aMixLabel, juce::Colours::black, juce::Justification::centredTop);
    addAndMakeVisible(la2aMixLabel);

    addAndMakeVisible(grMeterLA2A);
    VCLook::setupLabel(gainReductionLabel, juce::Colours::black, juce::Justification::centredTop);
    addAndMakeVisible(gainReductionLabel);
}

void PanelLA2A::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    g.drawImage(clearBackground, boundsF, juce::RectanglePlacement::fillDestination);
    for (const auto* dial : {&la2aGainDial, &la2aPeakReductionDial})
        AAUITools::drawKnobArcScale(g, dial->getBounds().toFloat().reduced(scaleInset), dial->getRotaryParameters().startAngleRadians, dial->getRotaryParameters().endAngleRadians,
                                    11, scaleTickLength, scaleLineThickness, scaleLabels, scaleFont, juce::Colours::black);
}

void PanelLA2A::resized()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto grMeterWidth = bounds.getWidth() * 0.6f;
    const auto grMeterHeight = bounds.getHeight() * 0.13f;
    const auto grMeterBounds = bounds.withSizeKeepingCentre(grMeterWidth, grMeterHeight).withY(bounds.getHeight() * 0.01f);
    grMeterLA2A.setBounds(grMeterBounds.toNearestInt());

    gainReductionLabel.setFont(VCFonts::barlowCondensedSemiBold(grMeterHeight * 0.6f));
    gainReductionLabel.setBounds(grMeterBounds.withY(grMeterBounds.getBottom()).toNearestInt());

    const auto boundsF = bounds.withTrimmedTop(grMeterHeight * 0.85f);
    const auto boundsFHeight = boundsF.getHeight();
    const auto boundsFWidth = boundsF.getWidth();

    const auto knobSize = boundsFHeight * 0.65f;
    const auto knobsGap = knobSize;
    const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
    la2aGainDial.setBounds(knobBounds.translated(-knobsGap, 0.f).toNearestInt());
    la2aPeakReductionDial.setBounds(knobBounds.translated(knobsGap, 0.f).toNearestInt());
    scaleFont = VCFonts::barlowCondensedMedium(knobSize * 0.11f);
    scaleInset = bounds.getHeight() * 0.07f;
    scaleTickLength = bounds.getHeight() * 0.03f;
    scaleLineThickness = bounds.getHeight() * 0.006f;

    const auto labelBounds = boundsF.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.2f);
    const auto labelsCY = boundsF.getCentreY() + knobSize * 0.5f + labelBounds.getHeight() * 0.5f;
    for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel})
        label->setFont(VCFonts::barlowCondensedSemiBold(boundsFHeight * 0.1f));
    for (auto* label : {&la2aCompModeLabel, &la2aLimitModeLabel})
        label->setFont(VCFonts::barlowCondensedSemiBold(labelBounds.getHeight() * 0.4f));
    la2aGainLabel.setBounds(
        labelBounds.withCentre({la2aGainDial.getBounds().toFloat().getCentreX(), labelsCY}).toNearestInt());
    la2aPeakReductionLabel.setBounds(labelBounds.withCentre({
        la2aPeakReductionDial.getBounds().toFloat().getCentreX(), labelsCY
    }).toNearestInt());

    const auto deltaY = bounds.getHeight() * 0.15f;
    const auto mixKnobSize = knobSize * 0.4f;
    const auto mixKnobBounds = boundsF.withSizeKeepingCentre(mixKnobSize, mixKnobSize).translated(0.f, -deltaY);
    la2aMixKnob.setBounds(mixKnobBounds.toNearestInt());
    la2aMixLabel.setFont(VCFonts::barlowCondensedSemiBold(labelBounds.getHeight() * 0.4f));
    la2aMixLabel.setBounds(mixKnobBounds.withHeight(labelBounds.getHeight()).withY(mixKnobBounds.getBottom() - labelBounds.getHeight() * 0.1f).toNearestInt());

    const auto modeSize = boundsFHeight * 0.25f;
    const auto modeSwitchBounds = boundsF.withSizeKeepingCentre(modeSize, modeSize).withY(boundsFHeight * 0.75f);
    la2aModeButton.setBounds(modeSwitchBounds.toNearestInt());
    la2aCompModeLabel.setBounds(labelBounds.withCentre({modeSwitchBounds.getCentreX(), modeSwitchBounds.getCentreY() + modeSize * 0.4166f}).toNearestInt());
    la2aLimitModeLabel.setBounds(labelBounds.withCentre({modeSwitchBounds.getCentreX(), modeSwitchBounds.getCentreY() - modeSize * 0.4502f}).toNearestInt());
}

void PanelLA2A::updateModeLabels()
{
    const bool isLimitMode = la2aModeButton.getToggleState();
    la2aLimitModeLabel.setVisible(isLimitMode);
    la2aCompModeLabel.setVisible(! isLimitMode);
}
