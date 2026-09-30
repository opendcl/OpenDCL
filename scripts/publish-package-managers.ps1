<#
.SYNOPSIS
  Publish OpenDCL Studio and Runtime to Chocolatey and WinGet from a dist folder.

.DESCRIPTION
  Reads signed MSIs under dist\<version>\, writes Chocolatey nupkgs and WinGet
  multi-file manifests, then optionally pushes / submits.

  Chocolatey: env CHOCOLATEY_API_KEY (community repo).
  WinGet:     env WINGET_GITHUB_TOKEN (PAT that can fork microsoft/winget-pkgs).

  Studio language UpgradeCodes must match scripts/build-wix.ps1 $StudioLangMeta
  (see packaging/catalog.json). ARP DisplayVersion is the 3-part MSI
  ProductVersion (Patch*100+Build), not the 4-part filename version.

.PARAMETER ProductVersion
  Four-part file version (A.B.C.D), matching GitHub tag vA.B.C.D.

.PARAMETER DistDir
  Folder with OpenDCL.Studio.<LANG>.<ver>.msi and OpenDCL.Runtime.<ver>.msi.
  Defaults to <repo>\dist\<ProductVersion>.

.PARAMETER SkipChocolatey / SkipWinget
  Skip that catalog entirely.

.PARAMETER SkipPush
  Pack Chocolatey nupkgs but do not choco push.

.PARAMETER SkipSubmit
  Write WinGet manifests but do not wingetcreate submit.
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [string] $ProductVersion,
  [string] $DistDir = "",
  [string] $RepoRoot = "",
  [string] $OutDir = "",
  [switch] $SkipChocolatey,
  [switch] $SkipWinget,
  [switch] $SkipPush,
  [switch] $SkipSubmit
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$utf8 = New-Object System.Text.UTF8Encoding $false

if (-not $RepoRoot) { $RepoRoot = Split-Path -Parent $PSScriptRoot }
$ProductVersion = $ProductVersion.Trim().TrimStart('v', 'V')
$parts = $ProductVersion.Split('.')
if ($parts.Count -lt 4) { throw "ProductVersion must be A.B.C.D (got '$ProductVersion')" }

if (-not $DistDir) { $DistDir = Join-Path $RepoRoot "dist\$ProductVersion" }
if (-not (Test-Path -LiteralPath $DistDir)) { throw "DistDir not found: $DistDir" }
if (-not $OutDir) { $OutDir = Join-Path $RepoRoot "packaging\out\$ProductVersion" }

$catalogPath = Join-Path $RepoRoot "packaging\catalog.json"
$catalog = Get-Content -LiteralPath $catalogPath -Raw -Encoding UTF8 | ConvertFrom-Json
$tag = "v$ProductVersion"
$releaseDate = [DateTime]::UtcNow.ToString("yyyy-MM-dd")
$baseUrl = "https://github.com/$($catalog.repo)/releases/download/$tag"

function Get-MsiProperty {
  param([string] $Path, [string] $Name)
  $installer = New-Object -ComObject WindowsInstaller.Installer
  $db = $installer.GetType().InvokeMember("OpenDatabase", "InvokeMethod", $null, $installer, @((Get-Item -LiteralPath $Path).FullName, 0))
  $q = "SELECT ``Value`` FROM ``Property`` WHERE ``Property``='$Name'"
  $view = $db.GetType().InvokeMember("OpenView", "InvokeMethod", $null, $db, @($q))
  [void]$view.GetType().InvokeMember("Execute", "InvokeMethod", $null, $view, $null)
  $rec = $view.GetType().InvokeMember("Fetch", "InvokeMethod", $null, $view, $null)
  if (-not $rec) { return $null }
  return [string]$rec.GetType().InvokeMember("StringData", "GetProperty", $null, $rec, 1)
}

