<#
.SYNOPSIS
  Build a full OpenDCL release folder: versioned installers + localization packs.

.DESCRIPTION
  Combines the post-package steps that used to live in separate batch files:

    0. (optional) scripts/verify-build-outputs.ps1 — fail if product tree incomplete
    1. (optional) scripts/build-wix.ps1            — WiX MSM/MSIs
    2. scripts/make-dist.ps1                      — versioned installer names
    3. scripts/make-localization-zips.ps1         — translator packs
    4. (optional) scripts/sign-files.ps1          — Authenticode

  Packaging **product** files resolve under -OpenDclRoot (classic tree or CMake
  binary dir with out\). Scripts, wix\out, and dist\ always live under this
  repository (the parent of scripts\), so CMake build dirs can be used as
  -OpenDclRoot without writing packages into build\.

  Output layout:

    dist/<ProductVersion>/
      OpenDCL.Runtime.<ver>.msi
      OpenDCL.Runtime.<ver>.msm
      OpenDCL.Studio.<LANG>.<ver>.msi
      OpenDCL.<LANG>.zip

.PARAMETER OpenDclRoot
  Compiled product root for WiX harvest (e.g. build\vs2022-full). Defaults to
  this repository root.

.PARAMETER ProductVersion
  Four-part version used in filenames and as ModuleVersion (e.g. 10.1.1.1).

.PARAMETER MsiProductVersion
  Three-part MSI ProductVersion. If empty, derived as major.minor.(patch*100+build).

.PARAMETER SkipVerify
  Skip verify-build-outputs.ps1 before packaging (not recommended).

.PARAMETER SkipPackage
  Skip WiX; use existing wix\out\<Configuration> outputs.

.PARAMETER SkipLocalization
  Skip building OpenDCL.<LANG>.zip packs.

.PARAMETER Sign
  Run sign-files.ps1 on the dist folder (YubiKey PIN when prompted).

