<#
.SYNOPSIS
  Compare CMake build/package outputs against a classic (pre-CMake) tree or release.

.DESCRIPTION
  Inventories runtime modules, resource DLLs, Studio, RxInstall, and WiX packages
  from two roots and writes a Markdown + CSV report of differences:

    - Presence (classic-only / cmake-only / both)
    - File size (delta bytes / percent)
    - PE machine (x86 vs x64) when applicable
    - Optional: PE export/import summary via dumpbin if available

  Typical classic sources:
    - Same repo after OpenDCL.sln MSBuild (outputs under Runtime\…\Release\)
    - P:\Work\OpenDCL\Releases\v9.3.3.1\ (packages only)
    - Sibling classic build tree

.PARAMETER ClassicRoot
  Root with classic layout (repo root containing Runtime\, Studio\) and/or packages.

.PARAMETER CMakeRoot
  CMake binary dir (e.g. build\vs2022-x64-full) or a dir whose out\ mirrors classic.

.PARAMETER ClassicPackageDir
  Optional folder of classic .msi/.msm (default: search ClassicRoot and well-known Releases).

.PARAMETER CMakePackageDir
  Optional folder of CMake-built .msi/.msm (default: wix\out\cmake-full-Release).

.PARAMETER Configuration
  Module config folder name (default Release).

.PARAMETER ReportDir
  Where to write compare-report.md / .csv (default wix\out\compare\<timestamp>).
