<#
.SYNOPSIS
  Authenticode-sign installer packages (and optionally PE binaries) with signtool.

.DESCRIPTION
  Primary production path (OpenDCL / ManuSoft, 2026+):

    SSL.com code signing certificate on a YubiKey
      → cert in Windows Personal store (Current User)
      → private key non-exportable on the token (Smart Card KSP)
      → sign with certificate SHA1 thumbprint
      → RFC3161 timestamp via SSL.com

  Enter the YubiKey PIN when Windows / signtool prompts (often once per session,
  sometimes per file). Never put the PIN in this script or in git.

  Credential modes (first match wins):
    1. -CertThumbprint / $env:SIGN_CERT_THUMBPRINT
    2. Auto-detect a single SSL.com Code Signing cert with a private key in the store
    3. -PfxPath + -PfxPassword  (legacy / rare; not used for YubiKey)
    4. -CertFile + -CspName + -KeyContainer  (legacy eToken-style; prefer thumbprint)

  Replaces historical @SignAll.bat / #Sign.bat (SafeNet eToken + Sectigo).

.PARAMETER Path
  File or directory. Directories default to *.msi and *.msm only (like @SignAll *.ms?).

.PARAMETER IncludeBinaries
  When Path is a directory, also sign *.exe, *.dll, *.cab, etc.

.PARAMETER Recurse
  Search directories recursively.

.EXAMPLE
  # Production: YubiKey plugged in; PIN when prompted
  $env:SIGN_CERT_THUMBPRINT = "535892C8273A64940E5DDB321965EB255241DA57"
  .\scripts\sign-files.ps1 -Path .\dist\10.1.1.1

.EXAMPLE
  .\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 -CertThumbprint "535892C8273A64940E5DDB321965EB255241DA57"
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [string[]] $Path,

  [string] $CertThumbprint = $env:SIGN_CERT_THUMBPRINT,
  [string] $PfxPath = $env:SIGN_PFX_PATH,
  [string] $PfxPassword = $env:SIGN_PFX_PASSWORD,
  [string] $CertFile = $env:SIGN_CERT_FILE,
  [string] $CspName = $env:SIGN_CSP_NAME,
  [string] $KeyContainer = $env:SIGN_KEY_CONTAINER,

  # SSL.com RFC3161 timestamp (verified with ManuSoft YubiKey cert, 2026)
  [string] $TimestampUrl = $(if ($env:SIGN_TIMESTAMP_URL) { $env:SIGN_TIMESTAMP_URL } else { "http://ts.ssl.com" }),
  [string] $DescriptionUrl = $(if ($env:SIGN_DESCRIPTION_URL) { $env:SIGN_DESCRIPTION_URL } else { "https://www.opendcl.com" }),
  [string] $FileDigest = "sha256",
  [switch] $IncludeBinaries,
  [switch] $Recurse,
  [switch] $SkipVerify,
  [string] $SignTool = ""
)

$ErrorActionPreference = "Stop"

function Find-SignTool {
  if ($SignTool -and (Test-Path $SignTool)) { return (Resolve-Path $SignTool).Path }
  $cmd = Get-Command signtool.exe -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }
  $kits = "${env:ProgramFiles(x86)}\Windows Kits\10\bin"
  if (Test-Path $kits) {
    $found = Get-ChildItem $kits -Recurse -Filter signtool.exe -ErrorAction SilentlyContinue |
      Where-Object { $_.FullName -match '\\x64\\signtool\.exe$' } |
      Sort-Object FullName -Descending |
      Select-Object -First 1
    if ($found) { return $found.FullName }
  }
  # Older layout
  $alt = "${env:ProgramFiles(x86)}\Windows Kits\10\bin\x64\signtool.exe"
  if (Test-Path $alt) { return $alt }
  throw "signtool.exe not found. Install Windows SDK or pass -SignTool."
}

function Get-SslComCodeSigningCerts {
  $stores = @(
    "Cert:\CurrentUser\My",
    "Cert:\LocalMachine\My"
  )
  $list = @()
  foreach ($s in $stores) {
    if (-not (Test-Path $s)) { continue }
    $list += Get-ChildItem $s -ErrorAction SilentlyContinue | Where-Object {
      $_.HasPrivateKey -and
      ($_.Issuer -match 'SSL\.com|SSL Corp') -and
      (
        ($_.EnhancedKeyUsageList | ForEach-Object { $_.FriendlyName }) -contains 'Code Signing' -or
        ($_.EnhancedKeyUsageList | ForEach-Object { $_.ObjectId }) -contains '1.3.6.1.5.5.7.3.3' -or
        -not $_.EnhancedKeyUsageList
      )
    }
  }
  return @($list | Sort-Object Thumbprint -Unique)
}

