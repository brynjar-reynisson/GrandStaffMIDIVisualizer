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

static const Font getCustomFont()
{
    static auto typeface = Typeface::createSystemTypefaceFor(BinaryData::InconsolataBold_ttf, BinaryData::InconsolataBold_ttfSize);
    return Font(typeface);
}

void MainComponent::init(PluginModel* model)
{
    this->pluginModel = model;
    chordsLabel.setFont(getCustomFont());
    
    chordsLabel.setText("", NotificationType::dontSendNotification);
    chordsLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    chordsLabel.setJustificationType(juce::Justification::left);

    holdNoteButton.addListener(this);
    holdNoteButton.setImages(noteSvg.get());
    holdNoteButton.setColour(TextButton::buttonColourId, Colours::lightgrey);
    holdNoteButton.setColour(TextButton::buttonOnColourId, Colours::white);
    holdNoteButton.setToggleable(true);
    holdNoteButton.setTooltip("Hold notes");

    int i = 1;
    for (String key : keys.getKeyNames())
    {
        keyMenu.addItem(key, i++);
    }
    keyMenu.onChange = [this] { keyMenuChanged(); };
    keyMenu.setSelectedItemIndex(1);
   
    leftArrowButton.addListener(this);
    leftArrowButton.setButtonText("<");
    leftArrowButton.setColour(TextButton::textColourOffId, Colours::black);
    leftArrowButton.setColour(TextButton::buttonColourId, Colours::white);
    leftArrowButton.setTooltip("Octave down");

    octaveLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    octaveLabel.setColour(juce::Label::backgroundColourId, Colour(230, 230, 230));
    octaveLabel.setJustificationType(juce::Justification::centred);

    rightArrowButton.addListener(this);
    rightArrowButton.setButtonText(">");
    rightArrowButton.setColour(TextButton::textColourOffId, Colours::black);
    rightArrowButton.setColour(TextButton::buttonColourId, Colours::white);
    rightArrowButton.setTooltip("Octave up");

    chordPlacementButton.addListener(this);
    chordPlacementButton.setColour(TextButton::textColourOffId, Colours::black);
    chordPlacementButton.setColour(TextButton::buttonColourId, Colours::white);

    onParametersChanged();

    addAndMakeVisible(chordsLabel);
    //addAndMakeVisible(sharpButton);
    //addAndMakeVisible(flatButton);
    addAndMakeVisible(holdNoteButton);
    addAndMakeVisible(keyMenu);
    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(octaveLabel);
    addAndMakeVisible(rightArrowButton);
    addAndMakeVisible(chordPlacementButton);
}

void MainComponent::onParametersChanged()
{
    keyMenu.setSelectedId(pluginModel->keyId + 1, false);
    holdNoteButton.setToggleState(pluginModel->holdNotes, false);
    octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
    updateChordPlacementButton();
    pluginModel->hasParamChanges = false;
}

