#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

namespace AAUITools
{
    inline juce::Font makeFont (const juce::Typeface::Ptr& typeface, const float fontHeight, const float kerning = 0.0f)
    {
        return juce::Font {juce::FontOptions (typeface).withHeight (juce::jmax (1.0f, fontHeight))}.withExtraKerningFactor (kerning);
    }

    inline void setIconButtonImages (juce::DrawableButton& button, const void* iconData, const int iconDataSize,
                                     const juce::Colour iconColour, const juce::Colour overColour, const juce::Colour activeColour, const juce::Colour activeOverColour)
    {
        if (const auto iconNormal = juce::Drawable::createFromImageData (iconData,  static_cast<size_t>(iconDataSize)))
        {
            const auto iconOver = iconNormal->createCopy();
            const auto iconActive = iconNormal->createCopy();
            const auto iconActiveOver = iconNormal->createCopy();
            iconOver->replaceColour (iconColour, overColour);
            iconActive->replaceColour (iconColour, activeColour);
            iconActiveOver->replaceColour (iconColour, activeOverColour);
            button.setImages (iconNormal.get(), iconOver.get(), iconActive.get(), nullptr, iconActive.get(), iconActiveOver.get());
            button.setEdgeIndent (0);
            button.setColour(juce::DrawableButton::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
            button.setColour(juce::DrawableButton::ColourIds::backgroundOnColourId, juce::Colours::transparentBlack);
        }
    }

    inline void drawOutlinedRoundedRectangle (juce::Graphics& g, const juce::Rectangle<float> bounds, const float cornerSize, const float outlineThickness, const juce::FillType& fill, const juce::Colour outlineColour)
    {
        g.setFillType (fill);
        g.fillRoundedRectangle (bounds, cornerSize);
        g.setColour (outlineColour);
        g.drawRoundedRectangle (bounds.reduced (outlineThickness * 0.5f), cornerSize, outlineThickness);
    }

    inline juce::Array<juce::Rectangle<float>> getKnobScaleLabelBounds (const juce::Slider& knob, const float knobInset, const float dotSize, const float labelGap,
                                                                        const juce::StringArray& labels, const juce::Font& font)
    {
        const auto knobBounds = knob.getBounds().toFloat().reduced (knobInset);
        const auto startAngle = knob.getRotaryParameters().startAngleRadians;
        const auto endAngle = knob.getRotaryParameters().endAngleRadians;
        const auto centre = knobBounds.getCentre();
        const auto fontHeight = font.getHeight();
        const auto dotRadius = juce::jmin (knobBounds.getWidth(), knobBounds.getHeight()) * 0.5f + dotSize * 1.5f;
        const auto labelRadius = dotRadius + dotSize * 0.5f + labelGap + fontHeight * 0.5f;

        juce::Array<juce::Rectangle<float>> labelBounds;
        for (int labelIndex = 0; labelIndex < labels.size(); ++labelIndex)
        {
            const auto angle = juce::jmap (static_cast<float> (labelIndex), 0.0f, static_cast<float> (labels.size() - 1), startAngle, endAngle);
            const auto textWidth = juce::GlyphArrangement::getStringWidth (font, labels[labelIndex]);
            labelBounds.add (juce::Rectangle<float> (textWidth, fontHeight).withCentre (centre.getPointOnCircumference (labelRadius, angle)));
        }
        return labelBounds;
    }

    inline void drawKnobScale (juce::Graphics& g, const juce::Slider& knob, const float knobInset,
                               const int numberOfDots, const float dotSize, const float labelGap,
                               const juce::StringArray& labels, const juce::Font& font, const juce::Colour colour = juce::Colours::white)
    {
        const auto knobBounds = knob.getBounds().toFloat().reduced (knobInset);
        const auto startAngle = knob.getRotaryParameters().startAngleRadians;
        const auto endAngle = knob.getRotaryParameters().endAngleRadians;
        const auto centre = knobBounds.getCentre();
        const auto dotRadius = juce::jmin (knobBounds.getWidth(), knobBounds.getHeight()) * 0.5f + dotSize * 1.5f;

        g.setColour (colour);
        g.setFont (font);

        for (int dotIndex = 0; dotIndex < numberOfDots; ++dotIndex)
        {
            const auto angle = juce::jmap (static_cast<float> (dotIndex), 0.0f, static_cast<float> (numberOfDots - 1), startAngle, endAngle);
            g.fillEllipse (juce::Rectangle<float> (dotSize, dotSize).withCentre (centre.getPointOnCircumference (dotRadius, angle)));
        }

        const auto labelBounds = getKnobScaleLabelBounds (knob, knobInset, dotSize, labelGap, labels, font);
        for (int labelIndex = 0; labelIndex < labels.size(); ++labelIndex)
            g.drawText (labels[labelIndex], labelBounds[labelIndex], juce::Justification::centred, false);
    }

    inline void drawKnobArcScale (juce::Graphics& g, const juce::Rectangle<float> knobBounds, const float startAngle, const float endAngle,
                                  const int numberOfTicks, const float tickLength, const float lineThickness,
                                  const juce::StringArray& labels, const juce::Font& font, const juce::Colour colour)
    {
        const auto centre = knobBounds.getCentre();
        const auto fontHeight = font.getHeight();
        const auto arcRadius = juce::jmin (knobBounds.getWidth(), knobBounds.getHeight()) * 0.5f + lineThickness * 2.0f;
        const auto labelRadius = arcRadius + tickLength + fontHeight * 0.6f;
        const juce::PathStrokeType stroke (lineThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        g.setColour (colour);
        g.setFont (font);

        juce::Path arc;
        arc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, endAngle, true);

        for (int tickIndex = 0; tickIndex < numberOfTicks; ++tickIndex)
        {
            const auto angle = juce::jmap (static_cast<float> (tickIndex), 0.0f, static_cast<float> (numberOfTicks - 1), startAngle, endAngle);
            arc.startNewSubPath (centre.getPointOnCircumference (arcRadius, angle));
            arc.lineTo (centre.getPointOnCircumference (arcRadius + tickLength, angle));
        }

        g.strokePath (arc, stroke);

        for (int labelIndex = 0; labelIndex < labels.size(); ++labelIndex)
        {
            const auto angle = juce::jmap (static_cast<float> (labelIndex), 0.0f, static_cast<float> (labels.size() - 1), startAngle, endAngle);
            const auto labelBounds = juce::Rectangle<float> (fontHeight * 2.0f, fontHeight).withCentre (centre.getPointOnCircumference (labelRadius, angle));
            g.drawText (labels[labelIndex], labelBounds, juce::Justification::centred, false);
        }
    }

    class PrintedText : public juce::Component
    {
    public:
        PrintedText (const juce::String& textToUse, const juce::Colour inkColorToUse = juce::Colours::whitesmoke, const juce::Justification justificationToUse = juce::Justification::centred)
            : text (textToUse), inkColor (inkColorToUse), justification (justificationToUse)
        {
            jassert (grainTile.isValid());
            setInterceptsMouseClicks (false, false);
        }

        void paint (juce::Graphics& g) override
        {
            if (textPathIsStale)
                rebuildTextPath();

            g.setColour (inkColor);
            g.fillPath (textPath);

            const auto originInTopLevel = getTopLevelComponent()->getLocalPoint (this, juce::Point<float>());
            g.setFillType (juce::FillType (grainTile, juce::AffineTransform::translation (-originInTopLevel)));
            g.fillPath (textPath);
        }

        void resized() override
        {
            textPathIsStale = true;
        }

        void setText (const juce::String& newText)
        {
            if (text == newText)
                return;

            text = newText;
            textPathIsStale = true;
            repaint();
        }

        void setFont (const juce::Font& newFont)
        {
            if (font == newFont)
                return;

            font = newFont;
            textPathIsStale = true;
            repaint();
        }

    private:
        void rebuildTextPath()
        {
            juce::GlyphArrangement glyphs;
            glyphs.addFittedText (font, text, 0.f, 0.f, static_cast<float> (getWidth()), static_cast<float> (getHeight()), justification, 1);

            textPath.clear();
            glyphs.createPath (textPath);
            textPathIsStale = false;
        }

        juce::Image grainTile = juce::ImageCache::getFromMemory (BinaryData::inkGrainTile_png, BinaryData::inkGrainTile_pngSize);
        juce::String text;
        juce::Colour inkColor;
        juce::Justification justification;
        juce::Font font {juce::FontOptions {}};
        juce::Path textPath;
        bool textPathIsStale {true};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PrintedText)
    };
}
