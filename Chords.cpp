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
		//intervals
		ChordPattern("100000100000", "tritone", Tritonic),
		ChordPattern("100000010000", "5", Major),
		//major
		ChordPattern("100010010000", "", Major),
		ChordPattern("100000010010", "57", Major),
		ChordPattern("100010100000", "(b5)", Major),
		ChordPattern("100010100010", "7(b5)", Major),
		ChordPattern("101010100010", "9(b5)", Major),
		ChordPattern("100010010100", "6", Major),
		ChordPattern("100010011000", "(b6)", Major),
		ChordPattern("101010010100", "69", Major),
		ChordPattern("100010010010", "7", Major),
		ChordPattern("100010010110", "7/6", Major),
		ChordPattern("101010010010", "9", Major),
		ChordPattern("110010100010", "7(b5,b9)", Major),
		ChordPattern("101011010010", "11", Major),
		ChordPattern("101010110010", "#11", Major),
		ChordPattern("100010110010", "7(#11)", Major),
		ChordPattern("100010011010", "7(b13)", Major),
		ChordPattern("101011010110", "13", Major),
		ChordPattern("100011010110", "13(no9)", Major),
		ChordPattern("110010010010", "7(b9)", Major),
		ChordPattern("100110010010", "7(#9)", Major),
		ChordPattern("100110110010", "7(#9,#11)", Major),
		ChordPattern("110011010010", "11(b9)", Major),
		ChordPattern("110011010110", "13(b9)", Major),
		ChordPattern("100010010001", "maj7", Major),
		ChordPattern("100010100001", "maj7(b5)", Major),
		ChordPattern("101010010001", "maj9", Major),
		ChordPattern("101010110001", "maj9(#11)", Major),
		ChordPattern("100110010001", "maj7(#9)", Major),
		ChordPattern("110010010001", "maj7(b9)", Major),
		ChordPattern("100010110001", "maj7(#11)", Major),
		ChordPattern("100110110001", "maj7(#9,#11)", Major),
		ChordPattern("101011010001", "maj11", Major),
		ChordPattern("101011010101", "maj13", Major),
		ChordPattern("101010010000", "add9", Major),
		ChordPattern("100110010000", "add#9", Major),
		ChordPattern("110010010000", "addb9", Major),
		ChordPattern("100011010000", "add11", Major),
		ChordPattern("100010110000", "add#11", Major),
		//minor
		ChordPattern("100100010000", "m", Minor),
		ChordPattern("100100001000", "m(#5)", Minor),
		ChordPattern("100100001010", "m7(#5)", Minor),
		ChordPattern("100100010100", "m6", Minor),
		ChordPattern("100100011000", "m(b6)", Minor),
		ChordPattern("101100010100", "m69", Minor),
		ChordPattern("100100010010", "m7", Minor),
		ChordPattern("110100010010", "m7(b9)", Minor),
		ChordPattern("100100110010", "m7(#11)", Minor),
		ChordPattern("101100010010", "m9", Minor),
		ChordPattern("101101010010", "m11", Minor),
		ChordPattern("101101010110", "m13", Minor),
		ChordPattern("110101010010", "m11(b9)", Minor),
		ChordPattern("100100010001", "m(maj7)", Minor),
		ChordPattern("101100010001", "m(maj9)", Minor),
		ChordPattern("101101010001", "m(maj11)", Minor),
		ChordPattern("101101010101", "m(maj13)", Minor),
		ChordPattern("110100010001", "m(maj7)(b9)", Minor),
		ChordPattern("101100010000", "m(add9)", Minor),
		ChordPattern("100101010000", "m(add11)", Minor),
		//augmented
		ChordPattern("100010001000", "aug", Aug),
		ChordPattern("100010001010", "aug7", Aug),
		ChordPattern("101010001010", "aug9", Aug),
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
		ChordPattern("100011000110", "13(no59)", Major),
		ChordPattern("110010000010", "7(b9)(no5)", Major),
		ChordPattern("100110000010", "7(#9)(no5)", Major),
		ChordPattern("100110100010", "7(#9,#11)(no5)", Major),
		ChordPattern("110011000010", "11(b9)(no5)", Major),
		ChordPattern("110011000110", "13(b9)(no5)", Major),
		ChordPattern("100010000001", "maj7(no5)", Major),
		ChordPattern("101010000001", "maj9(no5)", Major),
		ChordPattern("101010100001", "maj9(#11)(no5)", Major),
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
		//suspended 2nd (no 5)
		ChordPattern("101000000000", "sus2(no5)", Sus),
		ChordPattern("101000000010", "7sus2(no5)", Sus),
		ChordPattern("101000000110", "7/6sus2(no5)", Sus),
		ChordPattern("101000000001", "maj7sus2(no5)", Sus),
		//suspended 2nd (no 5)
		ChordPattern("100001000000", "sus4(no5)", Sus),
		ChordPattern("100001000010", "7sus4(no5)", Sus),
		ChordPattern("100001000110", "7/6sus4(no5)", Sus),
		ChordPattern("100001001010", "7sus4(b13)(no5)", Sus),
		ChordPattern("100001000001", "maj7sus4(no5)", Sus),
		ChordPattern("101001000010", "9sus4(no5)", Sus),
		//suspended 2nd and 4th (no 5)
		ChordPattern("101001000000", "sus24(no5)", Sus),
		ChordPattern("101001000001", "maj7sus24(no5)", Sus),
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

