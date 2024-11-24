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

#include "VSTParameters.h"

VSTParameters::VSTParameters(AudioProcessor& processorRef, PluginModel& pluginModelRef) :
    processor (processorRef),
    pluginModel (pluginModelRef),
    parameters(processor, nullptr, juce::Identifier("GrandStaffMIDIVisualizerParameters"),
        {
                std::make_unique<juce::AudioParameterChoice>(KEY, "Key", StringArray { "C", "C#", "Db", "D", "Eb", "E", "F", "F#", "Gb", "G", "Ab", "A", "Bb", "B", "Sharps", "Flats" }, 0),
                std::make_unique<juce::AudioParameterBool>(HOLD_NOTES, "Hold Notes", false),
                std::make_unique<juce::AudioParameterInt>(OCTAVES, "Octaves", -3, 3, 0),
                std::make_unique<juce::AudioParameterBool>(SHORT_NOTATION, "Display chords with short notation", false),
                std::make_unique<juce::AudioParameterChoice>(CHORD_PLACEMENT, "Chord Placement", StringArray { "Hidden", "Bottom Left", "Right to Staff", "Only show chords"}, 1),
                std::make_unique<juce::AudioParameterBool>(CHORD_FONT_BOLD, "Display chords with bold font", false),
                std::make_unique<juce::AudioParameterBool>(DARK_MODE, "Dark mode", false)
        }
        )
{
    keyParameter = (AudioParameterChoice*)parameters.getParameter(KEY);
    holdNotesParameter = parameters.getRawParameterValue(HOLD_NOTES);
    octavesParameter = parameters.getRawParameterValue(OCTAVES);
    shortNotationParameter = parameters.getRawParameterValue(SHORT_NOTATION);
    chordPlacementParameter = parameters.getRawParameterValue(CHORD_PLACEMENT);
    chordFontBoldParameter = parameters.getRawParameterValue(CHORD_FONT_BOLD);
    darkModeParameter = parameters.getRawParameterValue(DARK_MODE);

    parameters.addParameterListener(KEY, this);
    parameters.addParameterListener(HOLD_NOTES, this);
    parameters.addParameterListener(OCTAVES, this);
    parameters.addParameterListener(SHORT_NOTATION, this);
    parameters.addParameterListener(CHORD_PLACEMENT, this);
    parameters.addParameterListener(CHORD_FONT_BOLD, this);
    parameters.addParameterListener(DARK_MODE, this);

    pluginModel.paramChangedFromUI = [&] { pluginModelChangedFromUI(); };
}

VSTParameters::~VSTParameters()
{
    pluginModel.paramChangedFromUI = nullptr;
    parameters.removeParameterListener(KEY, this);
    parameters.removeParameterListener(HOLD_NOTES, this);
    parameters.removeParameterListener(OCTAVES, this);
    parameters.removeParameterListener(SHORT_NOTATION, this);
    parameters.removeParameterListener(CHORD_PLACEMENT, this);
    parameters.removeParameterListener(CHORD_FONT_BOLD, this);
    parameters.removeParameterListener(DARK_MODE, this);
}

void VSTParameters::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == KEY.getCharPointer())
        pluginModel.keyId = (int)*keyParameter;
    else if (parameterID == HOLD_NOTES.getCharPointer())
        pluginModel.holdNotes = *holdNotesParameter > 0.5;
    else if (parameterID == OCTAVES.getCharPointer())
        pluginModel.transposeOctaves = (int)*octavesParameter;
    else if (parameterID == SHORT_NOTATION.getCharPointer())
        pluginModel.shortNotation = *shortNotationParameter > 0.5;
    else if (parameterID == CHORD_PLACEMENT.getCharPointer())
        pluginModel.chordPlacement = (int)*chordPlacementParameter;
    else if (parameterID == CHORD_FONT_BOLD.getCharPointer())
        pluginModel.chordFontBold = *chordFontBoldParameter > 0.5;
    else if (parameterID == DARK_MODE.getCharPointer())
        pluginModel.darkMode = *darkModeParameter > 0.5;

    pluginModel.hasParamChanges = true;
    if (pluginModel.paramChangedFromHost != nullptr)
        pluginModel.paramChangedFromHost();
}

