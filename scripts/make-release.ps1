<#
.SYNOPSIS
  Build a full OpenDCL release folder: versioned installers + localization packs.

.DESCRIPTION
  Combines the post-package steps that used to live in separate batch files:

    1. (optional) scripts/build-wix.ps1     — WiX MSM/MSIs
    2. scripts/make-dist.ps1               — versioned installer names (!MakeNewDist)
    3. scripts/make-localization-zips.ps1  — translator packs (!MakeLocalizationZips)
    4. (optional) scripts/sign-files.ps1   — Authenticode (@SignAll)

  Output layout:

    dist/<ProductVersion>/
      OpenDCL.Runtime.<ver>.msi
      OpenDCL.Runtime.<ver>.msm
      OpenDCL.Studio.<LANG>.<ver>.msi
      OpenDCL.<LANG>.zip                  # localization packs (same folder)

.PARAMETER ProductVersion
  Four-part version used in filenames and as ModuleVersion (e.g. 10.1.1.1).

.PARAMETER MsiProductVersion
  Three-part MSI ProductVersion. If empty, derived as major.minor.(patch*100+build).

.PARAMETER SkipPackage
  Skip WiX; use existing wix\out\<Configuration> outputs.

.PARAMETER SkipLocalization
  Skip building OpenDCL.<LANG>.zip packs.

.PARAMETER Runtimes / ModuleSet / AvailableLanguages / SkipStudio
  Forwarded to build-wix.ps1 for custom subset packages. Full product release
  is the default (all modules + all languages). Custom sets produce
  OpenDCL.Runtime.custom.* under wix\out and are not the ship MSM identity.

.PARAMETER Sign
  Run sign-files.ps1 on the dist folder (uses SIGN_* env vars / parameters).

.EXAMPLE
  .\scripts\make-release.ps1 -ProductVersion 10.1.1.1 -Sign

.EXAMPLE
  # Dev custom installer (BRX 27 + ENU only, no Studio)
  .\scripts\make-release.ps1 -ProductVersion 10.1.1.1 `
    -Runtimes BRX.27.x64 -Languages ENU -ModuleSet Selected `
    -SkipStudio -SkipLocalization
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [Parameter(Mandatory = $true)]
  [string] $ProductVersion,
  [string] $MsiProductVersion = "",
  [string] $Configuration = "Release",
  [string[]] $Languages = @(),
  [string[]] $Runtimes = @(),
  [ValidateSet("Full", "Selected", "Available")]
  [string] $ModuleSet = "Full",
  [switch] $AvailableLanguages,
  [switch] $SkipStudio,
  [switch] $SkipRuntimeMsi,
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

if (-not $OpenDclRoot) {
  $OpenDclRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path
$scripts = Join-Path $OpenDclRoot "scripts"

$parts = $ProductVersion.Trim().Split('.')
if ($parts.Count -lt 4) {
  throw "ProductVersion must be A.B.C.D (got '$ProductVersion')"
}
if (-not $MsiProductVersion) {
  $MsiProductVersion = "{0}.{1}.{2}" -f $parts[0], $parts[1], ([int]$parts[2] * 100 + [int]$parts[3])
}

$dest = Join-Path $OpenDclRoot "dist\$ProductVersion"

Write-Host "=== make-release ==="
Write-Host "ProductVersion    = $ProductVersion"
Write-Host "MsiProductVersion = $MsiProductVersion"
Write-Host "Configuration     = $Configuration"
Write-Host "Dest              = $dest"
Write-Host "SkipPackage       = $SkipPackage"
Write-Host "SkipLocalization  = $SkipLocalization"
Write-Host "Sign              = $Sign"

if (-not $SkipPackage) {
  Write-Host "--- build-wix.ps1 ---"
  $wixArgs = @{
    OpenDclRoot      = $OpenDclRoot
    Configuration    = $Configuration
    ModuleVersion    = $ProductVersion
    ProductVersion   = $MsiProductVersion
    ModuleSet        = $ModuleSet
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
  -OpenDclRoot $OpenDclRoot `
  -ProductVersion $ProductVersion `
  -Configuration $Configuration `
  -DestDir $dest

if (-not $SkipLocalization) {
  Write-Host "--- make-localization-zips.ps1 ---"
  $zipArgs = @{
    OpenDclRoot = $OpenDclRoot
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
