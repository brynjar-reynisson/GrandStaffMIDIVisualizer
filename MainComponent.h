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

const static int CMD_MSG_MIDI_CHANGES = 10001;
const static int CMD_MSG_VST_PARAM_CHANGES = 10002;
const static int CMD_MSG_RESIZE = 10002;

static Colour darkModeForegroundColour(236, 236, 236);
static Colour darkModeBackgroundColour(36, 33, 33);
static Colour darkModeSelectedBackgroundColour(95, 95, 95);

static Typeface::Ptr customFontTypeface = Typeface::createSystemTypefaceFor(BinaryData::consola_ttf, BinaryData::consola_ttfSize);
static Font plainCustomFont = Font(customFontTypeface).withTypefaceStyle("Plain");
static Font boldCustomFont = Font(Typeface::createSystemTypefaceFor(BinaryData::consola_ttf, BinaryData::consola_ttfSize)).withTypefaceStyle("Bold");


static int floatToInt(float value)
{
    return static_cast<int>(round(value));
};



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
    ChordPattern()
    {
        pattern = "", name = "", shortName = "", chordType = None;
    }

    ChordPattern(String pattern, String name, ChordType chordType)
    {
        this->pattern = pattern;
        this->name = name;
        this->shortName = name;
        this->chordType = chordType;
        init();
    }

    ChordPattern(String pattern, String name, String shortName, ChordType chordType)
    {
        this->pattern = pattern;
        this->name = name;
        this->shortName = shortName;
        this->chordType = chordType;
        init();
    }

    void init();

    String pattern;
    String name;
    String shortName;
    ChordType chordType;
    bool flat5 = false;
    bool flat9 = false;
    bool flat11 = false;
    bool flat13 = false;
    bool sharp5 = false;
    bool sharp9 = false;
    bool sharp11 = false;
};

static ChordPattern nullPattern = ChordPattern();

//==============================================================================
struct Chord
{
    Chord(ChordPattern pattern = nullPattern, String rootNote = "", String bassNote = "");
    String name(bool shortName = false);

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
    bool isFlat5(int midiNote);
    bool isFlat9(int midiNote);
    bool isFlat11(int midiNote);
    bool isFlat13(int midiNote);
    bool isSharp5(int midiNote);
    bool isSharp9(int midiNote);
    bool isSharp11(int midiNote);
};

static Chord nullChord = Chord();

//==============================================================================
struct NoteDrawInfo
{
    float y = -500;
    float x = -500;
    bool sharp = false;
    bool flat = false;
    bool natural = false;
    bool doubleSharp = false;
    bool doubleFlat = false;
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

    inline static const String SHARPS = "Sharps";
    inline static const String FLATS = "Flats";
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
    void applyDimAnchorNoteAndAccents(int midiNote, String& noteName, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo);
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
        else if (chord.pattern.chordType == Minor && (chord.rootNote == "C" || chord.rootNote == "D" || chord.rootNote == "E" || chord.rootNote == "G" ||
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
    bool shortNotation = false;
    int chordPlacement = 1;
    bool chordFontBold = false;
    bool darkMode = false;
    bool hasParamChanges = false;
    bool hasUIChanges = false;
    int uiWidth = -1;
    int uiHeight = -1;

    void resetMidiNotes()
    {
        for (int i = 0; i < 127; i++)
            midiNotes[i] = 0;
    }
    std::function<void()> paramChangedFromUI;
    std::function<void()> paramChangedFromHost;

    CriticalSection criticalSection;
};

//==============================================================================
class StaffCalculator
{
public:
    StaffCalculator(Rectangle<int> bounds);
    void noteYPlacement(int midiNote, NoteDrawInfo& noteDrawInfo, bool sharp=false, bool applySharpFlat=true, int transpose=0);
    
    float buttonHeight;
    float buttonSpace;

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
class CustomDrawableButton : public DrawableButton
{
public:
    CustomDrawableButton(const String& buttonName,
        ButtonStyle buttonStyle) : DrawableButton(buttonName, buttonStyle)
    {
    }

    /*
    * This method allows sizing and placing the images any way one wants to inside the button bounds
    */
    virtual Rectangle<float> getImageBounds() const override
    {
        int width = getBounds().getWidth();
        int height = getBounds().getHeight();
        return Rectangle<float>(width * 0.33, height * 0.33, width * 0.33, height * 0.33);
    }

private:
};
//==============================================================================

static float fontHeightMultiplier = 0.7;
static LookAndFeel* defaultLookAndFeel = nullptr;

class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
    }

    void setLightModeLookAndFeel();
    void setDarkModeLookAndFeel();

    Font getTextButtonFont(TextButton& textButton, int buttonHeight);
    Font getLabelFont(Label& l);
    Font getComboBoxFont(ComboBox& c);
    Font getPopupMenuFont();
    Typeface::Ptr getTypefaceForFont(const Font& f) override;

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider);
    void drawComboBox(Graphics& g, int width, int height, bool,
        int, int, int, int, ComboBox& box);
    int getSliderPopupPlacement(Slider& slider)
    {
        return BubbleComponent::below;
    }

