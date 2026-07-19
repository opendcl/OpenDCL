<#
.SYNOPSIS
  Build OpenDCL installers with WiX Toolset v3 (MSM + Runtime MSI + Studio MSIs).

.DESCRIPTION
  Supports two packaging modes:

  **Full product release** (default): every catalog runtime module + all UI
  languages. Missing Release outputs fail the build. MSM keeps the historical
  modularization GUID for third-party consumers.

  **Custom / subset**: package only chosen or available runtimes and language(s)
  (typical CMake dev flow: BRX.27 + ENU). Outputs use a ".custom" name suffix
  and distinct MSM/MSI identity GUIDs so they never collide with the ship MSM.

  Product files resolve classic paths first, then CMake out\ layout
  (see Resolve-ProductFile).

  Outputs (default under <repo>\wix\out\Release):
    OpenDCL.Runtime.msm[.custom]
    OpenDCL.Runtime.msi[.custom]
    OpenDCL.Studio.<LANG>.msi

.PARAMETER OpenDclRoot
  Path to compiled OpenDCL tree. Defaults to this repository root (where
  OpenDCL.sln lives).

.PARAMETER Configuration
  Build configuration folder to harvest (default Release).

.PARAMETER ProductVersion
  MSI 3-part ProductVersion. Default 10.1.101 encodes file version 10.1.1.1
  as major.minor.(patch*100+revision) for MSI upgrade comparison.

.PARAMETER ModuleVersion
  Merge module 4-part version (default 10.1.1.1).

.PARAMETER WixBin
  Path to candle.exe/light.exe directory. Auto-detected if empty.

.PARAMETER Languages
  UI language codes to package (ENU, DEU, …). Empty = all catalog languages
  for full mode; with -AvailableLanguages, empty means “every language that
  has Runtime.Res present”.

.PARAMETER Runtimes
  Runtime IDs and/or families to package, e.g. BRX.27.x64, ARX.26.x64, BRX, ARX.
  Empty with default -ModuleSet Full = entire catalog. Required for -ModuleSet Selected.

.PARAMETER ModuleSet
  Full      — all catalog modules (default); missing files fail.
  Selected  — only -Runtimes (each must resolve); missing files fail.
  Available — catalog (optionally filtered by -Runtimes) where files exist;
              skip missing modules.

.PARAMETER AvailableLanguages
  Include only languages whose Runtime.Res.dll resolves. When set without
  -Languages, scans all catalog languages.

.PARAMETER SkipStudio / SkipRuntimeMsi / SkipMsm
  Skip individual package steps.
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
  [string[]] $Runtimes = @(),
  [ValidateSet("Full", "Selected", "Available")]
  [string] $ModuleSet = "Full",
  [switch] $AvailableLanguages,
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

function Resolve-ProductFile([string] $rel) {
  <#
    Resolve a classic relative product path under $OpenDclRoot.
    Search order:
      1) OpenDclRoot\<rel>              classic tree or staged outputs
      2) OpenDclRoot\out\<rel>          CMake binary dir layout
      3) RepoRoot\<rel>                 source assets (licenses, icons) when
                                        -OpenDclRoot is a CMake build tree
  #>
  $candidates = [System.Collections.Generic.List[string]]::new()
  [void]$candidates.Add((Join-Path $OpenDclRoot $rel))
  [void]$candidates.Add((Join-Path $OpenDclRoot (Join-Path "out" $rel)))
  if ($RepoRoot -and ($RepoRoot -ne $OpenDclRoot)) {
    [void]$candidates.Add((Join-Path $RepoRoot $rel))
  }
  foreach ($c in $candidates) {
    if (Test-Path -LiteralPath $c) { return (Resolve-Path -LiteralPath $c).Path }
  }
  # Prefer reporting the classic path under OpenDclRoot in errors
  return $candidates[0]
}

function Test-ProductFile([string] $rel) {
  $full = Resolve-ProductFile $rel
  return (Test-Path -LiteralPath $full)
}

function Get-RuntimeIdFromRel([string] $rel) {
  # Runtime\BRX\BRX.27.x64\Release\OpenDCL.x64.27.brx → BRX.27.x64
  $parts = $rel -split '[\\/]'
  if ($parts.Count -ge 3) { return $parts[2] }
  return [IO.Path]::GetFileNameWithoutExtension($rel)
}

function Get-RuntimeFamilyFromId([string] $id) {
  if ($id -match '^(ARX|BRX|GRX|ZRX)') { return $Matches[1] }
  return ""
}

