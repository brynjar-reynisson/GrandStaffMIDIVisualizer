# GrandStaffMIDIVisualizer

![GrandStaffGitHubImage](https://github.com/user-attachments/assets/9dea3afa-d680-46c2-a455-f2a682d8d281)

This plugin shows on the Grand Staff what MIDI notes are being played at each time. It will also display chord names if a matching chord type is found. In order to build it, one must install the JUCE framework: https://juce.com/download/

A special acknowledgement must be made to Diego Merino, who had the original idea and has been helping me to refine this work. He created the chords definition table, as well as many of the graphics used here. All the recognized chord patterns are found here: https://github.com/brynjar-reynisson/GrandStaffMIDIVisualizer/blob/main/ChordDefinitions.cpp

Another must mention, is that the chord recognition algorithm is based on the Chordback JSFX script written by Paul Heams. He can be contacted on X (Twitter) as @17BitRetro

   This file is part of the GrandStaffMIDIVisualizer plugin code.
   Copyright (c) Brynjar Reynisson

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.
