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
#include <cmath>
#include <set>

void MainComponent::init(PluginModel* model)
{
    this->pluginModel = model;
    chordsLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    chordsLabel.setText("Coming soon - chords!", NotificationType::dontSendNotification);
    chordsLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    chordsLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(chordsLabel);

    sharpButton.addListener(this);
    sharpButton.setImages(sharpSvg.get());
    sharpButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
    sharpButton.setColour(TextButton::buttonOnColourId, Colours::white);
    sharpButton.setToggleable(true);
    sharpButton.setToggleState(pluginModel->sharp, true);
    sharpButton.setTooltip(String("Sharp notation"));
    addAndMakeVisible(sharpButton);

    flatButton.addListener(this);
    flatButton.setImages(flatSvg.get());
    flatButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
    flatButton.setColour(TextButton::buttonOnColourId, Colours::white);
    flatButton.setToggleable(true);
    flatButton.setToggleState(!pluginModel->sharp, true);
    flatButton.setTooltip("Flat notation");
    addAndMakeVisible(flatButton);

    holdNoteButton.addListener(this);
    holdNoteButton.setImages(noteSvg.get());
    holdNoteButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
    holdNoteButton.setColour(TextButton::buttonOnColourId, Colours::white);
    holdNoteButton.setToggleable(true);
    holdNoteButton.setToggleState(pluginModel->holdNotes, true);
    holdNoteButton.setTooltip("Hold notes");
    addAndMakeVisible(holdNoteButton);

    leftArrowButton.addListener(this);
    leftArrowButton.setButtonText("<");
    leftArrowButton.setColour(TextButton::textColourOffId, Colours::black);
    leftArrowButton.setColour(TextButton::buttonColourId, Colours::white);
    leftArrowButton.setTooltip("Octave down");
    addAndMakeVisible(leftArrowButton);

    octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
    octaveLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    octaveLabel.setColour(juce::Label::backgroundColourId, Colour(230, 230, 230));
    octaveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(octaveLabel);

    rightArrowButton.addListener(this);
    rightArrowButton.setButtonText(">");
    rightArrowButton.setColour(TextButton::textColourOffId, Colours::black);
    rightArrowButton.setColour(TextButton::buttonColourId, Colours::white);
    rightArrowButton.setTooltip("Octave up");
    addAndMakeVisible(rightArrowButton);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    float labelHeight = bounds.getHeight() >= 200 ? 20 : bounds.getHeight() / 10;
    float labelFontHeight = labelHeight;
    chordsLabel.setFont(juce::Font("Consolas", labelFontHeight, juce::Font::bold));
    chordsLabel.setBounds(bounds.getX() + 3, bounds.getBottom() - labelHeight - 3, bounds.getRight() - 3, labelHeight);

    int size = bounds.getHeight() / 20;
    int buttonSpace = size / 10;
    sharpButton.setBounds(0, 0, size, size);
    flatButton.setBounds(size + buttonSpace, 0, size, size);
    holdNoteButton.setBounds(size * 2 + buttonSpace * 2, 0, size, size);

    leftArrowButton.setBounds(bounds.getWidth() - size * 4, 0, size, size);
    octaveLabel.setBounds(bounds.getWidth() - size * 3, 0, size * 2, size);
    rightArrowButton.setBounds(bounds.getWidth() - size, 0, size, size);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &sharpButton)
    {
        pluginModel->sharp = true;
        sharpButton.setToggleState(true, false);
        flatButton.setToggleState(false, false);
    }
    else if (button == &flatButton)
    {
        pluginModel->sharp = false;
        sharpButton.setToggleState(false, false);
        flatButton.setToggleState(true, false);
    }
    else if (button == &holdNoteButton)
    {
        bool curMode = holdNoteButton.getToggleState();
        holdNoteButton.setToggleState(!curMode, false);
        for (int i = 0; i < 127; i++)
            pluginModel->midiNotes[i] = 0;
        pluginModel->holdNotes = holdNoteButton.getToggleState();
    }
    else if (button == &leftArrowButton)
    {
        if (pluginModel->transposeOctaves > -3)
        {
            pluginModel->transposeOctaves--;
            octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
        }
    }
    else if (button == &rightArrowButton)
    {
        if (pluginModel->transposeOctaves < 3)
        {
            pluginModel->transposeOctaves++;
            octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
        }
    }

    repaint();
}


