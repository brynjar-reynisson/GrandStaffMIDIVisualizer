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

#pragma once

#include <JuceHeader.h>
//==============================================================================
enum ChordType
{
    None,
    Major,
    Sus,
    Aug,
    Tritonic,
    Minor,
    Dim,
    EasterEgg
};
//==============================================================================
struct ChordPattern
{
    ChordPattern(String pattern = "", String name = "", ChordType chordType = None)
    {
        this->pattern = pattern;
        this->name = name;
        this->chordType = chordType;
    }
    String pattern;
    String name;
    ChordType chordType;
};

static ChordPattern nullPattern = ChordPattern();

//==============================================================================
struct Chord
{
    Chord(ChordPattern pattern = nullPattern, String rootNote = "", String bassNote = "");
    String name();

    ChordPattern pattern;
    String rootNote;
    String bassNote;

    bool isMajor3rd() const
    {
        return pattern.chordType == Major || pattern.chordType == Sus || pattern.chordType == Aug;
    };

    bool isMinor3rd() const
    {
        return pattern.chordType == Minor || pattern.chordType == Dim;
    };
};
//==============================================================================
struct NoteDrawInfo
{
    float y = -500;
    float x = -500;
    bool sharp = false;
    bool flat = false;
    bool natural = false;
    int accentIndent = 0;
    int lineCount = 0;
    float linePositions[5];
    int midiNote = -1;
    int anchorNote = -1; //where the note will be drawn, e.g. F# on F, Ab on A
    int moveRight = 0;
};
//==============================================================================
struct Key
{
    String name;
    std::list<String> notes;
    int numSharps = 0;
    int numFlats = 0;
    bool hasNote(String& noteName);
    String selectChordBaseNoteName(int midiNote, ChordType chordType);

    inline static const String SHARP = "Sharp";
    inline static const String FLAT = "Flat";
};
//==============================================================================
class Keys
{
public:
    Keys();
    std::vector<String> getKeyNames();
    int getKeyIndex(String name);
    Key getKey(String name);
    String selectChordName(String keyName, String sharpChordName, String flatChordName);
    void applyAnchorNoteAndAccents(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo);

private:
    void addKey(String name, std::list<String>& notes, int numSharps, int numFlats);
    void applyAnchorNoteAndAccentsUsingChordKey(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo);
    std::map<String, Key> keys;
    std::vector<String> keyNames;
};
//==============================================================================
class Chords
{
public:
    Chords();

    /*
    * The chord recognition algorithm is based on the Chordback JSFX script written by Paul Heams
    * He can be contacted on X (Twitter) as @17BitRetro
    */
    void name(std::set<int>& midiNotes, Key& key, Chord& chord);

    static bool chordHasSuperPower(Chord& chord)
    {
        if (chord.isMajor3rd() && (chord.rootNote == "Bb" || chord.rootNote == "Eb" || chord.rootNote == "C" || chord.rootNote == "D" ||
            chord.rootNote == "E" || chord.rootNote == "F" || chord.rootNote == "G" || chord.rootNote == "A" || chord.rootNote == "B"))
            return true;
        else if (chord.isMinor3rd() && (chord.rootNote == "C" || chord.rootNote == "D" || chord.rootNote == "E" || chord.rootNote == "G" ||
            chord.rootNote == "A" || chord.rootNote == "B"))
            return true;
        return false;
    }

    static bool isChordSharp(Chord& chord)
    {
        if (chord.isMajor3rd() && (chord.rootNote == "Eb" || chord.rootNote == "Bb" || chord.rootNote == "F" || chord.rootNote == "C"))
            return false;
        else if (chord.isMinor3rd() && (chord.rootNote == "C" || chord.rootNote == "G" || chord.rootNote == "D" || chord.rootNote == "A"))
            return false;
        return true;
    }

private:
    std::map<String, ChordPattern> patterns;
};
//==============================================================================
class PluginModel
{
public:
    int midiNotes[127];
    int transposeOctaves = 0;
    int keyId = 0;
    bool holdNotes = false;
    int chordPlacement = 1;
    bool chordFontBold = false;
    bool hasParamChanges = false;
    bool hasUIChanges = false;
    int uiWidth = -1;
    int uiHeight = -1;

    std::function<void()> paramChangedFromUI;
    std::function<void()> paramChangedFromHost;
};

//==============================================================================
class StaffCalculator
{
public:
    StaffCalculator(Rectangle<int> bounds);
    void noteYPlacement(int midiNote, NoteDrawInfo& noteDrawInfo, bool sharp=false, bool applySharpFlat=true, int transpose=0);
    
    float noteMultiplyConstant;
    float noteWidth;
    float noteHeight;
    float lineThickness;
    float g1Anchor;
    float a2Anchor;
    float e3Anchor;
    float f4Anchor;
    
    float x;
    float y;
    float width;
    float height;

    float staffYIncrement;
    float staffHeight;

    float g5;
    float e5;
    float c5;
    float a4;

    float c3;

    float e1;
    float c1;
    float a0;
    float f0;

    float gLine1;
    float gLine2;
    float gLine3;
    float gLine4;
    float c3Line;
    float fLine1;
    float fLine2;
    float fLine3;
    float fLine4;

    float sharpFontSize;
    float flatFontSize;

    float sharpsX[14];//two of each - F# C# G# D# A# E# B#
    float sharpsY[14];
    float flatsX[14];//Bb Eb Ab Db Gb Cb Fb
    float flatsY[14];

private:
};
//==============================================================================
class MainComponent final : public Component, public Button::Listener {
public:
    MainComponent(PluginModel* model) :
        holdNoteButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground)
    {
        init(model);
    }
    ~MainComponent()
    {
        pluginModel->paramChangedFromHost = nullptr;
    }
    void paint(Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void onParametersChanged();
    void updateChordPlacementButton();

private:
    void init(PluginModel* model);
    void keyMenuChanged();
    //void noteYPlacement(StaffCalculator& staffCalculator, String& chordName, NoteDrawInfo& noteDrawInfo, int midiNote);
    void drawStaff(Graphics& g, StaffCalculator& staffCalculator);
    void drawKeySignature(Graphics& g, StaffCalculator& staffCalculator);
    void drawSharps(Graphics& g, StaffCalculator& staffCalculator, int numSharps);
    void drawFlats(Graphics& g, StaffCalculator& staffCalculator, int numFlats);

    const std::unique_ptr<Drawable> staffSvg = Drawable::createFromImageData(BinaryData::Grand_staff_02_svg, BinaryData::Grand_staff_02_svgSize);
    const std::unique_ptr<Drawable> noteSvg = Drawable::createFromImageData(BinaryData::Whole_note_svg, BinaryData::Whole_note_svgSize);
    const std::unique_ptr<Drawable> sharpSvg = Drawable::createFromImageData(BinaryData::Sharp_svg, BinaryData::Sharp_svgSize);
    const std::unique_ptr<Drawable> flatSvg = Drawable::createFromImageData(BinaryData::Flat_svg, BinaryData::Flat_svgSize);
    const std::unique_ptr<Drawable> naturalSvg = Drawable::createFromImageData(BinaryData::Natural_svg, BinaryData::Natural_svgSize);
    ComboBox keyMenu;
    DrawableButton holdNoteButton;
    TextButton leftArrowButton;
    Label octaveLabel;
    TextButton rightArrowButton;
    TextButton chordPlacementButton;
    TextButton chordFontBoldButton;

    PluginModel* pluginModel;
    Keys keys;
    Chords chords;

    TooltipWindow tooltipWindow{ this }; // instance required for ToolTips to work
};