copy /Y C:\Users\Lenovo\JUCE\projects\GrandStaffMIDIVisualizer\GrandStaffMIDIVisualizer\Builds\VisualStudio2022\x64\Release\VST3\GrandStaffMIDIVisualizer.vst3\Contents\x86_64-win\GrandStaffMIDIVisualizer.vst3 "G:\My Drive\GrandStaffMIDIVisualizer"
copy /Y "C:\Users\Lenovo\JUCE\projects\GrandStaffMIDIVisualizer\GrandStaffMIDIVisualizer\Builds\VisualStudio2022\x64\Release\Standalone Plugin\GrandStaffMIDIVisualizer.exe"  "G:\My Drive\GrandStaffMIDIVisualizer"  
copy /Y .\Installer\EULA.txt "G:\My Drive\GrandStaffMIDIVisualizer" 
copy /Y .\manual.txt "G:\My Drive\GrandStaffMIDIVisualizer" 

tar -a -c -C "G:\My Drive\GrandStaffMIDIVisualizer"  -f "G:\My Drive\GrandStaffMIDIVisualizer\GrandStaffMIDIVisualizer.zip" GrandStaffMIDIVisualizer.vst3 GrandStaffMIDIVisualizer.exe EULA.txt vcredist2015_2017_2019_2022_x64.exe manual.txt
