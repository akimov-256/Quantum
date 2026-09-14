#define MyAppName "Quantum"
#define MyAppPublisher "Mouloud Guenane"
#define MyAppURL "https://github.com/akimov-256/Quantum"
#define MyAppExeName "Quantum.exe"
#define NativeHostExeName "QuantumNativeHost.exe"
#define NativeHostName "com.mouloud_guenane.quantum"
#define ChromeExtensionId "amplpanejdkenkfiidednahiodkkeljd"

[Setup]
AppId={{B6C1E1B0-6F0D-4A6E-9C1D-9F1A2E3B4C5D}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=..\installer-output
OutputBaseFilename=Quantum-{#MyAppVersion}-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesInstallIn64BitMode=x64compatible
SetupIconFile=..\qml\assets\icons\icon.ico
LicenseFile=..\LICENSE

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"
Name: "startup"; Description: "Start {#MyAppName} when Windows starts"; GroupDescription: "Startup options:"

[Files]
Source: "{#MyAppExeDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: "{app}\{#MyAppExeName}"; Tasks: startup; Flags: uninsdeletevalue

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent

[Code]
const
  NativeHostName = '{#NativeHostName}';
  ChromeExtensionId = '{#ChromeExtensionId}';

// Build the manifest JSON content with the real absolute path to the host exe.
// Backslashes are doubled because JSON requires escaping.
function BuildManifest(const HostExePath: String): String;
var
  EscapedPath: String;
begin
  EscapedPath := HostExePath;
  StringChangeEx(EscapedPath, '\', '\\', True);

  Result :=
    '{' + #13#10 +
    '  "name": "' + NativeHostName + '",' + #13#10 +
    '  "description": "Quantum Native Host",' + #13#10 +
    '  "path": "' + EscapedPath + '",' + #13#10 +
    '  "type": "stdio",' + #13#10 +
    '  "allowed_origins": [' + #13#10 +
    '    "chrome-extension://' + ChromeExtensionId + '/"' + #13#10 +
    '  ],' + #13#10 +
    '  "allowed_extensions": [' + #13#10 +
    '    "qdm-integration@mouloud-guenane"' + #13#10 +
    '  ]' + #13#10 +
    '}' + #13#10;
end;

// Writes {app}\<NativeHostName>.json and returns its path.
function WriteNativeHostManifest(): String;
var
  ManifestPath: String;
  HostExePath: String;
  Content: String;
begin
  ManifestPath := ExpandConstant('{app}\' + NativeHostName + '.json');
  HostExePath  := ExpandConstant('{app}\{#NativeHostExeName}');
  Content := BuildManifest(HostExePath);

  if not SaveStringToFile(ManifestPath, Content, False) then
    MsgBox('Quantum: failed to write native messaging manifest to ' +
           ManifestPath, mbError, MB_OK);

  Result := ManifestPath;
end;

// Register the manifest with every supported browser under HKCU.
procedure RegisterNativeHost(const ManifestPath: String);
begin
  RegWriteStringValue(HKCU,
    'Software\Google\Chrome\NativeMessagingHosts\' + NativeHostName,
    '', ManifestPath);

  RegWriteStringValue(HKCU,
    'Software\Microsoft\Edge\NativeMessagingHosts\' + NativeHostName,
    '', ManifestPath);

  RegWriteStringValue(HKCU,
    'Software\Mozilla\NativeMessagingHosts\' + NativeHostName,
    '', ManifestPath);
end;

// Remove the registry keys (called on uninstall).
procedure UnregisterNativeHost();
begin
  RegDeleteKeyIncludingSubkeys(HKCU,
    'Software\Google\Chrome\NativeMessagingHosts\' + NativeHostName);
  RegDeleteKeyIncludingSubkeys(HKCU,
    'Software\Microsoft\Edge\NativeMessagingHosts\' + NativeHostName);
  RegDeleteKeyIncludingSubkeys(HKCU,
    'Software\Mozilla\NativeMessagingHosts\' + NativeHostName);
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ManifestPath: String;
begin
  if CurStep = ssPostInstall then
  begin
    ManifestPath := WriteNativeHostManifest();
    RegisterNativeHost(ManifestPath);
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
    UnregisterNativeHost();
end;