function Test-RuntimeSelector([string] $id, [string[]] $selectors) {
  if (-not $selectors -or $selectors.Count -eq 0) { return $true }
  $family = Get-RuntimeFamilyFromId $id
  foreach ($raw in $selectors) {
    $sel = "$raw".Trim()
    if (-not $sel) { continue }
    if ($sel -eq $id) { return $true }
    if ($sel -eq $family) { return $true }
    # Wildcard / -like patterns (e.g. BRX.2*, *.27.x64)
    if ($sel -match '[\*\?]' -and ($id -like $sel)) { return $true }
  }
  return $false
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

# --- Catalog of Runtime product files (intentional; no Detected Dependencies) ---
# Paths use {Config} for the harvest configuration (default Release).
$RuntimeModuleCatalogRels = @(
  # ARX
  "Runtime\ARX\ARX.16\{Config}\OpenDCL.16.arx",
  "Runtime\ARX\ARX.17\{Config}\OpenDCL.17.arx",
  "Runtime\ARX\ARX.17.x64\{Config}\OpenDCL.x64.17.arx",
  "Runtime\ARX\ARX.18\{Config}\OpenDCL.18.arx",
  "Runtime\ARX\ARX.18.x64\{Config}\OpenDCL.x64.18.arx",
  "Runtime\ARX\ARX.19\{Config}\OpenDCL.19.arx",
  "Runtime\ARX\ARX.19.x64\{Config}\OpenDCL.x64.19.arx",
  "Runtime\ARX\ARX.20\{Config}\OpenDCL.20.arx",
  "Runtime\ARX\ARX.20.x64\{Config}\OpenDCL.x64.20.arx",
  "Runtime\ARX\ARX.21\{Config}\OpenDCL.21.arx",
  "Runtime\ARX\ARX.21.x64\{Config}\OpenDCL.x64.21.arx",
  "Runtime\ARX\ARX.22\{Config}\OpenDCL.22.arx",
  "Runtime\ARX\ARX.22.x64\{Config}\OpenDCL.x64.22.arx",
  "Runtime\ARX\ARX.23\{Config}\OpenDCL.23.arx",
  "Runtime\ARX\ARX.23.x64\{Config}\OpenDCL.x64.23.arx",
  "Runtime\ARX\ARX.24.x64\{Config}\OpenDCL.x64.24.arx",
  "Runtime\ARX\ARX.25.x64\{Config}\OpenDCL.x64.25.arx",
  "Runtime\ARX\ARX.26.x64\{Config}\OpenDCL.x64.26.arx",
  # BRX
  "Runtime\BRX\BRX.9\{Config}\OpenDCL.9.brx",
  "Runtime\BRX\BRX.10\{Config}\OpenDCL.10.brx",
  "Runtime\BRX\BRX.11\{Config}\OpenDCL.11.brx",
  "Runtime\BRX\BRX.12\{Config}\OpenDCL.12.brx",
  "Runtime\BRX\BRX.13\{Config}\OpenDCL.13.brx",
  "Runtime\BRX\BRX.13.x64\{Config}\OpenDCL.x64.13.brx",
  "Runtime\BRX\BRX.14\{Config}\OpenDCL.14.brx",
  "Runtime\BRX\BRX.14.x64\{Config}\OpenDCL.x64.14.brx",
  "Runtime\BRX\BRX.15\{Config}\OpenDCL.15.brx",
  "Runtime\BRX\BRX.15.x64\{Config}\OpenDCL.x64.15.brx",
  "Runtime\BRX\BRX.16\{Config}\OpenDCL.16.brx",
  "Runtime\BRX\BRX.16.x64\{Config}\OpenDCL.x64.16.brx",
  "Runtime\BRX\BRX.17\{Config}\OpenDCL.17.brx",
  "Runtime\BRX\BRX.17.x64\{Config}\OpenDCL.x64.17.brx",
  "Runtime\BRX\BRX.18\{Config}\OpenDCL.18.brx",
  "Runtime\BRX\BRX.18.x64\{Config}\OpenDCL.x64.18.brx",
  "Runtime\BRX\BRX.19\{Config}\OpenDCL.19.brx",
  "Runtime\BRX\BRX.19.x64\{Config}\OpenDCL.x64.19.brx",
  "Runtime\BRX\BRX.20\{Config}\OpenDCL.20.brx",
  "Runtime\BRX\BRX.20.x64\{Config}\OpenDCL.x64.20.brx",
  "Runtime\BRX\BRX.21.x64\{Config}\OpenDCL.x64.21.brx",
  "Runtime\BRX\BRX.22.x64\{Config}\OpenDCL.x64.22.brx",
  "Runtime\BRX\BRX.23.x64\{Config}\OpenDCL.x64.23.brx",
  "Runtime\BRX\BRX.24.x64\{Config}\OpenDCL.x64.24.brx",
  "Runtime\BRX\BRX.25.x64\{Config}\OpenDCL.x64.25.brx",
  "Runtime\BRX\BRX.26.x64\{Config}\OpenDCL.x64.26.brx",
  "Runtime\BRX\BRX.27.x64\{Config}\OpenDCL.x64.27.brx",
  # GRX
  "Runtime\GRX\GRX.2015\{Config}\OpenDCL.2015.grx",
  "Runtime\GRX\GRX.2015.x64\{Config}\OpenDCL.x64.2015.grx",
  "Runtime\GRX\GRX.2016\{Config}\OpenDCL.2016.grx",
  "Runtime\GRX\GRX.2016.x64\{Config}\OpenDCL.x64.2016.grx",
  "Runtime\GRX\GRX.2017\{Config}\OpenDCL.2017.grx",
  "Runtime\GRX\GRX.2017.x64\{Config}\OpenDCL.x64.2017.grx",
  "Runtime\GRX\GRX.2018\{Config}\OpenDCL.2018.grx",
  "Runtime\GRX\GRX.2018.x64\{Config}\OpenDCL.x64.2018.grx",
  "Runtime\GRX\GRX.2019\{Config}\OpenDCL.2019.grx",
  "Runtime\GRX\GRX.2019.x64\{Config}\OpenDCL.x64.2019.grx",
  "Runtime\GRX\GRX.2020\{Config}\OpenDCL.2020.grx",
  "Runtime\GRX\GRX.2020.x64\{Config}\OpenDCL.x64.2020.grx",
  "Runtime\GRX\GRX.2021\{Config}\OpenDCL.2021.grx",
  "Runtime\GRX\GRX.2021.x64\{Config}\OpenDCL.x64.2021.grx",
  "Runtime\GRX\GRX.2022\{Config}\OpenDCL.2022.grx",
  "Runtime\GRX\GRX.2022.x64\{Config}\OpenDCL.x64.2022.grx",
  "Runtime\GRX\GRX.2023.x64\{Config}\OpenDCL.x64.2023.grx",
  "Runtime\GRX\GRX.2024.x64\{Config}\OpenDCL.x64.2024.grx",
  "Runtime\GRX\GRX.2025.x64\{Config}\OpenDCL.x64.2025.grx",
  "Runtime\GRX\GRX.2026.x64\{Config}\OpenDCL.x64.2026.grx",
  "Runtime\GRX\GRX.2027.x64\{Config}\OpenDCL.x64.2027.grx",
  # ZRX
  "Runtime\ZRX\ZRX.2014\{Config}\OpenDCL.2014.zrx",
  "Runtime\ZRX\ZRX.2015\{Config}\OpenDCL.2015.zrx",
  "Runtime\ZRX\ZRX.2017\{Config}\OpenDCL.2017.zrx",
  "Runtime\ZRX\ZRX.2017.x64\{Config}\OpenDCL.x64.2017.zrx",
  "Runtime\ZRX\ZRX.2018\{Config}\OpenDCL.2018.zrx",
  "Runtime\ZRX\ZRX.2018.x64\{Config}\OpenDCL.x64.2018.zrx",
  "Runtime\ZRX\ZRX.2019\{Config}\OpenDCL.2019.zrx",
  "Runtime\ZRX\ZRX.2019.x64\{Config}\OpenDCL.x64.2019.zrx",
  "Runtime\ZRX\ZRX.2020\{Config}\OpenDCL.2020.zrx",
  "Runtime\ZRX\ZRX.2020.x64\{Config}\OpenDCL.x64.2020.zrx",
  "Runtime\ZRX\ZRX.2021\{Config}\OpenDCL.2021.zrx",
  "Runtime\ZRX\ZRX.2021.x64\{Config}\OpenDCL.x64.2021.zrx",
  "Runtime\ZRX\ZRX.2022\{Config}\OpenDCL.2022.zrx",
  "Runtime\ZRX\ZRX.2022.x64\{Config}\OpenDCL.x64.2022.zrx",
  "Runtime\ZRX\ZRX.2023\{Config}\OpenDCL.2023.zrx",
  "Runtime\ZRX\ZRX.2023.x64\{Config}\OpenDCL.x64.2023.zrx",
  "Runtime\ZRX\ZRX.2025.x64\{Config}\OpenDCL.x64.2025.zrx"
)

$AllRuntimeLangs = @("ENU", "DEU", "ESM", "RUS", "CHS", "FRA", "CHT")

# Historical MSM modularization GUID — full product only (third-party merge identity).
$FullModulePackageId = "0C4E4759-A6C2-4D0E-BAE5-7719C3BCF049"
$FullRuntimeUpgradeCode = "A3675613-313B-47AC-8DD2-8707F33610AB"

$RuntimeModuleCatalog = @(
  foreach ($tmpl in $RuntimeModuleCatalogRels) {
    $rel = $tmpl.Replace("{Config}", $Configuration)
    $id = Get-RuntimeIdFromRel $rel
    [pscustomobject]@{
      Id     = $id
      Family = Get-RuntimeFamilyFromId $id
      Rel    = $rel
    }
  }
)

# --- Module selection ---
if ($ModuleSet -eq "Selected" -and (-not $Runtimes -or $Runtimes.Count -eq 0)) {
  throw "-ModuleSet Selected requires -Runtimes (e.g. -Runtimes BRX.27.x64,ARX.26.x64)."
}
if ($ModuleSet -eq "Full" -and $Runtimes -and $Runtimes.Count -gt 0) {
  # Convenience: -Runtimes without Selected implies Selected
  $ModuleSet = "Selected"
  Write-Host "Note: -Runtimes set; using ModuleSet=Selected"
}

$selectedCatalog = @($RuntimeModuleCatalog | Where-Object {
  Test-RuntimeSelector $_.Id $Runtimes
})

if ($ModuleSet -eq "Available") {
  $RuntimeModules = @(
    foreach ($m in $selectedCatalog) {
      if (Test-ProductFile $m.Rel) { $m.Rel }
    }
  )
  $skippedMods = @($selectedCatalog | Where-Object { -not (Test-ProductFile $_.Rel) } | ForEach-Object { $_.Id })
  if ($skippedMods.Count -gt 0) {
    Write-Host ("Available: skipping missing modules ({0}): {1}" -f $skippedMods.Count, ($skippedMods -join ", "))
  }
}
else {
  # Full or Selected — all selected must exist (assert later)
  $RuntimeModules = @($selectedCatalog | ForEach-Object { $_.Rel })
}

if ($RuntimeModules.Count -eq 0) {
  throw "No runtime modules selected (ModuleSet=$ModuleSet). Build modules or adjust -Runtimes."
}

# --- Language selection ---
if ($Languages -and $Languages.Count -gt 0) {
  $RuntimeLangs = @($Languages | ForEach-Object { "$_".ToUpperInvariant() })
  foreach ($lang in $RuntimeLangs) {
    if ($AllRuntimeLangs -notcontains $lang) {
      throw "Unknown language '$lang'. Catalog: $($AllRuntimeLangs -join ', ')"
    }
  }
}
else {
  $RuntimeLangs = @($AllRuntimeLangs)
}

if ($AvailableLanguages) {
  $presentLangs = @()
  foreach ($lang in $RuntimeLangs) {
    $resRel = "Runtime\Localized\$lang\Runtime.Res\$Configuration\Runtime.Res.dll"
    if (Test-ProductFile $resRel) { $presentLangs += $lang }
    else { Write-Host "AvailableLanguages: skip $lang (no Runtime.Res)" }
  }
  $RuntimeLangs = $presentLangs
  if ($RuntimeLangs.Count -eq 0) {
    throw "No languages with Runtime.Res found under OpenDclRoot."
  }
}

$isFullProduct = (
  $ModuleSet -eq "Full" -and
  $RuntimeModules.Count -eq $RuntimeModuleCatalog.Count -and
  $RuntimeLangs.Count -eq $AllRuntimeLangs.Count -and
  -not $AvailableLanguages
)

if ($isFullProduct) {
  $PackageSuffix = ""
  $ModulePackageId = $FullModulePackageId
  $RuntimeUpgradeCode = $FullRuntimeUpgradeCode
  $RuntimeProductName = "OpenDCL Runtime"
  $RuntimeArpComments = "OpenDCL Runtime"
}
else {
  $PackageSuffix = ".custom"
  $modSeed = ($selectedCatalog | Where-Object { $RuntimeModules -contains $_.Rel } | ForEach-Object { $_.Id } | Sort-Object) -join ","
  $langSeed = ($RuntimeLangs | Sort-Object) -join ","
  $ModulePackageId = Get-StableGuid "opendcl.runtime.msm.custom|mods=$modSeed|langs=$langSeed"
  $RuntimeUpgradeCode = Get-StableGuid "opendcl.runtime.msi.upgrade.custom|mods=$modSeed|langs=$langSeed"
  $RuntimeProductName = "OpenDCL Runtime (custom)"
  $RuntimeArpComments = "OpenDCL Runtime custom ($modSeed / $langSeed)"
}

$selectedIds = @(
  $RuntimeModuleCatalog |
    Where-Object { $RuntimeModules -contains $_.Rel } |
    ForEach-Object { $_.Id }
)

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
    $full = Resolve-ProductFile $rel
    Assert-File $full
    $name = [IO.Path]::GetFileName($full)
    $cid = "mod_" + (Get-SafeId $name)
    $guid = Get-StableGuid "opendcl.runtime.module|$name"
    # WiX Source can use single backslashes in XML; escape & only
    $srcXml = $full.Replace('&', '&amp;')
    [void]$sb.AppendLine((Write-ComponentXml -ComponentId $cid -Guid $guid -DirectoryId "OpenDCLFolder" -SourcePath $srcXml -FileName $name))
  }
  [void]$sb.AppendLine('    </ComponentGroup>')

  # Always emit ComponentGroups for every catalog language so Module ComponentGroupRefs stay valid.
  # Unselected languages get empty groups (no payload).
  foreach ($lang in $AllRuntimeLangs) {
    [void]$sb.AppendLine("    <ComponentGroup Id=`"RuntimeLang$lang`">")
    if ($RuntimeLangs -contains $lang) {
      $files = @(
        @{ Rel = "Runtime\Localized\$lang\Runtime.Res\$Configuration\Runtime.Res.dll"; Name = "Runtime.Res.dll" },
        @{ Rel = "Runtime\Localized\$lang\Content\License.txt"; Name = "License.txt" },
        @{ Rel = "Runtime\Localized\$lang\Content\GNU-GPL.txt"; Name = "GNU-GPL.txt" }
      )
      foreach ($f in $files) {
        $full = Resolve-ProductFile $f.Rel
        Assert-File $full
        $cid = "rt_" + $lang + "_" + (Get-SafeId $f.Name)
        $guid = Get-StableGuid "opendcl.runtime.lang|$lang|$($f.Name)"
        $srcXml = $full.Replace('&', '&amp;')
        [void]$sb.AppendLine((Write-ComponentXml -ComponentId $cid -Guid $guid -DirectoryId "Lang$lang" -SourcePath $srcXml -FileName $f.Name))
      }
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

function Test-PeIsX64([string] $pePath) {
  # Read COFF Machine from PE (0x8664 = AMD64). No hard-coded install paths.
  $fs = [IO.File]::OpenRead($pePath)
  try {
    $br = New-Object IO.BinaryReader $fs
    if ($br.ReadUInt16() -ne 0x5A4D) { return $false } # MZ
    $fs.Seek(0x3C, 'Begin') | Out-Null
    $peOff = $br.ReadInt32()
    $fs.Seek($peOff, 'Begin') | Out-Null
    if ($br.ReadUInt32() -ne 0x00004550) { return $false } # PE\0\0
    $machine = $br.ReadUInt16()
    return ($machine -eq 0x8664)
  }
  finally { $fs.Dispose() }
}

function Resolve-StudioExe([string] $configuration = "Release") {
  # Prefer x64 (modern CMake / ship), then classic Win32. Paths via Resolve-ProductFile.
  foreach ($plat in @("x64", "Win32")) {
    $rel = "Studio\$plat\$configuration\OpenDCL Studio.exe"
    $full = Resolve-ProductFile $rel
    if (Test-Path -LiteralPath $full) { return $full }
  }
  return (Resolve-ProductFile "Studio\x64\$configuration\OpenDCL Studio.exe")
}

function New-StudioFilesFragment([string] $lang) {
  $sb = New-Object System.Text.StringBuilder
  [void]$sb.AppendLine('<?xml version="1.0" encoding="UTF-8"?>')
  [void]$sb.AppendLine('<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">')
  [void]$sb.AppendLine('  <Fragment>')
  [void]$sb.AppendLine('    <ComponentGroup Id="StudioApp">')

  # Layout (paths are MSI [INSTALLDIR], never a hard-coded Program Files string):
  #   {INSTALLDIR}\OpenDCL Studio.exe
  #   {INSTALLDIR}\{Lang}\Studio.Res.dll, License.*, CHM, Samples\...
  # x64 PE → ProgramFiles64Folder + Win64 components; x86 PE → ProgramFilesFolder.
  # Advertised shortcuts must live on the same Component as their File (ICE69).
  $studioExe = Resolve-StudioExe $Configuration
  $script:StudioIsX64 = Test-PeIsX64 $studioExe
  $win64Attr = if ($script:StudioIsX64) { ' Win64="yes"' } else { '' }
  Write-Host ("  Studio PE: {0} ({1})" -f $studioExe, $(if ($script:StudioIsX64) { 'x64 -> ProgramFiles64' } else { 'x86 -> ProgramFiles' }))
  $appFiles = @(
    @{
      Full = $studioExe
      Dir = "INSTALLDIR"; Name = "OpenDCL Studio.exe"; FileId = "fil_StudioExe"
      Shortcuts = @(
        @{ Id = "sc_studio_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL Studio"; WorkingDirectory = "INSTALLDIR"; Icon = "OpenDCLStudio.exe"; IconIndex = 0 },
        @{ Id = "sc_studio_desktop"; Directory = "DesktopFolder"; Name = "OpenDCL Studio"; WorkingDirectory = "INSTALLDIR"; Icon = "OpenDCLStudio.exe"; IconIndex = 0 }
      )
    },
    @{ Full = (Resolve-ProductFile "Studio\Localized\$lang\Studio.Res\$Configuration\Studio.Res.dll"); Dir = "LangFolder"; Name = "Studio.Res.dll"; FileId = "fil_StudioRes" },
    @{
      Full = (Resolve-ProductFile "Studio\Localized\$lang\Content\OpenDCL.chm")
      Dir = "LangFolder"; Name = "OpenDCL.chm"; FileId = "fil_StudioChm"
      Shortcuts = @(
        # Icon from Studio.Res.dll #16536 IDI_CONTROLBROWSER (extracted multi-res group)
        @{ Id = "sc_help_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL Help"; WorkingDirectory = "LangFolder"; Icon = "OpenDCLHelp.ico"; IconIndex = 0 }
      )
    },
    @{
      Full = (Resolve-ProductFile "Studio\Localized\$lang\Content\License.txt")
      Dir = "LangFolder"; Name = "License.txt"; FileId = "fil_StudioLicenseTxt"
      Shortcuts = @(
        # Icon from Studio.Res.dll #16535 IDI_FORMEDITOR (extracted multi-res group)
        @{ Id = "sc_lic_menu"; Directory = "ProgramMenuOpenDCL"; Name = "OpenDCL License"; WorkingDirectory = "LangFolder"; Icon = "OpenDCLLicense.ico"; IconIndex = 0 }
      )
    },
    @{ Full = (Resolve-ProductFile "Studio\Localized\$lang\Content\License.htm"); Dir = "LangFolder"; Name = "License.htm"; FileId = "fil_StudioLicenseHtm" },
    @{ Full = (Resolve-ProductFile "Studio\Localized\$lang\Content\GNU-GPL.txt"); Dir = "LangFolder"; Name = "GNU-GPL.txt"; FileId = "fil_StudioGpl" }
  )

  foreach ($f in $appFiles) {
    Assert-File $f.Full
    $cid = "st_" + $lang + "_" + (Get-SafeId $f.Name)
    $guid = Get-StableGuid "opendcl.studio.app|$lang|$($f.Name)"
    $srcXml = $f.Full.Replace('&', '&amp;')
    [void]$sb.AppendLine("      <Component Id=`"$cid`" Guid=`"{$guid}`" Directory=`"$($f.Dir)`"$win64Attr>")
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
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_ProgramMenu`" Guid=`"{$menuGuid}`" Directory=`"ProgramMenuOpenDCL`"$win64Attr>")
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL Consortium\OpenDCL Studio\$lang`" Name=`"ProgramMenu`" Type=`"integer`" Value=`"1`" KeyPath=`"yes`" />")
  [void]$sb.AppendLine('        <RemoveFolder Id="rm_ProgramMenuOpenDCL" Directory="ProgramMenuOpenDCL" On="uninstall" />')
  [void]$sb.AppendLine("      </Component>")

  # --- Studio registry parity with vdproj 9.3.3.1 ---
  # HKCR OpenDCL.Project is required: RxInstall IsStudioInstalled() gates OPENDCLDEMO demand-load.
  # Sequence is safe: WriteRegistryValues runs before deferred RxInstallMachine (after StartServices).
  # All file paths use [INSTALLDIR] (never Program Files (x86)\… hard-coding).
  $assocGuid = Get-StableGuid "opendcl.studio.fileassoc|$lang"
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_FileAssoc`" Guid=`"{$assocGuid}`" Directory=`"INSTALLDIR`"$win64Attr>")
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
  [void]$sb.AppendLine("      <Component Id=`"st_${lang}_AppSettings`" Guid=`"{$appRegGuid}`" Directory=`"INSTALLDIR`"$win64Attr>")
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL`" Name=`"Language`" Type=`"string`" Value=`"$lang`" KeyPath=`"yes`" />")
  [void]$sb.AppendLine('        <RegistryValue Root="HKCU" Key="Software\OpenDCL" Name="RootFolder" Type="string" Value="[INSTALLDIR]" />')
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL`" Name=`"SamplesFolder`" Type=`"string`" Value=`"[INSTALLDIR]$lang\Samples\`" />")
  [void]$sb.AppendLine('        <RegistryValue Root="HKCU" Key="Software\OpenDCL" Name="AutoUpdateCheck" Type="integer" Value="1" />')
  [void]$sb.AppendLine("        <RegistryValue Root=`"HKCU`" Key=`"Software\OpenDCL\Runtime`" Name=`"Language`" Type=`"string`" Value=`"$lang`" />")
  [void]$sb.AppendLine('        <RemoveRegistryKey Root="HKCU" Key="Software\OpenDCL" Action="removeOnUninstall" />')
  [void]$sb.AppendLine("      </Component>")

  [void]$sb.AppendLine('    </ComponentGroup>')

  # Samples (source content; Resolve-ProductFile falls back to RepoRoot for CMake -OpenDclRoot)
  [void]$sb.AppendLine('    <ComponentGroup Id="StudioSamples">')
  $samplesDir = Resolve-ProductFile "Studio\Localized\$lang\Content\Samples"
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
    [void]$sb.AppendLine("      <Component Id=`"$cid`" Guid=`"{$guid}`" Directory=`"SamplesFolder`"$win64Attr>")
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
Write-Host "ModuleSet       : $ModuleSet"
Write-Host "Full product    : $isFullProduct"
Write-Host "Package suffix  : $(if ($PackageSuffix) { $PackageSuffix } else { '(none — ship identity)' })"
Write-Host "Runtimes ($($selectedIds.Count)) : $($selectedIds -join ', ')"
Write-Host "Languages       : $($RuntimeLangs -join ', ')"
Write-Host "ModulePackageId : $ModulePackageId"
Write-Host ""

