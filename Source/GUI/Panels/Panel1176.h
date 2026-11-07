#pragma once

#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/ArcImageKnob.h"
#include "../Components/GRDistressorMeter.h"
#include "../Components/Ratio1176Buttons.h"

class Panel1176 : public juce::Component
{
public:
    explicit Panel1176(VocalCompressorAudioProcessor& processor)     : audioProcessor(processor)
    {

        addAndMakeVisible(grMeter1176);
        VCLook::setupLabel(grLabel, VCColors::almostWhiteText, juce::Justification::centredLeft);
        VCLook::setupLabel(dBLabel, VCColors::almostWhiteText, juce::Justification::centredRight);
        addAndMakeVisible(grLabel);
        addAndMakeVisible(dBLabel);
        for (auto* label : {&input1176Label, &output1176Label, &attack1176Label, &release1176Label, &mix1176Label, &ratioLabel})
        {
            VCLook::setupLabel(*label, juce::Colours::white, juce::Justification::centredTop);
            addAndMakeVisible(*label);
        }

        for (auto* imageKnob : {&input1176Dial, &output1176Dial, &attack1176Dial, &release1176Dial})
        {
            addAndMakeVisible(*imageKnob);
        }

        mix1176Knob.setColour(juce::Slider::rotarySliderFillColourId, VCColors::almostWhiteText);
        mix1176Knob.setColour(juce::Slider::thumbColourId, juce::Colours::black);
        addAndMakeVisible(mix1176Knob);

        ratioButtons1176.onIndexChosen = [this](int newIndex) { ratioAttachment.setValueAsCompleteGesture(static_cast<float>(newIndex)); };
        ratioAttachment.sendInitialUpdate();
        addAndMakeVisible(ratioButtons1176);

    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.drawImage(darkBackground, boundsF, juce::RectanglePlacement::stretchToFit);

        for (const auto* dial : {&input1176Dial, &output1176Dial})
            AAUITools::drawKnobScale(g, *dial, bigScaleInset, 11, bigScaleDotSize, bigScaleLabelGap, bigScaleLabels, bigScaleFont);

        for (const auto* dial : {&attack1176Dial, &release1176Dial})
            AAUITools::drawKnobScale(g, *dial, 0.f, 4, smallScaleDotSize, smallScaleLabelGap, smallScaleLabels, smallScaleFont);
    }

    void resized() override
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto grMeterWidth = bounds.getWidth() * 0.6f;
        const auto grMeterHeight = bounds.getHeight() * 0.135f;
        const auto grMeterBounds = bounds.withSize(grMeterWidth, grMeterHeight).withCentre({bounds.getCentreX(), grMeterHeight * 0.5f});
        grMeter1176.setBounds(grMeterBounds.toNearestInt());

        const auto grLabelWidth = grMeterHeight;
        const auto grLabelGap = grMeterHeight * 0.2f;
        for (auto* label : {&grLabel, &dBLabel})
            label->setFont(VCFonts::barlowCondensedRegular(grMeterHeight * 0.5f));
        grLabel.setBounds(juce::Rectangle<float>(grMeterBounds.getRight() + grLabelGap, grMeterBounds.getY(), grLabelWidth, grMeterHeight).toNearestInt());
        dBLabel.setBounds(juce::Rectangle<float>(grMeterBounds.getX() - grLabelGap - grLabelWidth, grMeterBounds.getY(), grLabelWidth, grMeterHeight).toNearestInt());
        const auto boundsF = bounds.withTrimmedTop(bounds.getHeight() * 0.1f);
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        const auto knobSize = boundsFHeight * 0.675f;
        const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
        const auto knobsGap = knobSize;
        input1176Dial.setBounds(knobBounds.translated(- knobsGap, 0.f).toNearestInt());
        output1176Dial.setBounds(knobBounds.translated(knobsGap, 0.f).toNearestInt());

        bigScaleFont = VCFonts::barlowCondensedMedium(bounds.getHeight() * 0.07f);
        bigScaleInset = knobSize * 0.025f;
        bigScaleLabelGap = knobSize * 0.0333f;
        bigScaleDotSize = knobSize * 0.03f;

