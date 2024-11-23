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

StaffCalculator::StaffCalculator(Rectangle<int> bounds)
{
    buttonHeight = getButtonHeight(bounds);
    buttonSpace = buttonHeight * 0.1;

    noteMultiplyConstant = 0.97;

    x = buttonSpace * 2;
    y = 0;
    height = bounds.getHeight();

    staffYIncrement = y + (height * 0.2);
    staffHeight = height * 0.5;
    width = bounds.getWidth() >= staffHeight ? bounds.getWidth() : staffHeight;

    lineThickness = staffHeight / 100;
    noteWidth = height / 20 + lineThickness * 2;
    noteHeight = height / 30 + lineThickness / 2;

    g1Anchor = height - height / 3 + noteHeight * 0.5 - lineThickness * 1;
    a2Anchor = height - height / 2 + noteHeight * 0.5;
    f4Anchor = y + staffYIncrement + (1 * noteHeight) - lineThickness * 1.5;
    e3Anchor = f4Anchor + (4 * noteHeight) - lineThickness;

    g5 = f4Anchor - (4 * noteHeight * noteMultiplyConstant);
    e5 = f4Anchor - (3 * noteHeight * noteMultiplyConstant);
    c5 = f4Anchor - (2 * noteHeight * noteMultiplyConstant);
    a4 = f4Anchor - (1 * noteHeight * noteMultiplyConstant);

    c3 = e3Anchor + (1 * noteHeight * noteMultiplyConstant);

    e1 = g1Anchor + (1 * noteHeight);
    c1 = g1Anchor + (2 * noteHeight);
    a0 = g1Anchor + (3 * noteHeight);
    f0 = g1Anchor + (4 * noteHeight);

    gLine1 = a4 + (0.5 * noteHeight);
    gLine2 = c5 + (0.5 * noteHeight);
    gLine3 = e5 + (0.5 * noteHeight);
    gLine4 = g5 + (0.5 * noteHeight);
    c3Line = c3 + (0.5 * noteHeight);
    fLine1 = e1 + (0.5 * noteHeight);
    fLine2 = c1 + (0.5 * noteHeight);
    fLine3 = a0 + (0.5 * noteHeight);
    fLine4 = f0 + (0.5 * noteHeight);

    sharpFontSize = noteHeight * 1.5;
    flatFontSize = noteHeight * 1.5;

    float keyX = noteWidth * 2;

    //F#
    sharpsX[0] = keyX;
    sharpsX[1] = keyX;
    sharpsY[0] = f4Anchor - noteHeight * 0.75;
    sharpsY[1] = a2Anchor + noteHeight * 0.75;

    //C#
    sharpsX[2] = keyX + noteWidth * 0.75;
    sharpsX[3] = keyX + noteWidth * 0.75;
    sharpsY[2] = f4Anchor + noteHeight * 0.75;
    sharpsY[3] = sharpsY[1] + noteHeight * 1.5;

    //G#
    sharpsX[4] = keyX + noteWidth * 1.25;
    sharpsX[5] = keyX + noteWidth * 1.25;
    sharpsY[4] = f4Anchor - noteHeight * 1.5;
    sharpsY[5] = sharpsY[1] - noteHeight * 0.5;

    //D#
    sharpsX[6] = keyX + noteWidth * 1.85;
    sharpsX[7] = keyX + noteWidth * 1.85;
    sharpsY[6] = sharpsY[0] + noteHeight;
    sharpsY[7] = sharpsY[1] + noteHeight;

    //A#
    sharpsX[8] = keyX + noteWidth * 2.5;
    sharpsX[9] = keyX + noteWidth * 2.5;
    sharpsY[8] = sharpsY[0] + noteHeight * 2.5;
    sharpsY[9] = sharpsY[1] + noteHeight * 2.5;

    //E#
    sharpsX[10] = keyX + noteWidth * 3.15;
    sharpsX[11] = keyX + noteWidth * 3.15;
    sharpsY[10] = sharpsY[0] + noteHeight * 0.5;
    sharpsY[11] = sharpsY[1] + noteHeight * 0.5;

    //B#
    sharpsX[12] = keyX + noteWidth * 3.8;
    sharpsX[13] = keyX + noteWidth * 3.8;
    sharpsY[12] = sharpsY[0] + noteHeight * 2;
    sharpsY[13] = sharpsY[1] + noteHeight * 2;

    //Bb
    flatsX[0] = keyX;
    flatsX[1] = keyX;
    flatsY[0] = f4Anchor + noteHeight * 0.65;
    flatsY[1] = g1Anchor - noteHeight * 2.25;

    //Eb
    flatsX[2] = keyX + noteWidth * 0.75;
    flatsX[3] = keyX + noteWidth * 0.75;
    flatsY[2] = f4Anchor - noteHeight;
    flatsY[3] = flatsY[1] - noteHeight * 1.5;

    //Ab
    flatsX[4] = keyX + noteWidth * 1.25;
    flatsX[5] = keyX + noteWidth * 1.25;
    flatsY[4] = f4Anchor + noteHeight * 1.1;
    flatsY[5] = g1Anchor - noteHeight * 1.75;

    //Db
    flatsX[6] = keyX + noteWidth * 1.85;
    flatsX[7] = keyX + noteWidth * 1.85;
    flatsY[6] = f4Anchor - noteHeight * 0.35;
    flatsY[7] = flatsY[1] - noteHeight * 1;

    //Gb
    flatsX[8] = keyX + noteWidth * 2.5;
    flatsX[9] = keyX + noteWidth * 2.5;
    flatsY[8] = f4Anchor + noteHeight * 1.65;
    flatsY[9] = g1Anchor - noteHeight * 1.25;

    //Cb
    flatsX[10] = keyX + noteWidth * 3.15;
    flatsX[11] = keyX + noteWidth * 3.15;
    flatsY[10] = f4Anchor + noteHeight * 0.15;
    flatsY[11] = flatsY[1] - noteHeight * 0.5;

    //Fb
    flatsX[12] = keyX + noteWidth * 3.8;
    flatsX[13] = keyX + noteWidth * 3.8;
}

