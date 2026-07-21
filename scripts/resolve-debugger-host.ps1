#Requires -Version 5.1
<#
.SYNOPSIS
  Resolve the default CAD host .exe for Visual Studio F5 (LocalDebuggerCommand).

.DESCRIPTION
  Used by CMake (opendcl_resolve_debugger_command) when OPENDCL_*_DEBUGGER_COMMAND
  cache/env overrides are empty. Discovers install location from the product
  registry (same roots as Runtime/RxInstall), then returns the first existing host
  executable path. No hard-coded Program Files fallbacks.

  Family / Version match OpenDCL runtime matrix fields:
    ARX  VERSION = ARX major (16..26)  → prefer HKCU CurVer (last used) in R{ver}.* series
    BRX  VERSION = BRX major (9..27)   → HKLM\SOFTWARE\Bricsys\Bricscad\V{ver}[x64]
    ZRX  VERSION = product year        → HKLM\SOFTWARE\ZWSOFT\ZWCAD\{year}
    GRX  VERSION = product year        → HKLM\SOFTWARE\GstarSoft\GstarCAD\R{year-2000}...

  Prints one absolute path to stdout on success; exit 0. Exit 1 if not found.

.PARAMETER Family
  ARX | BRX | GRX | ZRX

.PARAMETER Version
  Matrix VERSION string for that runtime row.

.PARAMETER Arch
  x64 | x86 (matrix ARCH)
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [ValidateSet('ARX', 'BRX', 'GRX', 'ZRX')]
  [string] $Family,

  [Parameter(Mandatory = $true)]
  [string] $Version,

  [Parameter(Mandatory = $true)]
  [ValidateSet('x64', 'x86')]
  [string] $Arch
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'SilentlyContinue'

function Get-FirstExistingExe {
  param(
    [string] $Directory,
    [string[]] $Names
  )
  if ([string]::IsNullOrWhiteSpace($Directory)) {
    return $null
  }
  $dir = $Directory.Trim().TrimEnd('\', '/')
  if (-not (Test-Path -LiteralPath $dir)) {
    return $null
  }
  foreach ($name in $Names) {
    $candidate = Join-Path $dir $name
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
      return [System.IO.Path]::GetFullPath($candidate)
    }
  }
  return $null
}

function Get-SoftwareHives {
  param([string] $Arch)
  # Native view first for the module arch; WOW6432Node for 32-bit products on x64 OS.
  if ($Arch -eq 'x86') {
    return @(
      'HKLM:\SOFTWARE\WOW6432Node',
      'HKLM:\SOFTWARE'
    )
  }
  return @(
    'HKLM:\SOFTWARE',
    'HKLM:\SOFTWARE\WOW6432Node'
  )
}

function Get-PropertyString {
  param($Object, [string] $Name)
  if ($null -eq $Object) { return $null }
  $p = $Object.PSObject.Properties[$Name]
  if ($null -eq $p) { return $null }
  $v = $p.Value
  if ($null -eq $v) { return $null }
  return [string]$v
}

function Get-ArxExeFromSeriesKey {
  param(
    [string] $SeriesKeyName,  # e.g. R25.1
    [string] $Arch,
    [string[]] $ExeNames,
    [string[]] $DirProps
  )
  # Product last-used under this series: HKCU\…\AutoCAD\R25.1\CurVer = ACAD-9101:409
  $productCurVer = $null
  $hkcuSeries = "HKCU:\SOFTWARE\Autodesk\AutoCAD\$SeriesKeyName"
  if (Test-Path -LiteralPath $hkcuSeries) {
    $hkcuProps = Get-ItemProperty -LiteralPath $hkcuSeries -ErrorAction SilentlyContinue
    $productCurVer = Get-PropertyString $hkcuProps 'CurVer'
    if (-not $productCurVer) {
      $productCurVer = Get-PropertyString $hkcuProps 'CURVER'
    }
  }

  foreach ($hive in (Get-SoftwareHives -Arch $Arch)) {
    $seriesPath = Join-Path $hive "Autodesk\AutoCAD\$SeriesKeyName"
    if (-not (Test-Path -LiteralPath $seriesPath)) { continue }

    # 1) Prefer last-used product (CurVer) under this series.
    if ($productCurVer) {
      $prodPath = Join-Path $seriesPath $productCurVer
      if (Test-Path -LiteralPath $prodPath) {
        $props = Get-ItemProperty -LiteralPath $prodPath -ErrorAction SilentlyContinue
        foreach ($dp in $DirProps) {
          $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props $dp) -Names $ExeNames
          if ($exe) { return $exe }
        }
      }
    }

    # 2) Any product under the series with AcadLocation + acad.exe.
    foreach ($prod in (Get-ChildItem -LiteralPath $seriesPath -ErrorAction SilentlyContinue)) {
      $props = Get-ItemProperty -LiteralPath $prod.PSPath -ErrorAction SilentlyContinue
      foreach ($dp in $DirProps) {
        $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props $dp) -Names $ExeNames
        if ($exe) { return $exe }
      }
    }
  }
  return $null
}

