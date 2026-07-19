<#
.SYNOPSIS
  Compare a new installer package set to a previous release package set.

.DESCRIPTION
  Inventories .msi / .msm from two directories (normalized names, stripping
  A.B.C.D version suffixes) and writes Markdown + CSV of:

    - Presence (baseline-only / new-only / both)
    - File size (delta bytes / percent)

  Use this after a successful make-release to diff against the **previous**
  accepted package folder (e.g. prior dist\<ver>\ or archived GitHub Release
  assets). Do not point this at legacy source trees for ship gates.

.PARAMETER BaselinePackageDir
  Previous release package directory (versioned MSI/MSM).

.PARAMETER NewPackageDir
  New package directory (e.g. dist\10.1.1.1 or wix\out\Release).

.PARAMETER ReportDir
  Where to write compare-report.md / packages.csv (default wix\out\compare\<timestamp>).

.EXAMPLE
  .\scripts\compare-release-packages.ps1 `
    -BaselinePackageDir dist\10.0.0.0 `
    -NewPackageDir dist\10.1.1.1
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [string] $BaselinePackageDir,
  [Parameter(Mandatory = $true)]
  [string] $NewPackageDir,
  [string] $ReportDir = ""
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if (-not (Test-Path -LiteralPath $BaselinePackageDir)) {
  throw "BaselinePackageDir not found: $BaselinePackageDir"
}
if (-not (Test-Path -LiteralPath $NewPackageDir)) {
  throw "NewPackageDir not found: $NewPackageDir"
}
$BaselinePackageDir = (Resolve-Path -LiteralPath $BaselinePackageDir).Path
$NewPackageDir = (Resolve-Path -LiteralPath $NewPackageDir).Path

if (-not $ReportDir) {
  $ts = Get-Date -Format "yyyyMMdd-HHmmss"
  $ReportDir = Join-Path $RepoRoot "wix\out\compare\$ts"
}
New-Item -ItemType Directory -Force -Path $ReportDir | Out-Null

function Get-PackageIndex([string] $dir) {
  $map = @{}
  Get-ChildItem $dir -File -ErrorAction SilentlyContinue | Where-Object {
    $_.Extension -match '\.(msi|msm)$'
  } | ForEach-Object {
    $base = $_.Name -replace '\.\d+\.\d+\.\d+\.\d+', '' -replace '\.custom', ''
    $map[$base] = $_
  }
  return $map
}

$basePkgs = Get-PackageIndex $BaselinePackageDir
$newPkgs = Get-PackageIndex $NewPackageDir
$pkgNames = @($basePkgs.Keys + $newPkgs.Keys) | Select-Object -Unique | Sort-Object

$pkgRows = foreach ($name in $pkgNames) {
  $b = $basePkgs[$name]
  $n = $newPkgs[$name]
  $status = if ($b -and $n) { "both" } elseif ($b) { "baseline-only" } elseif ($n) { "new-only" } else { "missing-both" }
  $bSize = if ($b) { $b.Length } else { $null }
  $nSize = if ($n) { $n.Length } else { $null }
  $delta = if ($null -ne $bSize -and $null -ne $nSize) { $nSize - $bSize } else { $null }
  $pct = if ($null -ne $delta -and $bSize -gt 0) { [math]::Round(100.0 * $delta / $bSize, 2) } else { $null }
  [pscustomobject]@{
    Package       = $name
    Status        = $status
    BaselineSize  = $bSize
    NewSize       = $nSize
    DeltaBytes    = $delta
    DeltaPct      = $pct
    BaselineFile  = if ($b) { $b.FullName } else { "" }
    NewFile       = if ($n) { $n.FullName } else { "" }
  }
}

$csvPkgs = Join-Path $ReportDir "packages.csv"
$pkgRows | Export-Csv -NoTypeInformation -Encoding UTF8 $csvPkgs

$both = @($pkgRows | Where-Object Status -eq "both")
$bOnly = @($pkgRows | Where-Object Status -eq "baseline-only")
$nOnly = @($pkgRows | Where-Object Status -eq "new-only")
$sizeDiff = @($both | Where-Object { $_.DeltaBytes -ne 0 -and $null -ne $_.DeltaBytes })

$md = New-Object System.Text.StringBuilder
[void]$md.AppendLine("# Release package comparison")
[void]$md.AppendLine("")
[void]$md.AppendLine("- Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
[void]$md.AppendLine("- BaselinePackageDir: ``$BaselinePackageDir``")
[void]$md.AppendLine("- NewPackageDir: ``$NewPackageDir``")
[void]$md.AppendLine("")
[void]$md.AppendLine("## Summary")
[void]$md.AppendLine("")
[void]$md.AppendLine("| Status | Count |")
[void]$md.AppendLine("| --- | ---: |")
[void]$md.AppendLine("| both | $($both.Count) |")
[void]$md.AppendLine("| baseline-only | $($bOnly.Count) |")
[void]$md.AppendLine("| new-only | $($nOnly.Count) |")
[void]$md.AppendLine("| size differs (both) | $($sizeDiff.Count) |")
[void]$md.AppendLine("")
[void]$md.AppendLine("## Packages")
[void]$md.AppendLine("")
[void]$md.AppendLine("| Package | Status | Baseline bytes | New bytes | Delta | Delta% |")
[void]$md.AppendLine("| --- | --- | ---: | ---: | ---: | ---: |")
foreach ($p in $pkgRows) {
  $bs = if ($null -ne $p.BaselineSize) { $p.BaselineSize } else { "-" }
  $ns = if ($null -ne $p.NewSize) { $p.NewSize } else { "-" }
  $d = if ($null -ne $p.DeltaBytes) { $p.DeltaBytes } else { "-" }
  $pct = if ($null -ne $p.DeltaPct) { $p.DeltaPct } else { "-" }
  [void]$md.AppendLine("| ``$($p.Package)`` | $($p.Status) | $bs | $ns | $d | $pct |")
}
[void]$md.AppendLine("")
if ($bOnly.Count -gt 0) {
  [void]$md.AppendLine("### Baseline only (dropped or renamed)")
  [void]$md.AppendLine("")
  foreach ($p in $bOnly) { [void]$md.AppendLine("- ``$($p.Package)``") }
  [void]$md.AppendLine("")
}
if ($nOnly.Count -gt 0) {
  [void]$md.AppendLine("### New only (added)")
  [void]$md.AppendLine("")
  foreach ($p in $nOnly) { [void]$md.AppendLine("- ``$($p.Package)``") }
  [void]$md.AppendLine("")
}

$mdPath = Join-Path $ReportDir "compare-report.md"
[IO.File]::WriteAllText($mdPath, $md.ToString(), [Text.UTF8Encoding]::new($false))

Write-Host "=== compare-release-packages ==="
Write-Host "Baseline : $BaselinePackageDir"
Write-Host "New      : $NewPackageDir"
Write-Host "Report   : $mdPath"
Write-Host ("Packages : both={0} baseline-only={1} new-only={2} size-diff={3}" -f `
  $both.Count, $bOnly.Count, $nOnly.Count, $sizeDiff.Count)
return $ReportDir
