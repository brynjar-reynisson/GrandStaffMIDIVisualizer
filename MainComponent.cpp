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

static const Font getCustomFont(bool bold = true)
{
    if (bold)
        return Font(Typeface::createSystemTypefaceFor(BinaryData::InconsolataBold_ttf, BinaryData::InconsolataBold_ttfSize));
    else
        return Font(Typeface::createSystemTypefaceFor(BinaryData::InconsolataRegular_ttf, BinaryData::InconsolataRegular_ttfSize));
}

void MainComponent::onParametersChanged()
{
    keyMenu.setSelectedId(pluginModel->keyId + 1, false);
    holdNoteButton.setToggleState(pluginModel->holdNotes, false);
    octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
    updateChordPlacementButton();
    chordFontBoldButton.setToggleState(pluginModel->chordFontBold, false);
    updateColourScheme();
    pluginModel->hasParamChanges = false;
    repaint();
}

void MainComponent::init(PluginModel* model)
{
    this->pluginModel = model;
    pluginModel->paramChangedFromHost = [&] { onParametersChanged(); };

    holdNoteButton.addListener(this);
    holdNoteButton.setToggleable(true);
    holdNoteButton.setToggleState(pluginModel->holdNotes, false);
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
    leftArrowButton.setTooltip("Octave down");

    octaveLabel.setJustificationType(juce::Justification::centred);

    rightArrowButton.addListener(this);
    rightArrowButton.setButtonText(">");
    rightArrowButton.setTooltip("Octave up");

    chordPlacementButton.addListener(this);

    chordFontBoldButton.addListener(this);
    chordFontBoldButton.setButtonText("B");
    chordFontBoldButton.setTooltip("Display chords with bold font");

    darkModeButton.addListener(this);
    darkModeButton.setToggleable(true);
    darkModeButton.setToggleState(pluginModel->darkMode, false);
    darkModeButton.setButtonText("D");
    darkModeButton.setTooltip("Turn dark mode on");

    onParametersChanged();
    updateColourScheme();

    addAndMakeVisible(holdNoteButton);
    addAndMakeVisible(keyMenu);
    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(octaveLabel);
    addAndMakeVisible(rightArrowButton);
    addAndMakeVisible(chordPlacementButton);
    addAndMakeVisible(chordFontBoldButton);
    addAndMakeVisible(darkModeButton);
}

void MainComponent::updateChordPlacementButton()
{
    if (pluginModel->chordPlacement == 0)
    {
        chordPlacementButton.setButtonText("V");
        chordPlacementButton.setTooltip("Display chords in bottom left corner");
    }
    else if (pluginModel->chordPlacement == 1)
    {
        chordPlacementButton.setButtonText(">");
        chordPlacementButton.setTooltip("Display chords to the right of the Grand Staff");
    }
    else if (pluginModel->chordPlacement == 2)
    {
        chordPlacementButton.setButtonText("^");
        chordPlacementButton.setTooltip("Only show chords");
    }
    else if (pluginModel->chordPlacement == 3)
    {
        chordPlacementButton.setButtonText(" ");
        chordPlacementButton.setTooltip("Don't display chords");
    }
}

