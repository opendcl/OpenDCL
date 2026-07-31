<#
.SYNOPSIS
  Authenticode-sign installer packages (and optionally PE binaries) with jsign.

.DESCRIPTION
  Primary production path:

    SSL.com code signing certificate on a YubiKey
      -> jsign --storetype YUBIKEY
      -> PIN from environment (never on the command line; jsign "env:VAR" syntax)
      -> RFC3161 timestamp via SSL.com

  Credential modes (first match wins):
    1. -PfxPath / $env:SIGN_PFX_PATH  (+ password)  -> PKCS#12
    2. Default YubiKey / hardware token via -StoreType (default YUBIKEY)

  PIN / store password is read from the first non-empty env var among:
    SIGN_STORE_PASSWORD, SIGN_PIN, YUBIKEY_PIN
  (or -StorePasswordEnv to name a different variable). The value is passed to
  jsign as --storepass env:VARNAME so it never appears in process arguments.

  Requires: jsign on PATH (e.g. choco install jsign), Java, and for YUBIKEY
  the Yubico PIV Tool (libykcs11) + SSL.com cert on slot 9a library at the default install location.

  Replaces historical @SignAll.bat / #Sign.bat and the prior signtool path.

.PARAMETER Path
  File or directory. Directories default to *.msi and *.msm only (like @SignAll *.ms?).

.PARAMETER IncludeBinaries
  When Path is a directory, also sign ship PE: *.exe, *.dll, CAD host modules
  (*.arx, *.brx, *.grx, *.zrx, *.dbx), plus *.cab / *.sys / *.ocx / *.efi.
  Use with -Recurse on product out\ (or Runtime/Studio) trees before WiX.

.PARAMETER Recurse
  Search directories recursively.

.EXAMPLE
  $env:SIGN_STORE_PASSWORD = "<token-pin>"
  .\scripts\sign-files.ps1 -Path .\dist\10.1.1.1

.EXAMPLE
  $env:SIGN_STORE_PASSWORD = "<token-pin>"
  .\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 -Alias "X.509 Certificate for Digital Signature"
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [string[]] $Path,

  # Hardware / cloud storetype for jsign (default production: YUBIKEY (slot 9a / PIV Authentication))
  [string] $StoreType = $(if ($env:SIGN_STORE_TYPE) { $env:SIGN_STORE_TYPE } else { "YUBIKEY" }),

  # Name of the env var that holds the PIN/password (value is never logged)
  [string] $StorePasswordEnv = "",

  # Optional cert alias when the token has more than one (slot name or CN)
  [string] $Alias = $(if ($env:SIGN_CERT_ALIAS) { $env:SIGN_CERT_ALIAS } else { "X.509 Certificate for PIV Authentication" }),

  # Optional external PKCS#7 chain (.p7b / .spc / PEM)
  [string] $CertFile = $env:SIGN_CERT_FILE,

  # Legacy / rare: PKCS#12 file instead of YubiKey
  [string] $PfxPath = $env:SIGN_PFX_PATH,
  [string] $PfxPassword = $env:SIGN_PFX_PASSWORD,
  [string] $PfxPasswordEnv = $(if ($env:SIGN_PFX_PASSWORD) { "" } else { "SIGN_PFX_PASSWORD" }),

  # SSL.com RFC3161 timestamp (override with SIGN_TIMESTAMP_URL)
  [string] $TimestampUrl = $(if ($env:SIGN_TIMESTAMP_URL) { $env:SIGN_TIMESTAMP_URL } else { "http://ts.ssl.com" }),
  [string] $DescriptionUrl = $(if ($env:SIGN_DESCRIPTION_URL) { $env:SIGN_DESCRIPTION_URL } else { "https://www.opendcl.com" }),
  [string] $ProgramName = $(if ($env:SIGN_PROGRAM_NAME) { $env:SIGN_PROGRAM_NAME } else { "OpenDCL" }),
  [string] $FileDigest = "SHA-256",
  [switch] $IncludeBinaries,
  [switch] $Recurse,
  [switch] $SkipVerify,
  [switch] $Replace,
  [string] $Jsign = ""
)

$ErrorActionPreference = "Stop"

function Find-Jsign {
  if ($Jsign -and (Test-Path -LiteralPath $Jsign)) { return (Resolve-Path -LiteralPath $Jsign).Path }
  $cmd = Get-Command jsign.exe -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }
  $cmd = Get-Command jsign -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }
  $choco = "C:\ProgramData\chocolatey\bin\jsign.exe"
  if (Test-Path -LiteralPath $choco) { return $choco }
  throw "jsign not found on PATH. Install with: choco install jsign  (requires Java). Or pass -Jsign."
}

