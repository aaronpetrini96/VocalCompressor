#include "Panel1176.h"

Panel1176::Panel1176(VocalCompressorAudioProcessor& processor) : audioProcessor(processor)
{
    addAndMakeVisible(grMeter1176);
    for (auto* label : {&input1176Label, &output1176Label, &attack1176Label, &release1176Label, &mix1176Label, &ratioLabel, &gainReductionLabel})
    {
        VCLook::setupLabel(*label, VCColors::almostWhiteText, juce::Justification::centredTop);
        addAndMakeVisible(*label);
    }
    attack1176Label.setJustificationType(juce::Justification::centredBottom);

    for (auto* imageKnob : {&input1176Dial, &output1176Dial, &attack1176Dial, &release1176Dial})
        addAndMakeVisible(*imageKnob);

    input1176Dial.setMouseDragSensitivity(300);
    output1176Dial.setMouseDragSensitivity(300);
    attack1176Dial.setMouseDragSensitivity(250);
    release1176Dial.setMouseDragSensitivity(250);

    mix1176Knob.setColour(juce::Slider::rotarySliderFillColourId, VCColors::almostWhiteText);
    mix1176Knob.setColour(juce::Slider::thumbColourId, juce::Colours::black);
    mix1176Knob.setMouseDragSensitivity(200);
    addAndMakeVisible(mix1176Knob);

    addAndMakeVisible(ratioButtons1176);
}

void Panel1176::paint(juce::Graphics& g)
{
    const auto boundsF = getLocalBounds().toFloat();
    g.drawImage(background1176, boundsF, juce::RectanglePlacement::fillDestination);

    bigScaleFont = VCFonts::barlowCondensedRegular(knobSize * 0.11f);
    bigScaleInset = knobSize * 0.025f;
    bigScaleLabelGap = knobSize * 0.0333f;
    bigScaleDotSize = knobSize * 0.03f;
    for (const auto* dial : {&input1176Dial, &output1176Dial})
        AAUITools::drawKnobScale(g, *dial, bigScaleInset, 11, bigScaleDotSize, bigScaleLabelGap, bigScaleLabels, bigScaleFont);

    smallScaleFont = VCFonts::barlowCondensedRegular(smallKnobsSize * 0.25f);
    smallScaleLabelGap = smallKnobsSize * 0.05f;
    smallScaleDotSize = smallKnobsSize * 0.06f;
    for (const auto* dial : {&attack1176Dial, &release1176Dial})
        AAUITools::drawKnobScale(g, *dial, 0.f, 4, smallScaleDotSize, smallScaleLabelGap, smallScaleLabels, smallScaleFont);
}

void Panel1176::resized()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto grMeterWidth = bounds.getWidth() * 0.6f;
    const auto grMeterHeight = bounds.getHeight() * 0.13f;
    const auto grMeterBounds = bounds.withSizeKeepingCentre(grMeterWidth, grMeterHeight).withY(bounds.getHeight() * 0.01f);
    grMeter1176.setBounds(grMeterBounds.toNearestInt());
    gainReductionLabel.setFont(VCFonts::barlowCondensedMedium(grMeterHeight * 0.6f));
    gainReductionLabel.setBounds(grMeterBounds.withY(grMeterBounds.getBottom()).toNearestInt());

    const auto boundsF = bounds.withTrimmedTop(grMeterHeight * 0.85f);
    const auto boundsFHeight = boundsF.getHeight();
    const auto boundsFWidth = boundsF.getWidth();

    knobSize = boundsFHeight * 0.65f;
    const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
    const auto knobsGap = knobSize;
    const auto inputDialBounds = knobBounds.translated(- knobsGap, 0.f);
    const auto outputDialBounds = knobBounds.translated(knobsGap, 0.f);
    input1176Dial.setBounds(inputDialBounds.toNearestInt());
    output1176Dial.setBounds(outputDialBounds.toNearestInt());

    const auto labelBounds = knobBounds.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.2f);
    const auto labelsCY = knobBounds.getBottom() + labelBounds.getHeight() * 0.5f;
    for (auto* label : {&input1176Label, &output1176Label})
        label->setFont(VCFonts::barlowCondensedMedium(labelBounds.getHeight() * 0.5f));
    input1176Label.setBounds(labelBounds.withCentre({inputDialBounds.getCentreX(), labelsCY}).toNearestInt());
    output1176Label.setBounds(labelBounds.withCentre({outputDialBounds.getCentreX(), labelsCY}).toNearestInt());

    smallKnobsSize = boundsFHeight * 0.2f;
    const auto controlsXGap = boundsFWidth * 0.2f - (knobSize + smallKnobsSize) * 0.5f;
    const auto smallKnobsCX = outputDialBounds.getRight() + controlsXGap + smallKnobsSize * 0.5f;
    const auto smallKnobBounds = knobBounds.withSize(smallKnobsSize, smallKnobsSize).withCentre({smallKnobsCX, knobBounds.getCentreY()});
    const auto attackKnobOffset = boundsFHeight * 0.16f;
    const auto releaseKnobOffset = boundsFHeight * 0.17f;
    const auto smallKnobsLabelGap = smallKnobsSize * 0.72f;
    for (auto* label : {&attack1176Label, &release1176Label})
        label->setFont(VCFonts::barlowCondensedMedium(labelBounds.getHeight() * 0.35f));

    const auto attackCY = smallKnobBounds.getCentreY() - attackKnobOffset;
    const auto releaseCY = smallKnobBounds.getCentreY() + releaseKnobOffset;
    attack1176Dial.setBounds(smallKnobBounds.translated(0.f, -attackKnobOffset).toNearestInt());
    release1176Dial.setBounds(smallKnobBounds.translated(0.f, releaseKnobOffset).toNearestInt());
    attack1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, attackCY - smallKnobsLabelGap - labelBounds.getHeight() * 0.5f}).toNearestInt());
    release1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, releaseCY + smallKnobsLabelGap + labelBounds.getHeight() * 0.5f}).toNearestInt());

    const auto ratioButtonsWidth = boundsFWidth * 0.0575f;
    const auto ratioButtonsBounds = knobBounds.withSizeKeepingCentre(ratioButtonsWidth, ratioButtonsWidth * ratioButtons1176.heightToWidthRatio)
                                    .withRightX(inputDialBounds.getX() - controlsXGap);
    ratioButtons1176.setBounds(ratioButtonsBounds.toNearestInt());

    for (auto* label : {&ratioLabel, &mix1176Label})
        label->setFont(VCFonts::barlowCondensedMedium(labelBounds.getHeight() * 0.4f));

    ratioLabel.setBounds(ratioButtonsBounds.withHeight(labelBounds.getHeight()).withY(ratioButtonsBounds.getBottom() + labelBounds.getHeight() * 0.1f).toNearestInt());

    const auto mix1176KnobSize = knobSize * 0.4f;
    const auto mixKnobCX = (inputDialBounds.getCentreX() + outputDialBounds.getCentreX()) * 0.5f;
    const auto mixKnobBounds = knobBounds.withSize(mix1176KnobSize, mix1176KnobSize).withCentre({mixKnobCX, boundsF.getCentreY()});
    mix1176Knob.setBounds(mixKnobBounds.toNearestInt());
    mix1176Label.setBounds(mixKnobBounds.withHeight(labelBounds.getHeight()).withY(mixKnobBounds.getBottom() - labelBounds.getHeight() * 0.1f).toNearestInt());
}
