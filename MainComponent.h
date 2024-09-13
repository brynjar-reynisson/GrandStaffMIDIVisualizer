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
struct Key
{
    String name;
    String notes[7];
};
//==============================================================================
class Keys
{

};

//==============================================================================
class PluginModel
{
public:
    int midiNotes[127];
    int transposeOctaves = 0;
    bool sharp = true;
    bool holdNotes = false;
    bool chordsOnRight = false;

    std::function<void()> onChange;
};
//==============================================================================
class Chords
{
public:
    /*
    * The chord recognition algorithm is based on the Chordback JSFX script written by Paul Heams
    * He can be contacted on X (Twitter) as @17BitRetro
    */
    static String name(std::set<int>& midiNotes, bool sharp, bool includeNoteNames);
};
//==============================================================================
struct NoteDrawInfo
{
    float y = -500;
    float x = -500;
    bool sharp = false;
    bool flat = false;
    int accentIndent = 0;
    int lineCount = 0;
    float linePositions[5];
    int anchorNote = -1; //where the note will be drawn, e.g. F# on F, Ab on A
    bool dontMoveRight = false;
    bool selectedToMoveRight = false;
    bool hasPlacementConflict = false;
    bool doubleNote = false; //true if two notes have the same anchorNote
};
//==============================================================================
class StaffCalculator
{
public:
    StaffCalculator(Rectangle<int> bounds);
    void noteYPlacement(int midiNote, NoteDrawInfo& noteDrawInfo, bool sharp=false, int transpose=0);
    
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
private:
};
//==============================================================================
class MainComponent final : public Component, public Button::Listener {
public:
    MainComponent(PluginModel* model) :
        sharpButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground),
        flatButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground),
        holdNoteButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground)
    {
        init(model);
    }
    void paint(Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void onParametersChanged();
    void updateChordPlacementButton();

private:
    void drawStaff(Graphics& g, StaffCalculator& staffCalculator);
    void init(PluginModel* model);

    const std::unique_ptr<Drawable> staffSvg = Drawable::createFromImageData(BinaryData::Grand_staff_DM_03_svg, BinaryData::Grand_staff_DM_03_svgSize);
    const std::unique_ptr<Drawable> noteSvg = Drawable::createFromImageData(BinaryData::Whole_note_svg, BinaryData::Whole_note_svgSize);
    const std::unique_ptr<Drawable> sharpSvg = Drawable::createFromImageData(BinaryData::Sharp_svg, BinaryData::Sharp_svgSize);
    const std::unique_ptr<Drawable> flatSvg = Drawable::createFromImageData(BinaryData::Flat_svg, BinaryData::Flat_svgSize);
    const std::unique_ptr<Drawable> naturalSvg = Drawable::createFromImageData(BinaryData::Natural_svg, BinaryData::Natural_svgSize);
    Label chordsLabel;
    DrawableButton sharpButton;
    DrawableButton flatButton;
    DrawableButton holdNoteButton;
    TextButton leftArrowButton;
    Label octaveLabel;
    TextButton rightArrowButton;
    TextButton chordPlacementButton;
    PluginModel* pluginModel;

    TooltipWindow tooltipWindow{ this }; // instance required for ToolTips to work
};