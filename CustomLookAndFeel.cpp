/*
  ==============================================================================

   This file is part of the GrandStaffMIDIVisualizer plugin code.
   Copyright (c) Brynjar Reynisson

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

#include "MainComponent.h"

void CustomLookAndFeel::setLightModeLookAndFeel()
{
    setColour(TextButton::textColourOnId, Colours::black);
    setColour(TextButton::textColourOffId, Colours::darkgrey);
    setColour(TextButton::buttonOnColourId, Colours::lightgrey);
    setColour(TextButton::buttonColourId, Colours::white);
    setColour(Label::textColourId, Colours::black);
    setColour(Label::backgroundColourId, Colour(230, 230, 230));
    setColour(ComboBox::outlineColourId, Colours::black);
    setColour(ComboBox::focusedOutlineColourId, Colours::black);
    setColour(ComboBox::textColourId, Colours::black);
    setColour(ComboBox::arrowColourId, Colours::darkgrey);
    setColour(ComboBox::backgroundColourId, Colours::white);
    setColour(ComboBox::buttonColourId, Colours::white);
    setColour(PopupMenu::backgroundColourId, Colours::white);
    setColour(PopupMenu::headerTextColourId, Colours::black);
    setColour(PopupMenu::highlightedBackgroundColourId, Colours::lightgrey);
    setColour(PopupMenu::highlightedTextColourId, Colours::black);
    setColour(PopupMenu::textColourId, Colours::black);

    Colour grey = Colours::grey;
    setColour(Slider::thumbColourId, grey);
    setColour(Slider::trackColourId, grey);
    setColour(Slider::rotarySliderFillColourId, grey);
    setColour(Slider::rotarySliderOutlineColourId, grey);
    setColour(Slider::backgroundColourId, Colours::white);
}

void CustomLookAndFeel::setDarkModeLookAndFeel()
{
    setColour(TextButton::textColourOnId, darkModeForegroundColour);
    setColour(TextButton::textColourOffId, darkModeForegroundColour);
    setColour(TextButton::buttonColourId, darkModeBackgroundColour);
    setColour(TextButton::buttonOnColourId, darkModeSelectedBackgroundColour);
    setColour(Label::textColourId, darkModeForegroundColour);
    setColour(Label::backgroundColourId, darkModeSelectedBackgroundColour);
    setColour(ComboBox::outlineColourId, darkModeForegroundColour);
    setColour(ComboBox::focusedOutlineColourId, Colours::white);
    setColour(ComboBox::textColourId, darkModeForegroundColour);
    setColour(ComboBox::arrowColourId, darkModeForegroundColour);
    setColour(ComboBox::backgroundColourId, darkModeBackgroundColour);
    setColour(ComboBox::buttonColourId, darkModeBackgroundColour);
    setColour(PopupMenu::backgroundColourId, darkModeBackgroundColour);
    setColour(PopupMenu::headerTextColourId, darkModeForegroundColour);
    setColour(PopupMenu::highlightedBackgroundColourId, darkModeSelectedBackgroundColour);
    setColour(PopupMenu::highlightedTextColourId, Colours::white);
    setColour(PopupMenu::textColourId, darkModeForegroundColour);

    Colour grey = Colours::grey;
    setColour(Slider::thumbColourId, grey);
    setColour(Slider::trackColourId, grey);
    setColour(Slider::rotarySliderFillColourId, grey);
    setColour(Slider::rotarySliderOutlineColourId, grey);
    setColour(Slider::backgroundColourId, darkModeBackgroundColour);
}


Font CustomLookAndFeel::getTextButtonFont(TextButton& textButton, int buttonHeight)
{
    return plainCustomFont.withHeight(buttonHeight * fontHeightMultiplier);
}

Font CustomLookAndFeel::getLabelFont(Label& l)
{
    return plainCustomFont.withHeight(l.getBounds().getHeight() * fontHeightMultiplier);
}

Font CustomLookAndFeel::getComboBoxFont(ComboBox& c)
{
    return plainCustomFont.withHeight(c.getBounds().getHeight() * fontHeightMultiplier);
}

Font CustomLookAndFeel::getPopupMenuFont()
{
    return plainCustomFont;
}

Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font& f)
{
    return customFontTypeface;
}

void CustomLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int>(x, y, width, height).toFloat();

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = radius * 0.15f;
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(outline);
    g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(fill);
        g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi) * 0.5,
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi) * 0.5);

    Point<float> thumbPointOuter(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi) * 0.75,
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi) * 0.75);

    Point<float> thumbPointInner(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi) * 0.25,
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi) * 0.25);

    g.setColour(slider.findColour(Slider::thumbColourId));
    g.drawLine(thumbPointInner.x, thumbPointInner.y, thumbPointOuter.x, thumbPointOuter.y, lineW);
}