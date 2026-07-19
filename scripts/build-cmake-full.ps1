<#
.SYNOPSIS
  Configure + build CMake "classic parity" presets (x64 + Win32) and optionally package.

.DESCRIPTION
  Mirrors a full pre-CMake OpenDCL.sln ship using preset **vs2022-full**:
    - One VS .sln (x64) with nested Win32 projects imported into the solution (OpenDCL_Win32 umbrella)
    - Shared out/ for x64 + x86 modules, Studio, Res, RxInstall
    - All CAD families (AUTO = skip missing SDKs), all UI languages
    - Studio static MFC+/MT; modules /MD (FullDebug /MDd)

  Packaging (optional) always runs **verify-build-outputs.ps1** first unless
  -SkipVerify. Do not package while the build is incomplete.

.EXAMPLE
  .\scripts\build-cmake-full.ps1 -Fresh
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
  [string] $Preset = "vs2022-full",
  [switch] $SkipVerify,
  [switch] $VerifyOnly
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

if (-not $SkipConfigure -and -not $VerifyOnly) {
  Write-Host "==== Configure $Preset ====" -ForegroundColor Cyan
  $args = @("--preset", $Preset)
  if ($Fresh) { $args += "--fresh" }
  & cmake @args
  if ($LASTEXITCODE -ne 0) { throw "cmake configure failed for $Preset" }
}

if (-not $SkipBuild -and -not $VerifyOnly) {
  Write-Host "==== Build $Preset ($Configuration) — Win32 projects in .sln when OPENDCL_NEST_WIN32 / IN_ALL ====" -ForegroundColor Cyan
  $buildPreset = switch ($Configuration) {
    "Release" { if ($Preset -eq "vs2022-full") { "vs2022-full-release" } else { "$Preset-release" } }
    "Debug" { if ($Preset -eq "vs2022-full") { "vs2022-full-debug" } else { "$Preset-debug" } }
    "FullDebug" { if ($Preset -eq "vs2022-full") { "vs2022-full-fulldebug" } else { "$Preset-fulldebug" } }
    default { "" }
  }
  if ($buildPreset) {
    & cmake --build --preset $buildPreset
  }
  if (-not $buildPreset -or $LASTEXITCODE -ne 0) {
    & cmake --build "build/$Preset" --config $Configuration
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed for $Preset" }
  }
}

$cmakeRoot = Join-Path $RepoRoot "build\$Preset"
if (-not (Test-Path $cmakeRoot)) {
  throw "CMake build dir not found: $cmakeRoot"
}

if (($Package -or $VerifyOnly) -and -not $SkipVerify) {
  Write-Host "==== Verify build outputs ($PackageModuleSet) ====" -ForegroundColor Cyan
  $vArgs = @{
    OpenDclRoot   = $cmakeRoot
    Configuration = $Configuration
    ModuleSet     = $PackageModuleSet
  }
  & (Join-Path $PSScriptRoot "verify-build-outputs.ps1") @vArgs
  if ($LASTEXITCODE -ne 0) {
    throw "verify-build-outputs failed — packaging aborted."
  }
}

if ($VerifyOnly) {
  Write-Host "build-cmake-full.ps1 verify-only done." -ForegroundColor Green
  return
}

if ($Package) {
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
