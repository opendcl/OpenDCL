<#
.SYNOPSIS
  Configure + build CMake "classic parity" presets (x64 + Win32) and package.

.DESCRIPTION
  Mirrors what a full pre-CMake OpenDCL.sln Release ship aimed at:
    - All CAD families (ARX/BRX/GRX/ZRX) with AUTO = skip missing SDKs
    - All UI languages (ENU, DEU, ESM, RUS, CHS, FRA, CHT)
    - Studio (static MFC /MT) + Studio.Res + ENU CHM (hhc)
    - RxInstall (Win32 CA nested from x64)
    - Runtime modules: Debug/FullDebug/Release CRT rules (see CMAKE.md)
    - x64 and Win32 platforms as separate VS generator configs

  Packaging uses build-wix.ps1:
    -ModuleSet Available  (default) — ship what built (typical when older SDKs missing)
    -ModuleSet Full       — fail if any catalog module missing (true full-product gate)

.EXAMPLE
  # Build everything available + package Available set
  .\scripts\build-cmake-full.ps1

  # Package only (after a prior full build)
  .\scripts\build-cmake-full.ps1 -SkipConfigure -SkipBuild -Package

  # Require full catalog (fails if any module missing)
  .\scripts\build-cmake-full.ps1 -PackageModuleSet Full
#>
[CmdletBinding()]
param(
  [switch] $SkipConfigure,
  [switch] $SkipBuild,
  [switch] $SkipX64,
  [switch] $SkipWin32,
  [switch] $Package,
  [ValidateSet("Available", "Full", "Selected")]
  [string] $PackageModuleSet = "Available",
  [string] $Configuration = "Release",
  [string] $OutDir = "",
  [switch] $Fresh
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

$presets = @()
if (-not $SkipX64) { $presets += "vs2022-x64-full" }
if (-not $SkipWin32) { $presets += "vs2022-win32-full" }
if ($presets.Count -eq 0) { throw "Nothing to do (both -SkipX64 and -SkipWin32)." }

foreach ($preset in $presets) {
  if (-not $SkipConfigure) {
    Write-Host "==== Configure $preset ====" -ForegroundColor Cyan
    $args = @("--preset", $preset)
    if ($Fresh) { $args += "--fresh" }
    & cmake @args
    if ($LASTEXITCODE -ne 0) { throw "cmake configure failed for $preset" }
  }
  if (-not $SkipBuild) {
    Write-Host "==== Build $preset ($Configuration) ====" -ForegroundColor Cyan
    $buildPreset = switch ($Configuration) {
      "Release" { "$preset-release" }
      "Debug" { "$preset-debug" }
      "FullDebug" { "$preset-fulldebug" }
      default { "" }
    }
    if ($buildPreset) {
      & cmake --build --preset $buildPreset
    }
    if (-not $buildPreset -or $LASTEXITCODE -ne 0) {
      & cmake --build "build/$preset" --config $Configuration
      if ($LASTEXITCODE -ne 0) { throw "cmake build failed for $preset" }
    }
  }
}

if ($Package) {
  # Prefer x64 full out as OpenDclRoot (contains Studio x64 + modules); Resolve-ProductFile
  # also finds RepoRoot classic Win32 modules and content.
  $cmakeRoot = Join-Path $RepoRoot "build\vs2022-x64-full"
  if (-not (Test-Path $cmakeRoot)) {
    $cmakeRoot = Join-Path $RepoRoot "build\vs2022-win32-full"
  }
  if (-not (Test-Path $cmakeRoot)) {
    throw "No CMake full build dir found under build/vs2022-*-full"
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
  # All languages that have Runtime.Res when Available; Full uses all catalog langs.
  if ($PackageModuleSet -eq "Available") {
    $pkgArgs["AvailableLanguages"] = $true
  }
  & (Join-Path $PSScriptRoot "build-wix.ps1") @pkgArgs
  if ($LASTEXITCODE -ne 0) { throw "build-wix.ps1 failed" }
  Write-Host "Packages: $OutDir" -ForegroundColor Green
}

Write-Host "build-cmake-full.ps1 done." -ForegroundColor Green
