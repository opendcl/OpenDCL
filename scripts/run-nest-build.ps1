<#
.SYNOPSIS
  Run a nest cmake/MSBuild under a machine-wide mutex so parallel parent
  cmake --build -j cannot overlap Win32 nest compiles (C1083 on shared IntDirs).
#>
[CmdletBinding()]
param(
  [Parameter(Mandatory = $true, ValueFromRemainingArguments = $true)]
  [string[]] $Command
)

$ErrorActionPreference = 'Stop'
if (-not $Command -or $Command.Count -lt 1) {
  throw 'run-nest-build.ps1: expected a command to run'
}

$mutexName = 'Global\OpenDCL.NestMSBuild'
$mutex = New-Object System.Threading.Mutex($false, $mutexName)
$acquired = $false
try {
  while (-not $mutex.WaitOne(1000)) {
    Write-Host "run-nest-build: waiting for nest MSBuild mutex ($mutexName)..."
  }
  $acquired = $true
  Write-Host 'run-nest-build: acquired nest MSBuild mutex; running:'
  Write-Host ('  ' + ($Command -join ' '))
  if ($Command.Count -eq 1) {
    & $Command[0]
  } else {
    & $Command[0] @($Command[1..($Command.Count - 1)])
  }
  exit $LASTEXITCODE
}
finally {
  if ($acquired) { [void]$mutex.ReleaseMutex() }
  if ($mutex) { $mutex.Dispose() }
}