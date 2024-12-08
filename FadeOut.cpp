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


//replicating original text colour so that fading doesn't start right away

static std::vector<Colour> lmChordColours = {
    Colour(0, 0, 0),
    Colour(0, 0, 0),
    Colour(0, 0, 0),
    Colour(0, 0, 0),
    Colour(32, 32, 32),
    Colour(63, 63, 63),
    Colour(95, 95, 95),
    Colour(127, 127, 127),
    Colour(158, 158, 158),
    Colour(190, 190, 190),
    Colour(223, 223, 223),
    Colour(255, 255, 255)
};

static std::vector<Colour> dmChordColours = {
    darkModeForegroundColour,
    darkModeForegroundColour,
    darkModeForegroundColour,
    darkModeForegroundColour,
    Colour(213, 213, 213),
    Colour(191, 191, 191),
    Colour(159, 159, 159),
    Colour(127, 127, 127),
    Colour(95, 95, 95),
    Colour(63, 63, 63),
    Colour(50, 50, 50),
    darkModeBackgroundColour
};

static int COLOUR_ARRAY_LENGTH = lmChordColours.size();

void FadeOut::init()
{
    updater.addAnimator(animator);
}

void FadeOut::start()
{
    if (running)
        return;
    running = true;
    animator.start();
}

void FadeOut::stop()
{
    colourIndex = 0;
    running = false;
    animator.complete();
    if (onStopped != nullptr)
        onStopped();
}

Colour FadeOut::getColour()
{
    return model->darkMode ? dmChordColours[colourIndex] : lmChordColours[colourIndex];
}

std::function<float(float)> FadeOut::createFadeOutIndexes()
{
    return [](float x) { return x < 1 ? x + 0.05f : x;  };
}

static int counter = 0;
void FadeOut::valueChangedCallback(float value)
{
    counter++;
    colourIndex = (int) (value * COLOUR_ARRAY_LENGTH);
    if (colourIndex == COLOUR_ARRAY_LENGTH)
    {
        counter = 0;
        colourIndex = 0;
        stop();
    }
    else
        owner->repaint();
}
