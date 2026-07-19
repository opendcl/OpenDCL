<#
.SYNOPSIS
  Configure + build CMake "classic parity" presets (x64 + Win32) and package.

.DESCRIPTION
  Mirrors a full pre-CMake OpenDCL.sln ship using preset **vs2022-full**:
    - One VS .sln (x64) with nested Win32 via OpenDCL_Win32 / ALL_BUILD
    - Shared out/ for x64 + x86 modules, Studio, Res, RxInstall
    - All CAD families (AUTO = skip missing SDKs), all UI languages
    - Studio static MFC+/MT; modules /MD (FullDebug /MDd)

  Packaging: build-wix.ps1 -ModuleSet Available|Full from build\vs2022-full.

.EXAMPLE
  .\scripts\build-cmake-full.ps1 -Fresh -Package
  .\scripts\build-cmake-full.ps1 -SkipConfigure -SkipBuild -Package -PackageModuleSet Full
#>
[CmdletBinding()]
param(
  [switch] $SkipConfigure,
  [switch] $SkipBuild,
  [switch] $Package,
  [ValidateSet("Available", "Full", "Selected")]
  [string] $PackageModuleSet = "Available",
  [string] $Configuration = "Release",
  [string] $OutDir = "",
  [switch] $Fresh,
  [string] $Preset = "vs2022-full"
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

if (-not $SkipConfigure) {
  Write-Host "==== Configure $Preset ====" -ForegroundColor Cyan
  $args = @("--preset", $Preset)
  if ($Fresh) { $args += "--fresh" }
  & cmake @args
  if ($LASTEXITCODE -ne 0) { throw "cmake configure failed for $Preset" }
}
if (-not $SkipBuild) {
  Write-Host "==== Build $Preset ($Configuration) — includes nested Win32 when OPENDCL_WIN32_IN_ALL ====" -ForegroundColor Cyan
  $buildPreset = switch ($Configuration) {
    "Release" { "$Preset-release" }
    "Debug" { "$Preset-debug" }
    "FullDebug" { "$Preset-fulldebug" }
    default { "" }
  }
  # vs2022-full-release etc.
  if ($buildPreset -and $Preset -eq "vs2022-full") {
    $buildPreset = "vs2022-full-$($Configuration.ToLower())"
    if ($Configuration -eq "FullDebug") { $buildPreset = "vs2022-full-fulldebug" }
  }
  if ($buildPreset) {
    & cmake --build --preset $buildPreset
  }
  if (-not $buildPreset -or $LASTEXITCODE -ne 0) {
    & cmake --build "build/$Preset" --config $Configuration
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed for $Preset" }
  }
}

if ($Package) {
  $cmakeRoot = Join-Path $RepoRoot "build\$Preset"
  if (-not (Test-Path $cmakeRoot)) {
    throw "CMake build dir not found: $cmakeRoot"
  }
  if (-not $OutDir) {
    $OutDir = Join-Path $RepoRoot "wix\out\cmake-full-$Configuration"
  }
  Write-Host "==== Package ($PackageModuleSet) from $cmakeRoot ====" -ForegroundColor Cyan
  $pkgArgs = @{
    OpenDclRoot   = $cmakeRoot
    Configuration = $Configuration
    OutDir        = $OutDir
    ModuleSet     = $PackageModuleSet
  }
  if ($PackageModuleSet -eq "Available") {
    $pkgArgs["AvailableLanguages"] = $true
  }
  & (Join-Path $PSScriptRoot "build-wix.ps1") @pkgArgs
  if ($LASTEXITCODE -ne 0) { throw "build-wix.ps1 failed" }
  Write-Host "Packages: $OutDir" -ForegroundColor Green
}

Write-Host "build-cmake-full.ps1 done." -ForegroundColor Green
