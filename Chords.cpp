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
#include <random>

Chord::Chord(ChordPattern pattern, String rootNote, String bassNote)
{
	this->pattern = pattern;
	this->rootNote = rootNote;
	this->bassNote = bassNote;
};

String Chord::name()
{
	String chordName = rootNote;
	chordName += pattern.name;
	if (bassNote.length() > 0)
	{
		chordName += "/";
		chordName += bassNote;
	}
	return chordName;
}

Chords::Chords()
{
	std::vector<ChordPattern> patternList
	{
		//intervals
		ChordPattern("100000100000", "tritone", Tritonic),
		ChordPattern("100000010000", "5", Major),
		//major
		ChordPattern("100010010000", "", Major),
		ChordPattern("100010010010", "7", Major),
		ChordPattern("100000010010", "7(no3)", Major),
		ChordPattern("101010010010", "9", Major),
		ChordPattern("110010010010", "7(b9)", Major),
		ChordPattern("100110010010", "7(#9)", Major),
		ChordPattern("101011010010", "11", Major),
		ChordPattern("110011010010", "11(b9)", Major),
		ChordPattern("100111010010", "11(#9)", Major),
		ChordPattern("101010110010", "(#11)", Major),
		ChordPattern("100011010010", "7(add11)", Major),
		ChordPattern("100010110010", "7(add#11)", Major),
		ChordPattern("110010110010", "7(b9,#11)", Major),
		ChordPattern("100110110010", "7(#9,#11)", Major),
		ChordPattern("101011010110", "13", Major),
		ChordPattern("100010011010", "7(addb13)", Major),
		ChordPattern("110011010110", "13(b9)", Major),
		ChordPattern("110010110110", "13(b9,#11)", Major),
		ChordPattern("100111010110", "13(#9)", Major),
		ChordPattern("100110110110", "13(#9,#11)", Major),
		ChordPattern("101010110110", "13(#11)", Major),
		ChordPattern("100011010110", "13(no9)", Major),
		ChordPattern("101011011010", "(b13)", Major),
		ChordPattern("100010010001", "maj7", Major),
		ChordPattern("100000010001", "maj7(no3)", Major),
		ChordPattern("100010100001", "maj7(b5)", Major),
		ChordPattern("101010010001", "maj9", Major),
		ChordPattern("101010100001", "maj9(b5)", Major),
		ChordPattern("100110010001", "maj7(#9)", Major),
		ChordPattern("100110110001", "maj7(#9,#11)", Major),
		ChordPattern("110010010001", "maj7(b9)", Major),
		ChordPattern("110010110001", "maj7(b9,#11)", Major),
		ChordPattern("100010110001", "maj7(#11)", Major),
		ChordPattern("101010110001", "maj9(#11)", Major),
		ChordPattern("101011010001", "maj11", Major),
		ChordPattern("101011010101", "maj13", Major),
		ChordPattern("101010010000", "add9", Major),
		ChordPattern("100110010000", "add#9", Major),
		ChordPattern("110010010000", "addb9", Major),
		ChordPattern("100011010000", "add11", Major),
		ChordPattern("100010110000", "add#11", Major),
		ChordPattern("100010100000", "(b5)", Major),
		ChordPattern("100010100010", "7(b5)", Major),
		ChordPattern("101010100010", "9(b5)", Major),
		ChordPattern("110010100010", "7(b5,b9)", Major),
		ChordPattern("100110100010", "7(b5,#9)", Major),
		ChordPattern("100010010100", "6", Major),
		ChordPattern("100010010110", "7/6", Major),
		ChordPattern("100010010101", "maj7(6)", Major),
		ChordPattern("101010010100", "6/9", Major),
		ChordPattern("110010010100", "6(b9)", Major),
		ChordPattern("110011010100", "6(b9,11)", Major),
		ChordPattern("110010110100", "6(b9,#11)", Major),
		ChordPattern("100110010100", "6(#9)", Major),
		ChordPattern("100111010100", "6(#9,11)", Major),
		ChordPattern("100110110100", "6(#9,#11)", Major),
		ChordPattern("100011010100", "6(add11)", Major),
		ChordPattern("100010110100", "6(add#11)", Major),
		ChordPattern("100010011100", "6(addb13)", Major),
		ChordPattern("100010011000", "(b6)", Major),
		ChordPattern("110010011000", "(b6,b9)", Major),
		ChordPattern("100110011000", "(b6,#9)", Major),
		ChordPattern("110011011000", "(b6,b9,11)", Major),
		ChordPattern("110010111000", "(b6,b9,#11)", Major),
		//minor
		ChordPattern("100100010000", "m", Minor),
		ChordPattern("100100001000", "m(#5)", Minor),
		ChordPattern("100100001010", "m7(#5)", Minor),
		ChordPattern("100100010100", "m6", Minor),
		ChordPattern("100100011000", "m(b6)", Minor),
		ChordPattern("101100010100", "m6/9", Minor),
		ChordPattern("100100010010", "m7", Minor),
		ChordPattern("110100010010", "m7(b9)", Minor),
		ChordPattern("110110010010", "m7(b9,b11)", Minor),
		ChordPattern("110100110010", "m7(b9,#11)", Minor),
		ChordPattern("100100110010", "m7(add#11)", Minor),
		ChordPattern("100100011010", "m7(addb13)", Minor),
		ChordPattern("101100010010", "m9", Minor),
		ChordPattern("101110010010", "m9(b11)", Minor),
		ChordPattern("101100110010", "m9(#11)", Minor),
		ChordPattern("101101010010", "m11", Minor),
		ChordPattern("110101010010", "m11(b9)", Minor),
		ChordPattern("101101011010", "m11(b13)", Minor),
		ChordPattern("101101010110", "m13", Minor),
		ChordPattern("100100010001", "m(maj7)", Minor),
		ChordPattern("101100010001", "m(maj9)", Minor),
		ChordPattern("110100010001", "m(maj7)(b9)", Minor),
		ChordPattern("101101010001", "m(maj11)", Minor),
		ChordPattern("101101010101", "m(maj13)", Minor),
		ChordPattern("101100010000", "m(add9)", Minor),
		ChordPattern("100101010000", "m(add11)", Minor),
		//augmented
		ChordPattern("100010001000", "aug", Aug),
		ChordPattern("100010001010", "aug7", Aug),
		ChordPattern("100000001010", "aug7(no3)", Aug),
		ChordPattern("101010001010", "aug9", Aug),
		ChordPattern("101000001010", "aug9(no3)", Aug),
		ChordPattern("110010001010", "aug7(b9)", Aug),
		ChordPattern("100110001010", "aug7(#9)", Aug),
		ChordPattern("100010001001", "aug(maj7)", Aug),
		ChordPattern("101010001001", "aug(maj9)", Aug),
		ChordPattern("110010001001", "aug(maj7)(b9)", Aug),
		ChordPattern("100110001001", "aug(maj7)(#9)", Aug),
		//diminished
		ChordPattern("100100100000", "dim", Dim),
		ChordPattern("100100100010", "m7(b5)", Dim),
		ChordPattern("110100100010", "m7(b5,b9)", Dim),
		ChordPattern("101100100010", "m9(b5)", Dim),
		ChordPattern("100100100001", "m(maj7)(b5)", Dim),
		ChordPattern("110101100010", "m11(b5,b9)", Dim),
		ChordPattern("100100100100", "dim7", Dim),
		ChordPattern("101100100100", "dim9", Dim),
		ChordPattern("110100100100", "dim7(b9)", Dim),
		//suspended 2nd
		ChordPattern("101000010000", "sus2", Sus),
		ChordPattern("101000010010", "7sus2", Sus),
		ChordPattern("101000010110", "7/6sus2", Sus),
		ChordPattern("101000010001", "maj7sus2", Sus),
		ChordPattern("101000100000", "sus2(b5)", Sus),
		//suspended 4th
		ChordPattern("100001010000", "sus4", Sus),
		ChordPattern("100001010010", "7sus4", Sus),
		ChordPattern("100001010110", "7/6sus4", Sus),
		ChordPattern("100001011010", "7sus4(b13)", Sus),
		ChordPattern("100001010001", "maj7sus4", Sus),
		ChordPattern("101001010010", "9sus4", Sus),
		//suspended 2nd and 4th
		ChordPattern("101001010000", "sus24", Sus),
		ChordPattern("101001010001", "maj7sus24", Sus),
		//major (no5)
		ChordPattern("100010000000", "(no5)", Major),
		ChordPattern("100010000100", "6(no5)", Major),
		ChordPattern("101010000100", "69(no5)", Major),
		ChordPattern("100010000010", "7(no5)", Major),
		ChordPattern("100010000110", "7/6(no5)", Major),
		ChordPattern("101010000010", "9(no5)", Major),
		ChordPattern("101011000010", "11(no5)", Major),
		ChordPattern("101011000110", "13(no5)", Major),
		ChordPattern("101010100110", "13(#11)(no5)", Major),
		ChordPattern("100011000110", "13(no5,9)", Major),
		ChordPattern("110010000010", "7(b9)(no5)", Major),
		ChordPattern("100110000010", "7(#9)(no5)", Major),
		ChordPattern("110011000010", "11(b9)(no5)", Major),
		ChordPattern("110011000110", "13(b9)(no5)", Major),
		ChordPattern("100010000001", "maj7(no5)", Major),
		ChordPattern("101010000001", "maj9(no5)", Major),
		ChordPattern("100110000001", "maj7(#9)(no5)", Major),
		ChordPattern("110010000001", "maj7(b9)(no5)", Major),
		ChordPattern("100110100001", "maj7(#9,#11)(no5)", Major),
		ChordPattern("101011000001", "maj11(no5)", Major),
		ChordPattern("101011000101", "maj13(no5)", Major),
		ChordPattern("101010000000", "add9(no5)", Major),
		ChordPattern("100110000000", "add#9(no5)", Major),
		ChordPattern("110010000000", "addb9(no5)", Major),
		//minor (no5)
		ChordPattern("100100000000", "m(no5)", Minor),
		ChordPattern("100100000100", "m6(no5)", Minor),
		ChordPattern("101100000100", "m69(no5)", Minor),
		ChordPattern("100100000010", "m7(no5)", Minor),
		ChordPattern("110100000010", "m7(b9)(no5)", Minor),
		ChordPattern("101100000010", "m9(no5)", Minor),
		ChordPattern("101101000010", "m11(no5)", Minor),
		ChordPattern("101101000110", "m13(no5)", Minor),
		ChordPattern("110101000010", "m11(b9)(no5)", Minor),
		ChordPattern("100100000001", "m(maj7)(no5)", Minor),
		ChordPattern("101100000001", "m(maj9)(no5)", Minor),
		ChordPattern("101101000001", "m(maj11)(no5)", Minor),
		ChordPattern("101101000101", "m(maj13)(no5)", Minor),
		ChordPattern("110100000001", "m(maj7)(b9)(no5)", Minor),
		ChordPattern("101100000000", "m(add9)(no5)", Minor),
		ChordPattern("100101000000", "m(add11)(no5)", Minor),
		//suspended 2nd (no5
		ChordPattern("101000000000", "sus2(no5)", Sus),
		ChordPattern("101000000010", "7sus2(no5)", Sus),
		ChordPattern("101000000110", "7/6sus2(no5)", Sus),
		ChordPattern("101000000001", "maj7sus2(no5)", Sus),
		//suspended 4th (no5)
		ChordPattern("100001000000", "sus4(no5)", Sus),
		ChordPattern("100001000010", "7sus4(no5)", Sus),
		ChordPattern("100001000110", "7/6sus4(no5)", Sus),
		ChordPattern("100001001010", "7sus4(b13)(no5)", Sus),
		ChordPattern("100001000001", "maj7sus4(no5)", Sus),
		ChordPattern("101001000010", "9sus4(no5)", Sus),
		//suspended 2nd and 4th (no5)
		ChordPattern("101001000000", "sus24(no5)", Sus),
		ChordPattern("101001000001", "maj7sus24(no5)", Sus),
		//EasterEgg
		ChordPattern("111111111111", "-Easter-Egg", EasterEgg)
	};

	for (ChordPattern pattern : patternList)
	{
		this->patterns.insert({ pattern.pattern, pattern });
	}
}