        const auto labelBounds = knobBounds.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.2f);
        const auto labelsCY = knobBounds.getBottom() + labelBounds.getHeight() * 0.5f;
        for (auto* label : {&input1176Label, &output1176Label})
            label->setFont(VCFonts::barlowCondensedMedium(labelBounds.getHeight() * 0.45f));
        input1176Label.setBounds(labelBounds.withCentre({input1176Dial.getBounds().toFloat().getCentreX(), labelsCY}).toNearestInt());
        output1176Label.setBounds(labelBounds.withCentre({output1176Dial.getBounds().toFloat().getCentreX(), labelsCY}).toNearestInt());

        const auto smallKnobsSize = boundsFHeight * 0.24f;
        const auto controlsXGap = boundsFWidth * 0.7f - smallKnobsSize * 0.5f - knobBounds.getRight();
        const auto smallKnobsCX = output1176Dial.getBounds().toFloat().getRight() + controlsXGap + smallKnobsSize * 0.5f;
        const auto smallKnobBounds = knobBounds.withSize(smallKnobsSize, smallKnobsSize).withCentre({smallKnobsCX, knobBounds.getCentreY()});
        const auto smallKnobVerticalGap = boundsFHeight * 0.22f;
        const auto smallKnobsLabelGap = smallKnobsSize * 1.075f;
        for (auto* label : {&attack1176Label, &release1176Label, &ratioLabel})
            label->setFont(VCFonts::barlowCondensedRegular(labelBounds.getHeight() * 0.3f));
        smallScaleFont = VCFonts::barlowCondensedMedium(smallKnobsSize * 0.2625f);
        smallScaleLabelGap = smallKnobsSize * 0.05f;
        smallScaleDotSize = smallKnobsSize * 0.065f;

        attack1176Dial.setBounds(smallKnobBounds.translated(0.f, -smallKnobVerticalGap).toNearestInt());
        release1176Dial.setBounds(smallKnobBounds.translated(0.f, smallKnobVerticalGap).toNearestInt());
        juce::Rectangle<float> attackScaleBounds, releaseScaleBounds;
        for (const auto& numberBounds : AAUITools::getKnobScaleLabelBounds(attack1176Dial, 0.f, smallScaleDotSize, smallScaleLabelGap, smallScaleLabels, smallScaleFont))
            attackScaleBounds = attackScaleBounds.getUnion(numberBounds);
        for (const auto& numberBounds : AAUITools::getKnobScaleLabelBounds(release1176Dial, 0.f, smallScaleDotSize, smallScaleLabelGap, smallScaleLabels, smallScaleFont))
            releaseScaleBounds = releaseScaleBounds.getUnion(numberBounds);

        const auto labelBorderTop = static_cast<float>(release1176Label.getBorderSize().getTop());
        const auto smallLabelFontHeight = release1176Label.getFont().getHeight();
        const auto releaseTextTop = knobBounds.getCentreY() + smallKnobVerticalGap + smallKnobsLabelGap - labelBounds.getHeight() * 0.5f + labelBorderTop;
        const auto scaleToTextGap = releaseTextTop - releaseScaleBounds.getBottom();
        const auto attackTextTop = attackScaleBounds.getY() - scaleToTextGap - smallLabelFontHeight;
        const auto releaseTextBottom = releaseTextTop + smallLabelFontHeight;
        const auto smallKnobsYOffset = knobBounds.getCentreY() - (attackTextTop + releaseTextBottom) * 0.5f;
        const auto centredSmallKnobBounds = smallKnobBounds.translated(0.f, smallKnobsYOffset);
        attack1176Dial.setBounds    (centredSmallKnobBounds.translated(0.f, -smallKnobVerticalGap).toNearestInt());
        release1176Dial.setBounds   (centredSmallKnobBounds.translated(0.f, smallKnobVerticalGap).toNearestInt());

        const auto smallLabelX = smallKnobsCX - labelBounds.getWidth() * 0.5f;
        const auto attackLabelTop = attackTextTop + smallKnobsYOffset - labelBorderTop;
        attack1176Label.setBounds(juce::Rectangle<float>(smallLabelX, attackLabelTop, labelBounds.getWidth(), labelBounds.getHeight()).toNearestInt());
        release1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, release1176Dial.getBounds().toFloat().getCentreY()  + smallKnobsLabelGap}).toNearestInt());


        const auto ratioButtonsWidth = boundsFWidth * 0.07f;
        const auto ratioButtonsBounds = knobBounds.withSizeKeepingCentre(ratioButtonsWidth, ratioButtonsWidth * ratioButtons1176.heightToWidthRatio)
                                        .withRightX(input1176Dial.getBounds().toFloat().getX() - controlsXGap);
        ratioButtons1176.setBounds(ratioButtonsBounds.toNearestInt());
        const auto ratioLabelX = ratioButtonsBounds.getCentreX() - labelBounds.getWidth() * 0.5f;
        const auto ratioLabelTop = ratioButtonsBounds.getBottom() + scaleToTextGap - labelBorderTop;
        ratioLabel.setBounds(juce::Rectangle<float>(ratioLabelX, ratioLabelTop, labelBounds.getWidth(), labelBounds.getHeight()).toNearestInt());

        const auto mix1176KnobSize = knobSize * 0.45f;
        const auto mixKnobCX = (input1176Dial.getBounds().toFloat().getCentreX() + output1176Dial.getBounds().toFloat().getCentreX()) * 0.5f;
        const auto mixKnobBounds = knobBounds.withSize(mix1176KnobSize, mix1176KnobSize).withCentre({mixKnobCX, boundsF.getCentreY()});
        mix1176Knob.setBounds(mixKnobBounds.toNearestInt());
        mix1176Label.setFont(VCFonts::barlowCondensedLight(labelBounds.getHeight() * 0.45f));
        mix1176Label.setBounds(mixKnobBounds.withY(mixKnobBounds.getBottom()).toNearestInt());
    }