function Get-AssetInfo {
  param([string] $FileName, [string] $UpgradeCode)
  $path = Join-Path $DistDir $FileName
  if (-not (Test-Path -LiteralPath $path)) { throw "Missing dist asset: $path" }
  $productCode = Get-MsiProperty $path "ProductCode"
  $displayVer = Get-MsiProperty $path "ProductVersion"
  $msiUpgrade = Get-MsiProperty $path "UpgradeCode"
  if (-not $productCode) { throw "MSI Property ProductCode missing: $FileName" }
  if (-not $displayVer) { throw "MSI Property ProductVersion missing: $FileName" }
  if ($UpgradeCode -and $msiUpgrade -and ($msiUpgrade.Trim("{}") -ne $UpgradeCode.Trim("{}"))) {
    Write-Warning "UpgradeCode for $FileName is $msiUpgrade (catalog $($UpgradeCode)); using the MSI value for this version."
  }
  $uc = if ($msiUpgrade) { $msiUpgrade } else { $UpgradeCode }
  [pscustomobject]@{
    FileName       = $FileName
    Path           = $path
    Url            = "$baseUrl/$FileName"
    Sha256         = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpperInvariant()
    ProductCode    = "{" + $productCode.Trim("{}") + "}"
    DisplayVersion = $displayVer
    UpgradeCode    = $uc.Trim("{}").ToUpperInvariant()
  }
}

function Write-Utf8([string] $Path, [string] $Content) {
  $dir = Split-Path -Parent $Path
  if ($dir -and -not (Test-Path -LiteralPath $dir)) {
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
  }
  [IO.File]::WriteAllText($Path, $Content, $utf8)
}

function Copy-Tree([string] $Src, [string] $Dst) {
  if (Test-Path -LiteralPath $Dst) { Remove-Item -LiteralPath $Dst -Recurse -Force }
  New-Item -ItemType Directory -Force -Path $Dst | Out-Null
  Copy-Item -Path (Join-Path $Src "*") -Destination $Dst -Recurse -Force
}

$studioLangs = @()
foreach ($lang in $catalog.studio.languages) {
  $file = "OpenDCL.Studio.$($lang.code).$ProductVersion.msi"
  $info = Get-AssetInfo -FileName $file -UpgradeCode $lang.upgradeCode
  $studioLangs += [pscustomobject]@{
    Code           = $lang.code
    Locale         = $lang.locale
    UpgradeCode    = $info.UpgradeCode.Trim("{}").ToUpperInvariant()
    ProductCode    = $info.ProductCode
    DisplayVersion = $info.DisplayVersion
    Url            = $info.Url
    Sha256         = $info.Sha256
    FileName       = $file
  }
}
$runtimeFile = "OpenDCL.Runtime.$ProductVersion.msi"
$runtimeInfo = Get-AssetInfo -FileName $runtimeFile -UpgradeCode $catalog.runtime.upgradeCode
$runtimeUpgrade = $runtimeInfo.UpgradeCode.Trim("{}").ToUpperInvariant()

$distStudio = @(Get-ChildItem -LiteralPath $DistDir -Filter "OpenDCL.Studio.*.$ProductVersion.msi" -File)
foreach ($f in $distStudio) {
  if ($f.Name -match 'OpenDCL\.Studio\.([A-Z]{3})\.') {
    $code = $Matches[1]
    if ($catalog.studio.languages.code -notcontains $code) {
      throw "Dist has $code Studio MSI but packaging/catalog.json has no row (add locale + UpgradeCode)"
    }
  }
}

Write-Host "ProductVersion  : $ProductVersion"
Write-Host "ARP/MSI version : $($runtimeInfo.DisplayVersion)"
Write-Host "DistDir         : $DistDir"
Write-Host "OutDir          : $OutDir"
Write-Host "Studio langs    : $($studioLangs.Code -join ', ')"