static int dontMoveRight[] = {
    17, 21, 24, 28,  //F0 to F1 below F-clef
    31, 35, 38, 41, 45, //G1 to A2 on F-clef
    48, 52, 55, 59, 62, //C3 to D4 on G-clef
    65, 69, 72, 76, 79 //F4 to G5
};

static std::set<int> conflictLosers = {
    33, 36, 40, 43, 47,
    50, 53, 57, 60, 64
};

void MainComponent::paint(Graphics& g)
{
    g.fillAll(juce::Colours::white);
    StaffCalculator staffCalculator(getLocalBounds());
    drawStaff(g, staffCalculator);
    g.setColour(juce::Colours::black);

    std::set<int> midiNotes;
    for (int i = 0; i < 127; i++)
    {
        if (pluginModel->midiNotes[i] == 1)
            midiNotes.insert(i);
    }

    chordsLabel.setText(Chords::name(midiNotes, pluginModel->sharp, false), NotificationType::dontSendNotification);

    //Find out where to place the notes
    NoteDrawInfo noteDrawInfos[127];
    std::set<int> anchorNotes;
    for (int midiNote : midiNotes)
    {
        noteDrawInfos[midiNote] = NoteDrawInfo();
        staffCalculator.noteYPlacement(midiNote, noteDrawInfos[midiNote], pluginModel->sharp, 12 * pluginModel->transposeOctaves);
        for (int dontMove : dontMoveRight)
        {
            if (dontMove == noteDrawInfos[midiNote].anchorNote)
            {
                noteDrawInfos[midiNote].dontMoveRight = true;
                break;
            }
        }
    }

    //For neighbor notes, find out who needs to move right
    for (int midiNote : midiNotes)
    {
        int anchorNote = noteDrawInfos[midiNote].anchorNote;
        if (anchorNote < 0)
            continue;

        if (anchorNotes.find(anchorNote) != anchorNotes.end())
        {
            //same note repetition case, meaning one should be played with a flat or sharp accent, and the other one natural
            noteDrawInfos[midiNote].selectedToMoveRight = true;
            noteDrawInfos[midiNote].doubleNote = true;
            //find the other note
            for (int j = 0; j < 127; j++)
            {
                int testAnchorNote = noteDrawInfos[j].anchorNote;
                if (testAnchorNote < 0)
                    continue;
                if (testAnchorNote == anchorNote)
                {
                    noteDrawInfos[j].doubleNote = true;
                    break;
                }
            }
        }
        else
            anchorNotes.insert(anchorNote);
    }

    for (int midiNote : midiNotes)
    {
        int anchorNote = noteDrawInfos[midiNote].anchorNote;
        if (anchorNote < 0)
            continue;

        for (int cur : anchorNotes)
        {
            int diff = std::abs(cur - anchorNote);
            if (diff > 0 && diff < 3)
            {
                noteDrawInfos[midiNote].hasPlacementConflict = true;
                if ((anchorNote < 31 || anchorNote > 54) && !noteDrawInfos[midiNote].dontMoveRight)
                    noteDrawInfos[midiNote].selectedToMoveRight = true;
                else if(conflictLosers.find(anchorNote) != conflictLosers.end()) {
                    noteDrawInfos[midiNote].selectedToMoveRight = true;
                }
            }
        }
    }

    //Figure out flat/sharp indentation
    float lastY = -1;
    int lastAccentIndent = 0;
    std::set<int>::reverse_iterator rit;
    float diffMultiplier = pluginModel->sharp ? 3 : 2;//sharps need more space than flats
    for (rit = midiNotes.rbegin(); rit != midiNotes.rend(); rit++)
    {
        // *rit to access value
        int midiNote = *rit;
        if (!noteDrawInfos[midiNote].sharp && !noteDrawInfos[midiNote].flat)
            continue;

        //first note always indent 0, and update lastY
        if (lastY < 0) {
            lastY = noteDrawInfos[midiNote].y;
            continue;
        }

        float yDiff = noteDrawInfos[midiNote].y - lastY;
        if (yDiff < staffCalculator.noteHeight * diffMultiplier)
        {
            //if this is close to the indent 0 y, change indent, and don't update lastY
            if (lastAccentIndent == 4)
                lastAccentIndent = 0;
            else
                lastAccentIndent++;
            noteDrawInfos[midiNote].accentIndent = lastAccentIndent;
        }
        else
        {
            //enough space, set indent to 0 and update lastY
            lastAccentIndent = 0;
            lastY = noteDrawInfos[midiNote].y;
        }
    }
    //Done finding out where to put the notes

    float baseNoteX = staffCalculator.x + (staffCalculator.staffHeight / 2) + (staffCalculator.staffHeight / 4) + staffCalculator.noteWidth;
    int lineX = baseNoteX - staffCalculator.lineThickness;
    float accentX = baseNoteX - staffCalculator.noteWidth;

    for (int midiNote : midiNotes)
    {
        float noteX = baseNoteX;
        float accentX = baseNoteX - (staffCalculator.noteWidth) - (noteDrawInfos[midiNote].accentIndent * staffCalculator.noteWidth * 0.75);
        if (noteDrawInfos[midiNote].selectedToMoveRight && !noteDrawInfos[midiNote].dontMoveRight)
        {
            noteX += staffCalculator.noteWidth - staffCalculator.lineThickness * 2;
            if(noteDrawInfos[midiNote].doubleNote)
            {
                int extendedX = noteX + staffCalculator.noteWidth - staffCalculator.lineThickness * 2;
                noteSvg->drawWithin(g, Rectangle<float>(extendedX, noteDrawInfos[midiNote].y, staffCalculator.noteWidth, staffCalculator.noteHeight), juce::RectanglePlacement::Flags::xLeft, 1.0);
            }
        }

        noteSvg->drawWithin(g, Rectangle<float>(noteX, noteDrawInfos[midiNote].y - staffCalculator.lineThickness*2, staffCalculator.noteWidth, staffCalculator.noteHeight+staffCalculator.lineThickness*4), juce::RectanglePlacement::Flags::xLeft, 1.0);

        for (int j=0; j<noteDrawInfos[midiNote].lineCount; j++)
        {
            g.drawLine(lineX, noteDrawInfos[midiNote].linePositions[j], lineX + staffCalculator.noteWidth + staffCalculator.lineThickness * 2, noteDrawInfos[midiNote].linePositions[j], staffCalculator.lineThickness);
        }
        if (noteDrawInfos[midiNote].sharp)
        {
            sharpSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight*0.75, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
        else if (noteDrawInfos[midiNote].flat)
        {
            flatSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight, staffCalculator.noteWidth, staffCalculator.noteHeight * 2), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
    }
}


StaffCalculator::StaffCalculator(Rectangle<int> bounds)
{
    noteMultiplyConstant = 0.97;

    x = bounds.getX();
    y = bounds.getY();
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
}

static int accentedNotes[] = {
    18, 20, 22,
    25, 27, 30, 32, 34,
    37, 39, 42, 44, 46,
    49, 51, 54, 56, 58,
    61, 63, 66, 68, 70,
    73, 75, 78, 80, 82
};

void StaffCalculator::noteYPlacement(int midiNote, NoteDrawInfo& noteDrawInfo, bool sharp, int transpose)
{
    float y = -500;
    int drawnNote = midiNote + transpose;

    for (int note : accentedNotes)
    {
        if (drawnNote == note)
        {
            int drawnAccentedNote = sharp ? drawnNote - 1 : drawnNote + 1;
            noteYPlacement(drawnAccentedNote, noteDrawInfo, sharp, 0);
            if (sharp)
                noteDrawInfo.sharp = true;
            else
                noteDrawInfo.flat = true;
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

void MainComponent::drawStaff(Graphics& g, StaffCalculator& staffCalculator)
{
    staffSvg->drawWithin(g, Rectangle<float>(staffCalculator.x, staffCalculator.staffYIncrement, staffCalculator.width, staffCalculator.staffHeight), RectanglePlacement::Flags::xLeft | RectanglePlacement::Flags::yMid, 1.0);
}