private:
    VocalCompressorAudioProcessor& audioProcessor;

//   IMAGE LOADING
    const juce::Image darkBackground {juce::ImageCache::getFromMemory(BinaryData::darkSilverBackground2_png, BinaryData::darkSilverBackground2_pngSize)};
    const juce::Image knobImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Base_png, BinaryData::knob1176Base_pngSize)};
    const juce::Image knobRotorImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Rotor_png, BinaryData::knob1176Rotor_pngSize)};
    const juce::Image smallKnobImage1176 {juce::ImageCache::getFromMemory(BinaryData::small1176_png, BinaryData::small1176_pngSize)};
    const juce::Image mixKnobImage1176 {juce::ImageCache::getFromMemory(BinaryData::mixKnobLa2aBase_png, BinaryData::mixKnobLa2aBase_pngSize)};

//   KNOBS
    RotaryImageKnob input1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176InputName()), knobImage1176, knobRotorImage1176, 300};
    RotaryImageKnob output1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176OutputName()), knobImage1176, knobRotorImage1176, 300};
    RotaryImageKnob attack1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176AttackName()), smallKnobImage1176, {}, 200};
    RotaryImageKnob release1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176ReleaseName()), smallKnobImage1176, {}, 200};
    ArcImageKnob mix1176Knob {*audioProcessor.apvts.getParameter(ParamNames::get1176MixName()), mixKnobImage1176};

//    KNOB SCALES
    const juce::StringArray bigScaleLabels {juce::String(juce::CharPointer_UTF8("\xe2\x88\x9e")), "48", "36", "24", "12", "0"};
    juce::Font bigScaleFont {juce::FontOptions{}};
    float bigScaleInset {0.f}, bigScaleLabelGap {0.f}, bigScaleDotSize {0.f};
    const juce::StringArray smallScaleLabels {"1", "3", "5", "7"};
    juce::Font smallScaleFont {juce::FontOptions{}};
    float smallScaleLabelGap {0.f}, smallScaleDotSize {0.f};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"},
                attack1176Label {"ATTACK"}, release1176Label {"RELEASE"},
                mix1176Label {"MIX"}, ratioLabel {"RATIO"};

//    RATIO BUTTONS
    Ratio1176Buttons ratioButtons1176;
    juce::ParameterAttachment ratioAttachment {*audioProcessor.apvts.getParameter(ParamNames::get1176RatioName()),
                                               [this](float newValue) { ratioButtons1176.setSelectedIndex(juce::roundToInt(newValue)); }, nullptr};

//    GAIN REDUCTION
    GRDistressorMeter grMeter1176 {audioProcessor.gR1176, VCColors::almostWhiteText};
    juce::Label grLabel {"GR"}, dBLabel {"dB"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Panel1176)
};