if (Test-Path -LiteralPath $OutDir) { Remove-Item -LiteralPath $OutDir -Recurse -Force }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# --- Chocolatey ---
if (-not $SkipChocolatey) {
  $chocoOut = Join-Path $OutDir "chocolatey"
  New-Item -ItemType Directory -Force -Path $chocoOut | Out-Null

  function Publish-ChocoPackage {
    param(
      [string] $Id,
      [string] $SrcDir,
      [scriptblock] $Mutate
    )
    $stage = Join-Path $chocoOut $Id
    Copy-Tree $SrcDir $stage
    & $Mutate $stage
    $legal = Join-Path $stage "legal"
    New-Item -ItemType Directory -Force -Path $legal | Out-Null
    Copy-Item -LiteralPath (Join-Path $RepoRoot "LICENSE") (Join-Path $legal "LICENSE.txt") -Force
    $verify = New-Object System.Text.StringBuilder
    [void]$verify.AppendLine("VERIFICATION")
    [void]$verify.AppendLine("Installers are downloaded from GitHub Releases (not embedded).")
    [void]$verify.AppendLine("Release: https://github.com/$($catalog.repo)/releases/tag/$tag")
    [void]$verify.AppendLine("SHA256 values are computed from the signed MSIs in dist\$ProductVersion.")
    Write-Utf8 (Join-Path $legal "VERIFICATION.txt") $verify.ToString()

    $nuspec = Join-Path $stage "$Id.nuspec"
    $nustext = [IO.File]::ReadAllText($nuspec).Replace("{{VERSION}}", $ProductVersion)
    Write-Utf8 $nuspec $nustext

    $choco = Get-Command choco -ErrorAction SilentlyContinue
    if (-not $choco) { throw "choco.exe not on PATH (needed to pack $Id)" }
    Push-Location $stage
    try {
      & choco pack $nuspec --output-directory $chocoOut
      if ($LASTEXITCODE -ne 0) { throw "choco pack failed for $Id ($LASTEXITCODE)" }
    }
    finally { Pop-Location }

    $nupkg = Join-Path $chocoOut "$Id.$ProductVersion.nupkg"
    if (-not (Test-Path -LiteralPath $nupkg)) { throw "nupkg not produced: $nupkg" }
    Write-Host "Packed $nupkg"

    if ($SkipPush) {
      Write-Host "SkipPush: not uploading $Id"
      return
    }
    $key = $env:CHOCOLATEY_API_KEY
    $pushArgs = @($nupkg, "--source", "https://push.chocolatey.org/")
    if ($key) { $pushArgs += @("--api-key", $key) }
    & choco push @pushArgs
    if ($LASTEXITCODE -ne 0) { throw "choco push failed for $Id ($LASTEXITCODE)" }
    Write-Host "Pushed $Id $ProductVersion"
  }

  $checksumLines = ($studioLangs | ForEach-Object { "  '$($_.Code)' = '$($_.Sha256)'" }) -join "`r`n"
  Publish-ChocoPackage -Id $catalog.studio.chocoId `
    -SrcDir (Join-Path $RepoRoot "packaging\chocolatey\opendcl-studio") `
    -Mutate {
      param($stage)
      $install = Join-Path $stage "tools\chocolateyInstall.ps1"
      $t = [IO.File]::ReadAllText($install)
      $t = $t.Replace("{{VERSION}}", $ProductVersion).Replace("{{CHECKSUMS}}", $checksumLines)
      Write-Utf8 $install $t
    }

  Publish-ChocoPackage -Id $catalog.runtime.chocoId `
    -SrcDir (Join-Path $RepoRoot "packaging\chocolatey\opendcl-runtime") `
    -Mutate {
      param($stage)
      $install = Join-Path $stage "tools\chocolateyInstall.ps1"
      $t = [IO.File]::ReadAllText($install)
      $t = $t.Replace("{{VERSION}}", $ProductVersion).Replace("{{CHECKSUM}}", $runtimeInfo.Sha256)
      Write-Utf8 $install $t
    }
}

