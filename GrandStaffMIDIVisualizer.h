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

 name:                  GrandStaffMIDIVisualizer
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
#include "VSTParameters.h"

//==============================================================================
class GrandStaffMIDIVisualizerProcessor final : public AudioProcessor,
                                            private Timer
{
public:
    GrandStaffMIDIVisualizerProcessor()
        : AudioProcessor(getBusesLayout()),
        pluginModel { PluginModel() },
        vstParameters(*this, pluginModel)
    {
        for (int i = 0; i < 127; i++)
            pluginModel.midiNotes[i] = 0;
        editor = nullptr;        
    }

    ~GrandStaffMIDIVisualizerProcessor() override {}

    void processBlock (AudioBuffer<float>& audio,  MidiBuffer& midi) override 
    {
        audio.clear();
        ScopedLock lock(pluginModel.criticalSection);
        bool hasMidiChanges = false;
        for (const MidiMessageMetadata metadata : midi)
        {
            int noteNumber = metadata.getMessage().getNoteNumber();
            if (!pluginModel.holdNotes)
            {
                if (metadata.getMessage().isNoteOn() || metadata.getMessage().isNoteOff())
                {
                    bool onOff = metadata.getMessage().isNoteOn();
                    pluginModel.midiNotes[noteNumber] = onOff;
                    hasMidiChanges = true;
                }
            }
            else if (pluginModel.holdNotes && metadata.getMessage().isNoteOn())
            {
                bool currentlyOn = pluginModel.midiNotes[noteNumber];
                pluginModel.midiNotes[noteNumber] = !currentlyOn;
                hasMidiChanges = true;
            }
        }

        if (hasMidiChanges && editor != nullptr)
        {
            editor->postCommandMessage(CMD_MSG_MIDI_CHANGES);
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
        vstParameters.getStateInformation(destData);
    }

    void setStateInformation (const void* data, int size) override
    {
        vstParameters.setStateInformation(data, size);
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
            if (owner.pluginModel.uiWidth > 0 && owner.pluginModel.uiHeight > 0)
                setSize(owner.pluginModel.uiWidth, owner.pluginModel.uiHeight);
            else
                setSize(500, 500);

            setConstrainer(&constrainer);
            addAndMakeVisible(mainComponent);
        }

        ~Editor()
        {
            owner.editor = nullptr;
        }

        void paint (Graphics& g) override
        {
            g.fillAll (owner.pluginModel.darkMode ? darkModeBackgroundColour : Colours::white);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            
            if (owner.pluginModel.chordPlacement != 3)
            {
                constrainer.setMinimumWidth(std::max((int)(bounds.getHeight() * 0.85), 230));
                constrainer.setMinimumHeight(260);
                constrainer.setMaximumHeight(5000);
            }
            else
            {
                constrainer.setMinimumWidth(190);
                constrainer.setMinimumHeight(40);
                constrainer.setMaximumHeight(bounds.getWidth());
            }

            mainComponent.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

            lastUIWidth  = getWidth();
            lastUIHeight = getHeight();
        }

        void handleCommandMessage(int commandId)
        {
            if (commandId == CMD_MSG_VST_PARAM_CHANGES)
                mainComponent.onParametersChanged();
            else if (commandId == CMD_MSG_MIDI_CHANGES)
                mainComponent.onMidiChanged();
            repaint();
        }

        void onParametersChanged()
        {
            postCommandMessage(CMD_MSG_VST_PARAM_CHANGES);
        }

    private:
        void valueChanged (Value&) override
        {
            setSize (lastUIWidth.getValue(), lastUIHeight.getValue());
        }

        GrandStaffMIDIVisualizerProcessor& owner;
        MainComponent mainComponent;
        ComponentBoundsConstrainer constrainer;

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

    //ValueTree state { "state" };

    PluginModel pluginModel;
    VSTParameters vstParameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandStaffMIDIVisualizerProcessor)
};