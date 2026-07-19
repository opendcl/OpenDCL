<#
.SYNOPSIS
  Legacy entry point — use compare-release-packages.ps1.

.DESCRIPTION
  Forwards to scripts/compare-release-packages.ps1 (previous release package set
  vs new package set). Prefer calling compare-release-packages.ps1 directly.

  Parameter aliases for older docs/scripts:
    -ClassicPackageDir / -BaselinePackageDir
    -CMakePackageDir / -NewPackageDir

  -ClassicRoot and -CMakeRoot are ignored (module-tree compare against legacy
  source trees is not part of the ship gate).
#>
[CmdletBinding()]
param(
  [string] $ClassicRoot = "",
  [string] $CMakeRoot = "",
  [Alias("BaselinePackageDir")]
  [string] $ClassicPackageDir = "",
  [Alias("NewPackageDir")]
  [string] $CMakePackageDir = "",
  [string] $Configuration = "Release",
  [string] $ReportDir = ""
)

$ErrorActionPreference = "Stop"

if ($ClassicRoot -or $CMakeRoot) {
  Write-Warning "ClassicRoot/CMakeRoot are ignored. Compare release packages only (BaselinePackageDir vs NewPackageDir)."
}

if (-not $ClassicPackageDir -or -not $CMakePackageDir) {
  throw "Pass -BaselinePackageDir (or -ClassicPackageDir) and -NewPackageDir (or -CMakePackageDir) to package folders."
}

$args = @{
  BaselinePackageDir = $ClassicPackageDir
  NewPackageDir      = $CMakePackageDir
}
if ($ReportDir) { $args.ReportDir = $ReportDir }

& (Join-Path $PSScriptRoot "compare-release-packages.ps1") @args
