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

Chord::Chord(ChordPattern pattern, String baseNote, String bassNote)
{
	this->pattern = pattern;
	this->baseNote = baseNote;
	this->bassNote = bassNote;
};

String Chord::name()
{
	String chordName = baseNote;
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

		//2 notes
		ChordPattern("100001000000", "4", Sus),
		ChordPattern("100000100000", "-tritone", Tritonic),
		ChordPattern("100000010000", "5", Major),
		//3 notes
		ChordPattern("100010010000", "", Major),
		ChordPattern("100100010000", "m", Minor),
		ChordPattern("101000010000", "sus2", Sus),
		ChordPattern("101000100000", "sus2b5", Sus),
		ChordPattern("101100000000", "m-sus2", Minor),
		ChordPattern("100001010000", "sus4", Sus),
		ChordPattern("100100100000", "dim", Dim),
		ChordPattern("100010100000", "major b5", Major),
		ChordPattern("100010001000", "+ (aug)", Aug),
		ChordPattern("100000010010", "57", Major),
		//4 notes
		ChordPattern("100010010100", "6", Major),
		ChordPattern("100100010100", "m6", Minor),
		ChordPattern("100010010001", "maj7", Major),
		ChordPattern("100010010010", "7", Major),
		ChordPattern("100100010001", "m maj7", Minor),
		ChordPattern("100100010010", "m7", Minor),
		ChordPattern("100100100100", "dim7", Dim),
		ChordPattern("101010000010", "7add9", Major),
		ChordPattern("100010100001", "maj7b9", Major),
		ChordPattern("100010100010", "7b5", Major),
		ChordPattern("100001010010", "7sus4", Major),
		ChordPattern("101000010010", "7sus2", Major),
		ChordPattern("101000010001", "maj7sus2", Major),
		ChordPattern("100001010001", "maj7sus4", Major),
		ChordPattern("100100100001", "maj7b5", Major),
		ChordPattern("100010001001", "+maj7", Aug),
		ChordPattern("100010001010", "+7", Aug),
		ChordPattern("100100100010", "m7dim5", Minor),
		ChordPattern("101010010000", "9", Major),
		ChordPattern("100110010000", "maj#9", Major),
		ChordPattern("110010010000", "majb5", Major),
		ChordPattern("101100010000", "m9", Minor),
		ChordPattern("100011010000", "11", Major),
		ChordPattern("100010110000", "maj#11", Major),
		ChordPattern("100101010000", "m11", Minor),
		ChordPattern("101001010000", "sus24", Major),
		//5 notes
		ChordPattern("101010010010", "7add9", Major),
		ChordPattern("101010010001", "maj7add9", Major),
		ChordPattern("100110010001", "maj7#9", Major),
		ChordPattern("101100010010", "m7add9", Minor),
		ChordPattern("110100010010", "m7b9", Minor),
		ChordPattern("110010100010", "7b5b9", Major),
		ChordPattern("101100010001", "m maj7 add 9", Minor),
		ChordPattern("100110010010", "7#9", Major),
		ChordPattern("110010010010", "7b9", Major),
		ChordPattern("110010010001", "maj7b9", Major),
		ChordPattern("110100010001", "m maj7b9", Minor),
		ChordPattern("110010001001", "+ maj7b9", Aug),
		ChordPattern("110010001010", "+7b9", Aug),
		ChordPattern("100110001010", "7#5#9", Major),
		ChordPattern("101010001010", "+7add9", Aug),
		ChordPattern("101100100010", "dim7add9", Dim),
		ChordPattern("110100100010", "dim7b9", Dim),
		ChordPattern("101100100100", "dim9", Dim),
		ChordPattern("110100100100", "dimb9", Dim),
		ChordPattern("101010010100", "6add9", Major),
		ChordPattern("101001010001", "maj7sus24", Major),
		ChordPattern("101001010010", "7sus24", Major),
		ChordPattern("100010010110", "7/6", Major),
		ChordPattern("100010110010", "7#11", Major),
		ChordPattern("100010011010", "7b11", Major),
		ChordPattern("100010110001", "maj7#11", Major),
		ChordPattern("100100110010", "m7#11", Minor),
		ChordPattern("100001011010", "7sus b13", Major),
		ChordPattern("101010001001", "+7add9", Aug),
		ChordPattern("100110001001", "+7#9", Aug),
		//6 notes
		ChordPattern("101011010001", "11", Major),
		ChordPattern("101101010010", "m11", Minor),
		ChordPattern("101101010001", "m maj11", Minor),
		ChordPattern("101010010110", "7/6sus2", Major),
		ChordPattern("100110110001", "maj7#9#11", Major),
		ChordPattern("110101100010", "m11b5b9", Minor),
		ChordPattern("110101010010", "m11b9", Minor),
		ChordPattern("101011010010", "7add11", Major),
		//7 notes
		ChordPattern("101011010110", "7/6sus4", Major),
		//ChordPattern("101011010101", "maj7add13"),
		ChordPattern("101101010110", "m7 add 13", Minor),
		ChordPattern("101101010101", "m maj13", Minor)
	};

	for (ChordPattern pattern : patternList)
	{
		this->patterns.insert({ pattern.pattern, pattern });
	}
}

void Chords::name(std::set<int>& midiNotes, Key& key, Chord& chord)
{
	if (midiNotes.size() < 2)
		return;

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
					chord.baseNote = MidiMessage::getMidiNoteName(curBassNote, true, false, 3);
				else if(key.name == Key::FLAT)
					chord.baseNote = MidiMessage::getMidiNoteName(curBassNote, false, false, 3);
				else
				{

					ChordPattern chordPattern = it->second;
					chord.baseNote = key.selectChordBaseNoteName(curBassNote, chordPattern.chordType);
					//TODO, add midi notes to Chord once that's part of the signature
				}
				break;
			}
		}
		if (chord.baseNote == "")
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