function Resolve-ArxHost {
  param([string] $Version, [string] $Arch)
  # One ARX matrix VERSION can span several AutoCAD releases (R25.0=2025,
  # R25.1=2026). Prefer HKCU CurVer (last launched) when it matches the series;
  # otherwise any installed R{VERSION}.* product.
  #
  #   HKCU\SOFTWARE\Autodesk\AutoCAD\CurVer        = R26.0
  #   HKCU\SOFTWARE\Autodesk\AutoCAD\R26.0\CurVer  = ACAD-A101:409
  #   HKLM\…\AutoCAD\R26.0\ACAD-A101:409\AcadLocation → acad.exe
  $exeNames = @('acad.exe')
  $dirProps = @('AcadLocation', 'Location')
  $seriesRe = "^R{0}\.\d+" -f [regex]::Escape($Version)

  # Last-used AutoCAD release key (may be outside this ARX series).
  $globalCurVer = $null
  $hkcuRoot = 'HKCU:\SOFTWARE\Autodesk\AutoCAD'
  if (Test-Path -LiteralPath $hkcuRoot) {
    $rootProps = Get-ItemProperty -LiteralPath $hkcuRoot -ErrorAction SilentlyContinue
    $globalCurVer = Get-PropertyString $rootProps 'CurVer'
    if (-not $globalCurVer) {
      $globalCurVer = Get-PropertyString $rootProps 'CURVER'
    }
  }

  if ($globalCurVer -and ($globalCurVer -match $seriesRe)) {
    $exe = Get-ArxExeFromSeriesKey -SeriesKeyName $globalCurVer -Arch $Arch `
      -ExeNames $exeNames -DirProps $dirProps
    if ($exe) { return $exe }
  }

  # Other installed releases in this ARX major series (e.g. R25.0 when CurVer is R26.0).
  $tried = @{}
  if ($globalCurVer) { $tried[$globalCurVer] = $true }

  foreach ($hive in (Get-SoftwareHives -Arch $Arch)) {
    $root = Join-Path $hive 'Autodesk\AutoCAD'
    if (-not (Test-Path -LiteralPath $root)) { continue }
    foreach ($rk in (Get-ChildItem -LiteralPath $root -ErrorAction SilentlyContinue)) {
      $name = $rk.PSChildName
      if ($tried.ContainsKey($name)) { continue }
      if ($name -notmatch $seriesRe) { continue }
      $tried[$name] = $true
      $exe = Get-ArxExeFromSeriesKey -SeriesKeyName $name -Arch $Arch `
        -ExeNames $exeNames -DirProps $dirProps
      if ($exe) { return $exe }
    }
  }
  return $null
}

function Resolve-BrxHost {
  param([string] $Version, [string] $Arch)
  $exeNames = @('bricscad.exe')
  # RxInstall: V{ver}x64 for x64, V{ver} for x86 (and older x64 keys without suffix).
  if ($Arch -eq 'x64') {
    $keyNames = @("V${Version}x64", "V$Version")
  }
  else {
    $keyNames = @("V$Version", "V${Version}x64")
  }
  foreach ($hive in (Get-SoftwareHives -Arch $Arch)) {
    foreach ($kn in $keyNames) {
      $root = Join-Path $hive "Bricsys\Bricscad\$kn"
      if (-not (Test-Path -LiteralPath $root)) { continue }
      foreach ($lang in (Get-ChildItem -LiteralPath $root -ErrorAction SilentlyContinue)) {
        if ($lang.PSChildName -eq 'Applications') { continue }
        $props = Get-ItemProperty -LiteralPath $lang.PSPath -ErrorAction SilentlyContinue
        $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props 'InstallDir') -Names $exeNames
        if ($exe) { return $exe }
        foreach ($dp in @('Location', 'InstallPath', 'Path')) {
          $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props $dp) -Names $exeNames
          if ($exe) { return $exe }
        }
      }
    }
  }
  return $null
}

