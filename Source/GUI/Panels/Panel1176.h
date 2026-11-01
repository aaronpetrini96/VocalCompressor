#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../LAF/VCControlsLAF.h"
#include "../Components/RotaryImageKnob.h"
#include "../Components/GainReductionMeter.h"
#include "../Components/GRTextDisplay.h"
#include "../Components/BypassButton.h"

class Panel1176 : public juce::Component
{
public:
    explicit Panel1176(VocalCompressorAudioProcessor& processor)     : audioProcessor(processor)
    {
        for (auto* imageKnob : {&input1176Dial, &output1176Dial, &attack1176Dial, &release1176Dial})
        {
            addAndMakeVisible(*imageKnob);
            imageKnob->setAlwaysOnTop(true);
        }
        addAndMakeVisible(mix1176Knob);
        mix1176Knob.setAlwaysOnTop(true);

        for (auto* label : {&input1176Label, &output1176Label, &ratioLabel, &gr1176Label, &attack1176Label, &release1176Label, &mix1176Label})
        {
            VCLook::setupLabel(*label);
            addAndMakeVisible(*label);
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

        addAndMakeVisible(grMeter1176Left);
        addAndMakeVisible(grMeter1176Right);
        addAndMakeVisible(grTextDisplay1176);
        addAndMakeVisible(bypassButton1176);
    }

    void paint(juce::Graphics& g) override
    {
        const auto boundsF = getLocalBounds().toFloat();
        g.drawImage(darkBackground, boundsF, juce::RectanglePlacement::stretchToFit);
        VCLook::drawUnitTitle(g, "FET-76", boundsF);
    }

    void resized() override
    {
        const auto boundsF = getLocalBounds().toFloat();
        const auto boundsFWidth = boundsF.getWidth();
        const auto boundsFHeight = boundsF.getHeight();

        const auto knobBounds = boundsF.withSize(boundsFHeight * 0.728f, boundsFHeight * 0.728f);
        const auto smallKnobBounds = boundsF.withSize(boundsFHeight * 0.364f, boundsFHeight * 0.364f);
        const auto smallControlBounds = boundsF.withSize(boundsFHeight * 0.0971f, boundsFHeight * 0.0971f);
        const auto labelBounds = boundsF.withSize(boundsFWidth * 0.25f, boundsFHeight * 0.22f);

        for (auto* label : {&input1176Label, &output1176Label, &ratioLabel, &gr1176Label})
            label->setFont(VCFonts::barlowCondensedMedium(boundsFHeight * 0.1f));
        for (auto* label : {&attack1176Label, &release1176Label, &mix1176Label})
            label->setFont(VCFonts::barlowCondensedRegular(boundsFHeight * 0.075f));

        const auto labelsCY = boundsFHeight * 0.8825f;

        input1176Dial.setBounds(knobBounds.withCentre({boundsFWidth * 0.1733f, boundsFHeight * 0.5112f}).toNearestInt());
        input1176Label.setBounds(labelBounds.withCentre({boundsFWidth * 0.1733f, labelsCY}).toNearestInt());
        output1176Dial.setBounds(knobBounds.withCentre({boundsFWidth * 0.4716f, boundsFHeight * 0.5112f}).toNearestInt());
        output1176Label.setBounds(labelBounds.withCentre({boundsFWidth * 0.4716f, labelsCY}).toNearestInt());

        mix1176Knob.setBounds(smallControlBounds.withCentre({boundsFWidth * 0.3225f, boundsFHeight * 0.7782f}).toNearestInt());
        mix1176Label.setBounds(labelBounds.withCentre({boundsFWidth * 0.3225f, boundsFHeight * 0.8653f}).toNearestInt());

        const auto attackCX = boundsFWidth * 0.7074f;
        attack1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, boundsFHeight * 0.2466f}).toNearestInt());
        attack1176Label.setBounds(labelBounds.withCentre({attackCX, boundsFHeight * 0.4626f}).toNearestInt());
        release1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, boundsFHeight * 0.7223f}).toNearestInt());
        release1176Label.setBounds(labelBounds.withCentre({attackCX, boundsFHeight * 0.948f}).toNearestInt());

        const auto ratioColumnCX = boundsFWidth * 0.834f;
        auto ratioBtnBounds = boundsF.withSize(boundsFWidth * 0.0625f, boundsFHeight * 0.1456f).withCentre({ratioColumnCX, 0.f}).withY(boundsFHeight * 0.0934f);
        for (auto* ratioBtn : {&ratioAllBtn, &ratio20Btn, &ratio16Btn, &ratio8Btn, &ratio4Btn})
        {
            ratioBtn->setBounds(ratioBtnBounds.toNearestInt());
            ratioBtnBounds.translate(0.f, ratioBtnBounds.getHeight());
        }
        ratioLabel.setBounds(labelBounds.withCentre({ratioColumnCX, labelsCY}).toNearestInt());

        const auto grMetersCX = boundsFWidth * 0.9267f;
        const auto grMeterBounds = boundsF.withSize(boundsFWidth * 0.0333f, boundsFHeight * 0.5824f).withY(boundsFHeight * 0.239f);
        grMeter1176Left.setBounds(grMeterBounds.withRightX(grMetersCX).toNearestInt());
        grMeter1176Right.setBounds(grMeterBounds.withX(grMetersCX).toNearestInt());
        gr1176Label.setBounds(labelBounds.withCentre({grMetersCX, labelsCY}).toNearestInt());
        grTextDisplay1176.setBounds(boundsF.withSize(boundsFWidth * 0.0833f, boundsFHeight * 0.1941f).withCentre({grMetersCX, boundsFHeight * 0.1778f}).toNearestInt());

        bypassButton1176.setBounds(smallControlBounds.withPosition(boundsFWidth * 0.0167f, boundsFHeight * 0.0485f).toNearestInt());
    }

    void updateGainReduction()
    {
        const float gr1176 = audioProcessor.gR1176;
        grMeter1176Left.setReduction(gr1176);
        grMeter1176Right.setReduction(gr1176);
        grTextDisplay1176.setValue(gr1176);
    }

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
        ratioBtn->setColour(juce::TextButton::ColourIds::buttonOnColourId, VCColors::ratioButtonOn);
        ratioBtn->setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOnId, VCColors::text);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOffId, VCColors::text);
    }

    VocalCompressorAudioProcessor& audioProcessor;