static bool chordIndexSet(String& string, int idx)
{
	String subStr = string.substring(idx, idx + 1);
	return subStr == "1";
}

void ChordPattern::init()
{
	if (chordIndexSet(pattern, 1))
		flat9 = true;
	if (chordType != Tritonic && chordIndexSet(pattern, 6))
		flat5 = true;
	if (name.contains("b11"))
		flat11 = true;
	if (name.contains("b13"))
		flat13 = true;
	if (chordType == Aug)
		sharp5 = true;
	if (chordType != Minor && chordType != Dim && chordIndexSet(pattern, 3))
		sharp9 = true;
	if (name.contains("#11"))
	{
		sharp11 = true;
		flat5 = false;
	}
}

static std::vector<String> easterEggs =
{
	" Madness!",
	" You think this is a good idea?",
	" Someone forgot their meds",
	" Is the cat lying on the keyboard?",
	" Take it easy!",
	" Don't push me that hard!",
	" Maybe try the trumpet...",
	" Seriously?",
	" How many hands?"
};

static 	std::random_device rand_dev;
static std::mt19937 generator(rand_dev());
static const int range_from = 0;
static const int range_to = easterEggs.size() - 1;
static std::uniform_int_distribution<int>  distr(range_from, range_to);

static String randomEasterEgg()
{
	return easterEggs[distr(generator)];
}

