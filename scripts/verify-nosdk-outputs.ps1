<#
.SYNOPSIS
  Verify no-CAD-SDK (nosdk) preset outputs for Studio / Res / RxInstall / host libs.

.DESCRIPTION
  Gate for vs2022-nosdk / vs2022-nosdk-x64 PR builds. Does NOT use
  verify-build-outputs.ps1 Full/Available (those expect CAD modules).

  Expected Release outs under -OpenDclRoot (CMake binary dir with out\):
    - OpenDCL Studio.exe (host PE: Win32 or x64)
    - Studio.Res.dll (ENU; host Studio.Res path)
    - Runtime.Res.dll (ENU; classic_x86 from native Win32 or Res_Win32 on x64)
    - RxInstall.dll (x86 MSI CA; native on Win32, private rxinstall-win32 on x64)
    - Host zlib/png libs under out\Library\

  CHM / Studio help is NOT required (OPENDCL_BUILD_STUDIO_HELP=OFF on nosdk).

.PARAMETER OpenDclRoot
  CMake binary dir (e.g. build\vs2022-nosdk or build\vs2022-nosdk-x64).

.PARAMETER Arch
  Win32 or x64 — selects Studio PE path expectations.

.PARAMETER Configuration
  Config folder name (default Release).
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [string] $OpenDclRoot,
  [ValidateSet("Win32", "x64")]
  [string] $Arch = "Win32",
  [string] $Configuration = "Release",
  [switch] $Quiet
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $OpenDclRoot)) {
  throw "OpenDclRoot not found: $OpenDclRoot"
}
$OpenDclRoot = (Resolve-Path -LiteralPath $OpenDclRoot).Path

function Resolve-ProductFile([string] $rel) {
  $candidates = @(
    (Join-Path $OpenDclRoot $rel),
    (Join-Path $OpenDclRoot (Join-Path "out" $rel))
  )
  foreach ($c in $candidates) {
    if (Test-Path -LiteralPath $c) { return (Resolve-Path -LiteralPath $c).Path }
  }
  return $null
}

function Test-Rel([string] $rel) { $null -ne (Resolve-ProductFile $rel) }

$missing = New-Object System.Collections.Generic.List[string]
$ok = New-Object System.Collections.Generic.List[string]

function Require-OneOf([string] $label, [string[]] $rels) {
  foreach ($r in $rels) {
    if (Test-Rel $r) {
      [void]$ok.Add("$label => $r")
      return
    }
  }
  [void]$missing.Add("$label (tried: $($rels -join '; '))")
}

if ($Arch -eq "Win32") {
  Require-OneOf "Studio.exe" @(
    "Studio\Win32\$Configuration\OpenDCL Studio.exe",
    "Studio\$Configuration\OpenDCL Studio.exe"
  )
  Require-OneOf "Studio.Res.ENU" @(
    "Studio\Localized\ENU\Studio.Res\$Configuration\Studio.Res.dll",
    "Studio\Localized\ENU\Studio.Res\Win32\$Configuration\Studio.Res.dll"
  )
} else {
  Require-OneOf "Studio.exe" @(
    "Studio\x64\$Configuration\OpenDCL Studio.exe",
    "Studio\$Configuration\OpenDCL Studio.exe"
  )
  Require-OneOf "Studio.Res.ENU" @(
    "Studio\Localized\ENU\Studio.Res\$Configuration\Studio.Res.dll"
  )
}

Require-OneOf "Runtime.Res.ENU" @(
  "Runtime\Localized\ENU\Runtime.Res\$Configuration\Runtime.Res.dll"
)

Require-OneOf "RxInstall.dll" @(
  "Runtime\RxInstall\$Configuration\RxInstall.dll"
)

$libArch = if ($Arch -eq "Win32") { "x86" } else { "x64" }
$libHit = $false
$libRoots = @(
  (Join-Path $OpenDclRoot "out\Library"),
  (Join-Path $OpenDclRoot "Library")
)
foreach ($lr in $libRoots) {
  if (-not (Test-Path -LiteralPath $lr)) { continue }
  $anyLib = @(Get-ChildItem -LiteralPath $lr -Recurse -Filter "*.lib" -File -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -match [regex]::Escape($libArch) })
  if ($anyLib.Count -gt 0) {
    $libHit = $true
    [void]$ok.Add("Library($libArch) => $($anyLib.Count) .lib under $lr")
    break
  }
}
if (-not $libHit) {
  [void]$missing.Add("Library host libs ($libArch) under out\Library")
}

if (-not $Quiet) {
  Write-Host "=== verify-nosdk-outputs ==="
  Write-Host "OpenDclRoot : $OpenDclRoot"
  Write-Host "Arch        : $Arch"
  Write-Host "Config      : $Configuration"
  foreach ($line in $ok) { Write-Host "  OK   $line" }
  foreach ($line in $missing) { Write-Host "  MISS $line" }
}

if ($missing.Count -gt 0) {
  Write-Host "VERIFY FAILED - nosdk outputs incomplete." -ForegroundColor Red
  exit 1
}
Write-Host "VERIFY OK - nosdk outputs present." -ForegroundColor Green
exit 0
