<#
.SYNOPSIS
  Verify product files needed for WiX packaging exist under -OpenDclRoot before packaging.

.DESCRIPTION
  Uses the same product relative paths as build-wix.ps1 (OpenDclRoot, out\, win32\out\,
  packaging repo for source assets).

  Exit 0 if requirements for -ModuleSet are met; non-zero otherwise. Always prints a summary.

.PARAMETER OpenDclRoot
  Compiled product root (CMake binary dir with out\, or classic tree).

.PARAMETER ModuleSet
  Full — every catalog runtime module + all Runtime.Res langs + RxInstall + Studio (unless -SkipStudio).
  Available — at least one runtime module; report missing; fail only if zero modules or required Res empty.
  Selected — every -Runtimes id must resolve.

.PARAMETER Configuration
  Release / Debug / FullDebug folder name (default Release).

.PARAMETER SkipStudio
  Do not require Studio.exe / Studio.Res / CHM.
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [ValidateSet("Full", "Selected", "Available")]
  [string] $ModuleSet = "Full",
  [string] $Configuration = "Release",
  [string[]] $Runtimes = @(),
  [string[]] $Languages = @(),
  [switch] $SkipStudio,
  [switch] $Quiet
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if (-not $OpenDclRoot) {
  foreach ($c in @(
      (Join-Path $RepoRoot "build\vs2022-full"),
      (Join-Path $RepoRoot "build\vs2022-x64-full"),
      $RepoRoot
    )) {
    if (Test-Path $c) { $OpenDclRoot = $c; break }
  }
}
if (-not $OpenDclRoot -or -not (Test-Path $OpenDclRoot)) {
  throw "Pass -OpenDclRoot to a compiled tree (e.g. build\vs2022-full)."
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path

function Resolve-ProductFile([string] $rel) {
  $candidates = @(
    (Join-Path $OpenDclRoot $rel),
    (Join-Path $OpenDclRoot (Join-Path "out" $rel)),
    (Join-Path $OpenDclRoot (Join-Path "win32\out" $rel))
  )
  if ($RepoRoot -ne $OpenDclRoot) {
    $candidates += (Join-Path $RepoRoot $rel)
  }
  foreach ($c in $candidates) {
    if (Test-Path -LiteralPath $c) { return (Resolve-Path -LiteralPath $c).Path }
  }
  return $null
}

function Test-Rel([string] $rel) { $null -ne (Resolve-ProductFile $rel) }

# Keep in sync with build-wix.ps1 $RuntimeModuleCatalogRels (Release substitution).
$catalogRels = @(
  "Runtime\ARX\ARX.16\{0}\OpenDCL.16.arx",
  "Runtime\ARX\ARX.17\{0}\OpenDCL.17.arx",
  "Runtime\ARX\ARX.17.x64\{0}\OpenDCL.x64.17.arx",
  "Runtime\ARX\ARX.18\{0}\OpenDCL.18.arx",
  "Runtime\ARX\ARX.18.x64\{0}\OpenDCL.x64.18.arx",
  "Runtime\ARX\ARX.19\{0}\OpenDCL.19.arx",
  "Runtime\ARX\ARX.19.x64\{0}\OpenDCL.x64.19.arx",
  "Runtime\ARX\ARX.20\{0}\OpenDCL.20.arx",
  "Runtime\ARX\ARX.20.x64\{0}\OpenDCL.x64.20.arx",
  "Runtime\ARX\ARX.21\{0}\OpenDCL.21.arx",
  "Runtime\ARX\ARX.21.x64\{0}\OpenDCL.x64.21.arx",
  "Runtime\ARX\ARX.22\{0}\OpenDCL.22.arx",
  "Runtime\ARX\ARX.22.x64\{0}\OpenDCL.x64.22.arx",
  "Runtime\ARX\ARX.23\{0}\OpenDCL.23.arx",
  "Runtime\ARX\ARX.23.x64\{0}\OpenDCL.x64.23.arx",
  "Runtime\ARX\ARX.24.x64\{0}\OpenDCL.x64.24.arx",
  "Runtime\ARX\ARX.25.x64\{0}\OpenDCL.x64.25.arx",
  "Runtime\ARX\ARX.26.x64\{0}\OpenDCL.x64.26.arx",
  "Runtime\BRX\BRX.9\{0}\OpenDCL.9.brx",
  "Runtime\BRX\BRX.10\{0}\OpenDCL.10.brx",
  "Runtime\BRX\BRX.11\{0}\OpenDCL.11.brx",
  "Runtime\BRX\BRX.12\{0}\OpenDCL.12.brx",
  "Runtime\BRX\BRX.13\{0}\OpenDCL.13.brx",
  "Runtime\BRX\BRX.13.x64\{0}\OpenDCL.x64.13.brx",
  "Runtime\BRX\BRX.14\{0}\OpenDCL.14.brx",
  "Runtime\BRX\BRX.14.x64\{0}\OpenDCL.x64.14.brx",
  "Runtime\BRX\BRX.15\{0}\OpenDCL.15.brx",
  "Runtime\BRX\BRX.15.x64\{0}\OpenDCL.x64.15.brx",
  "Runtime\BRX\BRX.16\{0}\OpenDCL.16.brx",
  "Runtime\BRX\BRX.16.x64\{0}\OpenDCL.x64.16.brx",
  "Runtime\BRX\BRX.17\{0}\OpenDCL.17.brx",
  "Runtime\BRX\BRX.17.x64\{0}\OpenDCL.x64.17.brx",
  "Runtime\BRX\BRX.18\{0}\OpenDCL.18.brx",
  "Runtime\BRX\BRX.18.x64\{0}\OpenDCL.x64.18.brx",
  "Runtime\BRX\BRX.19\{0}\OpenDCL.19.brx",
  "Runtime\BRX\BRX.19.x64\{0}\OpenDCL.x64.19.brx",
  "Runtime\BRX\BRX.20\{0}\OpenDCL.20.brx",
  "Runtime\BRX\BRX.20.x64\{0}\OpenDCL.x64.20.brx",
  "Runtime\BRX\BRX.21.x64\{0}\OpenDCL.x64.21.brx",
  "Runtime\BRX\BRX.22.x64\{0}\OpenDCL.x64.22.brx",
  "Runtime\BRX\BRX.23.x64\{0}\OpenDCL.x64.23.brx",
  "Runtime\BRX\BRX.24.x64\{0}\OpenDCL.x64.24.brx",
  "Runtime\BRX\BRX.25.x64\{0}\OpenDCL.x64.25.brx",
  "Runtime\BRX\BRX.26.x64\{0}\OpenDCL.x64.26.brx",
  "Runtime\BRX\BRX.27.x64\{0}\OpenDCL.x64.27.brx",
  "Runtime\GRX\GRX.2015\{0}\OpenDCL.2015.grx",
  "Runtime\GRX\GRX.2015.x64\{0}\OpenDCL.x64.2015.grx",
  "Runtime\GRX\GRX.2016\{0}\OpenDCL.2016.grx",
  "Runtime\GRX\GRX.2016.x64\{0}\OpenDCL.x64.2016.grx",
  "Runtime\GRX\GRX.2017\{0}\OpenDCL.2017.grx",
  "Runtime\GRX\GRX.2017.x64\{0}\OpenDCL.x64.2017.grx",
  "Runtime\GRX\GRX.2018\{0}\OpenDCL.2018.grx",
  "Runtime\GRX\GRX.2018.x64\{0}\OpenDCL.x64.2018.grx",
  "Runtime\GRX\GRX.2019\{0}\OpenDCL.2019.grx",
  "Runtime\GRX\GRX.2019.x64\{0}\OpenDCL.x64.2019.grx",
  "Runtime\GRX\GRX.2020\{0}\OpenDCL.2020.grx",
  "Runtime\GRX\GRX.2020.x64\{0}\OpenDCL.x64.2020.grx",
  "Runtime\GRX\GRX.2021\{0}\OpenDCL.2021.grx",
  "Runtime\GRX\GRX.2021.x64\{0}\OpenDCL.x64.2021.grx",
  "Runtime\GRX\GRX.2022\{0}\OpenDCL.2022.grx",
  "Runtime\GRX\GRX.2022.x64\{0}\OpenDCL.x64.2022.grx",
  "Runtime\GRX\GRX.2023.x64\{0}\OpenDCL.x64.2023.grx",
  "Runtime\GRX\GRX.2024.x64\{0}\OpenDCL.x64.2024.grx",
  "Runtime\GRX\GRX.2025.x64\{0}\OpenDCL.x64.2025.grx",
  "Runtime\GRX\GRX.2026.x64\{0}\OpenDCL.x64.2026.grx",
  "Runtime\GRX\GRX.2027.x64\{0}\OpenDCL.x64.2027.grx",
  "Runtime\ZRX\ZRX.2014\{0}\OpenDCL.2014.zrx",
  "Runtime\ZRX\ZRX.2015\{0}\OpenDCL.2015.zrx",
  "Runtime\ZRX\ZRX.2017\{0}\OpenDCL.2017.zrx",
  "Runtime\ZRX\ZRX.2017.x64\{0}\OpenDCL.x64.2017.zrx",
  "Runtime\ZRX\ZRX.2018\{0}\OpenDCL.2018.zrx",
  "Runtime\ZRX\ZRX.2018.x64\{0}\OpenDCL.x64.2018.zrx",
  "Runtime\ZRX\ZRX.2019\{0}\OpenDCL.2019.zrx",
  "Runtime\ZRX\ZRX.2019.x64\{0}\OpenDCL.x64.2019.zrx",
  "Runtime\ZRX\ZRX.2020\{0}\OpenDCL.2020.zrx",
  "Runtime\ZRX\ZRX.2020.x64\{0}\OpenDCL.x64.2020.zrx",
  "Runtime\ZRX\ZRX.2021\{0}\OpenDCL.2021.zrx",
  "Runtime\ZRX\ZRX.2021.x64\{0}\OpenDCL.x64.2021.zrx",
  "Runtime\ZRX\ZRX.2022\{0}\OpenDCL.2022.zrx",
  "Runtime\ZRX\ZRX.2022.x64\{0}\OpenDCL.x64.2022.zrx",
  "Runtime\ZRX\ZRX.2023\{0}\OpenDCL.2023.zrx",
  "Runtime\ZRX\ZRX.2023.x64\{0}\OpenDCL.x64.2023.zrx",
  "Runtime\ZRX\ZRX.2025.x64\{0}\OpenDCL.x64.2025.zrx"
) | ForEach-Object { $_ -f $Configuration }

function Get-RuntimeIdFromRel([string] $rel) {
  $parts = $rel -split '[\\/]'
  if ($parts.Count -ge 3) { return $parts[2] }
  return [IO.Path]::GetFileNameWithoutExtension($rel)
}

function Test-RuntimeSelector([string] $id, [string[]] $selectors) {
  if (-not $selectors -or $selectors.Count -eq 0) { return $true }
  $family = if ($id -match '^(ARX|BRX|GRX|ZRX)') { $Matches[1] } else { "" }
  foreach ($raw in $selectors) {
    $sel = "$raw".Trim()
    if (-not $sel) { continue }
    if ($sel -eq $id -or $sel -eq $family) { return $true }
    if ($sel -match '[\*\?]' -and ($id -like $sel)) { return $true }
  }
  return $false
}

$AllLangs = @("ENU", "DEU", "ESM", "RUS", "CHS", "FRA", "CHT")
$langs = if ($Languages -and $Languages.Count -gt 0) {
  @($Languages | ForEach-Object { "$_".ToUpperInvariant() })
} else { $AllLangs }

$selected = @($catalogRels | Where-Object {
  Test-RuntimeSelector (Get-RuntimeIdFromRel $_) $Runtimes
})

$present = @()
$missing = @()
foreach ($rel in $selected) {
  $id = Get-RuntimeIdFromRel $rel
  if (Test-Rel $rel) { $present += $id } else { $missing += $id }
}

$rx = "Runtime\RxInstall\$Configuration\RxInstall.dll"
$rxOk = Test-Rel $rx

$resOk = @()
$resMissing = @()
foreach ($lang in $langs) {
  $rel = "Runtime\Localized\$lang\Runtime.Res\$Configuration\Runtime.Res.dll"
  if (Test-Rel $rel) { $resOk += $lang } else { $resMissing += $lang }
}

$studioIssues = @()
if (-not $SkipStudio) {
  $studioCandidates = @(
    "Studio\x64\$Configuration\OpenDCL Studio.exe",
    "Studio\Win32\$Configuration\OpenDCL Studio.exe",
    "Studio\$Configuration\OpenDCL Studio.exe"
  )
  $studioHit = $false
  foreach ($s in $studioCandidates) {
    if (Test-Rel $s) { $studioHit = $true; break }
  }
  if (-not $studioHit) { $studioIssues += "Studio.exe" }

  foreach ($lang in $langs) {
    $resRels = @(
      "Studio\Localized\$lang\Studio.Res\$Configuration\Studio.Res.dll",
      "Studio\Localized\$lang\Studio.Res\Win32\$Configuration\Studio.Res.dll"
    )
    $ok = $false
    foreach ($r in $resRels) { if (Test-Rel $r) { $ok = $true; break } }
    if (-not $ok) { $studioIssues += "Studio.Res.$lang" }

    if (-not (Test-Rel "Studio\Localized\$lang\Content\OpenDCL.chm")) {
      $studioIssues += "CHM.$lang"
    }
  }
}

if (-not $Quiet) {
  Write-Host "=== verify-build-outputs ==="
  Write-Host "OpenDclRoot : $OpenDclRoot"
  Write-Host "ModuleSet   : $ModuleSet"
  Write-Host "Config      : $Configuration"
  Write-Host ("Modules     : {0} present / {1} selected ({2} missing)" -f $present.Count, $selected.Count, $missing.Count)
  if ($missing.Count -gt 0 -and $missing.Count -le 40) {
    Write-Host ("  missing   : {0}" -f ($missing -join ", "))
  } elseif ($missing.Count -gt 40) {
    Write-Host ("  missing   : {0} … (+{1} more)" -f (($missing | Select-Object -First 20) -join ", "), ($missing.Count - 20))
  }
  Write-Host ("RxInstall   : {0}" -f $(if ($rxOk) { "OK" } else { "MISSING" }))
  Write-Host ("Runtime.Res : {0} / {1} langs" -f $resOk.Count, $langs.Count)
  if ($resMissing.Count) { Write-Host ("  missing   : {0}" -f ($resMissing -join ", ")) }
  if (-not $SkipStudio) {
    Write-Host ("Studio gate : {0}" -f $(if ($studioIssues.Count -eq 0) { "OK" } else { "ISSUES: " + ($studioIssues -join ", ") }))
  }
}

$fail = $false
switch ($ModuleSet) {
  "Full" {
    if ($missing.Count -gt 0) { $fail = $true }
    if (-not $rxOk) { $fail = $true }
    if ($resMissing.Count -gt 0) { $fail = $true }
    if (-not $SkipStudio -and $studioIssues.Count -gt 0) { $fail = $true }
  }
  "Selected" {
    if ($Runtimes.Count -eq 0) { throw "-ModuleSet Selected requires -Runtimes" }
    if ($missing.Count -gt 0) { $fail = $true }
    if (-not $rxOk) { $fail = $true }
  }
  "Available" {
    if ($present.Count -eq 0) { $fail = $true }
    if (-not $rxOk) { $fail = $true }
    if ($resOk.Count -eq 0) { $fail = $true }
  }
}

if ($fail) {
  Write-Host "VERIFY FAILED — do not package until outputs are complete." -ForegroundColor Red
  exit 1
}
Write-Host "VERIFY OK — safe to package." -ForegroundColor Green
exit 0
