<#
.SYNOPSIS
  Build per-language localization packs for translators (replaces !MakeLocalizationZips.bat).

.DESCRIPTION
  For each language code, creates OpenDCL.<LANG>.zip containing:

    Common/<LANG>/SharedRes.<LANG>.rc
    Runtime/Localized/<LANG>/Runtime.Res/Runtime.<LANG>.rc
    Studio/Localized/<LANG>/Studio.Res/Studio.<LANG>.rc
    Studio/Localized/<LANG>/Content/**  (help topics: .htm .png .css .js; no Samples/CHM)

  Paths inside the zip keep the repo-relative layout so packs can be expanded
  over a source tree or used as a basis for a new language clone.

.PARAMETER Languages
  Language codes. Default: all folders under Studio/Localized.

.PARAMETER OutDir
  Output directory for zips. Default: dist/localization

.PARAMETER IncludeSamples
  If set, include Content/Samples (large; off by default like historical zips).
#>
[CmdletBinding()]
param(
  [string] $OpenDclRoot = "",
  [string[]] $Languages = @(),
  [string] $OutDir = "",
  [switch] $IncludeSamples
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

if (-not $OpenDclRoot) {
  $OpenDclRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}
$OpenDclRoot = (Resolve-Path $OpenDclRoot).Path

if (-not $OutDir) {
  $OutDir = Join-Path $OpenDclRoot "dist\localization"
}
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

if (-not $Languages -or $Languages.Count -eq 0) {
  $Languages = Get-ChildItem (Join-Path $OpenDclRoot "Studio\Localized") -Directory |
    Where-Object { $_.Name -match '^[A-Z]{3}$' } |
    ForEach-Object { $_.Name } |
    Sort-Object
}

function Add-ZipFile {
  param(
    [System.IO.Compression.ZipArchive] $Zip,
    [string] $AbsolutePath,
    [string] $EntryName
  )
  if (-not (Test-Path -LiteralPath $AbsolutePath)) { return $false }
  $EntryName = $EntryName -replace '\\', '/'
  [void][System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
    $Zip,
    $AbsolutePath,
    $EntryName,
    [System.IO.Compression.CompressionLevel]::Optimal
  )
  return $true
}

function New-LangZip {
  param([string] $Lang)

  $zipPath = Join-Path $OutDir "OpenDCL.$Lang.zip"
  if (Test-Path $zipPath) { Remove-Item -LiteralPath $zipPath -Force }

  $contentRoot = Join-Path $OpenDclRoot "Studio\Localized\$Lang\Content"
  if (-not (Test-Path $contentRoot)) {
    Write-Warning "Skip $Lang - missing Content: $contentRoot"
    return $null
  }

  $candidates = @(
    @{
      Abs = Join-Path $OpenDclRoot "Common\$Lang\SharedRes.$Lang.rc"
      Entry = "Common/$Lang/SharedRes.$Lang.rc"
    },
    @{
      Abs = Join-Path $OpenDclRoot "Runtime\Localized\$Lang\Runtime.Res\Runtime.$Lang.rc"
      Entry = "Runtime/Localized/$Lang/Runtime.Res/Runtime.$Lang.rc"
    },
    @{
      Abs = Join-Path $OpenDclRoot "Studio\Localized\$Lang\Studio.Res\Studio.$Lang.rc"
      Entry = "Studio/Localized/$Lang/Studio.Res/Studio.$Lang.rc"
    }
  )

  $zip = [System.IO.Compression.ZipFile]::Open($zipPath, [System.IO.Compression.ZipArchiveMode]::Create)
  $n = 0
  try {
    foreach ($c in $candidates) {
      if (Add-ZipFile -Zip $zip -AbsolutePath $c.Abs -EntryName $c.Entry) {
        $n++
      }
      else {
        Write-Warning "Missing: $($c.Abs)"
      }
    }

    $includeExt = @('.htm', '.html', '.png', '.css', '.js', '.gif', '.jpg', '.jpeg')
    $files = Get-ChildItem -LiteralPath $contentRoot -Recurse -File | Where-Object {
      $rel = $_.FullName.Substring($contentRoot.Length).TrimStart('\', '/')
      $skipSample = (-not $IncludeSamples) -and ($rel -match '(?i)^Samples([\\/]|$)')
      $skipTemplate = ($rel -eq '@Template.htm')
      $ext = $_.Extension.ToLowerInvariant()
      ((-not $skipSample) -and (-not $skipTemplate) -and ($includeExt -contains $ext))
    }

    foreach ($f in $files) {
      $rel = $f.FullName.Substring($contentRoot.Length).TrimStart('\', '/')
      $entry = "Studio/Localized/$Lang/Content/" + ($rel -replace '\\', '/')
      if (Add-ZipFile -Zip $zip -AbsolutePath $f.FullName -EntryName $entry) {
        $n++
      }
    }

    $readmeLines = @(
      "OpenDCL localization pack - $Lang",
      "================================",
      "",
      "Contents (repo-relative paths):",
      "  Common/$Lang/SharedRes.$Lang.rc",
      "  Runtime/Localized/$Lang/Runtime.Res/Runtime.$Lang.rc",
      "  Studio/Localized/$Lang/Studio.Res/Studio.$Lang.rc",
      "  Studio/Localized/$Lang/Content/   (help HTML/images; Samples omitted)",
      "",
      "How to use",
      "----------",
      "1. Edit translated strings in the .rc files and help .htm files.",
      "2. Prefer submitting a pull request or a zip of your changes on the OpenDCL forum.",
      "3. To start a new language, clone ENU (or this pack) to a new three-letter code",
      "   and rewire projects + WiX (see add-language skill / docs).",
      "",
      "Source of truth: https://github.com/opendcl/OpenDCL",
      "Online help:     https://opendcl.github.io/HelpFiles/",
      "Packs page:      https://opendcl.github.io/localization/"
    )
    $readmeEntry = $zip.CreateEntry("README-$Lang.txt")
    $sw = New-Object System.IO.StreamWriter($readmeEntry.Open())
    try {
      foreach ($line in $readmeLines) { $sw.WriteLine($line) }
    }
    finally {
      $sw.Dispose()
    }
    $n++
  }
  finally {
    $zip.Dispose()
  }

  $mb = [math]::Round((Get-Item $zipPath).Length / 1MB, 2)
  Write-Host "OK OpenDCL.$Lang.zip  ($n entries, $mb MB)"
  return $zipPath
}

Write-Host "=== make-localization-zips ==="
Write-Host "Root: $OpenDclRoot"
Write-Host "Out:  $OutDir"
Write-Host "Lang: $($Languages -join ', ')"

$built = @()
foreach ($lang in $Languages) {
  $p = New-LangZip -Lang $lang
  if ($p) { $built += $p }
}

if ($built.Count -eq 0) { throw "No localization zips produced" }
Write-Host "Built $($built.Count) pack(s) -> $OutDir"
$built
