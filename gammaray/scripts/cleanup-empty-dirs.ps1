param(
  [Parameter(Mandatory=$false)][string]$Root = (Resolve-Path "..\").Path
)

$ErrorActionPreference = 'Stop'
$total = 0
for ($pass = 1; $pass -le 5; $pass++) {
  $deleted = 0
  Get-ChildItem -LiteralPath $Root -Recurse -Directory -Force |
    Sort-Object FullName -Descending |
    ForEach-Object {
      $items = @(Get-ChildItem -LiteralPath $_.FullName -Force -ErrorAction SilentlyContinue)
      if ($items.Count -eq 0) {
        try {
          Remove-Item -LiteralPath $_.FullName -Force -ErrorAction Stop
          if (-not (Test-Path -LiteralPath $_.FullName)) {
            $deleted++
            $total++
            Write-Output ("REMOVED: " + $_.FullName)
          }
        } catch {
          # ignore and continue
        }
      }
    }
  if ($deleted -eq 0) { break }
}
Write-Output ("REMOVED_EMPTY_DIRS=" + $total)