# --- WinGet ---
if (-not $SkipWinget) {
  $wgRoot = Join-Path $OutDir "winget"
  $arch = $catalog.architecture

  function New-WingetInstallerYaml {
    param(
      [string] $PackageId,
      [object[]] $Installers,
      [string] $DefaultScope = "machine"
    )
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.AppendLine("# yaml-language-server: `$schema=https://aka.ms/winget-manifest.installer.1.10.0.schema.json")
    [void]$sb.AppendLine("PackageIdentifier: $PackageId")
    [void]$sb.AppendLine("PackageVersion: $ProductVersion")
    [void]$sb.AppendLine("MinimumOSVersion: 10.0.0.0")
    [void]$sb.AppendLine("InstallerType: wix")
    [void]$sb.AppendLine("Scope: $DefaultScope")
    [void]$sb.AppendLine("InstallModes:")
    [void]$sb.AppendLine("  - interactive")
    [void]$sb.AppendLine("  - silent")
    [void]$sb.AppendLine("  - silentWithProgress")
    [void]$sb.AppendLine("InstallerSwitches:")
    [void]$sb.AppendLine("  Silent: /qn /norestart ALLUSERS=1")
    [void]$sb.AppendLine("  SilentWithProgress: /qb /norestart ALLUSERS=1")
    [void]$sb.AppendLine("UpgradeBehavior: install")
    [void]$sb.AppendLine("ReleaseDate: $releaseDate")
    [void]$sb.AppendLine("Installers:")
    foreach ($i in $Installers) {
      [void]$sb.AppendLine("  - Architecture: $arch")
      if ($i.Locale) { [void]$sb.AppendLine("    InstallerLocale: $($i.Locale)") }
      [void]$sb.AppendLine("    InstallerUrl: $($i.Url)")
      [void]$sb.AppendLine("    InstallerSha256: $($i.Sha256)")
      if ($i.ProductCode) { [void]$sb.AppendLine("    ProductCode: '$($i.ProductCode)'") }
      [void]$sb.AppendLine("    AppsAndFeaturesEntries:")
      [void]$sb.AppendLine("      - DisplayName: $($i.DisplayName)")
      [void]$sb.AppendLine("        Publisher: $($catalog.publisher)")
      [void]$sb.AppendLine("        DisplayVersion: $($i.DisplayVersion)")
      [void]$sb.AppendLine("        UpgradeCode: '{$($i.UpgradeCode)}'")
      [void]$sb.AppendLine("        InstallerType: wix")
    }
    [void]$sb.AppendLine("ManifestType: installer")
    [void]$sb.AppendLine("ManifestVersion: 1.10.0")
    return $sb.ToString()
  }

  function New-WingetLocaleYaml {
    param([string] $PackageId, [string] $PackageName, [string] $Short, [string] $Long)
    @"
# yaml-language-server: `$schema=https://aka.ms/winget-manifest.defaultLocale.1.10.0.schema.json
PackageIdentifier: $PackageId
PackageVersion: $ProductVersion
PackageLocale: en-US
Publisher: $($catalog.publisher)
PublisherUrl: $($catalog.homepage)
PublisherSupportUrl: $($catalog.supportUrl)
Author: $($catalog.publisher)
PackageName: $PackageName
PackageUrl: $($catalog.downloadUrl)
License: $($catalog.license)
LicenseUrl: $($catalog.licenseUrl)
Copyright: $($catalog.copyright)
ShortDescription: $Short
Description: |-
  $Long
Moniker: $($PackageId.ToLowerInvariant().Replace('.','-'))
Tags:
  - autolisp
  - autocad
  - bricscad
  - cad
  - dcl
  - opendcl
ReleaseNotesUrl: https://github.com/$($catalog.repo)/releases/tag/$tag
ManifestType: defaultLocale
ManifestVersion: 1.10.0
"@
  }

  function New-WingetVersionYaml([string] $PackageId) {
    @"
# yaml-language-server: `$schema=https://aka.ms/winget-manifest.version.1.10.0.schema.json
PackageIdentifier: $PackageId
PackageVersion: $ProductVersion
DefaultLocale: en-US
ManifestType: version
ManifestVersion: 1.10.0
"@
  }

  function Write-WingetPackage {
    param(
      [string] $PackageId,
      [string] $PackageName,
      [string] $Short,
      [string] $Long,
      [object[]] $Installers
    )
    $dir = Join-Path $wgRoot "$PackageId\$ProductVersion"
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    Write-Utf8 (Join-Path $dir "$PackageId.yaml") (New-WingetVersionYaml $PackageId)
    Write-Utf8 (Join-Path $dir "$PackageId.locale.en-US.yaml") (New-WingetLocaleYaml $PackageId $PackageName $Short $Long)
    Write-Utf8 (Join-Path $dir "$PackageId.installer.yaml") (New-WingetInstallerYaml $PackageId $Installers)
    return $dir
  }

  $studioInstallers = @(
    foreach ($l in $studioLangs) {
      [pscustomobject]@{
        Locale         = $l.Locale
        Url            = $l.Url
        Sha256         = $l.Sha256
        ProductCode    = $l.ProductCode
        DisplayName    = $catalog.studio.packageName
        DisplayVersion = $l.DisplayVersion
        UpgradeCode    = $l.UpgradeCode
      }
    }
  )
  $studioDir = Write-WingetPackage -PackageId $catalog.studio.wingetId `
    -PackageName $catalog.studio.packageName `
    -Short $catalog.studio.shortDescription `
    -Long "OpenDCL Studio is the visual dialog editor for AutoLISP. It installs the matching OpenDCL Runtime. Use winget install OpenDCL.Studio --locale de-DE (or another InstallerLocale) to pick a language MSI; the default follows the OS locale when listed, otherwise en-US." `
    -Installers $studioInstallers

  $runtimeDir = Write-WingetPackage -PackageId $catalog.runtime.wingetId `
    -PackageName $catalog.runtime.packageName `
    -Short $catalog.runtime.shortDescription `
    -Long "OpenDCL Runtime deploys CAD host modules for AutoLISP applications built with OpenDCL. It does not install the Studio editor. Prefer OpenDCL.Studio on developer machines." `
    -Installers @(
      [pscustomobject]@{
        Locale         = $null
        Url            = $runtimeInfo.Url
        Sha256         = $runtimeInfo.Sha256
        ProductCode    = $runtimeInfo.ProductCode
        DisplayName    = $catalog.runtime.packageName
        DisplayVersion = $runtimeInfo.DisplayVersion
        UpgradeCode    = $runtimeUpgrade
      }
    )

  Write-Host "WinGet manifests: $studioDir"
  Write-Host "WinGet manifests: $runtimeDir"

  if ($SkipSubmit) {
    Write-Host "SkipSubmit: not sending manifests to winget-pkgs"
  }
  else {
    $token = $env:WINGET_GITHUB_TOKEN
    if (-not $token) { $token = $env:WINGET_TOKEN }
    if (-not $token) { throw "WINGET_GITHUB_TOKEN (or WINGET_TOKEN) is not set" }

    $wgExe = Join-Path $OutDir "wingetcreate.exe"
    if (-not (Test-Path -LiteralPath $wgExe)) {
      Write-Host "Downloading wingetcreate..."
      Invoke-WebRequest -Uri "https://aka.ms/wingetcreate/latest" -OutFile $wgExe -UseBasicParsing
    }

    foreach ($pair in @(
        @{ Id = $catalog.studio.wingetId; Dir = $studioDir },
        @{ Id = $catalog.runtime.wingetId; Dir = $runtimeDir }
      )) {
      $title = "New version: $($pair.Id) version $ProductVersion"
      Write-Host "Submitting $($pair.Id)..."
      & $wgExe submit --prtitle $title --token $token $pair.Dir
      if ($LASTEXITCODE -ne 0) { throw "wingetcreate submit failed for $($pair.Id) ($LASTEXITCODE)" }
    }
  }
}

Write-Host "PACKAGE MANAGERS OK"
