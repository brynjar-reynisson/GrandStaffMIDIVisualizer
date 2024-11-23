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
    return Font(Typeface::createSystemTypefaceFor(BinaryData::consola_ttf, BinaryData::consola_ttfSize)).withTypefaceStyle(bold ? "Bold" : "Plain");
}

void MainComponent::onParametersChanged()
{
    keyMenu.setSelectedId(pluginModel->keyId + 1, false);
    holdNoteButton.setToggleState(pluginModel->holdNotes, false);
    octaveLabel.setText(std::to_string(pluginModel->transposeOctaves), NotificationType::dontSendNotification);
    notationButton.setToggleState(pluginModel->shortNotation, false);
    updateChordPlacementButton();
    chordFontBoldButton.setToggleState(pluginModel->chordFontBold, false);
    updateColourScheme();
    pluginModel->hasParamChanges = false;
    repaint();
}

void MainComponent::onMidiChanged()
{
    ScopedLock lock(pluginModel->criticalSection);
    this->midiNotes.clear();
    for (int i=0; i<127; i++)
        if (pluginModel->midiNotes[i])
            this->midiNotes.insert(i);

    chord = nullChord;
    chords.name(this->midiNotes, keys.getKey(keyMenu.getText()), chord);
}

void MainComponent::init(PluginModel* model)
{
    this->pluginModel = model;
    pluginModel->paramChangedFromHost = [&] { onParametersChanged(); };

    lightLookAndFeel.setColour(PopupMenu::backgroundColourId, Colours::white);
    lightLookAndFeel.setColour(PopupMenu::headerTextColourId, Colours::black);
    lightLookAndFeel.setColour(PopupMenu::highlightedBackgroundColourId, Colours::lightgrey);
    lightLookAndFeel.setColour(PopupMenu::highlightedTextColourId, Colours::black);
    lightLookAndFeel.setColour(PopupMenu::textColourId, Colours::black);

    darkLookAndFeel.setColour(PopupMenu::backgroundColourId, darkModeBackgroundColour);
    darkLookAndFeel.setColour(PopupMenu::headerTextColourId, darkModeForegroundColour);
    darkLookAndFeel.setColour(PopupMenu::highlightedBackgroundColourId, darkModeSelectedBackgroundColour);
    darkLookAndFeel.setColour(PopupMenu::highlightedTextColourId, Colours::white);
    darkLookAndFeel.setColour(PopupMenu::textColourId, darkModeForegroundColour);

    holdNoteButton.addListener(this);
    holdNoteButton.setToggleable(true);
    holdNoteButton.setToggleState(pluginModel->holdNotes, false);
    holdNoteButton.setTooltip("Hold notes");

    int i = 1;
    for (String key : keys.getKeyNames())
    {
        keyMenu.addItem(key, i++);
    }
    keyMenu.setSelectedItemIndex(1);
    keyMenu.onChange = [this] { keyMenuChanged(); };

    leftArrowButton.addListener(this);
    leftArrowButton.setButtonText("<");
    leftArrowButton.setTooltip("Octave down");

    octaveLabel.setJustificationType(juce::Justification::centred);

    rightArrowButton.addListener(this);
    rightArrowButton.setButtonText(">");
    rightArrowButton.setTooltip("Octave up");

    notationButton.addListener(this);
    notationButton.setButtonText(L"∆");
    notationButton.setTooltip("Short notation");
    notationButton.setToggleable(true);
    notationButton.setToggleState(pluginModel->shortNotation, false);

    chordPlacementButton.addListener(this);

    chordFontBoldButton.addListener(this);
    chordFontBoldButton.setButtonText("B");
    chordFontBoldButton.setTooltip("Bold font");

    darkModeButton.addListener(this);
    darkModeButton.setToggleable(true);
    darkModeButton.setToggleState(pluginModel->darkMode, false);
    darkModeButton.setButtonText("D");
    darkModeButton.setTooltip("Dark mode");

    onParametersChanged();
    updateColourScheme();

    addAndMakeVisible(holdNoteButton);
    addAndMakeVisible(keyMenu);
    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(octaveLabel);
    addAndMakeVisible(notationButton);
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
        chordPlacementButton.setTooltip("Chord name in bottom");
    }
    else if (pluginModel->chordPlacement == 1)
    {
        chordPlacementButton.setButtonText(">");
        chordPlacementButton.setTooltip("Chord name to the right");
    }
    else if (pluginModel->chordPlacement == 2)
    {
        chordPlacementButton.setButtonText("^");
        chordPlacementButton.setTooltip("Only chord name");
    }
    else if (pluginModel->chordPlacement == 3)
    {
        chordPlacementButton.setButtonText(" ");
        chordPlacementButton.setTooltip("No chord name");
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
        octaveLabel.setColour(juce::Label::backgroundColourId, darkModeSelectedBackgroundColour);
        darkModeButton.setToggleState(true, false);

        keyMenu.setLookAndFeel(&darkLookAndFeel);
        keyMenu.setColour(juce::ComboBox::outlineColourId, darkModeForegroundColour);
        keyMenu.setColour(juce::ComboBox::focusedOutlineColourId, Colours::white);
        keyMenu.setColour(juce::ComboBox::textColourId, darkModeForegroundColour);
        keyMenu.setColour(juce::ComboBox::arrowColourId, darkModeForegroundColour);
        keyMenu.setColour(juce::ComboBox::backgroundColourId, darkModeBackgroundColour);
        keyMenu.setColour(juce::ComboBox::buttonColourId, darkModeBackgroundColour);
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
        darkModeButton.setToggleState(false, false);

        keyMenu.setLookAndFeel(&lightLookAndFeel);
        keyMenu.setColour(juce::ComboBox::outlineColourId, juce::Colours::black);
        keyMenu.setColour(juce::ComboBox::focusedOutlineColourId, juce::Colours::black);
        keyMenu.setColour(juce::ComboBox::textColourId, juce::Colours::black);
        keyMenu.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
        keyMenu.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
        keyMenu.setColour(juce::ComboBox::buttonColourId, juce::Colours::white);
    }
    holdNoteButton.setImages(noteSvg);

    Button* buttons[] = { &holdNoteButton, &leftArrowButton, &rightArrowButton, &notationButton, &chordPlacementButton, &chordFontBoldButton, &darkModeButton };
    for (Button* button : buttons)
    {
        if (pluginModel->darkMode)
        {
            button->setColour(TextButton::textColourOnId, darkModeForegroundColour);
            button->setColour(TextButton::textColourOffId, darkModeForegroundColour);
            button->setColour(TextButton::buttonColourId, darkModeBackgroundColour);
            button->setColour(TextButton::buttonOnColourId, darkModeSelectedBackgroundColour);
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

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    pluginModel->uiWidth = bounds.getWidth();
    pluginModel->uiHeight = bounds.getHeight();

    int buttonSize = getButtonHeight(bounds);
    int buttonSpace = buttonSize * 0.1;
    keyMenu.setBounds(buttonSpace * 2, buttonSpace * 2, buttonSize * 4, buttonSize);
    notationButton.setBounds(buttonSize * 4 + buttonSpace * 3, buttonSpace * 2, buttonSize * 1.1, buttonSize);
    chordPlacementButton.setBounds(buttonSize * 5 + buttonSpace * 5, buttonSpace * 2, buttonSize * 1.1, buttonSize);
    chordFontBoldButton.setBounds(buttonSize * 6 + buttonSpace * 7, buttonSpace * 2, buttonSize * 1.1, buttonSize);

    darkModeButton.setBounds(bounds.getWidth() - buttonSize * 6.6 - buttonSpace * 4, buttonSpace * 2, buttonSize * 1.1, buttonSize);
    holdNoteButton.setBounds(bounds.getWidth() - buttonSize * 5.5 - buttonSpace * 3, buttonSpace * 2, buttonSize * 1.1, buttonSize);
    leftArrowButton.setBounds(bounds.getWidth() - buttonSize * 4.4 - buttonSpace * 2, buttonSpace * 2, buttonSize * 1.1, buttonSize);
    octaveLabel.setBounds(bounds.getWidth() - buttonSize * 3.3 - buttonSpace * 2, buttonSpace * 2, buttonSize * 2.2, buttonSize);
    rightArrowButton.setBounds(bounds.getWidth() - buttonSize * 1.1 - buttonSpace * 2, buttonSpace * 2, buttonSize * 1.1, buttonSize);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    pluginModel->hasUIChanges = true;
    if (button == &holdNoteButton)
    {
        this->midiNotes.clear();
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
    else if (button == &notationButton)
    {
        pluginModel->shortNotation = !pluginModel->shortNotation;
        notationButton.setToggleState(pluginModel->shortNotation, false);
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

void MainComponent::drawText(Graphics& g, String text, float x, float y, float width, float height, bool left)
{
    AttributedString chordStr;
    chordStr.setText(text);
    chordStr.setColour(pluginModel->darkMode ? darkModeForegroundColour : Colours::black);
    chordStr.setFont(getCustomFont(pluginModel->chordFontBold).withHeight(height));
    chordStr.setWordWrap(juce::AttributedString::WordWrap::none);
    chordStr.setJustification(left ? juce::Justification::left : juce::Justification::centred);
    chordStr.draw(g, Rectangle<float>(x, y, width, height));
}

void MainComponent::paint(Graphics& g)
{
    if (pluginModel->hasParamChanges)
        onParametersChanged();

    /*
    std::set<int> midiNotes{
        54, 57, 60, 63
    };
    */    

    Rectangle<int> localBounds = getLocalBounds();
    int buttonHeight = getButtonHeight(localBounds);
    int buttonSpace = buttonHeight * 0.1;

    g.fillAll(pluginModel->darkMode ? darkModeBackgroundColour : Colours::white);
    g.setColour(pluginModel->darkMode ? darkModeForegroundColour : Colours::black);
    if (pluginModel->chordPlacement == 3)
    {
        float x = buttonSpace * 2;
        float y = buttonHeight + buttonSpace * 3;
        float textWidth = localBounds.getWidth() - buttonSpace * 2;
        float textHeight = localBounds.getHeight() - y - buttonSpace * 2;
        drawText(g, chord.name(pluginModel->shortNotation), x, y, textWidth, textHeight, false);
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

    float baseNoteX = staffCalculator.x + (staffCalculator.staffHeight / 2) + (staffCalculator.staffHeight / 4) + staffCalculator.noteWidth * 4;
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
                    localBounds.getWidth() - localBounds.getWidth() * 0.05 - buttonSpace * 8;
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
                    chordX = buttonSpace * 6;
                    chordY = localBounds.getHeight() - textHeight - buttonSpace * 4;
                }
                drawText(g, chord.name(pluginModel->shortNotation), chordX, chordY, textWidth, textHeight);
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
    staffSvg->drawWithin(g, Rectangle<float>(staffCalculator.x, staffCalculator.staffYIncrement, staffCalculator.width, staffCalculator.staffHeight), RectanglePlacement::Flags::xLeft | RectanglePlacement::Flags::yMid, 1.0);
    if (pluginModel->darkMode)
        g.setColour(darkModeBackgroundColour);
    else
        g.setColour(Colours::white);

    //draw over left line
    g.drawLine(staffCalculator.x, staffCalculator.staffYIncrement, staffCalculator.x, staffCalculator.staffYIncrement + staffCalculator.staffHeight, staffCalculator.lineThickness * 2);

    if (pluginModel->darkMode)
        g.setColour(darkModeForegroundColour);
    else
        g.setColour(Colours::black);
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

        x1 = staffCalculator.sharpsX[idx] + staffCalculator.buttonSpace * 2; x2 = staffCalculator.sharpsX[idx+1] + staffCalculator.buttonSpace * 2;
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

        x1 = staffCalculator.flatsX[idx] + staffCalculator.buttonSpace * 2; x2 = staffCalculator.flatsX[idx + 1] + staffCalculator.buttonSpace * 2;
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