function Resolve-Thumbprint([string] $explicit) {
  if ($explicit) {
    $thumb = ($explicit -replace '\s', '').ToUpperInvariant()
    $cert = @(
      Get-ChildItem Cert:\CurrentUser\My, Cert:\LocalMachine\My -ErrorAction SilentlyContinue |
        Where-Object { $_.Thumbprint -eq $thumb }
    ) | Select-Object -First 1
    if (-not $cert) {
      Write-Warning "Thumbprint $thumb not found in CurrentUser/LocalMachine Personal stores. signtool may still find it."
    }
    elseif (-not $cert.HasPrivateKey) {
      throw @"
Certificate $thumb is in the store but HasPrivateKey=False.
Import the public cert, then with the YubiKey inserted run:
  certutil -user -repairstore My $thumb
Enter the YubiKey PIN if prompted.
"@
    }
    else {
      Write-Host ("Certificate: {0}" -f $cert.Subject)
      Write-Host ("Issuer:      {0}" -f $cert.Issuer)
      Write-Host ("NotAfter:    {0}" -f $cert.NotAfter)
    }
    return $thumb
  }

  $auto = Get-SslComCodeSigningCerts
  if ($auto.Count -eq 1) {
    Write-Host "Auto-detected SSL.com code signing cert:"
    Write-Host ("  Subject:    {0}" -f $auto[0].Subject)
    Write-Host ("  Thumbprint: {0}" -f $auto[0].Thumbprint)
    return $auto[0].Thumbprint.ToUpperInvariant()
  }
  if ($auto.Count -gt 1) {
    Write-Host "Multiple SSL.com code signing certs with private keys:"
    $auto | ForEach-Object { Write-Host ("  {0}  {1}" -f $_.Thumbprint, $_.Subject) }
    throw "Pass -CertThumbprint or set SIGN_CERT_THUMBPRINT to select one."
  }
  return $null
}

function Get-FilesToSign([string[]] $paths) {
  # Default: installers only (historical @SignAll *.ms?)
  $exts = @("*.msi", "*.msm")
  if ($IncludeBinaries) {
    $exts += @("*.cab", "*.exe", "*.dll", "*.sys", "*.ocx")
  }
  $list = New-Object System.Collections.Generic.List[string]
  foreach ($p in $paths) {
    if (-not (Test-Path -LiteralPath $p)) { throw "Path not found: $p" }
    $item = Get-Item -LiteralPath $p
    if ($item.PSIsContainer) {
      foreach ($pat in $exts) {
        Get-ChildItem -LiteralPath $item.FullName -Filter $pat -File -Recurse:$Recurse |
          ForEach-Object { [void]$list.Add($_.FullName) }
      }
    }
    else {
      [void]$list.Add($item.FullName)
    }
  }
  return @($list | Select-Object -Unique)
}

$signtoolPath = Find-SignTool
$files = @(Get-FilesToSign $Path)
if ($files.Count -eq 0) {
  throw "No signable files matched under: $($Path -join ', ') (directories default to *.msi/*.msm; use -IncludeBinaries for PE)"
}

$common = @(
  "sign",
  "/fd", $FileDigest,
  "/td", $FileDigest,
  "/tr", $TimestampUrl,
  "/du", $DescriptionUrl
)

$mode = $null
$thumb = Resolve-Thumbprint $CertThumbprint
if ($thumb) {
  $common += @("/sha1", $thumb)
  $mode = "YubiKey/store thumbprint"
  Write-Host "Signing mode: $mode"
  Write-Host "Thumbprint:   $thumb"
  Write-Host "YubiKey:      insert key; enter PIN when Windows prompts"
}
elseif ($PfxPath) {
  if (-not (Test-Path -LiteralPath $PfxPath)) { throw "PFX not found: $PfxPath" }
  if (-not $PfxPassword) { throw "PfxPassword / SIGN_PFX_PASSWORD required with -PfxPath" }
  $common += @("/f", $PfxPath, "/p", $PfxPassword)
  $mode = "PFX"
  Write-Host "Signing mode: $mode ($PfxPath)"
}
elseif ($CertFile -and $CspName -and $KeyContainer) {
  if (-not (Test-Path -LiteralPath $CertFile)) { throw "Cert file not found: $CertFile" }
  $common += @("/f", $CertFile, "/csp", $CspName, "/kc", $KeyContainer)
  $mode = "legacy CSP container"
  Write-Host "Signing mode: $mode (prefer YubiKey thumbprint instead)"
}
else {
  throw @"
No signing credentials configured.

Preferred (SSL.com YubiKey):
  1. YubiKey inserted; cert in Current User Personal with private key
     (certutil -user -repairstore My <thumbprint> if HasPrivateKey is False)
  2. Set SIGN_CERT_THUMBPRINT or pass -CertThumbprint
     ManuSoft SSL.com example (2026): 535892C8273A64940E5DDB321965EB255241DA57
  3. Run this script; enter YubiKey PIN when prompted

Never commit PINs, PFX passwords, or private keys to git.
"@
}

Write-Host "signtool:    $signtoolPath"
Write-Host "files:       $($files.Count)"
Write-Host "timestamp:   $TimestampUrl"
Write-Host "description: $DescriptionUrl"

$failed = @()
foreach ($f in $files) {
  Write-Host "Sign $f"
  $argList = $common + @($f)
  & $signtoolPath @argList
  if ($LASTEXITCODE -ne 0) {
    Write-Warning "signtool failed ($LASTEXITCODE): $f"
    $failed += $f
  }
}

if ($failed.Count -gt 0) {
  throw "Signing failed for $($failed.Count) file(s). Check YubiKey insertion and PIN."
}

Write-Host "OK signed $($files.Count) file(s)"

if (-not $SkipVerify) {
  foreach ($f in $files) {
    & $signtoolPath verify /pa $f | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Verify failed: $f" }
  }
  Write-Host "OK verified $($files.Count) file(s)"
}
