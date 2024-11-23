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
#include "MainComponent.h"


static String KEY = "key";
static String HOLD_NOTES = "holdNotes";
static String OCTAVES = "octaves";
static String SHORT_NOTATION = "shortNotation";
static String CHORD_PLACEMENT = "chordPlacement";
static String CHORD_FONT_BOLD = "chordFontBold";
static String DARK_MODE = "darkMode";

class VSTParameters : AudioProcessorValueTreeState::Listener
{
public:
	VSTParameters(AudioProcessor& processorRef, PluginModel& pluginModelRef);
	~VSTParameters();

	void parameterChanged(const String& parameterID, float newValue);
	void getStateInformation(MemoryBlock& destData);
	void setStateInformation(const void* data, int size);
private:
	void pluginModelChangedFromUI();

	AudioProcessor& processor;
	PluginModel& pluginModel;
	juce::AudioProcessorValueTreeState parameters;

	AudioParameterChoice* keyParameter;
	std::atomic<float>* holdNotesParameter = nullptr;
	std::atomic<float>* octavesParameter = nullptr;
	std::atomic<float>* shortNotationParameter = nullptr;
	std::atomic<float>* chordPlacementParameter = nullptr;
	std::atomic<float>* chordFontBoldParameter = nullptr;
	std::atomic<float>* darkModeParameter = nullptr;

};