void MainComponent::updateColourScheme()
{
    if (pluginModel->darkMode)
    {
        staffSvg = dmStaffSvg.get();
        noteSvg = dmNoteSvg.get();
        sharpSvg = dmSharpSvg.get();
        flatSvg = dmFlatSvg.get();
        naturalSvg = dmNaturalSvg.get();
        doubleSharpSvg = dmDoubleSharpSvg.get();
        doubleFlatSvg = dmDoubleFlatSvg.get();

        octaveLabel.setColour(juce::Label::textColourId, darkModeForegroundColour);
        octaveLabel.setColour(juce::Label::backgroundColourId, selectedDarkModeBackgroundColour);
        darkModeButton.setTooltip("Turn dark mode off");
        darkModeButton.setToggleState(true, false);
    }
    else
    {
        staffSvg = lmStaffSvg.get();
        noteSvg = lmNoteSvg.get();
        sharpSvg = lmSharpSvg.get();
        flatSvg = lmFlatSvg.get();
        naturalSvg = lmNaturalSvg.get();
        doubleSharpSvg = lmDoubleSharpSvg.get();
        doubleFlatSvg = lmDoubleFlatSvg.get();

        octaveLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        octaveLabel.setColour(juce::Label::backgroundColourId, Colour(230, 230, 230));
        darkModeButton.setTooltip("Turn dark mode on");
        darkModeButton.setToggleState(false, false);
    }
    holdNoteButton.setImages(noteSvg);


    Button* buttons[] = { &holdNoteButton, &leftArrowButton, &rightArrowButton, &chordPlacementButton, &chordFontBoldButton, &darkModeButton };
    for (Button* button : buttons)
    {
        if (pluginModel->darkMode)
        {
            button->setColour(TextButton::textColourOnId, darkModeForegroundColour);
            button->setColour(TextButton::textColourOffId, darkModeForegroundColour);
            button->setColour(TextButton::buttonColourId, darkModeBackgroundColour);
            button->setColour(TextButton::buttonOnColourId, selectedDarkModeBackgroundColour);
        }
        else
        {
            button->setColour(TextButton::textColourOnId, Colours::black);
            button->setColour(TextButton::textColourOffId, Colours::darkgrey);
            button->setColour(TextButton::buttonColourId, Colours::lightgrey);
            button->setColour(TextButton::buttonOnColourId, Colours::white);
        }
    }

    Drawable* drawables[] = { lmStaffSvg.get(), lmNoteSvg.get(), lmSharpSvg.get(), lmFlatSvg.get(), lmNaturalSvg.get(), lmDoubleSharpSvg.get(), lmDoubleFlatSvg.get() };
    for (Drawable* drawable : drawables)
    {
        if (pluginModel->darkMode)
        {
            drawable->replaceColour(Colours::black, darkModeForegroundColour);
        }
        else
        {
            drawable->replaceColour(darkModeForegroundColour, Colours::black);
        }
    }
}

int getButtonHeight(Rectangle<int> bounds)
{
    int minButtonHeight = 13;
    int calculatedButtonHeight = bounds.getHeight() * 0.05;
    return std::max((int)calculatedButtonHeight, minButtonHeight);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    pluginModel->uiWidth = bounds.getWidth();
    pluginModel->uiHeight = bounds.getHeight();

    int buttonSize = getButtonHeight(bounds);
    int buttonSpace = buttonSize / 10;
    keyMenu.setBounds(buttonSpace, 0, buttonSize * 4, buttonSize);

    holdNoteButton.setBounds(bounds.getWidth() - buttonSize * 5.5 - buttonSpace * 2, 0, buttonSize * 1.1, buttonSize);
    leftArrowButton.setBounds(bounds.getWidth() - buttonSize * 4.4 - buttonSpace, 0, buttonSize * 1.1, buttonSize);
    octaveLabel.setBounds(bounds.getWidth() - buttonSize * 3.3 - buttonSpace, 0, buttonSize * 2.2, buttonSize);
    rightArrowButton.setBounds(bounds.getWidth() - buttonSize * 1.1 - buttonSpace, 0, buttonSize * 1.1, buttonSize);

    chordPlacementButton.setBounds(buttonSpace, bounds.getHeight() - buttonSize - buttonSpace, buttonSize * 1.1, buttonSize);
    chordFontBoldButton.setBounds(buttonSize * 1.1 + buttonSpace * 2, bounds.getHeight() - buttonSize - buttonSpace, buttonSize * 1.1, buttonSize);
    darkModeButton.setBounds(buttonSize * 2.2 + buttonSpace * 3, bounds.getHeight() - buttonSize - buttonSpace, buttonSize * 1.1, buttonSize);
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
        pluginModel->chordPlacement = pluginModel->chordPlacement == 3 ? 0 : pluginModel->chordPlacement + 1;
        updateChordPlacementButton();
    }
    else if (button == &chordFontBoldButton)
    {
        bool curMode = chordFontBoldButton.getToggleState();
        chordFontBoldButton.setToggleState(!curMode, false);
        pluginModel->chordFontBold = chordFontBoldButton.getToggleState();
        if (pluginModel->chordFontBold)
            chordFontBoldButton.setTooltip("Display chords with plain font");
        else
            chordFontBoldButton.setTooltip("Display chords with bold font");
    }
    else if (button == &darkModeButton)
    {
        pluginModel->darkMode = !pluginModel->darkMode;
        updateColourScheme();
    }

    NullCheckedInvocation::invoke(pluginModel->paramChangedFromUI);
    repaint();
}


