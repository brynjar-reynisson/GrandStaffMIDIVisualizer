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

std::vector<String> keyNames;

bool Key::hasNote(String& noteName)
{
	for (String curNote : notes)
	{
		if (curNote == noteName)
			return true;	
	}
	return false;
}

String Key::selectChordBaseNoteName(int midiNote, ChordType chordType)
{
	String noteNameSharp = MidiMessage::getMidiNoteName(midiNote, true, false, 4);
	String noteNameFlat = MidiMessage::getMidiNoteName(midiNote, false, false, 4);

	if (chordType == Major && (noteNameSharp == "D#" || noteNameSharp == "A#"))
	{
		//D# loses because of F## and A# loses because of C##
		return noteNameFlat;
	}
	else if ((chordType == Minor || chordType == Dim) && noteNameFlat == "Gb")
	{
		//Gbm and Gbdim lose because of Bbb
		return noteNameSharp;
	}

	if (numSharps == 0)
	{
		return noteNameFlat;
	}
	else {
		return noteNameSharp;
	}
}

Keys::Keys()
{
	addKey("Sharp", std::list<String>({ "C","D","E","F","G","A","B" }), 0, 0);
	addKey("Flat", std::list<String>({ "C","D","E","F","G","A","B" }), 0, 0);
	addKey("C", std::list<String>({ "C","D","E","F","G","A","B" }), 0, 0);
	addKey("C#", std::list<String>({ "C#","D#","E#","F#","G#","A#","B#" }), 7, 0);
	addKey("Db", std::list<String>({ "Db","Eb","F", "Gb","Ab","Bb","C" }), 0, 5);
	addKey("D", std::list<String>({ "D","E","F#","G","A","B","C#"}), 2, 0);
	//D# not supported, has two double sharps
	addKey("Eb", std::list<String>({ "Eb","F", "G","Ab","Bb","C","D"}), 0, 3);
	addKey("E", std::list<String>({ "E","F#","G#","A","B","C#","D#"}), 4, 0);
	addKey("F", std::list<String>({ "F","G","A","Bb","C","D","E" }), 0, 1);
	addKey("F#", std::list<String>({ "F#","G#","A#","B","C#","D#","E#" }), 6, 0);
	addKey("Gb", std::list<String>({ "Gb","Ab","Bb","Cb","Db","Eb","F" }), 0, 6);
	addKey("G", std::list<String>({ "G","A","B","C","D","E","F#" }), 1, 0);
	//G# not supported, has a double sharp
	addKey("Ab", std::list<String>({ "Ab","Bb","C","Db","Eb","F","G"}), 0, 4);
	addKey("A", std::list<String>({ "A","B","C#","D","E","F#","G#"}), 3, 0);
	//A# not supported, 3 double sharps
	addKey("Bb", std::list<String>({ "Bb","C","D","Eb","F","G","A"}), 0, 2);
	addKey("B", std::list<String>({ "B","C#","D#","E","F#","G#","A#"}), 5, 0);		
}

void Keys::addKey(String name, std::list<String>& notes, int numSharps, int numFlats)
{
	Key key;
	key.name = name;
	key.notes = notes;
	key.numSharps = numSharps;
	key.numFlats = numFlats;
	keys[name] = key;
	keyNames.push_back(name);
};

int Keys::getKeyIndex(String name)
{
	for (int idx = 0; idx < keyNames.size(); idx++)
	{
		if (keyNames.at(idx) == name)
			return idx;
	}
	return -1;
}

std::vector<String> Keys::getKeyNames()
{
	return keyNames;
}

Key Keys::getKey(String name)
{
	return keys[name];
}

String Keys::selectChordName(String keyName, String sharpChordName, String flatChordName)
{
	//TODO, make intelligent
	return flatChordName;
}

static String getNoteName(int midiNote, Key& key, NoteDrawInfo& noteDrawInfo)
{
	String noteName = MidiMessage::getMidiNoteName(midiNote, key.numSharps > 0, false, 4);
	if ((key.numSharps > 0 && noteName == "F") || (key.numSharps > 1 && noteName == "C"))
	{
		noteDrawInfo.anchorNote = midiNote - 1;
		if (noteName == "F")
		{
			noteName = "E#";
			if (key.numSharps < 6)  //F# has 6, including E#
				noteDrawInfo.sharp = true;
		}
		else if (noteName == "C")
		{
			noteName = "B#";
			if (key.numSharps < 7) //C# has 7, including B#
				noteDrawInfo.sharp = true;
		}
	}
	else if ((key.numFlats > 0 && noteName == "B") || (key.numFlats > 1 && noteName == "E"))
	{
		//Bb is the first flat, and Eb the second one. We should not see E or B natural outside of these cases, thus this flattening
		noteDrawInfo.anchorNote = midiNote + 1;
		noteDrawInfo.flat = true;
		if (noteName == "B")
			noteName = "Cb";
		else if (noteName == "E")
			noteName = "Fb";
	}
	return noteName;
}

void Keys::applyAnchorNoteAndAccents(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo)
{
	if (chordHasSuperPower(chord))
	{
		applyAnchorNoteAndAccentsUsingChordKey(midiNote, key, chord, noteDrawInfo);
		return;
	}

	String noteName = getNoteName(midiNote, key, noteDrawInfo);

	//E#, B#, Cb and Fb get special treatment in getNoteName because they don't end up on the original natural place
	if (noteName == "E#" || noteName == "B#" || noteName == "Cb" || noteName == "Fb")
		return;

	//Change anchor note if we have sharp or flat
	if (noteName.length() == 2)
	{
		bool applyAccent = !key.hasNote(noteName);
		if (noteName.containsChar('#'))
		{
			noteDrawInfo.anchorNote = midiNote - 1;
			if (applyAccent)
				noteDrawInfo.sharp = true;
		}
		else if (noteName.containsChar('b'))
		{
			noteDrawInfo.anchorNote = midiNote + 1;
			if (applyAccent)
				noteDrawInfo.flat = true;
		}
	}
	else
		noteDrawInfo.anchorNote = midiNote;

	if (noteName.length() == 1 && !key.hasNote(noteName))
		noteDrawInfo.natural = true;
}

static std::map<String, String> minorToMajor = {
	{ "C", "Eb" },
	{ "D", "F" },
	{ "E", "G" },
	{ "G", "Bb" },
	{ "A", "C" },
	{ "B", "D" }
};

void Keys::applyAnchorNoteAndAccentsUsingChordKey(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo)
{
	Key chordKey = chord.isMajor3rd() ? getKey(chord.baseNote) : getKey(minorToMajor[chord.baseNote]);
	String noteName = MidiMessage::getMidiNoteName(midiNote, chordKey.numSharps > 0, false, 4);
	if (noteName.length() == 1)
	{
		noteDrawInfo.anchorNote = midiNote;
		if (!key.hasNote(noteName))
			noteDrawInfo.natural = true;
	}
	else if (noteName.endsWithChar('#'))
	{
		noteDrawInfo.anchorNote = midiNote - 1;
		if (!key.hasNote(noteName))
			noteDrawInfo.sharp = true;
	}
	else if (noteName.endsWithChar('b'))
	{
		noteDrawInfo.anchorNote = midiNote + 1;
		if (!key.hasNote(noteName))
			noteDrawInfo.flat = true;
	}

}