void VSTParameters::pluginModelChangedFromUI()
{
    *keyParameter = pluginModel.keyId;
    *holdNotesParameter = pluginModel.holdNotes;
    *octavesParameter = pluginModel.transposeOctaves;
    *shortNotationParameter = pluginModel.shortNotation;
    *chordPlacementParameter = pluginModel.chordPlacement;
    *chordFontBoldParameter = pluginModel.chordFontBold;
    *darkModeParameter = pluginModel.darkMode;

    Value holdNotes = parameters.getParameterAsValue(HOLD_NOTES);
    holdNotes = pluginModel.holdNotes;

    Value octaves = parameters.getParameterAsValue(OCTAVES);
    octaves = pluginModel.transposeOctaves;

    Value shortNotation = parameters.getParameterAsValue(SHORT_NOTATION);
    shortNotation = pluginModel.shortNotation;

    Value chordPlacement = parameters.getParameterAsValue(CHORD_PLACEMENT);
    chordPlacement = pluginModel.chordPlacement;

    Value chordFontBold = parameters.getParameterAsValue(CHORD_FONT_BOLD);
    chordFontBold = pluginModel.chordFontBold;

    Value darkMode = parameters.getParameterAsValue(DARK_MODE);
    darkMode = pluginModel.darkMode;

    pluginModel.hasUIChanges = false;
}

static juce::Identifier oldUiState("uiState");
static juce::Identifier UIState("UIState");

void VSTParameters::getStateInformation(MemoryBlock& destData)
{
    ValueTree uiState{ UIState, { { "width",  pluginModel.uiWidth }, { "height", pluginModel.uiHeight } }, {} };
    //uiState.addChild({ UIState, { { "width",  pluginModel.uiWidth }, { "height", pluginModel.uiHeight } }, {} }, -1, nullptr);
    
    auto paramState = parameters.copyState();

    //Remove all previous UIState xml elements
    std::vector<int> childrenToRemove;
    for (int idx = paramState.getNumChildren() - 1; idx > -1; idx--)
    {
        ValueTree child = paramState.getChild(idx);
        if (child.getType() == oldUiState || child.getType() == UIState)
        {
            childrenToRemove.push_back(idx);
        }
    }
    for (int idx = 0; idx < childrenToRemove.size(); idx++)
    {
        paramState.removeChild(childrenToRemove[idx], nullptr);
    }

    paramState.appendChild(uiState, nullptr);
    std::unique_ptr<juce::XmlElement> xml(paramState.createXml());
    String document = xml->createDocument("", false, false);
    processor.copyXmlToBinary(*xml, destData);
}

void VSTParameters::setStateInformation(const void* data, int size)
{
    if (auto xmlState = processor.getXmlFromBinary(data, size))
    {
        String document = xmlState->createDocument("", false, false);
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
            pluginModel.keyId = (int)*keyParameter;
            pluginModel.holdNotes = *holdNotesParameter > 0.5 ? true : false;
            pluginModel.transposeOctaves = (int)*octavesParameter;
            pluginModel.shortNotation = *shortNotationParameter > 0.5 ? true : false;
            pluginModel.chordPlacement = (int)*chordPlacementParameter;
            pluginModel.chordFontBold = *chordFontBoldParameter > 0.5 ? true : false;
            pluginModel.darkMode = *darkModeParameter > 0.5 ? true : false;
            pluginModel.hasParamChanges = true;

            //check if xmlState has UIState
            XmlElement* uiStateXml = xmlState->getChildByName(UIState);
            if (uiStateXml != nullptr)
            {
                pluginModel.uiWidth = uiStateXml->getAttributeValue(0).getIntValue();
                pluginModel.uiHeight = uiStateXml->getAttributeValue(1).getIntValue();
            }
        }
    }
}