.EXAMPLE
  # Full ship package from CMake dual-arch tree (after successful build + verify)
  .\scripts\make-release.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full) `
    -ProductVersion 10.1.1.1 -ModuleSet Full -Sign

.EXAMPLE
  # Dev custom installer (BRX 27 + ENU only, no Studio)
  .\scripts\make-release.ps1 -OpenDclRoot (Resolve-Path build\vs2022-x64-dev) `
    -ProductVersion 10.1.1.1 -Runtimes BRX.27.x64 -Languages ENU `
    -ModuleSet Selected -SkipStudio -SkipLocalization
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [Parameter(Mandatory = $true)]
  [string] $ProductVersion,
  [string] $MsiProductVersion = "",
  [string] $Configuration = "Release",
  [string] $PackageOutDir = "",
  [string[]] $Languages = @(),
  [string[]] $Runtimes = @(),
  [ValidateSet("Full", "Selected", "Available")]
  [string] $ModuleSet = "Full",
  [switch] $AvailableLanguages,
  [switch] $SkipStudio,
  [switch] $SkipRuntimeMsi,
  [switch] $SkipVerify,
  [switch] $SkipPackage,
  [switch] $SkipLocalization,
  [switch] $Sign,
  [string] $CertThumbprint = $env:SIGN_CERT_THUMBPRINT,
  [string] $PfxPath = $env:SIGN_PFX_PATH,
  [string] $PfxPassword = $env:SIGN_PFX_PASSWORD,
  [string] $CertFile = $env:SIGN_CERT_FILE,
  [string] $CspName = $env:SIGN_CSP_NAME,
  [string] $KeyContainer = $env:SIGN_KEY_CONTAINER
)

$ErrorActionPreference = "Stop"

# Repo that owns scripts/wix/dist (always this tree).
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$scripts = Join-Path $RepoRoot "scripts"

if (-not $OpenDclRoot) {
  $OpenDclRoot = $RepoRoot
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path

$parts = $ProductVersion.Trim().Split('.')
if ($parts.Count -lt 4) {
  throw "ProductVersion must be A.B.C.D (got '$ProductVersion')"
}
if (-not $MsiProductVersion) {
  $MsiProductVersion = "{0}.{1}.{2}" -f $parts[0], $parts[1], ([int]$parts[2] * 100 + [int]$parts[3])
}

if (-not $PackageOutDir) {
  $PackageOutDir = Join-Path $RepoRoot "wix\out\$Configuration"
}
$dest = Join-Path $RepoRoot "dist\$ProductVersion"

Write-Host "=== make-release ==="
Write-Host "RepoRoot          = $RepoRoot"
Write-Host "OpenDclRoot       = $OpenDclRoot  (product harvest)"
Write-Host "ProductVersion    = $ProductVersion"
Write-Host "MsiProductVersion = $MsiProductVersion"
Write-Host "Configuration     = $Configuration"
Write-Host "PackageOutDir     = $PackageOutDir"
Write-Host "Dest              = $dest"
Write-Host "ModuleSet         = $ModuleSet"
Write-Host "SkipVerify        = $SkipVerify"
Write-Host "SkipPackage       = $SkipPackage"
Write-Host "SkipLocalization  = $SkipLocalization"
Write-Host "Sign              = $Sign"

if (-not $SkipPackage -and -not $SkipVerify) {
  Write-Host "--- verify-build-outputs.ps1 (must pass before package) ---"
  $verifyArgs = @{
    OpenDclRoot   = $OpenDclRoot
    Configuration = $Configuration
    ModuleSet     = $ModuleSet
  }
  if ($Runtimes -and $Runtimes.Count -gt 0) { $verifyArgs.Runtimes = $Runtimes }
  if ($Languages -and $Languages.Count -gt 0) { $verifyArgs.Languages = $Languages }
  if ($SkipStudio) { $verifyArgs.SkipStudio = $true }
  & (Join-Path $scripts "verify-build-outputs.ps1") @verifyArgs
  if ($LASTEXITCODE -and $LASTEXITCODE -ne 0) {
    throw "verify-build-outputs.ps1 failed ($LASTEXITCODE) — fix the build before packaging."
  }
}

if (-not $SkipPackage) {
  Write-Host "--- build-wix.ps1 ---"
  $wixArgs = @{
    OpenDclRoot      = $OpenDclRoot
    Configuration    = $Configuration
    ModuleVersion    = $ProductVersion
    ProductVersion   = $MsiProductVersion
    ModuleSet        = $ModuleSet
    OutDir           = $PackageOutDir
  }
  if ($Languages -and $Languages.Count -gt 0) {
    $wixArgs.Languages = $Languages
  }
  if ($Runtimes -and $Runtimes.Count -gt 0) {
    $wixArgs.Runtimes = $Runtimes
  }
  if ($AvailableLanguages) { $wixArgs.AvailableLanguages = $true }
  if ($SkipStudio) { $wixArgs.SkipStudio = $true }
  if ($SkipRuntimeMsi) { $wixArgs.SkipRuntimeMsi = $true }
  & (Join-Path $scripts "build-wix.ps1") @wixArgs
  if ($LASTEXITCODE -and $LASTEXITCODE -ne 0) { throw "build-wix.ps1 failed ($LASTEXITCODE)" }
}

Write-Host "--- make-dist.ps1 ---"
& (Join-Path $scripts "make-dist.ps1") `
  -OpenDclRoot $RepoRoot `
  -ProductVersion $ProductVersion `
  -Configuration $Configuration `
  -SourceDir $PackageOutDir `
  -DestDir $dest

if (-not $SkipLocalization) {
  Write-Host "--- make-localization-zips.ps1 ---"
  $zipArgs = @{
    OpenDclRoot = $RepoRoot
    OutDir      = $dest
  }
  if ($Languages -and $Languages.Count -gt 0) {
    $zipArgs.Languages = $Languages
  }
  & (Join-Path $scripts "make-localization-zips.ps1") @zipArgs
}

if ($Sign) {
  Write-Host "--- sign-files.ps1 ---"
  $signArgs = @{
    Path            = $dest
    DescriptionUrl  = "https://www.opendcl.com"
  }
  if ($CertThumbprint) { $signArgs.CertThumbprint = $CertThumbprint }
  if ($PfxPath) { $signArgs.PfxPath = $PfxPath }
  if ($PfxPassword) { $signArgs.PfxPassword = $PfxPassword }
  if ($CertFile) { $signArgs.CertFile = $CertFile }
  if ($CspName) { $signArgs.CspName = $CspName }
  if ($KeyContainer) { $signArgs.KeyContainer = $KeyContainer }
  & (Join-Path $scripts "sign-files.ps1") @signArgs
}

Write-Host "=== make-release complete ==="
Get-ChildItem -LiteralPath $dest -File | Sort-Object Name |
  ForEach-Object { Write-Host ("  {0,-48} {1,8:N1} MB" -f $_.Name, ($_.Length / 1MB)) }
return $dest