//   IMAGE LOADING
    const juce::Image darkBackground {juce::ImageCache::getFromMemory(BinaryData::darkSilverBackground2_png, BinaryData::darkSilverBackground2_pngSize)};
    const juce::Image knobImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Base_png, BinaryData::knob1176Base_pngSize)};
    const juce::Image knobPointerImage1176 {juce::ImageCache::getFromMemory(BinaryData::knob1176Rotor_png, BinaryData::knob1176Rotor_pngSize)};
    const juce::Image slotScrewImage {juce::ImageCache::getFromMemory(BinaryData::small1176_png, BinaryData::small1176_pngSize)};

//   KNOBS
    RotaryImageKnob input1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176InputName()), knobImage1176, knobPointerImage1176};
    RotaryImageKnob output1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176OutputName()), knobImage1176, knobPointerImage1176};
    RotaryImageKnob attack1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176AttackName()), slotScrewImage};
    RotaryImageKnob release1176Dial {*audioProcessor.apvts.getParameter(ParamNames::get1176ReleaseName()), slotScrewImage};
    RotaryImageKnob mix1176Knob {*audioProcessor.apvts.getParameter(ParamNames::get1176MixName()), slotScrewImage};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"},
                attack1176Label {"ATTACK"}, release1176Label {"RELEASE"},
                ratioLabel {"RATIO"}, gr1176Label {"GR"},
                mix1176Label {"MIX"};

//    RATIO BUTTONS
    VCControlsLAF controlsLAF;
    juce::TextButton ratio4Btn {"4"}, ratio8Btn {"8"}, ratio16Btn {"16"}, ratio20Btn {"20"}, ratioAllBtn {"ALL"};
    std::vector<juce::TextButton*> ratioButtons {&ratio4Btn, &ratio8Btn, &ratio16Btn, &ratio20Btn, &ratioAllBtn};

//    GAIN REDUCTION + BYPASS
    GainReductionMeter grMeter1176Left, grMeter1176Right;
    GRTextDisplay grTextDisplay1176;
    BypassButton bypassButton1176 {"", audioProcessor.apvts, ParamNames::get1176BypassName(), 20};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Panel1176)
};