    static Font getCustomFont()
    {
        plainCustomFont.setTypefaceName("Consolas");
        plainCustomFont.setTypefaceStyle("Plain");
        return plainCustomFont;
    }

private:
    const std::unique_ptr<Drawable> lmKeySelectorArrowDownSvg = Drawable::createFromImageData(BinaryData::KeySelectorArrowDown_svg, BinaryData::KeySelectorArrowDown_svgSize);
    const std::unique_ptr<Drawable> dmKeySelectorArrowDownSvg = Drawable::createFromImageData(BinaryData::KeySelectorArrowDownDarkMode_svg, BinaryData::KeySelectorArrowDownDarkMode_svgSize);
    bool darkMode = false;
};
//==============================================================================
class OctaveSlider : public Slider
{
    String getTextFromValue(double value)
    {
        String plusModifier = value > 0 ? "+" : "";
        return "Octave " + plusModifier + std::to_string((int)value);
    }
};
//==============================================================================
class FadeOut final {
public:
    FadeOut(Component* owner, PluginModel* model) :
        updater(owner)
    {        
        this->owner = owner;
        this->model = model;
    }

    void init();
    void start();
    void stop();
    Colour getColour();
    bool isRunning()
    {
        return running;
    }

    std::function<void()> onStopped;
private:
    std::function<float(float)> createFadeOutIndexes();
    void valueChangedCallback(float value);

    Component* owner;
    PluginModel* model;
    bool running = false;

    int colourIndex = 0;
    VBlankAnimatorUpdater updater;

