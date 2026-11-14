#include "Panel1176.h"

Panel1176::Panel1176(VocalCompressorAudioProcessor& processor) : audioProcessor(processor)
{
    addAndMakeVisible(grMeter1176);
    for (auto* label : {&input1176Label, &output1176Label, &attack1176Label, &release1176Label, &mix1176Label, &ratioLabel, &gainReductionLabel})
        addAndMakeVisible(*label);

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

    const auto bigScaleDotSize = knobSize * 0.03f;
    for (const auto* dial : {&input1176Dial, &output1176Dial})
        AAUITools::drawKnobScale(g, *dial, knobSize * 0.025f, 11, bigScaleDotSize, knobSize * 0.0333f,
                            bigKnobsScale, VCFonts::barlowCondensedRegular(knobSize * 0.11f));

    const auto smallScaleDotSize = smallKnobsSize * 0.07f;
    for (const auto* dial : {&attack1176Dial, &release1176Dial})
        AAUITools::drawKnobScale(g, *dial, 0.f, 4, smallScaleDotSize, smallKnobsSize * 0.03f,
                             smallKnobsScale, VCFonts::barlowCondensedRegular(smallKnobsSize * 0.27f));

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

    smallKnobsSize = knobSize * 0.33f;
    const auto controlsXGap = boundsFWidth * 0.2f - (knobSize + smallKnobsSize) * 0.5f;
    const auto smallKnobBounds = knobBounds.withSizeKeepingCentre(smallKnobsSize, smallKnobsSize);
    const auto smallKnobsCX = outputDialBounds.getRight() + controlsXGap + smallKnobsSize * 0.5f;
    const auto smallKnobsYGap= boundsFHeight * 0.2f;
    const auto attackCY = smallKnobBounds.getCentreY()  - smallKnobsYGap;
    const auto releaseCY = smallKnobBounds.getCentreY() + smallKnobsYGap;
    attack1176Dial.setBounds(smallKnobBounds.withCentre({smallKnobsCX, attackCY}).toNearestInt());
    release1176Dial.setBounds(smallKnobBounds.withCentre({smallKnobsCX, releaseCY}).toNearestInt());

    for (auto* label : {&attack1176Label, &release1176Label})
        label->setFont(VCFonts::barlowCondensedRegular(labelBounds.getHeight() * 0.3f));

    const auto smallKnobsLabelGap = smallKnobsSize;
    attack1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, attackCY + smallKnobsLabelGap}).toNearestInt());
    release1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, releaseCY + smallKnobsLabelGap}).toNearestInt());

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
