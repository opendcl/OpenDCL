<#
.SYNOPSIS
  Update opendcl.github.io update-check / download version files for a public ship.

.DESCRIPTION
  Writes the three files that must stay in sync on GitHub Pages:

    version/version.txt       == assets/versions.js  stable
    version/version_dev.txt   == assets/versions.js  current

  Bodies of the .txt files are plain four-part A.B.C.D only (Runtime GET clients).

  Normal usage (most public ships):
    Only the **dev/current** channel changes when you publish a new build:

      .\scripts\update-site-versions.ps1 -DevVersion 10.1.1.1

  Promoting a shipped dev build to **stable** (usual stable release):
    Does not invent a new number; copies current → stable (optionally after
    setting -DevVersion in the same run):

      .\scripts\update-site-versions.ps1 -PromoteToStable
      .\scripts\update-site-versions.ps1 -DevVersion 10.1.1.1 -PromoteToStable

  Exceptions (rare): set stable and/or current explicitly without promote:

      .\scripts\update-site-versions.ps1 -StableVersion 10.0.0.0
      .\scripts\update-site-versions.ps1 -StableVersion 10.0.0.0 -DevVersion 10.1.1.1

  Never run this as part of a private dry-run. Prefer after GitHub Release
  assets for that version exist (use -RequireRelease to enforce).

.PARAMETER SiteRoot
  Clone of opendcl/opendcl.github.io. Default: sibling ../opendcl.github.io
  relative to this repository root.

.PARAMETER DevVersion
  Four-part version for version_dev.txt and versions.js "current".

.PARAMETER StableVersion
  Four-part version for version.txt and versions.js "stable".
  Prefer -PromoteToStable for the common case of promoting current → stable.

