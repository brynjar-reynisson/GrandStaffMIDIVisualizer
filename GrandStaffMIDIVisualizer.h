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

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:                  Grand Staff MIDI Visualizer
 version:               1.0.0
 vendor:                Brynjar Reynisson
 website:               https://breynisson.neocities.org/
 description:           Paints G and F clefs and shows what notes are being played. Also prints chord names.

 dependencies:          juce_audio_basics, juce_audio_devices, juce_audio_formats,
                        juce_audio_plugin_client, juce_audio_processors,
                        juce_audio_utils, juce_core, juce_data_structures,
                        juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:             xcode_mac, vs2022, linux_make

 moduleFlags:           JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:                  AudioProcessor
 mainClass:             GrandStaffMIDIVisualizerProcessor

 useLocalCopy:          1

 pluginCharacteristics: pluginWantsMidiIn, pluginProducesMidiOut, pluginIsMidiEffectPlugin

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include <iterator>
#include "MainComponent.h"


//==============================================================================
class GrandStaffMIDIVisualizerProcessor final : public AudioProcessor,
                                            private Timer
{
public:
    GrandStaffMIDIVisualizerProcessor()
        : AudioProcessor (getBusesLayout()),
        parameters(*this, nullptr, juce::Identifier("GrandStaffMIDIVisualizerParameters"),
            {
                std::make_unique<juce::AudioParameterBool>("sharp",            // parameterID
                                                             "Sharp",            // parameter name
                                                             true),             // default value
                std::make_unique<juce::AudioParameterBool>("holdNotes",            // parameterID
                                                             "Hold Notes",            // parameter name
                                                             false),             // default value
                std::make_unique<juce::AudioParameterInt>("octaves",      // parameterID
                                                            "Octaves",     // parameter name
                                                            -3,
                                                            3,
                                                            0),              // default value
                std::make_unique<juce::AudioParameterBool>("chordsOnRight",            // parameterID
                                                             "Display chords on right side",            // parameter name
                                                             false),             // default value
            })
    {
        state.addChild({ "uiState", { { "width",  600 }, { "height", 300 } }, {} }, -1, nullptr);

        for (int i = 0; i < 127; i++)
            pluginModel.midiNotes[i] = 0;
        editor = nullptr;

        sharpParameter = parameters.getRawParameterValue("sharp");
        holdNotesParameter = parameters.getRawParameterValue("holdNotes");
        octavesParameter = parameters.getRawParameterValue("octaves");
        chordPlacementParameter = parameters.getRawParameterValue("chordsOnRight");

        pluginModel.onChange = [&] { pluginModelChanged(); };
    }

    ~GrandStaffMIDIVisualizerProcessor() override {}

    void pluginModelChanged()
    {
        *sharpParameter = pluginModel.sharp;
        *holdNotesParameter = pluginModel.holdNotes;
        *octavesParameter = pluginModel.transposeOctaves;
        *chordPlacementParameter = pluginModel.chordsOnRight;
    }

    void processBlock (AudioBuffer<float>& audio,  MidiBuffer& midi) override 
    {
        bool hasParamChanges = false;
        bool newSharpValue = *sharpParameter > 0.5f ? true : false;
        bool newHoldNotesValue = *holdNotesParameter > 0.5 ? true : false;
        bool newChordPlacementValue = *chordPlacementParameter > 0.5 ? true : false;
        if (newSharpValue != pluginModel.sharp)
        {
            hasParamChanges = true;
            pluginModel.sharp = newSharpValue;
        }
        if (newHoldNotesValue != pluginModel.holdNotes)
        {
            hasParamChanges = true;
            pluginModel.holdNotes = newHoldNotesValue;
        }
        if ((int)*octavesParameter != pluginModel.transposeOctaves)
        {
            hasParamChanges = true;
            pluginModel.transposeOctaves = (int)*octavesParameter;
        }
        if (newChordPlacementValue != pluginModel.chordsOnRight)
        {
            hasParamChanges = true;
            pluginModel.chordsOnRight = newChordPlacementValue;
        }

        audio.clear();
        bool hasMidiChanges = false;
        for (const MidiMessageMetadata metadata : midi)
        {
            if (!pluginModel.holdNotes)
            {
                if (metadata.getMessage().isNoteOn() || metadata.getMessage().isNoteOff())
                {
                    int onOff = metadata.getMessage().isNoteOn() ? 1 : 0;
                    pluginModel.midiNotes[metadata.getMessage().getNoteNumber()] = onOff;
                    hasMidiChanges = true;
                }
            }
            else if (pluginModel.holdNotes && metadata.getMessage().isNoteOn())
            {
                int noteNumber = metadata.getMessage().getNoteNumber();
                bool currentlyOn = pluginModel.midiNotes[noteNumber] == 1;
                pluginModel.midiNotes[noteNumber] = currentlyOn ? 0 : 1;
                hasMidiChanges = true;
            }
        }
        if ((hasMidiChanges || hasParamChanges) && editor != nullptr)
        {
            editor->postCommandMessage(1);
        }
    }

    bool isBusesLayoutSupported (const BusesLayout&) const override           { return true; }
    bool isMidiEffect() const override                                        { return true; }
    bool hasEditor() const override                                           { return true; }
    AudioProcessorEditor* createEditor() override                             { editor = new Editor(*this); return editor; }

    const String getName() const override                                     { return "Grand Staff MIDI Visualizer"; }
    bool acceptsMidi() const override                                         { return true; }
    bool producesMidi() const override                                        { return true; }
    double getTailLengthSeconds() const override                              { return 0.0; }

    int getNumPrograms() override                                             { return 0; }
    int getCurrentProgram() override                                          { return 0; }
    void setCurrentProgram (int) override                                     {}
    const String getProgramName (int) override                                { return "None"; }
    void changeProgramName (int, const String&) override                      {}

    void prepareToPlay (double, int) override                                 {}
    void releaseResources() override                                          {}

    void getStateInformation (MemoryBlock& destData) override
    {
        if (auto xmlState = state.createXml())
            copyXmlToBinary(*xmlState, destData);

        auto paramState = parameters.copyState();
        std::unique_ptr<juce::XmlElement> xml(paramState.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation (const void* data, int size) override
    {
        if (auto xmlState = getXmlFromBinary(data, size))
        {
            state = ValueTree::fromXml(*xmlState);
            if (xmlState->hasTagName(parameters.state.getType()))
            {
                parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
                pluginModel.sharp = (bool)xmlState->getBoolAttribute("sharp", true);
                pluginModel.holdNotes = (bool)xmlState->getBoolAttribute("holdNotes", false);
                pluginModel.transposeOctaves = (int)xmlState->getIntAttribute("octaves", 0);
                pluginModel.chordsOnRight = (bool)xmlState->getBoolAttribute("chordsOnRight", false);
                if (editor != nullptr)
                    editor->postCommandMessage(1);
            }
        }
    }

private:
    class Editor final : public AudioProcessorEditor,
                         private Value::Listener
    {
    public:
        explicit Editor (GrandStaffMIDIVisualizerProcessor& ownerIn)
            : AudioProcessorEditor (ownerIn),
            owner (ownerIn), 
            mainComponent(&ownerIn.pluginModel)
        {
            setResizable (true, true);
            lastUIWidth.referTo(owner.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
            lastUIHeight.referTo(owner.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));
            setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

            lastUIWidth.addListener(this);
            lastUIHeight.addListener(this);

            addAndMakeVisible(mainComponent);
        }

        ~Editor()
        {
            owner.editor = nullptr;
        }

        void paint (Graphics& g) override
        {
            g.fillAll (Colours::white);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            mainComponent.setBounds(bounds.getX() + 2, bounds.getY() + 2, bounds.getWidth() - 4, bounds.getHeight() - 4);

            lastUIWidth  = getWidth();
            lastUIHeight = getHeight();
        }

        void handleCommandMessage(int commandId)
        {
            repaint();
        }

        void onParametersChanged()
        {
            mainComponent.onParametersChanged();
        }
    private:
        void valueChanged (Value&) override
        {
            setSize (lastUIWidth.getValue(), lastUIHeight.getValue());
        }

        GrandStaffMIDIVisualizerProcessor& owner;
        MainComponent mainComponent;

        Value lastUIWidth, lastUIHeight;
    };
    Editor* editor;

    void timerCallback() override
    {
        std::vector<MidiMessage> messages;
    }

    static BusesProperties getBusesLayout()
    {
        // Live and Cakewalk don't like to load midi-only plugins, so we add an audio output there.
        const PluginHostType host;
        return host.isAbletonLive() || host.isSonar()
             ? BusesProperties().withOutput ("out", AudioChannelSet::stereo())
             : BusesProperties();
    }

    ValueTree state { "state" };

    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* sharpParameter = nullptr;
    std::atomic<float>* holdNotesParameter = nullptr;
    std::atomic<float>* octavesParameter = nullptr;
    std::atomic<float>* chordPlacementParameter = nullptr;
    PluginModel pluginModel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandStaffMIDIVisualizerProcessor)
};