static int accentedNotes[] = {
    18, 20, 22,
    25, 27, 30, 32, 34,
    37, 39, 42, 44, 46,
    49, 51, 54, 56, 58,
    61, 63, 66, 68, 70,
    73, 75, 78, 80, 82
};

void StaffCalculator::noteYPlacement(int midiNote, NoteDrawInfo& noteDrawInfo, bool sharp, bool applySharpFlat, int transpose)
{
    float y = -500;
    int drawnNote = midiNote + transpose;

    for (int note : accentedNotes)
    {
        if (drawnNote == note && !(noteDrawInfo.anchorNote > 0))
        {
            int drawnAccentedNote = sharp ? drawnNote - 1 : drawnNote + 1;
            noteYPlacement(drawnAccentedNote, noteDrawInfo, sharp, false, 0);
            if (applySharpFlat)
            {
                if (sharp)
                    noteDrawInfo.sharp = true;
                else
                    noteDrawInfo.flat = true;
            }
            return;
        }
    }

    float noteMultiplyConstant = 0.97;
    if (drawnNote < 29)
    {
        //below F1
        switch (drawnNote)
        {
        case 16: //E0
            y = f0 + (0.5 * noteHeight);
            break;
        case 17:
            y = f0;
            break;
        case 19:
            y = a0 + (0.5 * noteHeight);
            break;
        case 21:
            y = a0;
            break;
        case 23:
            y = c1 + (0.5 * noteHeight);
            break;
        case 24: //C1
            y = c1;
            break;
        case 26:
            y = e1 + (0.5 * noteHeight);
            break;
        case 28: //E1
            y = e1;
            break;
        case 27:
            y = e1 - (0.5 * noteHeight);
            break;
        }
        if (drawnNote < 19)
        {
            noteDrawInfo.lineCount = 4;
            noteDrawInfo.linePositions[0] = fLine1;
            noteDrawInfo.linePositions[1] = fLine2;
            noteDrawInfo.linePositions[2] = fLine3;
            noteDrawInfo.linePositions[3] = fLine4;
        }
        else if (drawnNote < 23)
        {
            noteDrawInfo.lineCount = 3;
            noteDrawInfo.linePositions[0] = fLine1;
            noteDrawInfo.linePositions[1] = fLine2;
            noteDrawInfo.linePositions[2] = fLine3;
        }
        else if (drawnNote < 26)
        {
            noteDrawInfo.lineCount = 2;
            noteDrawInfo.linePositions[0] = fLine1;
            noteDrawInfo.linePositions[1] = fLine2;
        }
        else if (drawnNote < 29)
        {
            noteDrawInfo.lineCount = 1;
            noteDrawInfo.linePositions[0] = fLine1;
        }
    }
    else if (drawnNote >= 29 && drawnNote < 48)
    {
        //F1 to B2
        switch (drawnNote)
        {
        case 29:
            y = g1Anchor + (0.5 * noteHeight * noteMultiplyConstant);
            break;
        case 31:
            y = g1Anchor;
            break;
        case 33:
            y = g1Anchor - (0.5 * noteHeight * noteMultiplyConstant);
            break;
        case 35:
            y = g1Anchor - (1 * noteHeight * noteMultiplyConstant);
            break;
        case 36: //C2
            y = g1Anchor - (1.5 * noteHeight * noteMultiplyConstant);
            break;
        case 38:
            y = g1Anchor - (2 * noteHeight * noteMultiplyConstant);
            break;
        case 40:
            y = g1Anchor - (2.5 * noteHeight * noteMultiplyConstant);
            break;
        case 41:
            y = g1Anchor - (3 * noteHeight * noteMultiplyConstant);
            break;
        case 43:
            y = g1Anchor - (3.5 * noteHeight * noteMultiplyConstant);
            break;
        case 45:
            y = g1Anchor - (4 * noteHeight * noteMultiplyConstant);
            break;
        case 47:
            y = g1Anchor - (4.5 * noteHeight * noteMultiplyConstant);
            break;
        }
    }
    else if (drawnNote == 48)
    {
        y = c3;
        noteDrawInfo.lineCount = 1;
        noteDrawInfo.linePositions[0] = c3Line;
    }
    else if (drawnNote > 48 && drawnNote <= 65)
    {
        //D3 to F4
        switch (drawnNote)
        {
        case 50:
            y = e3Anchor + (0.5 * noteHeight * noteMultiplyConstant);
            break;
        case 52:
            y = e3Anchor;
            break;
        case 53:
            y = e3Anchor - (0.5 * noteHeight * noteMultiplyConstant);
            break;
        case 55:
            y = e3Anchor - (1 * noteHeight * noteMultiplyConstant);
            break;
        case 57:
            y = e3Anchor - (1.5 * noteHeight * noteMultiplyConstant);
            break;
        case 59:
            y = f4Anchor + (2 * noteHeight * noteMultiplyConstant);
            break;
        case 60: // C4
            y = f4Anchor + (1.5 * noteHeight * noteMultiplyConstant);
            break;
        case 62:
            y = f4Anchor + (1 * noteHeight * noteMultiplyConstant);
            break;
        case 64:
            y = f4Anchor + (0.5 * noteHeight * noteMultiplyConstant);
            break;
        case 65:
            y = f4Anchor;
            break;
        }
    }
    else if (drawnNote > 65)
    {
        //G4 to A5
        switch (drawnNote)
        {
        case 67:
            y = f4Anchor - (0.5 * noteHeight);
            break;
        case 69:
            y = a4;
            break;
        case 71:
            y = a4 - (0.5 * noteHeight);
            break;
        case 72: //C5
            y = c5;
            break;
        case 74:
            y = c5 - (0.5 * noteHeight);
            break;
        case 76:
            y = e5;
            break;
        case 77:
            y = e5 - (0.5 * noteHeight);
            break;
        case 79:
            y = g5;
            break;
        case 81:
            y = g5 - (0.5 * noteHeight);
            break;
        }
        if (drawnNote > 77 && drawnNote < 82)
        {
            noteDrawInfo.lineCount = 4;
            noteDrawInfo.linePositions[0] = gLine1;
            noteDrawInfo.linePositions[1] = gLine2;
            noteDrawInfo.linePositions[2] = gLine3;
            noteDrawInfo.linePositions[3] = gLine4;
        }
        else if (drawnNote > 74 && drawnNote < 78)
        {
            noteDrawInfo.lineCount = 3;
            noteDrawInfo.linePositions[0] = gLine1;
            noteDrawInfo.linePositions[1] = gLine2;
            noteDrawInfo.linePositions[2] = gLine3;
        }
        else if (drawnNote > 71 && drawnNote < 75)
        {
            noteDrawInfo.lineCount = 2;
            noteDrawInfo.linePositions[0] = gLine1;
            noteDrawInfo.linePositions[1] = gLine2;
        }
        else if (drawnNote > 67 && drawnNote < 72)
        {
            noteDrawInfo.lineCount = 1;
            noteDrawInfo.linePositions[0] = gLine1;
        }
    }

    noteDrawInfo.anchorNote = drawnNote;
    noteDrawInfo.y = y;
}