.PARAMETER PromoteToStable
  Set stable to the resulting current (after -DevVersion if provided, else
  the site's existing current). Typical "make this shipped dev the stable".

.PARAMETER RequireRelease
  Fail unless `gh release view v{ver}` succeeds on opendcl/OpenDCL for each
  version that will be written (needs network + gh auth).

.PARAMETER ProductRepo
  Product repo for -RequireRelease (default: opendcl/OpenDCL).

.PARAMETER GitCommit
  git add + commit the three files in SiteRoot (does not push).

.PARAMETER WhatIf
  Show planned writes without modifying files.

.EXAMPLE
  # After publishing GitHub Release v10.1.1.1 as a development ship
  .\scripts\update-site-versions.ps1 -DevVersion 10.1.1.1 -RequireRelease

.EXAMPLE
  # Later: promote that same build to stable (no new version number)
  .\scripts\update-site-versions.ps1 -PromoteToStable -RequireRelease -GitCommit
#>
[CmdletBinding(SupportsShouldProcess = $true)]
param(
  [string] $SiteRoot = "",
  [Alias("CurrentVersion")]
  [string] $DevVersion = "",
  [string] $StableVersion = "",
  [switch] $PromoteToStable,
  [switch] $RequireRelease,
  [string] $ProductRepo = "opendcl/OpenDCL",
  [switch] $GitCommit
)

$ErrorActionPreference = "Stop"

function Test-FourPartVersion([string] $v) {
  return ($v -match '^\d+\.\d+\.\d+\.\d+$')
}

function Read-PlainVersion([string] $path) {
  if (-not (Test-Path -LiteralPath $path)) {
    throw "Missing version file: $path"
  }
  $raw = [System.IO.File]::ReadAllText($path).Trim()
  # Strip UTF-8 BOM if present
  if ($raw.Length -gt 0 -and [int][char]$raw[0] -eq 0xFEFF) {
    $raw = $raw.Substring(1).Trim()
  }
  if (-not (Test-FourPartVersion $raw)) {
    throw "Invalid version in ${path}: '$raw' (expected A.B.C.D)"
  }
  return $raw
}

function Write-PlainVersion([string] $path, [string] $version) {
  # Single line, LF, no BOM — matches typical Pages/static hosts and client trim().
  $text = $version + "`n"
  $utf8NoBom = New-Object System.Text.UTF8Encoding $false
  [System.IO.File]::WriteAllText($path, $text, $utf8NoBom)
}

function Read-VersionsJs([string] $path) {
  $js = [System.IO.File]::ReadAllText($path)
  if ($js -notmatch 'stable:\s*"([^"]+)"') {
    throw "Could not parse stable in $path"
  }
  $stable = $Matches[1]
  if ($js -notmatch 'current:\s*"([^"]+)"') {
    throw "Could not parse current in $path"
  }
  $current = $Matches[1]
  return [pscustomobject]@{ Path = $path; Text = $js; Stable = $stable; Current = $current }
}

function Write-VersionsJs([string] $path, [string] $jsText, [string] $stable, [string] $current) {
  $updated = $jsText
  $updated = [regex]::Replace($updated, 'stable:\s*"[^"]*"', "stable: `"$stable`"")
  $updated = [regex]::Replace($updated, 'current:\s*"[^"]*"', "current: `"$current`"")
  $utf8NoBom = New-Object System.Text.UTF8Encoding $false
  [System.IO.File]::WriteAllText($path, $updated, $utf8NoBom)
}

function Assert-ReleaseExists([string] $version) {
  $tag = "v$version"
  Write-Host "Checking release $tag on $ProductRepo ..."
  & gh release view $tag -R $ProductRepo 1>$null 2>$null
  if ($LASTEXITCODE -ne 0) {
    throw "GitHub Release $tag not found on $ProductRepo (create the release before updating site versions, or omit -RequireRelease)."
  }
  Write-Host "  OK $tag"
}

# --- resolve site root ---
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
if (-not $SiteRoot) {
  $candidate = Join-Path $RepoRoot "..\opendcl.github.io"
  if (-not (Test-Path -LiteralPath $candidate)) {
    throw "SiteRoot not set and sibling not found: $candidate"
  }
  $SiteRoot = (Resolve-Path -LiteralPath $candidate).Path
}
else {
  $SiteRoot = (Resolve-Path -LiteralPath $SiteRoot).Path
}

$txtStable = Join-Path $SiteRoot "version\version.txt"
$txtDev = Join-Path $SiteRoot "version\version_dev.txt"
$jsPath = Join-Path $SiteRoot "assets\versions.js"

foreach ($p in @($txtStable, $txtDev, $jsPath)) {
  if (-not (Test-Path -LiteralPath $p)) {
    throw "Expected site file missing: $p"
  }
}

# --- read current site state ---
$oldStable = Read-PlainVersion $txtStable
$oldDev = Read-PlainVersion $txtDev
$js = Read-VersionsJs $jsPath

if ($js.Stable -ne $oldStable -or $js.Current -ne $oldDev) {
  Write-Warning @"
Site files are already out of sync before update:
  version.txt        = $oldStable
  version_dev.txt    = $oldDev
  versions.js stable = $($js.Stable)
  versions.js current= $($js.Current)
Continuing with .txt files as source of truth for 'old' values; versions.js will be rewritten to match the new pair.
"@
}

# --- resolve new versions ---
$newStable = $oldStable
$newDev = $oldDev

if ($DevVersion) {
  $DevVersion = $DevVersion.Trim()
  if (-not (Test-FourPartVersion $DevVersion)) {
    throw "DevVersion must be A.B.C.D (got '$DevVersion')"
  }
  $newDev = $DevVersion
}

if ($StableVersion) {
  $StableVersion = $StableVersion.Trim()
  if (-not (Test-FourPartVersion $StableVersion)) {
    throw "StableVersion must be A.B.C.D (got '$StableVersion')"
  }
  $newStable = $StableVersion
}

if ($PromoteToStable) {
  # Common path: promote the (possibly just-updated) current build to stable.
  if ($StableVersion) {
    # Explicit stable wins; still require it equals resulting current unless both set intentionally different.
    if ($newStable -ne $newDev) {
      Write-Warning "PromoteToStable with -StableVersion: stable=$newStable current=$newDev (not equal — exception path)."
    }
  }
  else {
    $newStable = $newDev
  }
}

if (-not $DevVersion -and -not $StableVersion -and -not $PromoteToStable) {
  throw @"
Nothing to do. Specify one of:
  -DevVersion A.B.C.D              # normal ship (current/dev only)
  -PromoteToStable                 # promote current → stable
  -StableVersion A.B.C.D           # exception: set stable only
  combinations of the above
"@
}

if ($newStable -eq $oldStable -and $newDev -eq $oldDev) {
  Write-Host "No change: stable=$newStable current=$newDev"
  return
}

Write-Host "=== update-site-versions ==="
Write-Host "SiteRoot     = $SiteRoot"
Write-Host "stable       : $oldStable  ->  $newStable"
Write-Host "current/dev  : $oldDev  ->  $newDev"

if ($RequireRelease) {
  $toCheck = @($newStable, $newDev) | Select-Object -Unique
  foreach ($v in $toCheck) {
    Assert-ReleaseExists $v
  }
}

if (-not $PSCmdlet.ShouldProcess($SiteRoot, "Write version.txt / version_dev.txt / versions.js")) {
  return
}

Write-PlainVersion $txtStable $newStable
Write-PlainVersion $txtDev $newDev
Write-VersionsJs $jsPath $js.Text $newStable $newDev

# verify round-trip
$v1 = Read-PlainVersion $txtStable
$v2 = Read-PlainVersion $txtDev
$js2 = Read-VersionsJs $jsPath
if ($v1 -ne $newStable -or $v2 -ne $newDev -or $js2.Stable -ne $newStable -or $js2.Current -ne $newDev) {
  throw "Post-write verification failed (files do not match intended stable/current)."
}

Write-Host "OK wrote:"
Write-Host "  version/version.txt     = $newStable"
Write-Host "  version/version_dev.txt = $newDev"
Write-Host "  assets/versions.js      stable=$newStable current=$newDev"

if ($GitCommit) {
  Push-Location $SiteRoot
  try {
    git add -- "version/version.txt" "version/version_dev.txt" "assets/versions.js"
    $status = git status --porcelain -- "version/version.txt" "version/version_dev.txt" "assets/versions.js"
    if (-not $status) {
      Write-Host "Git: nothing to commit (already up to date)."
    }
    else {
      $msg = "Site versions: stable=$newStable current=$newDev"
      git commit -m $msg
      if ($LASTEXITCODE -ne 0) { throw "git commit failed ($LASTEXITCODE)" }
      Write-Host "Git: committed — $msg"
      Write-Host "Push when ready: git -C `"$SiteRoot`" push"
    }
  }
  finally {
    Pop-Location
  }
}
else {
  Write-Host "Tip: review git diff in SiteRoot, then commit/push (or re-run with -GitCommit)."
}

Write-Host "After Pages deploy, verify:"
Write-Host "  https://www.opendcl.com/version/version.txt"
Write-Host "  https://www.opendcl.com/version/version_dev.txt"
