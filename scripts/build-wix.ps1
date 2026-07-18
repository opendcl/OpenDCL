<#
.SYNOPSIS
  Build OpenDCL installers with WiX Toolset v3 (MSM + Runtime MSI + Studio MSIs).

.DESCRIPTION
  Expects a prior Release compile of OpenDCL (ARX/BRX/GRX/ZRX modules, Runtime.Res,
  RxInstall, Studio exe / Studio.Res, help CHM, samples).

  Outputs (default under <repo>\wix\out\Release):
    OpenDCL.Runtime.msm          -- kept for third-party installer authors
    OpenDCL.Runtime.msi
    OpenDCL.Studio.<LANG>.msi    -- ENU DEU ESM RUS CHS FRA CHT

  Does NOT use .vdproj. Layout: scripts\build-wix.ps1 + wix\ next to OpenDCL.sln.

.PARAMETER OpenDclRoot
  Path to compiled OpenDCL tree. Defaults to this repository root (where
  OpenDCL.sln lives).

.PARAMETER Configuration
  Currently only Release outputs are harvested (default Release).

.PARAMETER ProductVersion
  MSI 3-part ProductVersion. Default 10.1.101 encodes file version 10.1.1.1
  as major.minor.(patch*100+revision) for MSI upgrade comparison.

.PARAMETER ModuleVersion
  Merge module 4-part version (default 10.1.1.1).

.PARAMETER WixBin
  Path to candle.exe/light.exe directory. Auto-detected if empty.
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [string] $Configuration = "Release",
  [string] $ProductVersion = "10.1.101",
  [string] $ModuleVersion = "10.1.1.1",
  [string] $WixBin = "",
  [string] $OutDir = "",
  [string[]] $Languages = @(),
  [switch] $SkipStudio,
  [switch] $SkipRuntimeMsi,
  [switch] $SkipMsm
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$WixRoot = Join-Path $RepoRoot "wix"