function Resolve-StorePasswordEnvName {
  if ($StorePasswordEnv) {
    if (-not [string]::IsNullOrEmpty([Environment]::GetEnvironmentVariable($StorePasswordEnv))) {
      return $StorePasswordEnv
    }
    throw "Store password env var '$StorePasswordEnv' is not set or empty."
  }
  foreach ($name in @("SIGN_STORE_PASSWORD", "SIGN_PIN", "YUBIKEY_PIN")) {
    foreach ($scope in @("Process", "User", "Machine")) {
      $val = [Environment]::GetEnvironmentVariable($name, $scope)
      if (-not [string]::IsNullOrEmpty($val)) {
        # Ensure jsign env:VAR reads a process-visible value
        if ($scope -ne "Process") {
          [Environment]::SetEnvironmentVariable($name, $val, "Process")
        }
        return $name
      }
    }
  }
  return $null
}

function Get-FilesToSign([string[]] $paths) {
  # Default: installers only (historical @SignAll *.ms?)
  $exts = @(".msi", ".msm")
  if ($IncludeBinaries) {
    # OpenDCL ship PE + common Authenticode targets (not .lib / .obj / intermediates)
    $exts += @(
      ".cab", ".exe", ".dll", ".sys", ".ocx", ".efi",
      ".arx", ".brx", ".grx", ".zrx", ".dbx"
    )
  }
  $list = New-Object System.Collections.Generic.List[string]
  foreach ($p in $paths) {
    if (-not (Test-Path -LiteralPath $p)) { throw "Path not found: $p" }
    $item = Get-Item -LiteralPath $p
    if ($item.PSIsContainer) {
      $gci = @{ Path = $item.FullName; File = $true; ErrorAction = "SilentlyContinue" }
      if ($Recurse) { $gci.Recurse = $true }
      Get-ChildItem @gci |
        Where-Object { $exts -contains $_.Extension.ToLowerInvariant() } |
        ForEach-Object { [void]$list.Add($_.FullName) }
    }
    else {
      [void]$list.Add($item.FullName)
    }
  }
  return @($list | Select-Object -Unique)
}

function Get-SignBatches {
  param(
    [Parameter(Mandatory = $true)]
    [string[]] $Files,
    [int] $MaxFiles = 30,
    [int] $MaxChars = 6000
  )
  # Windows CreateProcess command-line limit is ~8191; keep jsign arg lists smaller.
  # Return List[string[]] (not nested @() unwrap tricks) for reliable PS 7 binding.
  $batches = [System.Collections.Generic.List[string[]]]::new()
  $cur = [System.Collections.Generic.List[string]]::new()
  $curLen = 0
  foreach ($f in $Files) {
    $add = $f.Length + 3
    $wouldExceed = ($cur.Count -ge $MaxFiles) -or (($cur.Count -gt 0) -and (($curLen + $add) -gt $MaxChars))
    if ($wouldExceed) {
      $batches.Add($cur.ToArray())
      $cur = [System.Collections.Generic.List[string]]::new()
      $curLen = 0
    }
    $cur.Add($f)
    $curLen += $add
  }
  if ($cur.Count -gt 0) {
    $batches.Add($cur.ToArray())
  }
  return $batches
}

function Normalize-Digest([string] $d) {
  switch -Regex ($d.Trim().ToUpperInvariant()) {
    '^(SHA-?256|SHA256)$' { return "SHA-256" }
    '^(SHA-?1|SHA1)$'     { return "SHA-1" }
    '^(SHA-?384|SHA384)$' { return "SHA-384" }
    '^(SHA-?512|SHA512)$' { return "SHA-512" }
    default { return $d }
  }
}

$jsignPath = Find-Jsign
$files = @(Get-FilesToSign $Path)
if ($files.Count -eq 0) {
  throw "No signable files matched under: $($Path -join ', ') (directories default to *.msi/*.msm; use -IncludeBinaries for PE)"
}

$digest = Normalize-Digest $FileDigest
$argList = [System.Collections.Generic.List[string]]::new()
[void]$argList.Add("sign")
[void]$argList.AddRange([string[]]@("--alg", $digest))
[void]$argList.AddRange([string[]]@("--tsaurl", $TimestampUrl))
[void]$argList.AddRange([string[]]@("--tsmode", "RFC3161"))
if ($ProgramName) { [void]$argList.AddRange([string[]]@("--name", $ProgramName)) }
if ($DescriptionUrl) { [void]$argList.AddRange([string[]]@("--url", $DescriptionUrl)) }
if ($Replace) { [void]$argList.Add("--replace") }
if ($CertFile) {
  if (-not (Test-Path -LiteralPath $CertFile)) { throw "Cert file not found: $CertFile" }
  [void]$argList.AddRange([string[]]@("--certfile", $CertFile))
}