$RxInstallDll = Resolve-ProductFile "Runtime\RxInstall\$Configuration\RxInstall.dll"
Assert-File $RxInstallDll
# Source tree assets always live under the packaging repo (not the CMake binary dir).
Assert-File (Join-Path $RepoRoot "Runtime\Install\OpenDCL.ico")
Write-Host "RxInstall.dll   : $RxInstallDll"

# --- 1) Runtime MSM ---
$msmName = "OpenDCL.Runtime$PackageSuffix.msm"
$msmPath = Join-Path $OutDir $msmName
if ($SkipMsm) {
  if (-not (Test-Path $msmPath)) { throw "SkipMsm set but MSM not found: $msmPath" }
  Write-Host "==== Reusing $msmName ===="
  Write-Host "OK $msmPath ($([math]::Round((Get-Item $msmPath).Length/1MB,1)) MB)"
}
else {
  Write-Host "==== Building $msmName ===="
  $runtimeFilesWxs = New-RuntimeFilesFragment
  Invoke-CandleLight `
    -WxsFiles @((Join-Path $WixRoot "Runtime\OpenDCL.Runtime.wxs"), $runtimeFilesWxs) `
    -OutFile $msmPath `
    -ObjectPrefix "msm_" `
    -Defines @{
      OpenDclRoot      = $OpenDclRoot
      ModuleVersion    = $ModuleVersion
      RxInstallDll     = $RxInstallDll
      ModulePackageId  = $ModulePackageId
      RuntimeProductName = $RuntimeProductName
    }

  if (-not (Test-Path $msmPath)) { throw "MSM not produced: $msmPath" }
  Write-Host "OK $msmPath ($([math]::Round((Get-Item $msmPath).Length/1MB,1)) MB)"
}

