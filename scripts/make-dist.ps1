<#
.SYNOPSIS
  Versioned rename/copy of WiX installer outputs (replaces Build 9.0\!MakeNewDist.bat copy steps).

.DESCRIPTION
  Reads MSIs/MSM from wix\out\<Configuration>\ and writes:

    OpenDCL.Runtime.<ver>.msi
    OpenDCL.Runtime.<ver>.msm
    OpenDCL.Studio.<LANG>.<ver>.msi

  Default destination is <repo>\dist\<ver>\ (gitignored-friendly). Historical
  process copied next to a parent "OpenDCL" folder; pass -DestDir for that.

.PARAMETER ProductVersion
  Four-part file version used in filenames (e.g. 10.1.1.1). If empty, tries
  scripts/build-wix.ps1 defaults / Runtime ARX.rc is not parsed here — pass explicitly.

.PARAMETER Configuration
  WiX out folder name under wix\out\ (default Release).

.PARAMETER DestDir
  Output directory for versioned packages.

.PARAMETER SourceDir
  Directory containing unversioned WiX outputs. Default: wix\out\<Configuration>
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [string] $ProductVersion = "10.1.1.1",
  [string] $Configuration = "Release",
  [string] $DestDir = "",
  [string] $SourceDir = ""
)

$ErrorActionPreference = "Stop"

if (-not $OpenDclRoot) {
  $OpenDclRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path

if (-not $SourceDir) {
  $SourceDir = Join-Path $OpenDclRoot "wix\out\$Configuration"
}
if (-not (Test-Path $SourceDir)) {
  throw "WiX output dir not found: $SourceDir (run scripts\build-wix.ps1 first)"
}

if (-not $DestDir) {
  $DestDir = Join-Path $OpenDclRoot "dist\$ProductVersion"
}
New-Item -ItemType Directory -Force -Path $DestDir | Out-Null

$langs = @("ENU", "DEU", "ESM", "RUS", "CHS", "FRA", "CHT")
$copied = @()

function Copy-Versioned([string] $srcName, [string] $destName) {
  $src = Join-Path $SourceDir $srcName
  if (-not (Test-Path $src)) {
    Write-Warning "Missing (skip): $src"
    return
  }
  $dest = Join-Path $DestDir $destName
  Copy-Item -LiteralPath $src -Destination $dest -Force
  $script:copied += $dest
  Write-Host "  $destName  ($([math]::Round((Get-Item $dest).Length / 1MB, 1)) MB)"
}

Write-Host "=== make-dist ==="
Write-Host "Source: $SourceDir"
Write-Host "Dest:   $DestDir"
Write-Host "Ver:    $ProductVersion"

# Full ship identity
Copy-Versioned "OpenDCL.Runtime.msi" "OpenDCL.Runtime.$ProductVersion.msi"
Copy-Versioned "OpenDCL.Runtime.msm" "OpenDCL.Runtime.$ProductVersion.msm"
# Custom subset packages (build-wix -ModuleSet Selected|Available)
Copy-Versioned "OpenDCL.Runtime.custom.msi" "OpenDCL.Runtime.custom.$ProductVersion.msi"
Copy-Versioned "OpenDCL.Runtime.custom.msm" "OpenDCL.Runtime.custom.$ProductVersion.msm"
foreach ($lang in $langs) {
  Copy-Versioned "OpenDCL.Studio.$lang.msi" "OpenDCL.Studio.$lang.$ProductVersion.msi"
}

if ($copied.Count -eq 0) {
  throw "No packages copied from $SourceDir"
}

Write-Host "OK $($copied.Count) file(s) -> $DestDir"
return $DestDir
