#include "PanelLA2A.h"

PanelLA2A::PanelLA2A(VocalCompressorAudioProcessor& processor) : audioProcessor(processor)
{
    for (auto* label : {&la2aCompModeLabel, &la2aLimitModeLabel})
    {
        VCLook::setupLabel(*label, juce::Colours::black);
        addAndMakeVisible(*label);
    }
    for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel, &la2aMixLabel, &gainReductionLabel})
    {
        VCLook::setupLabel(*label, juce::Colours::black, juce::Justification::centredTop);
        addAndMakeVisible(*label);
    }

    addAndMakeVisible(grMeterLA2A);

    for (auto* imageKnob : {&la2aGainDial, &la2aPeakReductionDial})
    {
        imageKnob->setMouseDragSensitivity(300);
        addAndMakeVisible(*imageKnob);
    }

    la2aMixKnob.setColour(juce::Slider::rotarySliderFillColourId, VCColors::almostBlack);
    la2aMixKnob.setColour(juce::Slider::thumbColourId, VCColors::almostBlack);
    la2aMixKnob.setMouseDragSensitivity(200);
    addAndMakeVisible(la2aMixKnob);

    la2aModeButton.setImages(false, true, true,
                             la2aModeImageUP, 1.f, juce::Colours::transparentBlack,
                               la2aModeImageUP, 1.f, juce::Colours::transparentBlack,
                              la2aModeImageDOWN, 1.f, juce::Colours::transparentBlack);
    la2aModeButton.setClickingTogglesState(true);
    addAndMakeVisible(la2aModeButton);

}

void PanelLA2A::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    g.drawImage(clearBackground, boundsF, juce::RectanglePlacement::fillDestination);

    const auto scaleFont = VCFonts::barlowCondensedSemiBold(knobSize * 0.12f);
    const auto scaleInset = knobSize * 0.125f;
    const auto scaleLineThickness = knobSize * 0.0125f;
    const auto scaleTickLength = knobSize * 0.05f;
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

    knobSize = boundsFHeight * 0.65f;
    const auto knobActualSize = boundsFHeight * 0.77f;
    const auto knobBounds = boundsF.withSizeKeepingCentre(knobActualSize, knobActualSize);
    const auto knobsGap = knobSize;
    const auto gainDialBounds = knobBounds.translated(-knobsGap, 0.f);
    const auto reductionDialBounds = knobBounds.translated(knobsGap, 0.f);
    la2aGainDial.setBounds(gainDialBounds.toNearestInt());
    la2aPeakReductionDial.setBounds(reductionDialBounds.toNearestInt());

    const auto bigKnobLabelBounds = boundsF.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.2f);
    const auto bigKnobLabelCY = boundsF.getCentreY() + (knobSize + bigKnobLabelBounds.getHeight()) * 0.5f;
    for (auto* label : {&la2aGainLabel, &la2aPeakReductionLabel})
        label->setFont(VCFonts::barlowCondensedSemiBold(bigKnobLabelBounds.getHeight() * 0.5f));
    la2aGainLabel.setBounds(bigKnobLabelBounds.withCentre({gainDialBounds.getCentreX(), bigKnobLabelCY}).toNearestInt());
    la2aPeakReductionLabel.setBounds(bigKnobLabelBounds.withCentre({reductionDialBounds.getCentreX(), bigKnobLabelCY}).toNearestInt());

    for (auto* label : {&la2aMixLabel, &la2aCompModeLabel, &la2aLimitModeLabel})
        label->setFont(VCFonts::barlowCondensedMedium(bigKnobLabelBounds.getHeight() * 0.4f));

    const auto deltaY = bounds.getHeight() * 0.15f;
    const auto mixKnobSize = knobSize * 0.4f;
    const auto mixKnobBounds = boundsF.withSizeKeepingCentre(mixKnobSize, mixKnobSize).translated(0.f, -deltaY);
    la2aMixKnob.setBounds(mixKnobBounds.toNearestInt());
    la2aMixLabel.setBounds(mixKnobBounds.withHeight(bigKnobLabelBounds.getHeight()).withY(mixKnobBounds.getBottom() - bigKnobLabelBounds.getHeight() * 0.1f).toNearestInt());

    const auto modeSwitchSize = boundsFHeight * 0.3f;
    const auto modeLabelsGap = modeSwitchSize * 0.4f;
    const auto modeSwitchBounds = boundsF.withSizeKeepingCentre(modeSwitchSize, modeSwitchSize).withY(boundsFHeight * 0.75f);
    la2aModeButton.setBounds(modeSwitchBounds.toNearestInt());
    la2aCompModeLabel.setBounds(bigKnobLabelBounds.withCentre(modeSwitchBounds.getCentre()).translated(0.f,modeLabelsGap).toNearestInt());
    la2aLimitModeLabel.setBounds(bigKnobLabelBounds.withCentre(modeSwitchBounds.getCentre()).translated(0.f,-modeLabelsGap).toNearestInt());
}