static int dontMoveRight[] = {
    17, 21, 24, 28,  //F0 to F1 below F-clef
    31, 35, 38, 41, 45, //G1 to A2 on F-clef
    48, 52, 55, 59, 62, //C3 to D4 on G-clef
    65, 69, 72, 76, 79 //F4 to G5
};

static std::set<int> conflictLosers = {
    12, 16, 19, 23, 26, 29,
    33, 36, 40, 43, 47,
    50, 53, 57, 60, 64,
    67, 71, 74, 77, 81
};

/*
* When notes are placed close to neighbor notes (e.g. E and F), one or more notes need to move to the right.
* This method figures out how much to the right notes need to move (three places at most)
*/
static void resolveNeighborConflicts(std::set<int>& midiNotes, NoteDrawInfo* noteDrawInfos)
{
    std::map<int, int> noteCount;
    for (int midiNote : midiNotes)
    {
        /*
        * Here we mark the note count for each anchorNote (the actual graphical placement of the midi note).
        * For all double notes, move the second one of them, one place to the right.
        */
        NoteDrawInfo& noteDrawInfo = noteDrawInfos[midiNote];
        if (noteCount.find(noteDrawInfo.anchorNote) != noteCount.end())
        {
            noteCount[noteDrawInfo.anchorNote] = 2;
            noteDrawInfo.moveRight = 1;
        }
        else
        {
            noteCount[noteDrawInfo.anchorNote] = 1;
        }
    }
    for (int midiNote : midiNotes)
    {
        /*
        * We don't want to move notes that are over a staff line too much to the right,
        * other notes (conflictLosers) will by default move as much to the right as needed.
        */
        NoteDrawInfo& noteDrawInfo = noteDrawInfos[midiNote];
        int cur = noteDrawInfo.anchorNote;
        if (conflictLosers.find(cur) != conflictLosers.end())
        {
            int belowNeighbor = -1;
            int aboveNeighbor = -1;
            if (MidiMessage::getMidiNoteName(cur, true, false, 4) == "E" || MidiMessage::getMidiNoteName(cur, true, false, 4) == "B")
            {
                belowNeighbor = cur - 2;
                aboveNeighbor = cur + 1;
            }
            else if (MidiMessage::getMidiNoteName(cur, true, false, 4) == "F" || MidiMessage::getMidiNoteName(cur, true, false, 4) == "C")
            {
                belowNeighbor = cur - 1;
                aboveNeighbor = cur + 2;
            }
            else
            {
                belowNeighbor = cur - 2;
                aboveNeighbor = cur + 2;
            }
            int neighborCount = 0;
            if (noteCount.find(belowNeighbor) != noteCount.end())
            {
                neighborCount = noteCount[belowNeighbor];
            }
            if (noteCount.find(aboveNeighbor) != noteCount.end())
            {
                neighborCount = noteCount[aboveNeighbor] > neighborCount ? noteCount[aboveNeighbor] : neighborCount;
            }
            noteDrawInfo.moveRight += neighborCount;
        }
    }
}