if (-not $OpenDclRoot) {
  if (-not (Test-Path (Join-Path $RepoRoot "OpenDCL.sln"))) {
    throw "OpenDCL.sln not found under $RepoRoot; pass -OpenDclRoot to the compiled tree."
  }
  $OpenDclRoot = $RepoRoot
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path

if (-not $OutDir) {
  $OutDir = Join-Path $WixRoot "out\$Configuration"
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$GenDir = Join-Path $WixRoot "out\gen"
New-Item -ItemType Directory -Force -Path $GenDir | Out-Null

if (-not $WixBin) {
  $candidates = @(
    "${env:ProgramFiles(x86)}\WiX Toolset v3.14\bin",
    "${env:ProgramFiles}\WiX Toolset v3.14\bin",
    "${env:ProgramFiles(x86)}\WiX Toolset v3.11\bin"
  )
  foreach ($c in $candidates) {
    if (Test-Path (Join-Path $c "candle.exe")) { $WixBin = $c; break }
  }
}
if (-not $WixBin -or -not (Test-Path (Join-Path $WixBin "candle.exe"))) {
  throw "WiX Toolset v3 candle.exe not found. Install WiX 3.14 or pass -WixBin."
}
$Candle = Join-Path $WixBin "candle.exe"
$Light = Join-Path $WixBin "light.exe"

function Get-StableGuid([string] $seed) {
  $md5 = [System.Security.Cryptography.MD5]::Create()
  try {
    $bytes = $md5.ComputeHash([System.Text.Encoding]::UTF8.GetBytes($seed.ToLowerInvariant()))
    # UUID version 4 + RFC 4122 variant bits
    $bytes[6] = ($bytes[6] -band 0x0F) -bor 0x40
    $bytes[8] = ($bytes[8] -band 0x3F) -bor 0x80
    return ([guid]::new($bytes)).ToString().ToUpperInvariant()
  }
  finally { $md5.Dispose() }
}

function Get-SafeId([string] $name) {
  $id = ($name -replace '[^A-Za-z0-9_]', '_')
  if ($id -match '^[0-9]') { $id = "F_$id" }
  if ($id.Length -gt 60) { $id = $id.Substring(0, 60) }
  return $id
}

function Assert-File([string] $path) {
  if (-not (Test-Path -LiteralPath $path)) {
    throw "Required build output missing: $path"
  }
}

# Extract full multi-resolution RT_GROUP_ICON resources from a PE into .ico files.
# (MSI Icon table needs real ICO streams; embedding a whole DLL loses high-res images.)
if (-not ("PeIconExtract" -as [type])) {
  Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;

public static class PeIconExtract {
  [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
  static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hFile, uint dwFlags);
  [DllImport("kernel32", SetLastError = true)] static extern bool FreeLibrary(IntPtr hModule);
  [DllImport("kernel32", SetLastError = true)] static extern IntPtr FindResource(IntPtr hModule, IntPtr lpName, IntPtr lpType);
  [DllImport("kernel32", SetLastError = true)] static extern IntPtr LoadResource(IntPtr hModule, IntPtr hResInfo);
  [DllImport("kernel32", SetLastError = true)] static extern IntPtr LockResource(IntPtr hResData);
  [DllImport("kernel32", SetLastError = true)] static extern uint SizeofResource(IntPtr hModule, IntPtr hResInfo);

  const uint LOAD_LIBRARY_AS_DATAFILE = 0x00000002;
  static readonly IntPtr RT_ICON = (IntPtr)3;
  static readonly IntPtr RT_GROUP_ICON = (IntPtr)14;

  public static void ExportIconGroup(string pePath, int groupId, string icoPath) {
    IntPtr hMod = LoadLibraryEx(pePath, IntPtr.Zero, LOAD_LIBRARY_AS_DATAFILE);
    if (hMod == IntPtr.Zero)
      throw new Exception("LoadLibraryEx failed " + Marshal.GetLastWin32Error() + ": " + pePath);
    try {
      IntPtr hRes = FindResource(hMod, (IntPtr)groupId, RT_GROUP_ICON);
      if (hRes == IntPtr.Zero)
        throw new Exception("GROUP_ICON #" + groupId + " not found in " + pePath);
      uint gsize = SizeofResource(hMod, hRes);
      IntPtr pData = LockResource(LoadResource(hMod, hRes));
      byte[] grp = new byte[gsize];
      Marshal.Copy(pData, grp, 0, (int)gsize);

      ushort count = BitConverter.ToUInt16(grp, 4);
      var images = new List<byte[]>();
      var entries = new List<byte[]>();
      int pos = 6;
      for (int i = 0; i < count; i++) {
        byte w = grp[pos], h = grp[pos + 1], cc = grp[pos + 2], resv = grp[pos + 3];
        ushort planes = BitConverter.ToUInt16(grp, pos + 4);
        ushort bitcount = BitConverter.ToUInt16(grp, pos + 6);
        uint bytesInRes = BitConverter.ToUInt32(grp, pos + 8);
        ushort nId = BitConverter.ToUInt16(grp, pos + 12);
        pos += 14;

        IntPtr hIconRes = FindResource(hMod, (IntPtr)nId, RT_ICON);
        if (hIconRes == IntPtr.Zero)
          throw new Exception("RT_ICON #" + nId + " missing for GROUP_ICON #" + groupId);
        uint isize = SizeofResource(hMod, hIconRes);
        IntPtr pIcon = LockResource(LoadResource(hMod, hIconRes));
        byte[] img = new byte[isize];
        Marshal.Copy(pIcon, img, 0, (int)isize);
        images.Add(img);

        byte[] ent = new byte[16];
        ent[0] = w; ent[1] = h; ent[2] = cc; ent[3] = resv;
        BitConverter.GetBytes(planes).CopyTo(ent, 4);
        BitConverter.GetBytes(bitcount).CopyTo(ent, 6);
        BitConverter.GetBytes(bytesInRes).CopyTo(ent, 8);
        entries.Add(ent);
      }

      string dir = Path.GetDirectoryName(icoPath);
      if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
        Directory.CreateDirectory(dir);

      using (var fs = File.Create(icoPath))
      using (var bw = new BinaryWriter(fs)) {
        bw.Write((ushort)0);
        bw.Write((ushort)1);
        bw.Write(count);
        int offset = 6 + 16 * count;
        for (int i = 0; i < count; i++) {
          BitConverter.GetBytes(offset).CopyTo(entries[i], 12);
          bw.Write(entries[i]);
          offset += images[i].Length;
        }
        for (int i = 0; i < count; i++)
          bw.Write(images[i]);
      }
    }
    finally { FreeLibrary(hMod); }
  }
}
'@
}

# App icon: full multi-res GROUP_ICON #10 from Studio.Res.dll (same as pre-WiX vdproj).
# Help/License use checked-in multi-res ICOs under wix\ui\icons\ (Fluent + crosshair badge).
function Export-StudioAppIcon([string] $studioResDll, [string] $destIco) {
  Assert-File $studioResDll
  $dir = Split-Path -Parent $destIco
  if ($dir) { New-Item -ItemType Directory -Force -Path $dir | Out-Null }
  [PeIconExtract]::ExportIconGroup($studioResDll, 10, $destIco)
  return $destIco
}

# --- Inventory of Runtime product files (intentional; no Detected Dependencies) ---
$RuntimeModules = @(
  # ARX
  "Runtime\ARX\ARX.16\Release\OpenDCL.16.arx",
  "Runtime\ARX\ARX.17\Release\OpenDCL.17.arx",
  "Runtime\ARX\ARX.17.x64\Release\OpenDCL.x64.17.arx",
  "Runtime\ARX\ARX.18\Release\OpenDCL.18.arx",
  "Runtime\ARX\ARX.18.x64\Release\OpenDCL.x64.18.arx",
  "Runtime\ARX\ARX.19\Release\OpenDCL.19.arx",
  "Runtime\ARX\ARX.19.x64\Release\OpenDCL.x64.19.arx",
  "Runtime\ARX\ARX.20\Release\OpenDCL.20.arx",
  "Runtime\ARX\ARX.20.x64\Release\OpenDCL.x64.20.arx",
  "Runtime\ARX\ARX.21\Release\OpenDCL.21.arx",
  "Runtime\ARX\ARX.21.x64\Release\OpenDCL.x64.21.arx",
  "Runtime\ARX\ARX.22\Release\OpenDCL.22.arx",
  "Runtime\ARX\ARX.22.x64\Release\OpenDCL.x64.22.arx",
  "Runtime\ARX\ARX.23\Release\OpenDCL.23.arx",
  "Runtime\ARX\ARX.23.x64\Release\OpenDCL.x64.23.arx",
  "Runtime\ARX\ARX.24.x64\Release\OpenDCL.x64.24.arx",
  "Runtime\ARX\ARX.25.x64\Release\OpenDCL.x64.25.arx",
  "Runtime\ARX\ARX.26.x64\Release\OpenDCL.x64.26.arx",
  # BRX
  "Runtime\BRX\BRX.9\Release\OpenDCL.9.brx",
  "Runtime\BRX\BRX.10\Release\OpenDCL.10.brx",
  "Runtime\BRX\BRX.11\Release\OpenDCL.11.brx",
  "Runtime\BRX\BRX.12\Release\OpenDCL.12.brx",
  "Runtime\BRX\BRX.13\Release\OpenDCL.13.brx",
  "Runtime\BRX\BRX.13.x64\Release\OpenDCL.x64.13.brx",
  "Runtime\BRX\BRX.14\Release\OpenDCL.14.brx",
  "Runtime\BRX\BRX.14.x64\Release\OpenDCL.x64.14.brx",
  "Runtime\BRX\BRX.15\Release\OpenDCL.15.brx",
  "Runtime\BRX\BRX.15.x64\Release\OpenDCL.x64.15.brx",
  "Runtime\BRX\BRX.16\Release\OpenDCL.16.brx",
  "Runtime\BRX\BRX.16.x64\Release\OpenDCL.x64.16.brx",
  "Runtime\BRX\BRX.17\Release\OpenDCL.17.brx",
  "Runtime\BRX\BRX.17.x64\Release\OpenDCL.x64.17.brx",
  "Runtime\BRX\BRX.18\Release\OpenDCL.18.brx",
  "Runtime\BRX\BRX.18.x64\Release\OpenDCL.x64.18.brx",
  "Runtime\BRX\BRX.19\Release\OpenDCL.19.brx",
  "Runtime\BRX\BRX.19.x64\Release\OpenDCL.x64.19.brx",
  "Runtime\BRX\BRX.20\Release\OpenDCL.20.brx",
  "Runtime\BRX\BRX.20.x64\Release\OpenDCL.x64.20.brx",
  "Runtime\BRX\BRX.21.x64\Release\OpenDCL.x64.21.brx",
  "Runtime\BRX\BRX.22.x64\Release\OpenDCL.x64.22.brx",
  "Runtime\BRX\BRX.23.x64\Release\OpenDCL.x64.23.brx",
  "Runtime\BRX\BRX.24.x64\Release\OpenDCL.x64.24.brx",
  "Runtime\BRX\BRX.25.x64\Release\OpenDCL.x64.25.brx",
  "Runtime\BRX\BRX.26.x64\Release\OpenDCL.x64.26.brx",
  # GRX
  "Runtime\GRX\GRX.2015\Release\OpenDCL.2015.grx",
  "Runtime\GRX\GRX.2015.x64\Release\OpenDCL.x64.2015.grx",
  "Runtime\GRX\GRX.2016\Release\OpenDCL.2016.grx",
  "Runtime\GRX\GRX.2016.x64\Release\OpenDCL.x64.2016.grx",
  "Runtime\GRX\GRX.2017\Release\OpenDCL.2017.grx",
  "Runtime\GRX\GRX.2017.x64\Release\OpenDCL.x64.2017.grx",
  "Runtime\GRX\GRX.2018\Release\OpenDCL.2018.grx",
  "Runtime\GRX\GRX.2018.x64\Release\OpenDCL.x64.2018.grx",
  "Runtime\GRX\GRX.2019\Release\OpenDCL.2019.grx",
  "Runtime\GRX\GRX.2019.x64\Release\OpenDCL.x64.2019.grx",
  "Runtime\GRX\GRX.2020\Release\OpenDCL.2020.grx",
  "Runtime\GRX\GRX.2020.x64\Release\OpenDCL.x64.2020.grx",
  "Runtime\GRX\GRX.2021\Release\OpenDCL.2021.grx",
  "Runtime\GRX\GRX.2021.x64\Release\OpenDCL.x64.2021.grx",
  "Runtime\GRX\GRX.2022\Release\OpenDCL.2022.grx",
  "Runtime\GRX\GRX.2022.x64\Release\OpenDCL.x64.2022.grx",
  "Runtime\GRX\GRX.2023.x64\Release\OpenDCL.x64.2023.grx",
  "Runtime\GRX\GRX.2024.x64\Release\OpenDCL.x64.2024.grx",
  "Runtime\GRX\GRX.2025.x64\Release\OpenDCL.x64.2025.grx",
  "Runtime\GRX\GRX.2026.x64\Release\OpenDCL.x64.2026.grx",
  "Runtime\GRX\GRX.2027.x64\Release\OpenDCL.x64.2027.grx",
  # ZRX
  "Runtime\ZRX\ZRX.2014\Release\OpenDCL.2014.zrx",
  "Runtime\ZRX\ZRX.2015\Release\OpenDCL.2015.zrx",
  "Runtime\ZRX\ZRX.2017\Release\OpenDCL.2017.zrx",
  "Runtime\ZRX\ZRX.2017.x64\Release\OpenDCL.x64.2017.zrx",
  "Runtime\ZRX\ZRX.2018\Release\OpenDCL.2018.zrx",
  "Runtime\ZRX\ZRX.2018.x64\Release\OpenDCL.x64.2018.zrx",
  "Runtime\ZRX\ZRX.2019\Release\OpenDCL.2019.zrx",
  "Runtime\ZRX\ZRX.2019.x64\Release\OpenDCL.x64.2019.zrx",
  "Runtime\ZRX\ZRX.2020\Release\OpenDCL.2020.zrx",
  "Runtime\ZRX\ZRX.2020.x64\Release\OpenDCL.x64.2020.zrx",
  "Runtime\ZRX\ZRX.2021\Release\OpenDCL.2021.zrx",
  "Runtime\ZRX\ZRX.2021.x64\Release\OpenDCL.x64.2021.zrx",
  "Runtime\ZRX\ZRX.2022\Release\OpenDCL.2022.zrx",
  "Runtime\ZRX\ZRX.2022.x64\Release\OpenDCL.x64.2022.zrx",
  "Runtime\ZRX\ZRX.2023\Release\OpenDCL.2023.zrx",
  "Runtime\ZRX\ZRX.2023.x64\Release\OpenDCL.x64.2023.zrx",
  "Runtime\ZRX\ZRX.2025.x64\Release\OpenDCL.x64.2025.zrx"
)

$RuntimeLangs = @("ENU", "DEU", "ESM", "RUS", "CHS", "FRA", "CHT")
if ($Languages -and $Languages.Count -gt 0) {
  $RuntimeLangs = @($Languages | ForEach-Object { "$_".ToUpperInvariant() })
}

$StudioLangMeta = @{
  ENU = @{ ProductCode = "BA22A3D6-E594-45CD-BB25-12ADD982C87A"; UpgradeCode = "F8AB94DC-2F57-4C29-8842-E2BB5F898E6C"; LangId = 1033; Comments = "OpenDCL Studio (US English)" }
  DEU = @{ ProductCode = "1D3CD6D2-BE15-489C-90AF-19289D21127B"; UpgradeCode = "9BE5DB7C-1263-4235-90AC-B4E5E907789D"; LangId = 1031; Comments = "OpenDCL Studio (German)" }
  ESM = @{ ProductCode = "1168CA37-1C22-47C6-A550-89CE14A990A7"; UpgradeCode = "13AE30FE-5152-40D5-B724-3BE317FC4DF2"; LangId = 3082; Comments = "OpenDCL Studio (Spanish)" }
  RUS = @{ ProductCode = "8E7BDDB2-EC97-4FBD-B6B9-67E353F71245"; UpgradeCode = "30C0C78D-D3B1-4C36-B37E-357F50E691D3"; LangId = 1049; Comments = "OpenDCL Studio (Russian)" }
  CHS = @{ ProductCode = "C6622C7F-E024-4FBF-B14D-DBAFDAC3C34B"; UpgradeCode = "AF16E7D3-800C-4BE7-BA12-B9C62FF78F51"; LangId = 2052; Comments = "OpenDCL Studio (Simplified Chinese)" }
  FRA = @{ ProductCode = "DE91BFF8-E532-4612-8899-F965FC87885C"; UpgradeCode = "140D3C25-5463-4688-8A52-FEA8E9FA4940"; LangId = 1036; Comments = "OpenDCL Studio (French)" }
  # Historical CHS/CHT shared UpgradeCode — preserved for upgrade continuity
  CHT = @{ ProductCode = "82CB8DF7-A50B-446D-82C6-362F009A6070"; UpgradeCode = "AF16E7D3-800C-4BE7-BA12-B9C62FF78F51"; LangId = 1028; Comments = "OpenDCL Studio (Traditional Chinese)" }
}

function Write-ComponentXml {
  param(
    [string] $ComponentId,
    [string] $Guid,
    [string] $DirectoryId,
    [string] $SourcePath,
    [string] $FileName,
    [switch] $KeyPath
  )
  $fileId = "fil_" + (Get-SafeId $ComponentId)
  $kp = if ($KeyPath) { ' KeyPath="yes"' } else { ' KeyPath="yes"' }
  @"
      <Component Id="$ComponentId" Guid="{$Guid}" Directory="$DirectoryId">
        <File Id="$fileId" Source="$SourcePath" Name="$FileName"$kp />
      </Component>
"@
}

function New-RuntimeFilesFragment {
  $sb = New-Object System.Text.StringBuilder
  [void]$sb.AppendLine('<?xml version="1.0" encoding="UTF-8"?>')
  [void]$sb.AppendLine('<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">')
  [void]$sb.AppendLine('  <Fragment>')
  [void]$sb.AppendLine('    <ComponentGroup Id="RuntimeModules">')

  foreach ($rel in $RuntimeModules) {
    $full = Join-Path $OpenDclRoot $rel
    Assert-File $full
    $name = [IO.Path]::GetFileName($full)
    $cid = "mod_" + (Get-SafeId $name)
    $guid = Get-StableGuid "opendcl.runtime.module|$name"
    $src = $full -replace '\\', '\\'
    # WiX Source can use single backslashes in XML; escape & only
    $srcXml = $full.Replace('&', '&amp;')
    [void]$sb.AppendLine((Write-ComponentXml -ComponentId $cid -Guid $guid -DirectoryId "OpenDCLFolder" -SourcePath $srcXml -FileName $name))
  }
  [void]$sb.AppendLine('    </ComponentGroup>')

  foreach ($lang in $RuntimeLangs) {
    [void]$sb.AppendLine("    <ComponentGroup Id=`"RuntimeLang$lang`">")
    $files = @(
      @{ Rel = "Runtime\Localized\$lang\Runtime.Res\Release\Runtime.Res.dll"; Name = "Runtime.Res.dll" },
      @{ Rel = "Runtime\Localized\$lang\Content\License.txt"; Name = "License.txt" },
      @{ Rel = "Runtime\Localized\$lang\Content\GNU-GPL.txt"; Name = "GNU-GPL.txt" }
    )
    foreach ($f in $files) {
      $full = Join-Path $OpenDclRoot $f.Rel
      Assert-File $full
      $cid = "rt_" + $lang + "_" + (Get-SafeId $f.Name)
      $guid = Get-StableGuid "opendcl.runtime.lang|$lang|$($f.Name)"
      $srcXml = $full.Replace('&', '&amp;')
      [void]$sb.AppendLine((Write-ComponentXml -ComponentId $cid -Guid $guid -DirectoryId "Lang$lang" -SourcePath $srcXml -FileName $f.Name))
    }
    [void]$sb.AppendLine('    </ComponentGroup>')
  }

  [void]$sb.AppendLine('  </Fragment>')
  [void]$sb.AppendLine('</Wix>')
  $path = Join-Path $GenDir "OpenDCL.Runtime.Files.wxs"
  [IO.File]::WriteAllText($path, $sb.ToString(), [Text.UTF8Encoding]::new($false))
  return $path
}

function Resolve-WixUiBitmaps {
  <#
    Use checked-in exact-size WixUI bitmaps under wix\ui\ (no runtime stretch of logos).
      WixUIBanner.bmp  493x58  — color logo at native size on the right
      WixUIDialog.bmp  493x312 — CAD-themed left strip (164x312 art), white right
  #>
  param(
    [string] $WixRoot,
    [string] $GenDir
  )
  $uiDir = Join-Path $WixRoot "ui"
  $bannerSrc = Join-Path $uiDir "WixUIBanner.bmp"
  $dialogSrc = Join-Path $uiDir "WixUIDialog.bmp"
  Assert-File $bannerSrc
  Assert-File $dialogSrc

  # Copy to gen for the light path (defines); leave originals untouched
  $bannerPath = Join-Path $GenDir "WixUIBanner.bmp"
  $dialogPath = Join-Path $GenDir "WixUIDialog.bmp"
  Copy-Item -Force $bannerSrc $bannerPath
  Copy-Item -Force $dialogSrc $dialogPath

  Add-Type -AssemblyName System.Drawing
  $b = [System.Drawing.Image]::FromFile($bannerPath)
  $d = [System.Drawing.Image]::FromFile($dialogPath)
  try {
    if ($b.Width -ne 493 -or $b.Height -ne 58) {
      throw "WixUIBanner.bmp must be 493x58 (got $($b.Width)x$($b.Height))"
    }
    if ($d.Width -ne 493 -or $d.Height -ne 312) {
      throw "WixUIDialog.bmp must be 493x312 (got $($d.Width)x$($d.Height))"
    }
  }
  finally {
    $b.Dispose()
    $d.Dispose()
  }
  Write-Host "  WixUI bitmaps (checked-in): $bannerPath ; $dialogPath"
  return @{ BannerBmp = $bannerPath; DialogBmp = $dialogPath }
}

function New-StudioFilesFragment([string] $lang) {
  $sb = New-Object System.Text.StringBuilder
  [void]$sb.AppendLine('<?xml version="1.0" encoding="UTF-8"?>')
  [void]$sb.AppendLine('<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">')
  [void]$sb.AppendLine('  <Fragment>')
  [void]$sb.AppendLine('    <ComponentGroup Id="StudioApp">')

  # Match legacy vdproj layout (and app path logic in GetLanguageSubfolderPath):
  #   {INSTALLDIR}\OpenDCL Studio.exe
  #   {INSTALLDIR}\{Lang}\Studio.Res.dll, License.*, CHM, Samples\...
  # Advertised shortcuts must live on the same Component as their File (ICE69).
  $appFiles = @(
    @{
      Full = Join-Path $OpenDclRoot "Studio\Win32\Release\OpenDCL Studio.exe"
      Dir = "INSTALLDIR"; Name = "OpenDCL Studio.exe"; FileId = "fil_StudioExe"
      Shortcuts = @(
        @{ Id = "sc_studio_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL Studio"; WorkingDirectory = "INSTALLDIR"; Icon = "OpenDCLStudio.exe"; IconIndex = 0 },
        @{ Id = "sc_studio_desktop"; Directory = "DesktopFolder"; Name = "OpenDCL Studio"; WorkingDirectory = "INSTALLDIR"; Icon = "OpenDCLStudio.exe"; IconIndex = 0 }
      )
    },
    @{ Full = Join-Path $OpenDclRoot "Studio\Localized\$lang\Studio.Res\Release\Studio.Res.dll"; Dir = "LangFolder"; Name = "Studio.Res.dll"; FileId = "fil_StudioRes" },
    @{
      Full = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\OpenDCL.chm"
      Dir = "LangFolder"; Name = "OpenDCL.chm"; FileId = "fil_StudioChm"
      Shortcuts = @(
        # Icon from Studio.Res.dll #16536 IDI_CONTROLBROWSER (extracted multi-res group)
        @{ Id = "sc_help_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL Help"; WorkingDirectory = "LangFolder"; Icon = "OpenDCLHelp.ico"; IconIndex = 0 }
      )
    },
    @{
      Full = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\License.txt"
      Dir = "LangFolder"; Name = "License.txt"; FileId = "fil_StudioLicenseTxt"
      Shortcuts = @(
        # Icon from Studio.Res.dll #16535 IDI_FORMEDITOR (extracted multi-res group)
        @{ Id = "sc_lic_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL License"; WorkingDirectory = "LangFolder"; Icon = "OpenDCLLicense.ico"; IconIndex = 0 }
      )
    },
    @{ Full = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\License.htm"; Dir = "LangFolder"; Name = "License.htm"; FileId = "fil_StudioLicenseHtm" },
    @{ Full = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\GNU-GPL.txt"; Dir = "LangFolder"; Name = "GNU-GPL.txt"; FileId = "fil_StudioGpl" }
  )

  foreach ($f in $appFiles) {
    Assert-File $f.Full
    $cid = "st_" + $lang + "_" + (Get-SafeId $f.Name)
    $guid = Get-StableGuid "opendcl.studio.app|$lang|$($f.Name)"
    $srcXml = $f.Full.Replace('&', '&amp;')
    [void]$sb.AppendLine("      <Component Id=`"$cid`" Guid=`"{$guid}`" Directory=`"$($f.Dir)`">")
    if ($f.Shortcuts) {
      [void]$sb.AppendLine("        <File Id=`"$($f.FileId)`" Source=`"$srcXml`" Name=`"$($f.Name)`" KeyPath=`"yes`">")
      foreach ($sc in $f.Shortcuts) {
        $wd = if ($sc.WorkingDirectory) { " WorkingDirectory=`"$($sc.WorkingDirectory)`"" } else { "" }
        # Advertised shortcuts need Icon= (Icon table PE/ICO stream); IconIndex selects the PE resource order
        $icon = ""
        if ($sc.Icon) {
          $idx = if ($null -ne $sc.IconIndex) { [int]$sc.IconIndex } else { 0 }
          $icon = " Icon=`"$($sc.Icon)`" IconIndex=`"$idx`""
        }
        [void]$sb.AppendLine("          <Shortcut Id=`"$($sc.Id)`" Directory=`"$($sc.Directory)`" Name=`"$($sc.Name)`"$wd$icon Advertise=`"yes`" />")
      }
      [void]$sb.AppendLine("        </File>")
    }
    else {
      [void]$sb.AppendLine("        <File Id=`"$($f.FileId)`" Source=`"$srcXml`" Name=`"$($f.Name)`" KeyPath=`"yes`" />")
    }
    [void]$sb.AppendLine("      </Component>")
  }

  # Start-menu folder cleanup only (HKCU keypath — not mixed with per-machine files)
  $menuGuid = Get-StableGuid "opendcl.studio.programmenu|$lang"
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_ProgramMenu`" Guid=`"{$menuGuid}`" Directory=`"ProgramMenuOpenDCL`">")
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL Consortium\OpenDCL Studio\$lang`" Name=`"ProgramMenu`" Type=`"integer`" Value=`"1`" KeyPath=`"yes`" />")
  [void]$sb.AppendLine('        <RemoveFolder Id="rm_ProgramMenuOpenDCL" Directory="ProgramMenuOpenDCL" On="uninstall" />')
  [void]$sb.AppendLine("      </Component>")

  # --- Studio registry parity with vdproj 9.3.3.1 ---
  # HKCR OpenDCL.Project is required: RxInstall IsStudioInstalled() gates OPENDCLDEMO demand-load.
  # Sequence is safe: WriteRegistryValues runs before deferred RxInstallMachine (after StartServices).
  $assocGuid = Get-StableGuid "opendcl.studio.fileassoc|$lang"
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_FileAssoc`" Guid=`"{$assocGuid}`" Directory=`"INSTALLDIR`">")
  # .odcl -> OpenDCL.Project (ProgID)
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key=".odcl" Type="string" Value="OpenDCL.Project" KeyPath="yes" />')
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="OpenDCL.Project" Type="string" Value="OpenDCL Project" />')
  # Paths use [INSTALLDIR] (not [#fil_StudioExe]) to avoid ICE69 cross-component refs; matches vdproj.
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="OpenDCL.Project\DefaultIcon" Type="string" Value="[INSTALLDIR]OpenDCL Studio.exe,0" />')
  # Shell open + DDE (literal [open("%1")] — escape MSI format brackets)
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="OpenDCL.Project\shell\open\command" Type="string" Value="&quot;[INSTALLDIR]OpenDCL Studio.exe&quot; &quot;%1&quot;" />')
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="OpenDCL.Project\shell\open\ddeexec" Type="string" Value="[\[]open(&quot;%1&quot;)[\]]" />')
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="OpenDCL.Project\shell\open\ddeexec\application" Type="string" Value="OPENDCL" />')
  # Context menu on AutoLISP files (vdproj DeleteAtUninstall)
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="AutoLISPFile\shell\decode" Type="string" Value="Open as Open&amp;DCL Project" />')
  [void]$sb.AppendLine('        <RegistryValue Root="HKCR" Key="AutoLISPFile\shell\decode\command" Type="string" Value="&quot;[INSTALLDIR]OpenDCL Studio.exe&quot; &quot;%1&quot;" />')
  [void]$sb.AppendLine('        <RemoveRegistryKey Root="HKCR" Key="AutoLISPFile\shell\decode" Action="removeOnUninstall" />')
  [void]$sb.AppendLine("      </Component>")

  # HKCU Software\OpenDCL app settings (language-specific paths; separate component from HKCR)
  $appRegGuid = Get-StableGuid "opendcl.studio.appsettings|$lang"
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_AppSettings`" Guid=`"{$appRegGuid}`" Directory=`"INSTALLDIR`">")
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL`" Name=`"Language`" Type=`"string`" Value=`"$lang`" KeyPath=`"yes`" />")
  [void]$sb.AppendLine('        <RegistryValue Root="HKCU" Key="Software\OpenDCL" Name="RootFolder" Type="string" Value="[INSTALLDIR]" />')
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL`" Name=`"SamplesFolder`" Type=`"string`" Value=`"[INSTALLDIR]$lang\Samples\`" />")
  [void]$sb.AppendLine('        <RegistryValue Root="HKCU" Key="Software\OpenDCL" Name="AutoUpdateCheck" Type="integer" Value="1" />')
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL\Runtime`" Name=`"Language`" Type=`"string`" Value=`"$lang`" />")
  [void]$sb.AppendLine('        <RemoveRegistryKey Root="HKCU" Key="Software\OpenDCL" Action="removeOnUninstall" />')
  [void]$sb.AppendLine("      </Component>")

  [void]$sb.AppendLine('    </ComponentGroup>')

  # Samples
  [void]$sb.AppendLine('    <ComponentGroup Id="StudioSamples">')
  $samplesDir = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\Samples"
  Assert-File $samplesDir
  $sampleFiles = Get-ChildItem -LiteralPath $samplesDir -File | Sort-Object Name
  foreach ($sf in $sampleFiles) {
    $cid = "smp_" + $lang + "_" + (Get-SafeId $sf.Name)
    # disambiguate if truncated
    $guid = Get-StableGuid "opendcl.studio.sample|$lang|$($sf.Name)"
    $fileId = "fil_" + (Get-SafeId $cid)
    # Ensure unique component ids when names collide after sanitize
    $srcXml = $sf.FullName.Replace('&', '&amp;')
    $nameXml = $sf.Name.Replace('&', '&amp;')
    [void]$sb.AppendLine("      <Component Id=`"$cid`" Guid=`"{$guid}`" Directory=`"SamplesFolder`">")
    [void]$sb.AppendLine("        <File Id=`"$fileId`" Source=`"$srcXml`" Name=`"$nameXml`" KeyPath=`"yes`" />")
    [void]$sb.AppendLine("      </Component>")
  }
  [void]$sb.AppendLine('    </ComponentGroup>')
  [void]$sb.AppendLine('  </Fragment>')
  [void]$sb.AppendLine('</Wix>')
  $path = Join-Path $GenDir "OpenDCL.Studio.$lang.Files.wxs"
  [IO.File]::WriteAllText($path, $sb.ToString(), [Text.UTF8Encoding]::new($false))
  return $path
}

function Invoke-CandleLight {
  param(
    [string[]] $WxsFiles,
    [string] $OutFile,
    [hashtable] $Defines,
    [string[]] $Extensions = @(),
    [string] $ObjectPrefix
  )
  $objDir = Join-Path $GenDir "obj"
  New-Item -ItemType Directory -Force -Path $objDir | Out-Null
  $defineArgs = @()
  foreach ($k in $Defines.Keys) {
    # Quote each -d so values with spaces (paths, ARP comments) survive
    $defineArgs += "-d$k=$($Defines[$k])"
  }
  $extArgs = @()
  foreach ($e in $Extensions) { $extArgs += "-ext"; $extArgs += $e }

  $wixobjs = @()
  foreach ($wxs in $WxsFiles) {
    $base = [IO.Path]::GetFileNameWithoutExtension($wxs)
    $wixobj = Join-Path $objDir "$ObjectPrefix$base.wixobj"
    Write-Host "  candle $base"
    $candleArgs = @("-nologo", "-out", $wixobj) + $defineArgs + $extArgs + @($wxs)
    & $Candle @candleArgs
    if ($LASTEXITCODE -ne 0) { throw "candle failed for $wxs" }
    $wixobjs += $wixobj
  }

  Write-Host "  light -> $OutFile"
  $lightArgs = @("-nologo", "-out", $OutFile) + $extArgs + $wixobjs
  # Suppress ICE noise common for merge modules / multi-lang packages
  $lightArgs += @("-sice:ICE03", "-sice:ICE82", "-sice:ICE61")
  & $Light @lightArgs
  if ($LASTEXITCODE -ne 0) { throw "light failed for $OutFile" }
}

Write-Host "=== OpenDCL WiX package ==="
Write-Host "OpenDclRoot     : $OpenDclRoot"
Write-Host "OutDir          : $OutDir"
Write-Host "ProductVersion  : $ProductVersion"
Write-Host "ModuleVersion   : $ModuleVersion"
Write-Host "WixBin          : $WixBin"
Write-Host ""

Assert-File (Join-Path $OpenDclRoot "Runtime\RxInstall\Release\RxInstall.dll")
Assert-File (Join-Path $OpenDclRoot "Runtime\Install\OpenDCL.ico")

# --- 1) Runtime MSM ---
$msmPath = Join-Path $OutDir "OpenDCL.Runtime.msm"
if ($SkipMsm) {
  if (-not (Test-Path $msmPath)) { throw "SkipMsm set but MSM not found: $msmPath" }
  Write-Host "==== Reusing OpenDCL.Runtime.msm ===="
  Write-Host "OK $msmPath ($([math]::Round((Get-Item $msmPath).Length/1MB,1)) MB)"
}
else {
  Write-Host "==== Building OpenDCL.Runtime.msm ===="
  $runtimeFilesWxs = New-RuntimeFilesFragment
  Invoke-CandleLight `
    -WxsFiles @((Join-Path $WixRoot "Runtime\OpenDCL.Runtime.wxs"), $runtimeFilesWxs) `
    -OutFile $msmPath `
    -ObjectPrefix "msm_" `
    -Defines @{
      OpenDclRoot    = $OpenDclRoot
      ModuleVersion  = $ModuleVersion
    }

  if (-not (Test-Path $msmPath)) { throw "MSM not produced: $msmPath" }
  Write-Host "OK $msmPath ($([math]::Round((Get-Item $msmPath).Length/1MB,1)) MB)"
}

# --- 2) Runtime MSI ---
if (-not $SkipRuntimeMsi) {
  Write-Host "==== Building OpenDCL.Runtime.msi ===="
  $runtimeMsi = Join-Path $OutDir "OpenDCL.Runtime.msi"
  Invoke-CandleLight `
    -WxsFiles @((Join-Path $WixRoot "Runtime\OpenDCL.Runtime.MSI.wxs")) `
    -OutFile $runtimeMsi `
    -ObjectPrefix "rtmsi_" `
    -Defines @{
      OpenDclRoot    = $OpenDclRoot
      ProductVersion = $ProductVersion
      MsmPath        = $msmPath
    }
  Write-Host "OK $runtimeMsi"
}

# --- 3) Studio language MSIs ---
if (-not $SkipStudio) {
  Assert-File (Join-Path $OpenDclRoot "Studio\Win32\Release\OpenDCL Studio.exe")
  Assert-File (Join-Path $OpenDclRoot "Studio\OpenDCL.ico")

  $uiBmps = Resolve-WixUiBitmaps -WixRoot $WixRoot -GenDir $GenDir
  $bannerBmp = $uiBmps.BannerBmp
  $dialogBmp = $uiBmps.DialogBmp

  foreach ($lang in $RuntimeLangs) {
    Write-Host "==== Building OpenDCL.Studio.$lang.msi ===="
    $meta = $StudioLangMeta[$lang]
    $filesWxs = New-StudioFilesFragment $lang
    # Checked-in RTF (not generated at package time) — reviewable installer EULA source
    $licenseRtf = Join-Path $OpenDclRoot "Studio\Localized\$lang\Content\License.rtf"
    Assert-File $licenseRtf

    # App icon from Studio.Res.dll #10; Help/License ICOs from wix\ui\icons\
    $studioResDll = Join-Path $OpenDclRoot "Studio\Localized\$lang\Studio.Res\Release\Studio.Res.dll"
    $iconStudio = Join-Path $GenDir "icons\$lang\StudioRes.App.ico"
    Export-StudioAppIcon -studioResDll $studioResDll -destIco $iconStudio | Out-Null
    $iconHelp = Join-Path $OpenDclRoot "wix\ui\icons\OpenDCLHelp.ico"
    $iconLicense = Join-Path $OpenDclRoot "wix\ui\icons\OpenDCLLicense.ico"
    Assert-File $iconHelp
    Assert-File $iconLicense
    Write-Host ("  icons: app={0:N0}b help={1:N0}b license={2:N0}b" -f `
      (Get-Item $iconStudio).Length, (Get-Item $iconHelp).Length, (Get-Item $iconLicense).Length)

    $studioMsi = Join-Path $OutDir "OpenDCL.Studio.$lang.msi"
    Invoke-CandleLight `
      -WxsFiles @((Join-Path $WixRoot "Studio\OpenDCL.Studio.wxs"), $filesWxs) `
      -OutFile $studioMsi `
      -ObjectPrefix "studio_${lang}_" `
      -Extensions @("WixUIExtension") `
      -Defines @{
        OpenDclRoot       = $OpenDclRoot
        ProductVersion    = $ProductVersion
        UpgradeCode       = $meta.UpgradeCode
        ProductLanguage   = $meta.LangId
        ArpComments       = $meta.Comments
        Lang              = $lang
        MsmPath           = $msmPath
        LicenseRtf        = $licenseRtf
        BannerBmp         = $bannerBmp
        DialogBmp         = $dialogBmp
        IconStudio        = $iconStudio
      }
    Write-Host "OK $studioMsi"
  }
}

Write-Host ""
Write-Host "WIX PACKAGE OK"
Write-Host "Outputs: $OutDir"
Get-ChildItem $OutDir -File | Where-Object { $_.Extension -match '\.(msm|msi)$' } |
  Select-Object Name, @{N = 'MB'; E = { [math]::Round($_.Length / 1MB, 1) } } |
  Format-Table -AutoSize
Write-Host "Next (manual today): versioned dist copy + code signing - see wix/README.md (post-package steps)."