static ChordType lastChordType = None;
static String lastEasterEggName = "";

void Chords::name(std::set<int>& midiNotes, Key& key, Chord& chord)
{
	if (midiNotes.size() < 2)
	{
		lastChordType = None;
		return;
	}

	String sharpSymbol = MidiMessage::getMidiNoteName(1, true, false, 3).substring(1);
	String flatSymbol = MidiMessage::getMidiNoteName(1, false, false, 3).substring(1);

	int bassNote = *(midiNotes.begin()) % 12;
	int highestNote = *(midiNotes.rbegin()) % 12;

	bool notes[36] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	notes[bassNote] = true;

	for (int midiNote : midiNotes)
	{
		int condensedNote = midiNote % 12;
		if (condensedNote < bassNote)
			condensedNote += 12;
		notes[condensedNote] = true;
	}

	int curBassNote = -1;
	String chordName = "";
	for (int outerIdx = 0; outerIdx < 36; outerIdx++)
	{
		int innerIdx = 0;
		for (; innerIdx < 36; innerIdx++)
		{
			if (notes[innerIdx])
			{
				break;
			}
		}
		//innerIdx is the lowest note in the chord
		curBassNote = innerIdx;
		String chordMatcher = "";
		for (; innerIdx < curBassNote + 12; innerIdx++)
		{
			chordMatcher += notes[innerIdx] ? "1" : "0";
		}
		//loop over truths to see if current matches
		for (std::map<String, ChordPattern>::iterator it = patterns.begin(); it != patterns.end(); ++it)
		{
			if (it->first == chordMatcher)
			{
				chord.pattern = it->second;
				if (key.name == Key::SHARP)
					chord.rootNote = MidiMessage::getMidiNoteName(curBassNote, true, false, 3);
				else if(key.name == Key::FLAT)
					chord.rootNote = MidiMessage::getMidiNoteName(curBassNote, false, false, 3);
				else
				{

					ChordPattern chordPattern = it->second;
					chord.rootNote = key.selectChordBaseNoteName(curBassNote, chordPattern.chordType);
					//TODO, add midi notes to Chord once that's part of the signature
				}
				break;
			}
		}
		if (chord.rootNote == "" && midiNotes.size() > 2) //we don't want to find bass notes for 2 note chords
		{
			//raise the cur bass note by 12 and try again
			notes[curBassNote] = false;
			if(curBassNote + 12 < 36)
				notes[curBassNote + 12] = true;
			else
			{
				return;
			}
		}
		else
			break;
	}
	if (chord.pattern.pattern.length() == 0)
		return;

	if (chord.pattern.chordType == EasterEgg)
	{
		if (lastChordType != EasterEgg)
		{
			lastEasterEggName = randomEasterEgg();
			
		}
		chord.pattern.name = lastEasterEggName;		
	}
	
	lastChordType = chord.pattern.chordType;

	//see if the bass note is not the same as the chord base note
	if (bassNote == curBassNote)
		return;

	if (key.name == Key::SHARP)
		chord.bassNote = MidiMessage::getMidiNoteName(bassNote, true, false, 3);
	else if (key.name == Key::FLAT)
		chord.bassNote = MidiMessage::getMidiNoteName(bassNote, false, false, 3);
	else
	{
		//Some chords decide their own bass note name, others respect the key we're in
		if (chordHasSuperPower(chord))
		{
			bool sharp = isChordSharp(chord);
			chord.bassNote = MidiMessage::getMidiNoteName(bassNote, sharp, false, 4);
		}
		else
		{
			chord.bassNote = MidiMessage::getMidiNoteName(bassNote, key.numSharps > 0, false, 4);
		}
	}
}