void MainComponent::paint(Graphics& g)
{
    if (pluginModel->hasParamChanges)
        onParametersChanged();

    std::set<int> midiNotes;
    for (int i = 0; i < 127; i++)
    {
        if (pluginModel->midiNotes[i] == 1)
            midiNotes.insert(i);
    }

    /*
    std::set<int> midiNotes{
        54, 57, 60, 63
    };
    */

    Chord chord;
    chords.name(midiNotes, keys.getKey(keyMenu.getText()), chord);

    Rectangle<int> localBounds = getLocalBounds();
    int buttonHeight = getButtonHeight(localBounds);
    g.fillAll(pluginModel->darkMode ? darkModeBackgroundColour : Colours::white);
    g.setColour(pluginModel->darkMode ? darkModeForegroundColour : Colours::black);
    if (pluginModel->chordPlacement == 3)
    {
        float x = buttonHeight * 0.2;
        float y = 0;
        float textWidth = localBounds.getWidth() - buttonHeight * 0.4;
        float textHeight = localBounds.getHeight();
        //float y = localBounds.getHeight() * 0.5 - textHeight * 0.5;
        g.setFont(getCustomFont(pluginModel->chordFontBold));
        g.setFont(textHeight);
        g.drawText(chord.name(), x, y, textWidth, textHeight, Justification::centred);
        return;
    }

    StaffCalculator staffCalculator(localBounds);
    drawStaff(g, staffCalculator);
    drawKeySignature(g, staffCalculator);
        
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
    }

    resolveNeighborConflicts(midiNotes, noteDrawInfos);

    //Figure out flat/sharp indentation
    float lastY = -1;
    int lastAccentIndent = 0;
    std::set<int>::reverse_iterator rit;
    for (rit = midiNotes.rbegin(); rit != midiNotes.rend(); rit++)
    {
        // *rit to access value
        int midiNote = *rit;
        if (!noteDrawInfos[midiNote].sharp && !noteDrawInfos[midiNote].flat && !noteDrawInfos[midiNote].natural &&
            !noteDrawInfos[midiNote].doubleSharp && !noteDrawInfos[midiNote].doubleFlat)
            continue;

        //first note always indent 0, and update lastY
        if (lastY < 0) {
            lastY = noteDrawInfos[midiNote].y;
            continue;
        }

        float yDiff = noteDrawInfos[midiNote].y - lastY;
        float diffMultiplier = noteDrawInfos[midiNote].sharp || noteDrawInfos[midiNote].doubleFlat ? 3 : 2;
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
    //Done finding out where to put the notes and accents

    float baseNoteX = staffCalculator.x + (staffCalculator.staffHeight / 2) + (staffCalculator.staffHeight / 4) + staffCalculator.noteWidth * 5;
    int lineX = baseNoteX - staffCalculator.lineThickness;
    bool firstNote = true;

    for (int midiNote : midiNotes)
    {
        if (firstNote)
        {
            if (pluginModel->chordPlacement > 0)
            {
                float textWidth = pluginModel->chordPlacement == 2 ?
                    localBounds.getWidth() - baseNoteX - staffCalculator.noteWidth * 3 :
                    localBounds.getWidth() - localBounds.getWidth() * 0.05 - staffCalculator.lineThickness * 2;
                float textHeight = pluginModel->chordPlacement == 2 ?
                    textWidth / 7.5f :
                    localBounds.getHeight() * 0.1;
                float chordX;
                float chordY;
                if (pluginModel->chordPlacement == 2)
                {
                    chordX = staffCalculator.staffHeight * 1.66;
                    chordY = noteDrawInfos[midiNote].y - textHeight * 0.5 + staffCalculator.noteHeight * 0.5;
                }
                else if (pluginModel->chordPlacement == 1) {
                    chordX = buttonHeight * 4.0;//localBounds.getHeight() * 0.1 + staffCalculator.lineThickness * 10;
                    chordY = localBounds.getHeight() - textHeight;
                }
                g.setFont(getCustomFont(pluginModel->chordFontBold));
                g.setFont(textHeight);
                g.drawText(chord.name(), chordX, chordY, textWidth, textHeight, Justification::centredLeft);
            }
            firstNote = false;
        }

        float noteX = baseNoteX + staffCalculator.noteWidth * noteDrawInfos[midiNote].moveRight - staffCalculator.lineThickness * noteDrawInfos[midiNote].moveRight * 2;
        float accentX = baseNoteX - (staffCalculator.noteWidth) - (noteDrawInfos[midiNote].accentIndent * staffCalculator.noteWidth * 0.75);
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
        else if (noteDrawInfos[midiNote].doubleSharp)
        {
            doubleSharpSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y, staffCalculator.noteWidth, staffCalculator.noteHeight), juce::RectanglePlacement::Flags::xLeft, 1.0);
        }
        else if (noteDrawInfos[midiNote].doubleFlat)
        {
            doubleFlatSvg->drawWithin(g, Rectangle<float>(accentX, noteDrawInfos[midiNote].y - staffCalculator.noteHeight * 1.25, staffCalculator.noteWidth, staffCalculator.noteHeight * 2.5f), juce::RectanglePlacement::Flags::xLeft, 1.0);
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
    NullCheckedInvocation::invoke(pluginModel->paramChangedFromUI);
    repaint();
}