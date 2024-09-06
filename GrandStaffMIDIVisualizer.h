/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

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
        : AudioProcessor (getBusesLayout())
    {
        state.addChild ({ "uiState", { { "width",  600 }, { "height", 300 } }, {} }, -1, nullptr);
        startTimerHz (60);
        for (int i = 0; i < 127; i++)
            pluginModel.midiNotes[i] = 0;
    }

    ~GrandStaffMIDIVisualizerProcessor() override { stopTimer(); }
    PluginModel pluginModel;

    void processBlock (AudioBuffer<float>& audio,  MidiBuffer& midi) override 
    {
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
        if (hasMidiChanges)
        {
            editor->postCommandMessage(1);
        }
    }

    bool isBusesLayoutSupported (const BusesLayout&) const override           { return true; }
    bool isMidiEffect() const override                                        { return true; }
    bool hasEditor() const override                                           { return true; }
    AudioProcessorEditor* createEditor() override                             { editor = new Editor(*this); return editor; }

    const String getName() const override                                     { return "MIDI Logger"; }
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
            copyXmlToBinary (*xmlState, destData);
    }

    void setStateInformation (const void* data, int size) override
    {
        if (auto xmlState = getXmlFromBinary (data, size))
            state = ValueTree::fromXml (*xmlState);
    }

private:
    class Editor final : public AudioProcessorEditor,
                         private Value::Listener
    {
    public:
        explicit Editor (GrandStaffMIDIVisualizerProcessor& ownerIn)
            : AudioProcessorEditor (ownerIn),
              owner (ownerIn), mainComponent(&ownerIn.pluginModel)
        {
            setResizable (true, true);
            lastUIWidth .referTo (owner.state.getChildWithName ("uiState").getPropertyAsValue ("width",  nullptr));
            lastUIHeight.referTo (owner.state.getChildWithName ("uiState").getPropertyAsValue ("height", nullptr));
            setSize (lastUIWidth.getValue(), lastUIHeight.getValue());

            lastUIWidth. addListener (this);
            lastUIHeight.addListener (this);

            addAndMakeVisible(mainComponent);            
        }

        void paint (Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandStaffMIDIVisualizerProcessor)
};
