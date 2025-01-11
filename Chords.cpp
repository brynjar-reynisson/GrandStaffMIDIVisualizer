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

String Chord::name(bool shortName)
{
	if (pattern.chordType == EasterEgg)
		return pattern.name;

	String chordName = rootNote;
	chordName += shortName ? pattern.shortName : pattern.name;
	if (bassNote.length() > 0)
	{
		chordName += "/";
		chordName += bassNote;
	}
	return chordName;
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
				if (key.name == Key::SHARPS)
					chord.rootNote = MidiMessage::getMidiNoteName(curBassNote, true, false, 3);
				else if(key.name == Key::FLATS)
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

	if (key.name == Key::SHARPS)
		chord.bassNote = MidiMessage::getMidiNoteName(bassNote, true, false, 3);
	else if (key.name == Key::FLATS)
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