function Resolve-ZrxHost {
  param([string] $Version, [string] $Arch)
  # Matrix VERSION is calendar year (2014..); RxInstall uses SOFTWARE\ZWSOFT\ZWCAD\{year}.
  $exeNames = @('ZWCAD.exe', 'zwcad.exe')
  $dirProps = @('InstallDir', 'Location', 'InstallPath', 'Path', 'LocationFolder')
  foreach ($hive in (Get-SoftwareHives -Arch $Arch)) {
    $root = Join-Path $hive "ZWSOFT\ZWCAD\$Version"
    if (-not (Test-Path -LiteralPath $root)) { continue }
    $queue = New-Object System.Collections.Generic.Queue[string]
    $queue.Enqueue($root)
    $depth = 0
    while ($queue.Count -gt 0 -and $depth -lt 32) {
      $depth++
      $key = $queue.Dequeue()
      $props = Get-ItemProperty -LiteralPath $key -ErrorAction SilentlyContinue
      foreach ($dp in $dirProps) {
        $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props $dp) -Names $exeNames
        if ($exe) { return $exe }
      }
      foreach ($sub in (Get-ChildItem -LiteralPath $key -ErrorAction SilentlyContinue)) {
        $queue.Enqueue($sub.PSPath)
      }
    }
  }
  return $null
}

function Resolve-GrxHost {
  param([string] $Version, [string] $Arch)
  # Matrix VERSION is calendar year; RxInstall major = year-2000 → R15, R15.0, …
  $exeNames = @('gcad.exe', 'GstarCAD.exe', 'gstarcad.exe', 'GCAD.exe')
  $dirProps = @('InstallDir', 'Location', 'InstallPath', 'Path', 'LocationFolder')
  $maj = $null
  if ($Version -match '^\d{4}$') {
    $maj = [string]([int]$Version - 2000)
  }
  elseif ($Version -match '^\d+$') {
    $maj = $Version
  }

  foreach ($hive in (Get-SoftwareHives -Arch $Arch)) {
    $root = Join-Path $hive 'GstarSoft\GstarCAD'
    if (-not (Test-Path -LiteralPath $root)) { continue }
    $keys = Get-ChildItem -LiteralPath $root -ErrorAction SilentlyContinue | Where-Object {
      $n = $_.PSChildName
      if ($n -eq "R$Version" -or $n -eq $Version) { return $true }
      if ($null -ne $maj) {
        if ($n -eq "R$maj") { return $true }
        if ($n -match ("^R{0}\." -f [regex]::Escape($maj))) { return $true }
      }
      return $false
    } | Sort-Object { $_.PSChildName } -Descending

    foreach ($k in $keys) {
      $queue = New-Object System.Collections.Generic.Queue[string]
      $queue.Enqueue($k.PSPath)
      $depth = 0
      while ($queue.Count -gt 0 -and $depth -lt 32) {
        $depth++
        $key = $queue.Dequeue()
        $props = Get-ItemProperty -LiteralPath $key -ErrorAction SilentlyContinue
        foreach ($dp in $dirProps) {
          $exe = Get-FirstExistingExe -Directory (Get-PropertyString $props $dp) -Names $exeNames
          if ($exe) { return $exe }
        }
        foreach ($sub in (Get-ChildItem -LiteralPath $key -ErrorAction SilentlyContinue)) {
          $queue.Enqueue($sub.PSPath)
        }
      }
    }
  }
  return $null
}

$exe = $null
switch ($Family) {
  'ARX' { $exe = Resolve-ArxHost -Version $Version -Arch $Arch }
  'BRX' { $exe = Resolve-BrxHost -Version $Version -Arch $Arch }
  'ZRX' { $exe = Resolve-ZrxHost -Version $Version -Arch $Arch }
  'GRX' { $exe = Resolve-GrxHost -Version $Version -Arch $Arch }
}

if ($exe) {
  Write-Output $exe
  exit 0
}
exit 1
