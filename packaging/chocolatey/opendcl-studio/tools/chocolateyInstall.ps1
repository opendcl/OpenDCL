$ErrorActionPreference = 'Stop'
# Checksums and version are filled by scripts/publish-package-managers.ps1
$version = '{{VERSION}}'
$checksums = @{
{{CHECKSUMS}}
}

$pp = Get-PackageParameters
$lang = [string]$pp['Language']
if (-not $lang) {
  $culture = (Get-UICulture).Name
  $lang = switch -Regex ($culture) {
    '^(zh-Hant|zh-TW|zh-HK|zh-MO)' { 'CHT'; break }
    '^zh' { 'CHS'; break }
    '^de' { 'DEU'; break }
    '^es' { 'ESM'; break }
    '^fr' { 'FRA'; break }
    '^ru' { 'RUS'; break }
    default { 'ENU' }
  }
  Write-Host "Language not specified; using $lang from UI culture $culture"
}
$lang = $lang.ToUpperInvariant()
if (-not $checksums.ContainsKey($lang)) {
  throw "Unknown Language '$lang'. Use: $($checksums.Keys -join ', '). Example: choco install opendcl-studio --params `"'/Language:DEU'`""
}

$fileName = "OpenDCL.Studio.$lang.$version.msi"
$url = "https://github.com/opendcl/OpenDCL/releases/download/v$version/$fileName"

$packageArgs = @{
  packageName    = $env:ChocolateyPackageName
  fileType       = 'msi'
  url            = $url
  checksum       = $checksums[$lang]
  checksumType   = 'sha256'
  silentArgs     = '/qn /norestart ALLUSERS=1'
  validExitCodes = @(0, 3010, 1641)
  softwareName   = 'OpenDCL Studio*'
}
Install-ChocolateyPackage @packageArgs
