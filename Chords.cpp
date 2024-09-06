#include "MainComponent.h"

static std::map<String, String> truthMap =
{
	//2 notes
	{ "100001000000", "4" },
	{ "100000100000", "-tritone" },
	{ "100000010000", "5" },
	//3 notes
	{ "100010010000", "" },
	{ "100100010000", "m" },
	{ "101000010000", "sus2" },
	{ "101000100000", "sus2b5"},
	{ "101100000000", "m-sus2"},
	{ "100001010000", "sus4" },
	{ "100100100000", "dim" },
	{ "100010100000", "major b5"},
	{ "100010001000", "+ (aug)"},
	{ "100000010010", "57" },
	//4 notes
	{ "100010010100", "6" },
	{ "100100010100", "m6" },
	{ "100010010001", "maj7" },
	{ "100010010010", "7" },
	{ "100100010001", "m maj7" },
	{ "100100010010", "m7" },
	{ "100100100100", "dim7" },
	{ "101010000010", "7add9" },
	{ "100010100001", "maj7b9"},
	{ "100010100010", "7b5" },
	{ "100001010010", "7sus4" },
	{ "101000010010", "7sus2" },
	{ "101000010001", "maj7sus2" },
	{ "100001010001", "maj7sus4" },
	{ "100100100001", "maj7b5" },
	{ "100010001001", "+maj7" },
	{ "100010001010", "+7" },
	{ "100100100010", "m7dim5" },
	{ "101010010000", "9" },
	{ "100110010000", "maj#9"},
	{ "110010010000", "majb5" },
	{ "101100010000", "m9" },
	{ "100011010000", "11" },
	{ "100010110000", "maj#11" },
	{ "100101010000", "m11" },
	{ "101001010000", "sus24" },
	//5 notes
	{ "101010010010", "7add9" },
	{ "101010010001", "maj7add9" },
	{ "100110010001", "maj7#9"},
	{ "101100010010", "m7add9" },
	{ "110100010010", "m7b9"},
	{ "110010100010", "7b5b9" },
	{ "101100010001", "m maj7 add 9"},
	{ "100110010010", "7#9" },
	{ "110010010010", "7b9" },
	{ "110010010001", "maj7b9" },
	{ "110100010001", "m maj7b9"},
	{ "110010001001", "+ maj7b9" },
	{ "110010001010", "+7b9" },
	{ "100110001010", "7#5#9"},
	{ "101010001010", "+7add9" },
	{ "101100100010", "dim7add9" },
	{ "110100100010", "dim7b9" },
	{ "101100100100", "dim9" },
	{ "110100100100", "dimb9"},
	{ "101010010100", "6add9" },
	{ "101001010001", "maj7sus24" },
	{ "101001010010", "7sus24" },
	{ "100010010110", "7/6" },
	{ "100010110010", "7#11" },
	{ "100010011010", "7b11" },
	{ "100010110001", "maj7#11" },
	{ "100100110010", "m7#11" },
	{ "100001011010", "7sus b13" },
	{ "101010001001", "+7add9" },
	{ "100110001001", "+7#9" },
	//6 notes
	{ "101011010001", "11" },
	{ "101101010010", "m11" },
	{ "101101010001", "m maj11" },
	{ "101010010110", "7/6sus2" },
	{ "100110110001", "maj7#9#11" },
	{ "110101100010", "m11b5b9" },
	{ "110101010010", "m11b9" },
	{ "101011010010", "7add11" },
	//7 notes
	{ "101011010110", "7/6sus4" },
	//{ "101011010101", "maj7add13" },
	{ "101101010110", "m7 add 13" },
	{ "101101010101", "m maj13" }
};

String Chords::name(std::set<int>& midiNotes, bool sharp, bool includeNoteNames)
{
	if (midiNotes.size() < 2)
		return "";

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
		for (std::map<String, String>::iterator it = truthMap.begin(); it != truthMap.end(); ++it)
		{
			if (it->first == chordMatcher)
			{
				chordName += MidiMessage::getMidiNoteName(curBassNote, sharp, false, 3);
				chordName += it->second;
				break;
			}
		}
		if (chordName == "")
		{
			//raise the cur bass note by 12 and try again
			notes[curBassNote] = false;
			if(curBassNote + 12 < 36)
				notes[curBassNote + 12] = true;
			else
			{
				return chordName;
			}
		}
		else
			break;
	}
	if (chordName == "")
		return chordName;

	//see if the bass note is not the same as the chord base note
	if (bassNote == curBassNote)
		return chordName;

	chordName += "/";
	chordName += MidiMessage::getMidiNoteName(bassNote, sharp, false, 3);

	/*
	int i = 0;
	for (; i < 24; i++)
	{
		if (notes[i])
		{
			break;
		}
	}
	//i is the bass note
	String chordMatcher = "";
	for (; i < bassNote + 12; i++)
	{
		chordMatcher += notes[i] ? "1" : "0";
	}

	//loop over truths to see if current matches
	for (std::map<String, String>::iterator it = truthMap.begin(); it != truthMap.end(); ++it)
	{
		if (it->first == chordMatcher)
		{
			chordName += MidiMessage::getMidiNoteName(bassNote, sharp, false, 3);
			chordName += it->second;
			return chordName;
		}
	}
	*/

	return chordName;
}