# --- 2) Runtime MSI ---
if (-not $SkipRuntimeMsi) {
  $msiName = "OpenDCL.Runtime$PackageSuffix.msi"
  Write-Host "==== Building $msiName ===="
  $runtimeMsi = Join-Path $OutDir $msiName
  # Icon path: prefer OpenDclRoot, fall back to packaging repo (CMake binary dir as root).
  $runtimeIco = Join-Path $RepoRoot "Runtime\Install\OpenDCL.ico"
  Invoke-CandleLight `
    -WxsFiles @((Join-Path $WixRoot "Runtime\OpenDCL.Runtime.MSI.wxs")) `
    -OutFile $runtimeMsi `
    -ObjectPrefix "rtmsi_" `
    -Defines @{
      OpenDclRoot         = $OpenDclRoot
      ProductVersion      = $ProductVersion
      MsmPath             = $msmPath
      RuntimeUpgradeCode  = $RuntimeUpgradeCode
      RuntimeProductName  = $RuntimeProductName
      RuntimeArpComments  = $RuntimeArpComments
      RuntimeIcon         = $runtimeIco
    }
  Write-Host "OK $runtimeMsi"
}

# --- 3) Studio language MSIs ---
if (-not $SkipStudio) {
  $studioExeForPkg = Resolve-StudioExe $Configuration
  Assert-File $studioExeForPkg
  $studioIsX64 = Test-PeIsX64 $studioExeForPkg
  $studioPlatform = if ($studioIsX64) { "x64" } else { "x86" }
  $pfDir = if ($studioIsX64) { "ProgramFiles64Folder" } else { "ProgramFilesFolder" }
  Write-Host ("Studio package platform: {0} ({1})" -f $studioPlatform, $pfDir)

  $studioIco = Join-Path $RepoRoot "Studio\OpenDCL.ico"
  if (-not (Test-Path -LiteralPath $studioIco)) {
    $studioIco = Resolve-ProductFile "Studio\OpenDCL.ico"
  }
  Assert-File $studioIco

  $uiBmps = Resolve-WixUiBitmaps -WixRoot $WixRoot -GenDir $GenDir
  $bannerBmp = $uiBmps.BannerBmp
  $dialogBmp = $uiBmps.DialogBmp

  foreach ($lang in $RuntimeLangs) {
    Write-Host "==== Building OpenDCL.Studio.$lang.msi ===="
    $meta = $StudioLangMeta[$lang]
    $filesWxs = New-StudioFilesFragment $lang
    # Checked-in RTF (not generated at package time) — reviewable installer EULA source
    $licenseRtf = Resolve-ProductFile "Studio\Localized\$lang\Content\License.rtf"
    Assert-File $licenseRtf

    # App icon from Studio.Res.dll #10; Help/License ICOs from wix\ui\icons\ (RepoRoot)
    $studioResDll = Resolve-ProductFile "Studio\Localized\$lang\Studio.Res\$Configuration\Studio.Res.dll"
    Assert-File $studioResDll
    $iconStudio = Join-Path $GenDir "icons\$lang\StudioRes.App.ico"
    Export-StudioAppIcon -studioResDll $studioResDll -destIco $iconStudio | Out-Null
    $iconHelp = Join-Path $RepoRoot "wix\ui\icons\OpenDCLHelp.ico"
    $iconLicense = Join-Path $RepoRoot "wix\ui\icons\OpenDCLLicense.ico"
    if (-not (Test-Path -LiteralPath $iconHelp)) {
      $iconHelp = Resolve-ProductFile "wix\ui\icons\OpenDCLHelp.ico"
    }
    if (-not (Test-Path -LiteralPath $iconLicense)) {
      $iconLicense = Resolve-ProductFile "wix\ui\icons\OpenDCLLicense.ico"
    }
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
        IconHelp          = $iconHelp
        IconLicense       = $iconLicense
        StudioPlatform    = $studioPlatform
        PfDir             = $pfDir
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