#>
[CmdletBinding()]
param(
  [string] $ClassicRoot = "",
  [string] $CMakeRoot = "",
  [string] $ClassicPackageDir = "",
  [string] $CMakePackageDir = "",
  [string] $Configuration = "Release",
  [string] $ReportDir = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if (-not $ClassicRoot) { $ClassicRoot = $RepoRoot }
if (-not $CMakeRoot) {
  foreach ($c in @(
      (Join-Path $RepoRoot "build\vs2022-x64-full"),
      (Join-Path $RepoRoot "build\vs2022-x64-dev"),
      (Join-Path $RepoRoot "build\vs2022-x64-auto")
    )) {
    if (Test-Path $c) { $CMakeRoot = $c; break }
  }
}
if (-not $CMakeRoot) { throw "Pass -CMakeRoot (CMake binary dir)." }

$ClassicRoot = (Resolve-Path $ClassicRoot).Path
$CMakeRoot = (Resolve-Path $CMakeRoot).Path

if (-not $ReportDir) {
  $ts = Get-Date -Format "yyyyMMdd-HHmmss"
  $ReportDir = Join-Path $RepoRoot "wix\out\compare\$ts"
}
New-Item -ItemType Directory -Force -Path $ReportDir | Out-Null

function Get-PeMachine([string] $path) {
  if (-not (Test-Path -LiteralPath $path)) { return "" }
  try {
    $fs = [IO.File]::OpenRead($path)
    $br = New-Object IO.BinaryReader $fs
    if ($br.ReadUInt16() -ne 0x5A4D) { $fs.Dispose(); return "?" }
    $fs.Seek(0x3C, 'Begin') | Out-Null
    $peOff = $br.ReadInt32()
    $fs.Seek($peOff, 'Begin') | Out-Null
    if ($br.ReadUInt32() -ne 0x00004550) { $fs.Dispose(); return "?" }
    $m = $br.ReadUInt16()
    $fs.Dispose()
    switch ($m) {
      0x14c  { return "x86" }
      0x8664 { return "x64" }
      default { return ("0x{0:X}" -f $m) }
    }
  }
  catch { return "?" }
}

function Resolve-UnderRoot([string] $root, [string] $rel) {
  $c1 = Join-Path $root $rel
  if (Test-Path -LiteralPath $c1) { return (Resolve-Path -LiteralPath $c1).Path }
  $c2 = Join-Path $root (Join-Path "out" $rel)
  if (Test-Path -LiteralPath $c2) { return (Resolve-Path -LiteralPath $c2).Path }
  return $null
}

# --- Module catalog from build-wix.ps1 ---
$catalogFile = Join-Path $RepoRoot "scripts\build-wix.ps1"
$catalogLines = Select-String -Path $catalogFile -Pattern 'Runtime\\.*\\\{Config\}\\' | ForEach-Object {
  if ($_.Line -match '"([^"]+\{Config\}[^"]+)"') { $Matches[1] }
}
$rels = foreach ($tmpl in $catalogLines) {
  $tmpl.Replace("{Config}", $Configuration)
}

# Also Studio / Res / RxInstall product files
$extraRels = @(
  "Runtime\RxInstall\$Configuration\RxInstall.dll",
  "Studio\x64\$Configuration\OpenDCL Studio.exe",
  "Studio\Win32\$Configuration\OpenDCL Studio.exe"
)
foreach ($lang in @("ENU", "DEU", "ESM", "RUS", "CHS", "FRA", "CHT")) {
  $extraRels += "Runtime\Localized\$lang\Runtime.Res\$Configuration\Runtime.Res.dll"
  $extraRels += "Studio\Localized\$lang\Studio.Res\$Configuration\Studio.Res.dll"
  $extraRels += "Studio\Localized\$lang\Content\OpenDCL.chm"
}

$allRels = @($rels) + $extraRels | Select-Object -Unique

$rows = foreach ($rel in $allRels) {
  $cPath = Resolve-UnderRoot $ClassicRoot $rel
  $mPath = Resolve-UnderRoot $CMakeRoot $rel
  # CMake often mirrors to classic paths under RepoRoot too - if ClassicRoot is repo,
  # prefer non-out path; if cmake also wrote mirror, sizes may match intentionally.
  $cSize = if ($cPath) { (Get-Item -LiteralPath $cPath).Length } else { $null }
  $mSize = if ($mPath) { (Get-Item -LiteralPath $mPath).Length } else { $null }
  $status = if ($cPath -and $mPath) { "both" }
  elseif ($cPath) { "classic-only" }
  elseif ($mPath) { "cmake-only" }
  else { "missing-both" }

  $delta = $null
  $pct = $null
  if ($null -ne $cSize -and $null -ne $mSize) {
    $delta = $mSize - $cSize
    if ($cSize -gt 0) { $pct = [math]::Round(100.0 * $delta / $cSize, 2) }
  }

  [pscustomobject]@{
    Rel           = $rel
    Status        = $status
    ClassicSize   = $cSize
    CMakeSize     = $mSize
    DeltaBytes    = $delta
    DeltaPct      = $pct
    ClassicArch   = if ($cPath -and $rel -match '\.(exe|dll|arx|brx|grx|zrx)$') { Get-PeMachine $cPath } else { "" }
    CMakeArch     = if ($mPath -and $rel -match '\.(exe|dll|arx|brx|grx|zrx)$') { Get-PeMachine $mPath } else { "" }
    ClassicPath   = $cPath
    CMakePath     = $mPath
  }
}

# --- Packages ---
if (-not $ClassicPackageDir) {
  $candidates = @(
    (Join-Path $ClassicRoot "wix\out\Release"),
    (Join-Path $ClassicRoot "Runtime\Install\Release"),
    "P:\Work\OpenDCL\Releases\v9.3.3.1",
    "P:\Work\OpenDCL\Releases\v10.1.1.1"
  )
  foreach ($c in $candidates) {
    if (Test-Path $c) {
      $has = Get-ChildItem $c -Filter "*.msi" -ErrorAction SilentlyContinue
      if ($has) { $ClassicPackageDir = $c; break }
    }
  }
}
if (-not $CMakePackageDir) {
  foreach ($c in @(
      (Join-Path $RepoRoot "wix\out\cmake-full-$Configuration"),
      (Join-Path $RepoRoot "wix\out\Release"),
      (Join-Path $RepoRoot "wix\out\smoke-studio-brx27-enu")
    )) {
    if (Test-Path $c) { $CMakePackageDir = $c; break }
  }
}

function Get-PackageIndex([string] $dir) {
  if (-not $dir -or -not (Test-Path $dir)) { return @{} }
  $map = @{}
  Get-ChildItem $dir -File -ErrorAction SilentlyContinue | Where-Object {
    $_.Extension -match '\.(msi|msm)$'
  } | ForEach-Object {
    # Normalize name: strip version suffix .9.3.3.1 / .custom
    $base = $_.Name -replace '\.\d+\.\d+\.\d+\.\d+', '' -replace '\.custom', ''
    $map[$base] = $_
  }
  return $map
}

$classicPkgs = Get-PackageIndex $ClassicPackageDir
$cmakePkgs = Get-PackageIndex $CMakePackageDir
$pkgNames = @($classicPkgs.Keys + $cmakePkgs.Keys) | Select-Object -Unique | Sort-Object

$pkgRows = foreach ($name in $pkgNames) {
  $c = $classicPkgs[$name]
  $m = $cmakePkgs[$name]
  $status = if ($c -and $m) { "both" } elseif ($c) { "classic-only" } elseif ($m) { "cmake-only" } else { "missing-both" }
  $cSize = if ($c) { $c.Length } else { $null }
  $mSize = if ($m) { $m.Length } else { $null }
  $delta = if ($null -ne $cSize -and $null -ne $mSize) { $mSize - $cSize } else { $null }
  $pct = if ($null -ne $delta -and $cSize -gt 0) { [math]::Round(100.0 * $delta / $cSize, 2) } else { $null }
  [pscustomobject]@{
    Package     = $name
    Status      = $status
    ClassicSize = $cSize
    CMakeSize   = $mSize
    DeltaBytes  = $delta
    DeltaPct    = $pct
    ClassicFile = if ($c) { $c.FullName } else { "" }
    CMakeFile   = if ($m) { $m.FullName } else { "" }
  }
}

# --- Write reports ---
$csvMods = Join-Path $ReportDir "modules.csv"
$csvPkgs = Join-Path $ReportDir "packages.csv"
$rows | Export-Csv -NoTypeInformation -Encoding UTF8 $csvMods
$pkgRows | Export-Csv -NoTypeInformation -Encoding UTF8 $csvPkgs

$both = @($rows | Where-Object Status -eq "both")
$cOnly = @($rows | Where-Object Status -eq "classic-only")
$mOnly = @($rows | Where-Object Status -eq "cmake-only")
$missing = @($rows | Where-Object Status -eq "missing-both")
$sizeDiff = @($both | Where-Object { $_.DeltaBytes -ne 0 -and $null -ne $_.DeltaBytes })
$archDiff = @($both | Where-Object { $_.ClassicArch -and $_.CMakeArch -and ($_.ClassicArch -ne $_.CMakeArch) })

$md = New-Object System.Text.StringBuilder
[void]$md.AppendLine("# CMake vs classic comparison")
[void]$md.AppendLine("")
[void]$md.AppendLine("- Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
[void]$md.AppendLine("- ClassicRoot: ``$ClassicRoot``")
[void]$md.AppendLine("- CMakeRoot: ``$CMakeRoot``")
[void]$md.AppendLine("- Configuration: ``$Configuration``")
[void]$md.AppendLine("- ClassicPackageDir: ``$ClassicPackageDir``")
[void]$md.AppendLine("- CMakePackageDir: ``$CMakePackageDir``")
[void]$md.AppendLine("")
[void]$md.AppendLine("## Module / product file inventory")
[void]$md.AppendLine("")
[void]$md.AppendLine("| Status | Count |")
[void]$md.AppendLine("| --- | ---: |")
[void]$md.AppendLine("| both | $($both.Count) |")
[void]$md.AppendLine("| classic-only | $($cOnly.Count) |")
[void]$md.AppendLine("| cmake-only | $($mOnly.Count) |")
[void]$md.AppendLine("| missing-both | $($missing.Count) |")
[void]$md.AppendLine("| size differs (both) | $($sizeDiff.Count) |")
[void]$md.AppendLine("| arch differs (both) | $($archDiff.Count) |")
[void]$md.AppendLine("")

function Append-Table($title, $list, $limit = 40) {
  [void]$md.AppendLine("### $title")
  [void]$md.AppendLine("")
  if (-not $list -or $list.Count -eq 0) {
    [void]$md.AppendLine("_None._")
    [void]$md.AppendLine("")
    return
  }
  [void]$md.AppendLine("| Rel | Classic | CMake | Delta | Arch C/M |")
  [void]$md.AppendLine("| --- | ---: | ---: | ---: | --- |")
  $n = 0
  foreach ($r in ($list | Sort-Object Rel)) {
    if ($n -ge $limit) {
      [void]$md.AppendLine("| … | | | | _($($list.Count - $limit) more in CSV)_ |")
      break
    }
    $cs = if ($null -ne $r.ClassicSize) { $r.ClassicSize } else { "-" }
    $ms = if ($null -ne $r.CMakeSize) { $r.CMakeSize } else { "-" }
    $d = if ($null -ne $r.DeltaBytes) { $r.DeltaBytes } else { "-" }
    $a = "$(if ($r.ClassicArch) { $r.ClassicArch } else { '-' })/$(if ($r.CMakeArch) { $r.CMakeArch } else { '-' })"
    [void]$md.AppendLine("| ``$($r.Rel)`` | $cs | $ms | $d | $a |")
    $n++
  }
  [void]$md.AppendLine("")
}

Append-Table "Classic only" $cOnly
Append-Table "CMake only" $mOnly
Append-Table "Size differences (both present)" ($sizeDiff | Sort-Object { [math]::Abs([int64]$_.DeltaBytes) } -Descending)
Append-Table "Architecture differences" $archDiff

[void]$md.AppendLine("## Packages")
[void]$md.AppendLine("")
[void]$md.AppendLine("| Package | Status | Classic bytes | CMake bytes | Delta | Delta% |")
[void]$md.AppendLine("| --- | --- | ---: | ---: | ---: | ---: |")
foreach ($p in $pkgRows) {
  $cs = if ($null -ne $p.ClassicSize) { $p.ClassicSize } else { "-" }
  $ms = if ($null -ne $p.CMakeSize) { $p.CMakeSize } else { "-" }
  $d = if ($null -ne $p.DeltaBytes) { $p.DeltaBytes } else { "-" }
  $pct = if ($null -ne $p.DeltaPct) { $p.DeltaPct } else { "-" }
  [void]$md.AppendLine("| ``$($p.Package)`` | $($p.Status) | $cs | $ms | $d | $pct |")
}
[void]$md.AppendLine("")
[void]$md.AppendLine("## Notes")
[void]$md.AppendLine("")
[void]$md.AppendLine("- CMake FullDebug modules use **/MDd** for all families; classic ARX FullDebug was hybrid **/MD**.")
[void]$md.AppendLine("- Studio is static MFC+/MT; x64 packages install to **ProgramFiles64Folder**.")
[void]$md.AppendLine("- Runtime MSM remains **CommonFilesFolder** (x86 MSM) - unchanged.")
[void]$md.AppendLine("- Size deltas vs older versioned Releases (e.g. 9.3.3.1) include product growth, not only CMake.")
[void]$md.AppendLine("- CSVs: ``modules.csv``, ``packages.csv`` in this report folder.")
[void]$md.AppendLine("")

$mdPath = Join-Path $ReportDir "compare-report.md"
[IO.File]::WriteAllText($mdPath, $md.ToString(), [Text.UTF8Encoding]::new($false))

Write-Host "Report: $mdPath" -ForegroundColor Green
Write-Host "Modules CSV: $csvMods"
Write-Host "Packages CSV: $csvPkgs"
Write-Host ("Summary: both={0} classic-only={1} cmake-only={2} sizeDiff={3}" -f `
  $both.Count, $cOnly.Count, $mOnly.Count, $sizeDiff.Count)

# Print short summary to console
$pkgRows | Format-Table Package, Status, ClassicSize, CMakeSize, DeltaPct -AutoSize
return $ReportDir