static std::map<String, int> noteNameToMidiNote
{
	{ "C", 0 },
	{ "C#", 1 },
	{ "Db", 1 },
	{ "D", 2 },
	{ "D#", 3 },
	{ "Eb", 3 },
	{ "E", 4 },
	{ "F", 5 },
	{ "F#", 6 },
	{ "Gb", 6 },
	{ "G", 7 },
	{ "G#", 8 },
	{ "Ab", 8 },
	{ "A", 9 },
	{ "A#", 10 },
	{ "Bb", 10 },
	{ "B", 11}
};

static bool isSpecialFlatOrSharp(Chord& chord, bool testVar, int midiNote, int distanceFromC)
{
	if (!testVar)
		return false;

	int rootMidiNote = noteNameToMidiNote[chord.rootNote];
	int remains = midiNote % 12;
	return remains == rootMidiNote + distanceFromC;
}

bool Chord::isFlat5(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.flat5, midiNote, 6);
}

bool Chord::isFlat9(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.flat9, midiNote, 1);
}

bool Chord::isFlat11(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.flat11, midiNote, 4);
}

bool Chord::isFlat13(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.flat13, midiNote, 8);
}

bool Chord::isSharp5(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.sharp5, midiNote, 8);
}

bool Chord::isSharp9(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.sharp9, midiNote, 3);
}

bool Chord::isSharp11(int midiNote)
{
	return isSpecialFlatOrSharp(*this, pattern.sharp11, midiNote, 6);
}