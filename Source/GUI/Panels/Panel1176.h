#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../LAF/VCControlsLAF.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/GRDistressorMeter.h"
#include "../Components/GRTextDisplay.h"

class Panel1176 : public juce::Component
{
public:
    explicit Panel1176(VocalCompressorAudioProcessor& processor)     : audioProcessor(processor)
    {

        addAndMakeVisible(grMeter1176);
        for (auto* label : {&input1176Label, &output1176Label, &ratioLabel, &attack1176Label, &release1176Label, &mix1176Label})
        {
            VCLook::setupLabel(*label, juce::Colours::white, juce::Justification::centredTop);
            addAndMakeVisible(*label);
        }

        for (auto* imageKnob : {&input1176Dial, &output1176Dial, &attack1176Dial, &release1176Dial, &mix1176Knob})
        {
            addAndMakeVisible(*imageKnob);
        }


        for (auto* ratioBtn : ratioButtons)
        {
            setupRatioButtons(ratioBtn);
            ratioBtn->setLookAndFeel(&controlsLAF);
            addAndMakeVisible(*ratioBtn);
        }
        ratio4Btn.onClick = [this] { setRatioIndex(0); };
        ratio8Btn.onClick = [this] { setRatioIndex(1); };
        ratio16Btn.onClick = [this] { setRatioIndex(2); };
        ratio20Btn.onClick = [this] { setRatioIndex(3); };
        ratioAllBtn.onClick = [this] { setRatioIndex(4); };
        const int ratioIndex = static_cast<int>(audioProcessor.apvts.getRawParameterValue(ParamNames::get1176RatioName())->load());
        updateRatioButtons(ratioIndex);

    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.drawImage(darkBackground, boundsF, juce::RectanglePlacement::stretchToFit);

        const auto bigKnobSize = static_cast<float>(input1176Dial.getWidth());
        const auto scaleFont = VCFonts::barlowCondensedMedium(boundsF.getHeight() * 0.07f);
        const auto bigScaleInset = bigKnobSize * 0.025f;
        const auto bigLabelGap = bigKnobSize * 0.0333f;
        const auto dotSize = bigKnobSize * 0.03f;
        for (const auto* dial : {&input1176Dial, &output1176Dial})
            AAUITools::drawKnobScale(g, *dial, bigScaleInset, 11, dotSize, bigLabelGap,
                            {static_cast<juce::String>(juce::CharPointer_UTF8("\xe2\x88\x9e")), "48", "36", "24", "12", "0"},
                                    scaleFont);

        const auto smallKnobSize = static_cast<float>(attack1176Dial.getWidth());
        const auto smallScaleFont = VCFonts::barlowCondensedMedium(smallKnobSize * 0.2625f);
        const auto smallLabelGap = smallKnobSize * 0.05f;
        const auto smallScaleDotSize = smallKnobSize * 0.065f;
        for (const auto* dial : {&attack1176Dial, &release1176Dial})
            AAUITools::drawKnobScale(g, *dial, 0.f, 4, smallScaleDotSize, smallLabelGap, {"1", "3", "5", "7"}, smallScaleFont);
    }

    void resized() override
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto grMeterWidth = bounds.getWidth() * 0.6f;
        const auto grMeterHeight = bounds.getHeight() * 0.14f;
        const auto grMeterCentre = juce::Point<float> (bounds.getCentreX(), grMeterHeight * 0.5f);
        grMeter1176.setBounds(bounds.withSize(grMeterWidth, grMeterHeight).withCentre(grMeterCentre).toNearestInt());

        const auto boundsF = bounds.withTrimmedTop(grMeterHeight);
        const auto boundsFHeight = boundsF.getHeight();
        const auto boundsFWidth = boundsF.getWidth();

        const auto knobSize = boundsFHeight * 0.65f;
        const auto knobBounds = boundsF.withSizeKeepingCentre(knobSize, knobSize);
        const auto bigKnobsXGap = -knobSize * 1.75f;
        input1176Dial.setBounds(knobBounds.translated(bigKnobsXGap, 0.f).toNearestInt());
        output1176Dial.setBounds(knobBounds.toNearestInt());

