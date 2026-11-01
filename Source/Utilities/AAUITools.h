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
                                     const juce::Colour iconColour, const juce::Colour normalColour, const juce::Colour overColour, const juce::Colour activeColour)
    {
        if (const auto icon = juce::Drawable::createFromImageData (iconData, (size_t) iconDataSize))
        {
            const auto iconNormal = icon->createCopy();
            const auto iconOver = icon->createCopy();
            const auto iconActive = icon->createCopy();
            iconNormal->replaceColour (iconColour, normalColour);
            iconOver->replaceColour (iconColour, overColour);
            iconActive->replaceColour (iconColour, activeColour);
            button.setImages (iconNormal.get(), iconOver.get(), iconActive.get(), nullptr, iconActive.get());
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