    Animator animator = juce::ValueAnimatorBuilder{}
        .withEasing(createFadeOutIndexes())
        .withDurationMs(1000)
        .withValueChangedCallback([this](auto value) {
            valueChangedCallback(value);
        })
        .build();
};
//==============================================================================
class MainComponent final : public Component, public Button::Listener, public SliderListener<Slider> {
public:
    MainComponent(PluginModel* model) :
        chordFadeOut(this, model),
        holdNoteButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground),
        chordPlacementButton("", DrawableButton::ButtonStyle::ImageOnButtonBackground)
    {
        chordFadeOut.init();
        init(model);
    }
    ~MainComponent()
    {
        pluginModel->paramChangedFromHost = nullptr;
        keyMenu.setLookAndFeel(nullptr);
        notationButton.setLookAndFeel(nullptr);
        chordFontBoldButton.setLookAndFeel(nullptr);
        darkModeButton.setLookAndFeel(nullptr);
        chordPlacementButton.setLookAndFeel(nullptr);
        holdNoteButton.setLookAndFeel(nullptr);
        octaveSlider.setLookAndFeel(nullptr);
    }
    void paint(Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void updateChordPlacementButton();
    void updateColourScheme();
    void onParametersChanged();
    void onMidiChanged();

private:
    void init(PluginModel* model);
    void keyMenuChanged();
    void drawStaff(Graphics& g, StaffCalculator& staffCalculator);
    void drawKeySignature(Graphics& g, StaffCalculator& staffCalculator);
    void drawSharps(Graphics& g, StaffCalculator& staffCalculator, int numSharps);
    void drawFlats(Graphics& g, StaffCalculator& staffCalculator, int numFlats);
    void drawText(Graphics& g, String text, float x, float y, float width, float height, bool left = true);
    void onChordFadeOutStopped();


    const std::unique_ptr<Drawable> lmStaffSvg = Drawable::createFromImageData(BinaryData::Grand_staff_02_svg, BinaryData::Grand_staff_02_svgSize);
    const std::unique_ptr<Drawable> lmNoteSvg = Drawable::createFromImageData(BinaryData::Whole_note_svg, BinaryData::Whole_note_svgSize);
    const std::unique_ptr<Drawable> lmIncrementSvg = Drawable::createFromImageData(BinaryData::Increment_Symbol_svg, BinaryData::Increment_Symbol_svgSize);
    const std::unique_ptr<Drawable> lmSharpSvg = Drawable::createFromImageData(BinaryData::Sharp_svg, BinaryData::Sharp_svgSize);
    const std::unique_ptr<Drawable> lmFlatSvg = Drawable::createFromImageData(BinaryData::Flat_svg, BinaryData::Flat_svgSize);
    const std::unique_ptr<Drawable> lmNaturalSvg = Drawable::createFromImageData(BinaryData::Natural_svg, BinaryData::Natural_svgSize);
    const std::unique_ptr<Drawable> lmDoubleSharpSvg = Drawable::createFromImageData(BinaryData::DoubleSharp_svg, BinaryData::DoubleSharp_svgSize);
    const std::unique_ptr<Drawable> lmDoubleFlatSvg = Drawable::createFromImageData(BinaryData::DoubleFlat_svg, BinaryData::DoubleFlat_svgSize);
    const std::unique_ptr<Drawable> lmArrowDownSvg = Drawable::createFromImageData(BinaryData::ArrowDown_svg, BinaryData::ArrowDown_svgSize);
    const std::unique_ptr<Drawable> lmArrowRightSvg = Drawable::createFromImageData(BinaryData::ArrowRight_svg, BinaryData::ArrowRight_svgSize);
    const std::unique_ptr<Drawable> lmArrowUpSvg = Drawable::createFromImageData(BinaryData::ArrowUp_svg, BinaryData::ArrowUp_svgSize);

    const std::unique_ptr<Drawable> dmStaffSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Grand_Staff_svg, BinaryData::Dark_Mode_Grand_Staff_svgSize);
    const std::unique_ptr<Drawable> dmNoteSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Whole_Note_svg, BinaryData::Dark_Mode_Whole_Note_svgSize);
    const std::unique_ptr<Drawable> dmIncrementSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Increment_Symbol_svg, BinaryData::Dark_Mode_Increment_Symbol_svgSize);
    const std::unique_ptr<Drawable> dmSharpSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Sharp_svg, BinaryData::Dark_Mode_Sharp_svgSize);
    const std::unique_ptr<Drawable> dmFlatSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Flat_svg, BinaryData::Dark_Mode_Flat_svgSize);
    const std::unique_ptr<Drawable> dmNaturalSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Natural_svg, BinaryData::Dark_Mode_Natural_svgSize);
    const std::unique_ptr<Drawable> dmDoubleSharpSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Double_Sharp_svg, BinaryData::Dark_Mode_Double_Sharp_svgSize);
    const std::unique_ptr<Drawable> dmDoubleFlatSvg = Drawable::createFromImageData(BinaryData::Dark_Mode_Double_Flat_svg, BinaryData::Dark_Mode_Double_Flat_svgSize);
    const std::unique_ptr<Drawable> dmArrowDownSvg = Drawable::createFromImageData(BinaryData::ArrowDownDarkMode_svg, BinaryData::ArrowDownDarkMode_svgSize);
    const std::unique_ptr<Drawable> dmArrowRightSvg = Drawable::createFromImageData(BinaryData::ArrowRightDarkMode_svg, BinaryData::ArrowRightDarkMode_svgSize);
    const std::unique_ptr<Drawable> dmArrowUpSvg = Drawable::createFromImageData(BinaryData::ArrowUpDarkMode_svg, BinaryData::ArrowUpDarkMode_svgSize);

    const std::unique_ptr<Drawable> blankSvg = Drawable::createFromImageData(BinaryData::Blank_svg, BinaryData::Blank_svgSize);

    Drawable* staffSvg = nullptr;
    Drawable* noteSvg = nullptr;
    Drawable* incrementSvg = nullptr;
    Drawable* arrowDownSvg = nullptr;
    Drawable* arrowRightSvg = nullptr;
    Drawable* arrowUpSvg = nullptr;
    Drawable* sharpSvg = nullptr;
    Drawable* flatSvg = nullptr;
    Drawable* naturalSvg = nullptr;
    Drawable* doubleSharpSvg = nullptr;
    Drawable* doubleFlatSvg = nullptr;

    ComboBox keyMenu;
    TextButton notationButton;
    CustomDrawableButton chordPlacementButton;
    TextButton chordFontBoldButton;
    TextButton darkModeButton;
    DrawableButton holdNoteButton;
    OctaveSlider octaveSlider;

    PluginModel* pluginModel;
    Keys keys;
    Chords chords;
    std::set<int> midiNotes;
    Chord chord;

    TooltipWindow tooltipWindow{ this }; // instance required for ToolTips to work
    CustomLookAndFeel darkLookAndFeel;
    CustomLookAndFeel lightLookAndFeel;
    CustomLookAndFeel customFontLookAndFeel;

    FadeOut chordFadeOut;
};

static int getButtonHeight(Rectangle<int> bounds)
{
    int minButtonHeight = 13;
    int calculatedButtonHeight = bounds.getHeight() * 0.05;
    return std::max((int)calculatedButtonHeight, minButtonHeight);
}