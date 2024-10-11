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

	if (chordType == Dim && noteNameSharp.length() == 2)
	{
		//Dims with accented root note are always sharp, otherwise they would need to be full of double flats
		return noteNameSharp;
	}
	else if (chordType == Major && (noteNameSharp == "D#" || noteNameSharp == "A#"))
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

	addKey("Sharp", std::list<String>({ "C","D","E","F","G","A","B" }), 0, 0);
	addKey("Flat", std::list<String>({ "C","D","E","F","G","A","B" }), 0, 0);
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

static String getNoteName(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo)
{
	if (chord.pattern.chordType == Dim)
	{
		String curName = MidiMessage::getMidiNoteName(midiNote, true, false, 4);
		if (curName == chord.rootNote)
			//root names are sharp if accented
			return curName;
		else
			//other notes are flat (except in two specially handled cases)
			return MidiMessage::getMidiNoteName(midiNote, false, false, 4);
	}

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

void Keys::applyDimAnchorNoteAndAccents(int midiNote, String& noteName, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo)
{
	//special handling here - all accented notes except the root note will be flat
	if (noteName == chord.rootNote && noteName.length() == 2)
	{
		noteDrawInfo.anchorNote = midiNote - 1;
		if (!key.hasNote(noteName))
			noteDrawInfo.sharp = true;
	}
	else
	{
		if (noteName.length() == 2 && noteName != "E#" && noteName != "B#")
		{
			//we don't care if the previous algorithm decided sharp or flat - it's always flat when not the root note
			//except in two cases :)
			if ((chord.rootNote == "D#" && (noteName == "F#" || noteName == "Gb")) ||
				(chord.rootNote == "A#" && (noteName == "C#" || noteName == "Db")))
			{
				noteDrawInfo.anchorNote = midiNote - 1;
				if (chord.rootNote == "D#")
					noteName = "F#";
				else if (chord.rootNote == "A#")
					noteName = "C#";
				if (!key.hasNote(noteName))
					noteDrawInfo.sharp = true;
			}
			else
			{
				noteDrawInfo.anchorNote = midiNote + 1;
				if (!key.hasNote(noteName))
					noteDrawInfo.flat = true;
				else
				{
					noteDrawInfo.flat = false;
				}
			}
		}
		else if ((noteName == "D" && chord.rootNote == "F") || (noteName == "A" && chord.rootNote == "C"))
		{
			//two very special cases have a double flat
			noteDrawInfo.anchorNote = midiNote + 2;
			noteDrawInfo.doubleFlat = true;
		}
		else if ((noteName == "B" && chord.rootNote == "F") || (noteName == "B" && chord.rootNote == "D") || (noteName == "E" && chord.rootNote == "G"))
		{
			noteDrawInfo.anchorNote = midiNote + 1;
			noteDrawInfo.flat = true;
		}
		else
		{
			applyAnchorNoteAndAccentsUsingChordKey(midiNote, key, chord, noteDrawInfo);
		}
	}
}

void Keys::applyAnchorNoteAndAccents(int midiNote, Key& key, Chord& chord, NoteDrawInfo& noteDrawInfo)
{
	String noteName = getNoteName(midiNote, key, chord, noteDrawInfo);
	if ((noteName == "G" && chord.rootNote == "G#") || (noteName == "D" && chord.rootNote == "D#") || (noteName == "A" && chord.rootNote == "A#"))
	{
		//special double sharp cases
		noteDrawInfo.anchorNote = midiNote - 2;
		noteDrawInfo.doubleSharp = true;
		return;
	}

	if (chord.pattern.chordType == Dim)
	{
		applyDimAnchorNoteAndAccents(midiNote, noteName, key, chord, noteDrawInfo);
		return;
	}

	if (chord.isFlat5(midiNote) || chord.isFlat9(midiNote) || chord.isFlat11(midiNote) || chord.isFlat13(midiNote))
	{
		if (noteName == "B#")
			noteName = "C";
		else if (noteName == "E#")
			noteName = "F";

		//we want all of them flat or natural
		if (noteName.length() == 2)
		{
			noteName = MidiMessage::getMidiNoteName(midiNote, false, false, 4);
			noteDrawInfo.anchorNote = midiNote + 1;
			if (!key.hasNote(noteName))
			{
				noteDrawInfo.flat = true;
			}			
		}
		else
		{
			noteDrawInfo.anchorNote = midiNote;
			if (!key.hasNote(noteName))
			{
				noteDrawInfo.natural = true;
				noteDrawInfo.sharp = false;//it's set to true in the E# and B# cases
			}
		}
		return;
	}

	if (chord.isSharp5(midiNote) || chord.isSharp9(midiNote) || chord.isSharp11(midiNote))
	{
		if (noteName == "Cb")
			noteName = "B";
		else if (noteName == "Fb")
			noteName = "E";

		//we want all of them sharp or natural
		if (noteName.length() == 2)
		{
			noteName = MidiMessage::getMidiNoteName(midiNote, true, false, 4);
			noteDrawInfo.anchorNote = midiNote - 1;
			if (!key.hasNote(noteName))
			{
				noteDrawInfo.sharp = true;
			}
		}
		else
		{
			noteDrawInfo.anchorNote = midiNote;
			if (!key.hasNote(noteName))
			{
				noteDrawInfo.natural = true;
				noteDrawInfo.flat = false;//it's set to true in the Cb and Fb cases
			}
		}
		return;
	}

	if (Chords::chordHasSuperPower(chord))
	{
		applyAnchorNoteAndAccentsUsingChordKey(midiNote, key, chord, noteDrawInfo);
		return;
	}

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
	Key chordKey = chord.isMajor3rd() ? getKey(chord.rootNote) : getKey(minorToMajor[chord.rootNote]);
	String noteName = MidiMessage::getMidiNoteName(midiNote, chordKey.numSharps > 0, false, 4);
	if (noteName.length() == 1)
	{
		noteDrawInfo.anchorNote = midiNote;
		if (!key.hasNote(noteName))
		{
			noteDrawInfo.natural = true;
			noteDrawInfo.sharp = false;
			noteDrawInfo.flat = false;
		}
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