$mode = $null
if ($PfxPath) {
  if (-not (Test-Path -LiteralPath $PfxPath)) { throw "PFX not found: $PfxPath" }
  [void]$argList.AddRange([string[]]@("--keystore", $PfxPath))
  [void]$argList.AddRange([string[]]@("--storetype", "PKCS12"))
  if ($PfxPassword) {
    # Inline only when caller passed -PfxPassword explicitly; prefer env: otherwise.
    [void]$argList.AddRange([string[]]@("--storepass", $PfxPassword))
  }
  elseif ($PfxPasswordEnv -and -not [string]::IsNullOrEmpty([Environment]::GetEnvironmentVariable($PfxPasswordEnv))) {
    [void]$argList.AddRange([string[]]@("--storepass", "env:$PfxPasswordEnv"))
  }
  else {
    throw "PFX requires -PfxPassword or env SIGN_PFX_PASSWORD."
  }
  if ($Alias) { [void]$argList.AddRange([string[]]@("--alias", $Alias)) }
  $mode = "PKCS12 ($PfxPath)"
}
else {
  $pinEnv = Resolve-StorePasswordEnvName
  if (-not $pinEnv) {
    throw @"
No signing credentials configured.

Preferred (SSL.com YubiKey + jsign):
  1. YubiKey inserted; Yubico PIV Tool installed (ykcs11); SSL.com cert in PIV slot 9a
  2. Set PIN in the process/user environment (not git):
       `$env:SIGN_STORE_PASSWORD = '<token-pin>'
     (also accepted: SIGN_PIN, YUBIKEY_PIN)
  3. Run this script. Optional: -Alias if the token has multiple certs.

PKCS#12 alternative:
  -PfxPath file.pfx with SIGN_PFX_PASSWORD / -PfxPassword

Never commit PINs, PFX passwords, or private keys to git.
"@
  }
  [void]$argList.AddRange([string[]]@("--storetype", $StoreType))
  [void]$argList.AddRange([string[]]@("--storepass", "env:$pinEnv"))
  # YubiKey PIV PIN is 6-8 characters; longer values are usually a different token password
  # and ykcs11 returns CKR_ARGUMENTS_BAD (does not always decrement try counter).
  if ($StoreType -match '^(?i)YUBIKEY|PIV$') {
    $pinLen = ([Environment]::GetEnvironmentVariable($pinEnv, 'Process')).Length
    if ($pinLen -lt 6 -or $pinLen -gt 8) {
      throw "YubiKey PIV PIN in env:$pinEnv has length $pinLen (expected 6-8). Refusing to login so we do not burn PIN tries. Set the YubiKey user PIN in SIGN_STORE_PASSWORD (not the SafeNet eToken password)."
    }
  }
  if ($Alias) { [void]$argList.AddRange([string[]]@("--alias", $Alias)) }
  $mode = "$StoreType (PIN from env:$pinEnv)"
}

Write-Host "Signing mode: $mode"
Write-Host "jsign:        $jsignPath"
Write-Host "files:        $($files.Count)"
Write-Host "timestamp:    $TimestampUrl"
Write-Host "digest:       $digest"
Write-Host "name/url:     $ProgramName / $DescriptionUrl"
if ($Alias) { Write-Host "alias:        $Alias" }

# Batch invocations so long PE path lists stay under Windows cmdline limits.
# PIN comes from env each time; YubiKey still avoids interactive prompts.
# Named -Files is required: a bare $files array expands into positional args
# (second path -> [int] parameter => "Argument types do not match").
$batches = Get-SignBatches -Files $files
Write-Host "batches:      $($batches.Count)"
$bi = 0
foreach ($batch in $batches) {
  $bi++
  $batchFiles = [string[]]$batch
  Write-Host "--- jsign batch $bi / $($batches.Count) ($($batchFiles.Count) file(s)) ---"
  foreach ($f in $batchFiles) { Write-Host "Sign $f" }
  $allArgs = $argList.ToArray() + $batchFiles
  & $jsignPath @allArgs
  if ($LASTEXITCODE -ne 0) {
    throw "jsign failed (exit $LASTEXITCODE) on batch $bi. Check YubiKey insertion, PIN env var (YubiKey PIN, not SafeNet), and alias (slot 9a)."
  }
}

Write-Host "OK signed $($files.Count) file(s)"

if (-not $SkipVerify) {
  $bad = @()
  foreach ($f in $files) {
    $sig = Get-AuthenticodeSignature -FilePath $f
    if ($sig.Status -ne "Valid") {
      Write-Warning "Verify $($sig.Status): $f - $($sig.StatusMessage)"
      $bad += $f
    }
  }
  if ($bad.Count -gt 0) {
    throw "Authenticode verify failed for $($bad.Count) file(s)."
  }
  Write-Host "OK verified $($files.Count) file(s)"
}
