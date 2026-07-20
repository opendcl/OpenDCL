<#
.SYNOPSIS
  Regenerate cmake/OpenDCLRuntimeMatrix.cmake from VI/*.props + sibling vcxproj.
#>
[CmdletBinding()]
param(
  [string] $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

$ErrorActionPreference = "Stop"
Set-Location $RepoRoot

function ConvertTo-BarList([string] $s) {
  if (-not $s) { return "" }
  return (($s -split ";" | Where-Object { $_ }) -join "|")
}

function Parse-Props([string] $propsPath) {
  $viDir = Split-Path $propsPath -Parent
  $projDir = Split-Path $viDir -Parent
  $id = Split-Path $projDir -Leaf
  $family = Split-Path (Split-Path $projDir -Parent) -Leaf

  $content = Get-Content $propsPath -Raw
  $ext = if ($content -match 'TargetExt>([^<]+)') { $Matches[1] } else { ".dll" }
  $defs = if ($content -match "PreprocessorDefinitions>([^<]+)") {
    ($Matches[1] -replace "%\(PreprocessorDefinitions\)", "").Trim(";")
  } else { "" }
  $incs = if ($content -match "AdditionalIncludeDirectories>([^<]+)") { $Matches[1] } else { "" }
  $libdirs = if ($content -match "AdditionalLibraryDirectories>([^<]+)") { $Matches[1] } else { "" }

  $libs = @()
  foreach ($m in [regex]::Matches($content, "<AdditionalDependencies>([^<]*)</AdditionalDependencies>")) {
    $part = ($m.Groups[1].Value -replace "%\(AdditionalDependencies\)", "").Trim(";")
    if ($part) { $libs += ($part -split ";" | Where-Object { $_ }) }
  }
  $libs = ($libs | Select-Object -Unique) -join ";"

  $wd = [regex]::Matches($content, "/wd(\d+)") | ForEach-Object { $_.Groups[1].Value } | Select-Object -Unique
  $std = if ($content -match "LanguageStandard>stdcpp(\d+)") { $Matches[1] } else { "" }

  $sdk = ""
  $m = [regex]::Match($content, '\$\((ARX[A-Z0-9]+|BRX\d+|GRX\d+|ZRX\d+)\)')
  if ($m.Success) { $sdk = $m.Groups[1].Value }

  $arch = if ($id -match "\.x64$") { "x64" } else { "x86" }
  $ver = $id -replace "^$family\.", "" -replace "\.x64$", ""

  $vcx = Join-Path $projDir "$id.vcxproj"
  $toolset = "v142"
  $outName = if ($arch -eq "x64") { "OpenDCL.x64.$ver" } else { "OpenDCL.$ver" }
  if (Test-Path $vcx) {
    $vc = Get-Content $vcx -Raw
    $ts = [regex]::Matches($vc, "<PlatformToolset>([^<]+)</PlatformToolset>") |
      ForEach-Object { $_.Groups[1].Value } | Select-Object -First 1
    if ($ts) { $toolset = $ts }
    if ($vc -match "<RootNamespace>([^<]+)</RootNamespace>") { $outName = $Matches[1] }
  }

  $sdkIncs = @()
  $sdkLibs = @()
  if ($sdk) {
    $sdkIncs = [regex]::Matches($incs, "\$\($sdk\)\\([^;]+)") | ForEach-Object { ($_.Groups[1].Value -replace "\\", "/") }
    $sdkLibs = [regex]::Matches($libdirs, "\$\($sdk\)\\([^;]+)") | ForEach-Object { ($_.Groups[1].Value -replace "\\", "/") }
  }

  [PSCustomObject]@{
    Id = $id; Family = $family; Ver = $ver; Arch = $arch; Ext = $ext; SdkEnv = $sdk
    Toolset = $toolset; Output = $outName
    Defines = (ConvertTo-BarList $defs)
    Libs = (ConvertTo-BarList $libs)
    WarningDisables = (ConvertTo-BarList (($wd | Where-Object { $_ }) -join ";"))
    CxxStd = $std
    SdkInc = (ConvertTo-BarList (($sdkIncs | Select-Object -Unique) -join ";"))
    SdkLib = (ConvertTo-BarList (($sdkLibs | Select-Object -Unique) -join ";"))
    ViDir = "Runtime/$family/$id/VI"
  }
}

$rows = @()
foreach ($family in @("ARX", "BRX", "GRX", "ZRX")) {
  Get-ChildItem "Runtime\$family" -Directory -ErrorAction SilentlyContinue | ForEach-Object {
    Get-ChildItem (Join-Path $_.FullName "VI") -Filter *.props -ErrorAction SilentlyContinue | ForEach-Object {
      $rows += Parse-Props $_.FullName
    }
  }
}
$rows = $rows | Sort-Object Family, Ver, Arch

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine("# Generated from VI/*.props for the OpenDCL CMake runtime matrix.")
[void]$sb.AppendLine("# List fields use | instead of ; (CMake list separator).")
[void]$sb.AppendLine("# Regenerate: pwsh scripts/generate-runtime-matrix.ps1")
[void]$sb.AppendLine("include_guard(GLOBAL)")
[void]$sb.AppendLine("")

foreach ($r in $rows) {
  [void]$sb.AppendLine("opendcl_register_runtime(")
  [void]$sb.AppendLine("  ID `"$($r.Id)`"")
  [void]$sb.AppendLine("  FAMILY $($r.Family)")
  [void]$sb.AppendLine("  VERSION `"$($r.Ver)`"")
  [void]$sb.AppendLine("  ARCH $($r.Arch)")
  [void]$sb.AppendLine("  EXT `"$($r.Ext)`"")
  [void]$sb.AppendLine("  OUTPUT_NAME `"$($r.Output)`"")
  [void]$sb.AppendLine("  TOOLSET `"$($r.Toolset)`"")
  [void]$sb.AppendLine("  SDK_ENV `"$($r.SdkEnv)`"")
  [void]$sb.AppendLine("  SDK_INC `"$($r.SdkInc)`"")
  [void]$sb.AppendLine("  SDK_LIB `"$($r.SdkLib)`"")
  [void]$sb.AppendLine("  VI_DIR `"$($r.ViDir)`"")
  [void]$sb.AppendLine("  DEFINES `"$($r.Defines)`"")
  if ($r.Libs) { [void]$sb.AppendLine("  LIBS `"$($r.Libs)`"") }
  if ($r.WarningDisables) { [void]$sb.AppendLine("  WARNING_DISABLES `"$($r.WarningDisables)`"") }
  if ($r.CxxStd) { [void]$sb.AppendLine("  CXX_STANDARD $($r.CxxStd)") }
  [void]$sb.AppendLine(")")
  [void]$sb.AppendLine("")
}

$outPath = Join-Path $RepoRoot "cmake\OpenDCLRuntimeMatrix.cmake"
# UTF-8 no BOM
$utf8 = New-Object System.Text.UTF8Encoding $false
[System.IO.File]::WriteAllText($outPath, $sb.ToString(), $utf8)
Write-Host "Wrote $($rows.Count) runtimes -> $outPath"