void MainComponent::updateChordPlacementButton()
{
    if (pluginModel->chordsOnRight)
    {
        chordPlacementButton.setButtonText("V");
        chordPlacementButton.setTooltip("Display chords in bottom left corner");
    }
    else
    {
        chordPlacementButton.setButtonText(">");
        chordPlacementButton.setTooltip("Display chords to the right of the Grand Staff");
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    int size = bounds.getHeight() / 20;
    int buttonSpace = size / 10;
    keyMenu.setBounds(buttonSpace, 0, size * 4, size);

    holdNoteButton.setBounds(bounds.getWidth() - size * 5 - buttonSpace, 0, size, size);
    leftArrowButton.setBounds(bounds.getWidth() - size * 4, 0, size, size);
    octaveLabel.setBounds(bounds.getWidth() - size * 3, 0, size * 2, size);
    rightArrowButton.setBounds(bounds.getWidth() - size, 0, size, size);

    chordPlacementButton.setBounds(buttonSpace, bounds.getHeight() - size - buttonSpace, size * 1.1, size);

    float labelHeight = size * 2;//bounds.getHeight() >= 200 ? 20 : bounds.getHeight() / 10;
    float labelFontHeight = labelHeight;
    //chordsLabel.setFont(juce::Font("Consolas", labelFontHeight, juce::Font::bold));
    chordsLabel.setFont(getCustomFont());
    chordsLabel.setFont(Font::bold);
    chordsLabel.setFont(labelFontHeight);
    chordsLabel.setBounds(size * 2 + buttonSpace * 5, bounds.getBottom() - labelHeight - buttonSpace, bounds.getWidth() - size * 2 - buttonSpace * 5, labelHeight);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    pluginModel->hasUIChanges = true;
    if (button == &holdNoteButton)
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
    else if (button == &chordPlacementButton)
    {
        pluginModel->chordsOnRight = !pluginModel->chordsOnRight;
        updateChordPlacementButton();
    }

    NullCheckedInvocation::invoke(pluginModel->onChange);
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
    if (pluginModel->hasParamChanges)
        onParametersChanged();

    g.fillAll(juce::Colours::white);
    StaffCalculator staffCalculator(getLocalBounds());
    drawStaff(g, staffCalculator);
    g.setColour(juce::Colours::black);

    drawKeySignature(g, staffCalculator);
    
    
    std::set<int> midiNotes;
    for (int i = 0; i < 127; i++)
    {
        if (pluginModel->midiNotes[i] == 1)
            midiNotes.insert(i);
    }
    
    
    /*
    std::set<int> midiNotes{
        48, 51, 52, 54, 59
    };
    */

    Chord chord;
    chords.name(midiNotes, keys.getKey(keyMenu.getText()), chord);
    
    if (!pluginModel->chordsOnRight)
        chordsLabel.setText(chord.name(), NotificationType::dontSendNotification);
    else
        chordsLabel.setText("", NotificationType::dontSendNotification);

    //Find out where to place the notes
    NoteDrawInfo noteDrawInfos[127];
    std::set<int> anchorNotes;
    for (int midiNote : midiNotes)
    {
        noteDrawInfos[midiNote] = NoteDrawInfo();
        if (keyMenu.getSelectedId() < 3)
        {
            bool sharp = keyMenu.getText() == Key::SHARP ? true : false;
            staffCalculator.noteYPlacement(midiNote, noteDrawInfos[midiNote], sharp, true, 12 * pluginModel->transposeOctaves);
        }
        else
        {
            keys.applyAnchorNoteAndAccents(midiNote, keys.getKey(keyMenu.getText()), chord, noteDrawInfos[midiNote]);
            staffCalculator.noteYPlacement(noteDrawInfos[midiNote].anchorNote, noteDrawInfos[midiNote], false, false, 12 * pluginModel->transposeOctaves);
        }

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
            noteDrawInfos[midiNote].dontMoveRight = false;
            //find the other note
            for (int j = 0; j < 127; j++)
            {
                int testAnchorNote = noteDrawInfos[j].anchorNote;
                if (testAnchorNote < 0)
                    continue;
                if (testAnchorNote == anchorNote)
                {
                    noteDrawInfos[j].doubleNote = true;
                    //noteDrawInfos[j].selectedToMoveRight = true;
                    //noteDrawInfos[j].hasPlacementConflict = true;
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
    //float sharpDiffMultiplier = 3;
    //float flatDiffMultiplier = 2;
    //float diffMultiplier = keyMenu.getText() == Key::SHARP ? 3 : 2;//sharps need more space than flats
    for (rit = midiNotes.rbegin(); rit != midiNotes.rend(); rit++)
    {
        // *rit to access value
        int midiNote = *rit;
        if (!noteDrawInfos[midiNote].sharp && !noteDrawInfos[midiNote].flat && !noteDrawInfos[midiNote].natural)
            continue;

        //first note always indent 0, and update lastY
        if (lastY < 0) {
            lastY = noteDrawInfos[midiNote].y;
            continue;
        }

        float yDiff = noteDrawInfos[midiNote].y - lastY;
        float diffMultiplier = noteDrawInfos[midiNote].sharp ? 3 : 2;
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

    float baseNoteX = staffCalculator.x + (staffCalculator.staffHeight / 2) + (staffCalculator.staffHeight / 4) + staffCalculator.noteWidth * 5;
    int lineX = baseNoteX - staffCalculator.lineThickness;
    float accentX = baseNoteX - staffCalculator.noteWidth;
    bool firstNote = true;

    for (int midiNote : midiNotes)
    {
        if (firstNote && pluginModel->chordsOnRight)
        {
            float textWidth = getLocalBounds().getWidth() - baseNoteX - staffCalculator.noteWidth * 3;
            float textHeight = textWidth / 7.5f;

            float chordY = noteDrawInfos[midiNote].y - textHeight * 0.5 + staffCalculator.noteHeight * 0.5;//staffCalculator.noteHeight / 2;
            float chordX = staffCalculator.staffHeight * 1.66;
            g.setFont(getCustomFont());
            g.setFont(textHeight);
            //g.setFont(getLocalBounds().getHeight() / 5);
            //g.drawText(chord.name(), chordX, chordY, staffCalculator.noteWidth * 50, staffCalculator.noteHeight * 2, Justification::centredLeft);
            g.drawText(chord.name(), chordX, chordY, textWidth, textHeight, Justification::centredLeft);
            firstNote = false;
        }

        float noteX = baseNoteX;
        float accentX = baseNoteX - (staffCalculator.noteWidth) - (noteDrawInfos[midiNote].accentIndent * staffCalculator.noteWidth * 0.75);
        if (noteDrawInfos[midiNote].selectedToMoveRight && !noteDrawInfos[midiNote].dontMoveRight)
        {
            noteX += staffCalculator.noteWidth - staffCalculator.lineThickness * 2;
            //if(noteDrawInfos[midiNote].doubleNote)
            //{
            //    int extendedX = noteX + staffCalculator.noteWidth - staffCalculator.lineThickness * 2;
            //    noteSvg->drawWithin(g, Rectangle<float>(extendedX, noteDrawInfos[midiNote].y, staffCalculator.noteWidth, staffCalculator.noteHeight), juce::RectanglePlacement::Flags::xLeft, 1.0);
            //}
            //else {
                noteSvg->drawWithin(g, Rectangle<float>(noteX, noteDrawInfos[midiNote].y - staffCalculator.lineThickness * 2, staffCalculator.noteWidth, staffCalculator.noteHeight + staffCalculator.lineThickness * 4), juce::RectanglePlacement::Flags::xLeft, 1.0);
            //}
        } else
            noteSvg->drawWithin(g, Rectangle<float>(noteX, noteDrawInfos[midiNote].y - staffCalculator.lineThickness*2, staffCalculator.noteWidth, staffCalculator.noteHeight+staffCalculator.lineThickness*4), juce::RectanglePlacement::Flags::xLeft, 1.0);

        for (int j=0; j<noteDrawInfos[midiNote].lineCount; j++)
        {
            g.drawLine(lineX, noteDrawInfos[midiNote].linePositions[j], lineX + staffCalculator.noteWidth + staffCalculator.lineThickness * 2, noteDrawInfos[midiNote].linePositions[j], staffCalculator.lineThickness);
        }
        if (noteDrawInfos[midiNote].sharp)
        {
            sharpSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight * 0.75, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
        else if (noteDrawInfos[midiNote].flat)
        {
            flatSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight * 1.25, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
        else if (noteDrawInfos[midiNote].natural)
        {
            naturalSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight * 0.75, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
    }
}



void MainComponent::drawStaff(Graphics& g, StaffCalculator& staffCalculator)
{
    staffSvg->drawWithin(g, Rectangle<float>(staffCalculator.x - staffCalculator.lineThickness * 2, staffCalculator.staffYIncrement, staffCalculator.width, staffCalculator.staffHeight), RectanglePlacement::Flags::xLeft | RectanglePlacement::Flags::yMid, 1.0);
}

void MainComponent::drawKeySignature(Graphics& g, StaffCalculator& staffCalculator)
{
    String keyName = keyMenu.getText();
    Key key = keys.getKey(keyName);
    if (key.name == "Sharp" || key.name == "Flat")
        return;

    if (key.numSharps > 0)
        drawSharps(g, staffCalculator, key.numSharps);
    else if (key.numFlats > 0)
        drawFlats(g, staffCalculator, key.numFlats);
}

void MainComponent::drawSharps(Graphics& g, StaffCalculator& staffCalculator, int numSharps)
{
    for (int i = 0; i < numSharps; i++)
    {
        float x1 = -1, x2 = -1, y1 = -1, y2 = -1;
        int idx = i * 2;

        x1 = staffCalculator.sharpsX[idx]; x2 = staffCalculator.sharpsX[idx+1];
        y1 = staffCalculator.sharpsY[idx]; y2 = staffCalculator.sharpsY[idx+1];

        sharpSvg->drawWithin(g, Rectangle(x1, y1, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), RectanglePlacement::yMid, 1.0);
        sharpSvg->drawWithin(g, Rectangle(x2, y2, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), RectanglePlacement::yMid, 1.0);
    }
}

void MainComponent::drawFlats(Graphics& g, StaffCalculator& staffCalculator, int numFlats)
{
    for (int i = 0; i < numFlats; i++)
    {
        float x1 = -1, x2 = -1, y1 = -1, y2 = -1;
        int idx = i * 2;

        x1 = staffCalculator.flatsX[idx]; x2 = staffCalculator.flatsX[idx + 1];
        y1 = staffCalculator.flatsY[idx]; y2 = staffCalculator.flatsY[idx + 1];

        flatSvg->drawWithin(g, Rectangle(x1, y1, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), RectanglePlacement::yMid, 1.0);
        flatSvg->drawWithin(g, Rectangle(x2, y2, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), RectanglePlacement::yMid, 1.0);
    }
}

void MainComponent::keyMenuChanged()
{  
    pluginModel->hasUIChanges = true;
    pluginModel->keyId = keyMenu.getSelectedId() - 1;
    pluginModel->onChange();
    repaint();
}

