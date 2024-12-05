#define MyAppName "GrandStaffMIDIVisualizer"
#define MyAppExeName "GrandStaffMIDIVisualizer.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{929C800A-A63F-4BF5-9916-84DD52672210}
AppName=GrandStaffMIDIVisualizer
AppVersion=1.0.0
;AppVerName=GrandStaffMIDIVisualizer 1.0.0
AppPublisher=Brynjar Reynisson
AppPublisherURL=https://sites.google.com/view/brynjarsoftware/products
AppSupportURL=https://sites.google.com/view/brynjarsoftware/products
AppUpdatesURL=https://sites.google.com/view/brynjarsoftware/products
DefaultDirName=C:\Program Files\Common Files\VST3\GrandStaffMIDIVisualizer
DefaultGroupName=GrandStaffMIDIVisualizer
OutputBaseFilename=GrandStaffMIDIVisualizer-Installer-v1.0.0
LicenseFile=EULA.txt

[Files]
Source: "g-clef.ico"; DestDir: "{app}"
Source: "C:\Users\Lenovo\JUCE\projects\GrandStaffMIDIVisualizer\GrandStaffMIDIVisualizer\Builds\VisualStudio2022\x64\Release\VST3\GrandStaffMIDIVisualizer.vst3\Contents\x86_64-win\GrandStaffMIDIVisualizer.vst3"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Lenovo\JUCE\projects\GrandStaffMIDIVisualizer\GrandStaffMIDIVisualizer\Builds\VisualStudio2022\x64\Release\Standalone Plugin\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Lenovo\InnoSetup\vcredist2015_2017_2019_2022_x64.exe"; DestDir: "{tmp}"; Flags: ignoreversion

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
    GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
    
[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{group}"; \
      IconFilename: "{app}\g-clef.ico"; Tasks: desktopicon
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; \
      IconFilename: "{app}\g-clef.ico"; Tasks: desktopicon
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon

[Run]
Filename: "{tmp}\vcredist2015_2017_2019_2022_x64.exe"; \
  Parameters: "/quiet"; \
  StatusMsg: "Installing VC2017 redist"; \
  Check: VC2017RedistNeedsInstall; Flags: waituntilterminated

    
[Code]
function VC2017RedistNeedsInstall: Boolean;
var 
  Version: String;
begin
  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
       'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Version',
       Version) then
  begin
    // Is the installed version at least 14.14 ? 
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.14.26429.03')<0);
  end  
  else 
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('vcredist2015_2017_2019_2022_x64.exe');
  end;
end;