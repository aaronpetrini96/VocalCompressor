#pragma once
#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "../VCLook.h"
#include "../Components/ImageKnob.h"
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

        for (auto* label : {&input1176Label, &output1176Label, &ratioLabel, &gr1176Label})
        {
            VCLook::setupLabel(*label);
            addAndMakeVisible(*label);
        }
        for (auto* label : {&attack1176Label, &release1176Label, &mix1176Label})
        {
            VCLook::setupLabel(*label, 14.f);
            addAndMakeVisible(*label);
        }

        for (auto* ratioBtn : ratioButtons)
        {
            setupRatioButtons(ratioBtn);
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

        const auto labelFontHeight = labelBounds.getHeight() * 0.3556f;
        const auto smallLabelFontHeight = labelBounds.getHeight() * 0.3111f;
        for (auto* label : {&input1176Label, &output1176Label, &ratioLabel, &gr1176Label})
            label->setFont(label->getFont().withHeight(labelFontHeight));
        for (auto* label : {&attack1176Label, &release1176Label, &mix1176Label})
            label->setFont(label->getFont().withHeight(smallLabelFontHeight));

        const auto knobsCY = boundsFHeight * 0.5112f;
        const auto labelsCY = knobsCY + knobSizeRef * 0.51f;
        const auto inputCX = knobGap + knobSizeRef * 0.5f;
        const auto outputCX = inputCX + knobSizeRef + knobGap;

        input1176Dial.setBounds(knobBounds.withCentre({inputCX, knobsCY}).toNearestInt());
        input1176Label.setBounds(labelBounds.withCentre({inputCX, labelsCY}).toNearestInt());
        output1176Dial.setBounds(knobBounds.withCentre({outputCX, knobsCY}).toNearestInt());
        output1176Label.setBounds(labelBounds.withCentre({outputCX, labelsCY}).toNearestInt());

        const auto mixScrewCX = inputCX + (knobSizeRef + knobGap) * 0.5f;
        const auto mixScrewCY = knobsCY + knobSizeRef * 0.5f - smallControlSize;
        const auto mixScrewBounds = bypassBounds.withCentre({mixScrewCX, mixScrewCY});
        mix1176Knob.setBounds(mixScrewBounds.toNearestInt());
        mix1176Label.setBounds(labelBounds.withCentre({mixScrewCX, mixScrewBounds.getBottom() + knobSizeRef * 0.053f}).toNearestInt());

        const auto attackCX = outputCX + (knobSizeRef + smallKnobSize) * 0.5f + knobGap;
        const auto attackCY = boundsFHeight * 0.2466f;
        const auto releaseCY = attackCY + smallKnobSize * 1.3067f;
        attack1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, attackCY}).toNearestInt());
        attack1176Label.setBounds(labelBounds.withCentre({attackCX, attackCY + smallKnobSize * 0.5933f}).toNearestInt());
        release1176Dial.setBounds(smallKnobBounds.withCentre({attackCX, releaseCY}).toNearestInt());
        release1176Label.setBounds(labelBounds.withCentre({attackCX, releaseCY + smallKnobSize * 0.62f}).toNearestInt());

        const auto ratioColumnCX = boundsFWidth * 0.834f;
        const auto ratioColumnTopY = boundsFHeight * 0.0934f;
        const auto ratioBtnBounds = boundsF.withSize(knobSizeRef * 0.25f, knobSizeRef * 0.2f).withCentre({ratioColumnCX, 0.f});
        auto ratioBtnY = ratioColumnTopY;
        for (auto* ratioBtn : {&ratioAllBtn, &ratio20Btn, &ratio16Btn, &ratio8Btn, &ratio4Btn})
        {
            ratioBtn->setBounds(ratioBtnBounds.withY(ratioBtnY).toNearestInt());
            ratioBtnY += ratioBtnBounds.getHeight();
        }
        ratioLabel.setBounds(labelBounds.withCentre({ratioColumnCX, labelsCY}).toNearestInt());

        const auto grMetersCX = boundsFWidth * 0.9267f;
        const auto grMetersTopY = ratioColumnTopY + ratioBtnBounds.getHeight();
        grMeter1176Left.setBounds(grMeterBounds.withPosition(grMetersCX - smallControlSize, grMetersTopY).toNearestInt());
        grMeter1176Right.setBounds(grMeterBounds.withPosition(grMetersCX, grMetersTopY).toNearestInt());
        gr1176Label.setBounds(labelBounds.withCentre({grMetersCX, labelsCY}).toNearestInt());
        grTextDisplay1176.setBounds(grTextBounds.withCentre({grMetersCX, boundsFHeight * 0.1778f}).toNearestInt());

        bypassButton1176.setBounds(bypassBounds.withPosition(smallControlSize * 0.5f, smallControlSize * 0.5f).toNearestInt());
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
        ratioBtn->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
        ratioBtn->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::red.withAlpha(0.7f).darker(0.15f));
        ratioBtn->setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
        ratioBtn->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
    }

    VocalCompressorAudioProcessor& audioProcessor;
    const int knobSize = 150;
    const int versionHint = 1;

//   IMAGE LOADING
    const juce::Image darkBackground {juce::ImageCache::getFromMemory(BinaryData::darkSilverBackground2_png, BinaryData::darkSilverBackground2_pngSize)};
    const juce::Image knobImage1176 {juce::ImageCache::getFromMemory(BinaryData::spriteSheet1176_png, BinaryData::spriteSheet1176_pngSize)};
    const juce::Image slotScrewImage {juce::ImageCache::getFromMemory(BinaryData::slotScrew256_png, BinaryData::slotScrew256_pngSize)};

//   KNOBS
    ImageKnob input1176Dial {"1176 Input Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176InputName(), versionHint), knobImage1176, "", knobSize, false, false};
    ImageKnob output1176Dial {"1176 Output Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176OutputName(), versionHint), knobImage1176, "", knobSize, false, false};
    ImageKnob attack1176Dial {"1176 Attack Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176AttackName(), versionHint), knobImage1176, "", knobSize / 2, false, false};
    ImageKnob release1176Dial {"1176 Release Dial", audioProcessor.apvts, juce::ParameterID(ParamNames::get1176ReleaseName(), versionHint), knobImage1176, "", knobSize / 2, false, false};
    RotaryImageKnob mix1176Knob {*audioProcessor.apvts.getParameter(ParamNames::get1176MixName()), slotScrewImage};

//    LABELS
    juce::Label input1176Label {"INPUT"}, output1176Label {"OUTPUT"},
                attack1176Label {"ATTACK"}, release1176Label {"RELEASE"},
                ratioLabel {"RATIO"}, gr1176Label {"GR"},
                mix1176Label {"MIX"};

//    RATIO BUTTONS
    juce::TextButton ratio4Btn {"4"}, ratio8Btn {"8"}, ratio16Btn {"16"}, ratio20Btn {"20"}, ratioAllBtn {"ALL"};
    std::vector<juce::TextButton*> ratioButtons {&ratio4Btn, &ratio8Btn, &ratio16Btn, &ratio20Btn, &ratioAllBtn};

//    GAIN REDUCTION + BYPASS
    GainReductionMeter grMeter1176Left, grMeter1176Right;
    GRTextDisplay grTextDisplay1176;
    BypassButton bypassButton1176 {"", audioProcessor.apvts, ParamNames::get1176BypassName(), 20};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Panel1176)
};