        const auto labelBounds = knobBounds.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.2f);
        const auto labelsCY = knobBounds.getBottom() + labelBounds.getHeight() * 0.5f;
        for (auto* label : {&input1176Label, &output1176Label})
            label->setFont(VCFonts::barlowCondensedMedium(labelBounds.getHeight() * 0.5f));
        for (auto* label : {&attack1176Label, &release1176Label, &ratioLabel})
            label->setFont(VCFonts::barlowCondensedRegular(labelBounds.getHeight() * 0.3f));
        input1176Label.setBounds(labelBounds.withCentre({input1176Dial.getBounds().toFloat().getCentreX(), labelsCY}).toNearestInt());
        output1176Label.setBounds(labelBounds.withCentre({output1176Dial.getBounds().toFloat().getCentreX(), labelsCY}).toNearestInt());

        const auto mix1176KnobSize = knobSize * 0.25f;
        const auto mixKnobCX = (input1176Dial.getBounds().toFloat().getCentreX() + output1176Dial.getBounds().toFloat().getCentreX()) * 0.5f;
        const auto mixKnobBounds = knobBounds.withSizeKeepingCentre(mix1176KnobSize, mix1176KnobSize).withX(mixKnobCX - mix1176KnobSize * 0.5f);
        mix1176Knob.setBounds(mixKnobBounds.toNearestInt());
        mix1176Label.setFont(VCFonts::barlowCondensedLight(labelBounds.getHeight() * 0.3f));
        mix1176Label.setBounds(mixKnobBounds.withY(mixKnobBounds.getBottom()).toNearestInt());

        const auto smallKnobsSize = boundsFHeight * 0.225f;
        const auto smallKnobsCX = boundsFWidth * 0.7f;
        const auto smallKnobBounds = knobBounds.withSize(smallKnobsSize, smallKnobsSize);
        const auto outputKnobBounds = output1176Dial.getBounds().toFloat();
        const auto attackCY = outputKnobBounds.getY() + outputKnobBounds.getHeight() * 0.25f;
        const auto releaseCY = outputKnobBounds.getY() + outputKnobBounds.getHeight() * 0.75f;
        attack1176Dial.setBounds(smallKnobBounds.withCentre({smallKnobsCX, attackCY}).toNearestInt());
        release1176Dial.setBounds(smallKnobBounds.withCentre({smallKnobsCX, releaseCY}).toNearestInt());

        const auto smallKnobsLabelGap = smallKnobsSize * 1.075f;
        attack1176Label.setBounds(labelBounds.withCentre({smallKnobsCX,  attackCY + smallKnobsLabelGap}).toNearestInt());
        release1176Label.setBounds(labelBounds.withCentre({smallKnobsCX, releaseCY + smallKnobsLabelGap}).toNearestInt());

        const auto ratioColumnCX = boundsFWidth * 0.834f;
        auto ratioBtnBounds = boundsF.withSize(boundsFWidth * 0.05f, boundsFHeight * 0.1456f).withCentre({ratioColumnCX, 0.f}).withY(boundsFHeight * 0.0934f);
        for (auto* ratioBtn : {&ratioAllBtn, &ratio20Btn, &ratio16Btn, &ratio8Btn, &ratio4Btn})
        {
            ratioBtn->setBounds(ratioBtnBounds.toNearestInt());
            ratioBtnBounds.translate(0.f, ratioBtnBounds.getHeight());
        }
        ratioLabel.setBounds(labelBounds.withCentre({ratioColumnCX, labelsCY}).toNearestInt());
    }

    void paintOverChildren(juce::Graphics& g) override
    {
        if (shouldDim)
            g.fillAll(VCColors::almostBlack.withAlpha(JUCE_LIVE_CONSTANT(0.45f)));
    }

    bool shouldDim {false};

private:
    void setRatioIndex(int index)
    {
        if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter(ParamNames::get1176RatioName())))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(static_cast<float>(index) / static_cast<float>(param->choices.size() - 1));
            param->endChangeGesture();
            updateRatioButtons(index);
        }
    }

    void updateRatioButtons(int selectedIndex)
    {
        ratio4Btn.setToggleState(selectedIndex == 0, juce::dontSendNotification);
        ratio8Btn.setToggleState(selectedIndex == 1, juce::dontSendNotification);
        ratio16Btn.setToggleState(selectedIndex == 2, juce::dontSendNotification);
        ratio20Btn.setToggleState(selectedIndex == 3, juce::dontSendNotification);
    }

    void setupRatioButtons(juce::TextButton* ratioBtn)
    {
        ratioBtn->setRadioGroupId(1515);
        ratioBtn->setClickingTogglesState(true);
        ratioBtn->setColour(juce::TextButton::ColourIds::buttonColourId, VCColors::ratioButtonOff);
        ratioBtn->setColour(juce::TextButton::ColourIds::buttonOnColourId, VCColors::darkRed);
        ratioBtn->setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    }

    VocalCompressorAudioProcessor& audioProcessor;

//   IMAGE LOADING
    const juce::Image darkBackground {juce::ImageCache::getFromMemory(BinaryData::darkSilverBackground2_png, BinaryData::darkSilverBackground2_pngSize)};
    const juce::Image knobImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Base_png, BinaryData::knob1176Base_pngSize)};
    const juce::Image knobRotorImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Rotor_png, BinaryData::knob1176Rotor_pngSize)};
    const juce::Image smallKnobImage1176 {juce::ImageCache::getFromMemory(BinaryData::small1176_png, BinaryData::small1176_pngSize)};

//   KNOBS
    RotaryImageKnob input1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176InputName()), knobImage1176, knobRotorImage1176, 350};
    RotaryImageKnob output1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176OutputName()), knobImage1176, knobRotorImage1176, 350};
    RotaryImageKnob attack1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176AttackName()), smallKnobImage1176};
    RotaryImageKnob release1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176ReleaseName()), smallKnobImage1176};
    RotaryImageKnob mix1176Knob {*audioProcessor.apvts.getParameter(ParamNames::get1176MixName()), smallKnobImage1176};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"},
                attack1176Label {"ATTACK"}, release1176Label {"RELEASE"},
                ratioLabel {"RATIO"},             mix1176Label {"MIX"};

//    RATIO BUTTONS
    VCControlsLAF controlsLAF;
    juce::TextButton ratio4Btn {"4"}, ratio8Btn {"8"}, ratio16Btn {"16"}, ratio20Btn {"20"}, ratioAllBtn {"ALL"};
    std::vector<juce::TextButton*> ratioButtons {&ratio4Btn, &ratio8Btn, &ratio16Btn, &ratio20Btn, &ratioAllBtn};

//    GAIN REDUCTION
    GRDistressorMeter grMeter1176 {audioProcessor.gR1176, VCColors::almostWhiteText};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Panel1176)
};
