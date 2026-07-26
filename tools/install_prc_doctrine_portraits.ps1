param(
    [string]$CodeXResource,
    [string]$OutputResource = (Join-Path (Split-Path $PSScriptRoot -Parent) "resource")
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

function Resolve-CodeXResource {
    param([string]$ExplicitPath)

    if ($ExplicitPath) {
        if (-not (Test-Path $ExplicitPath)) {
            throw "Code:X resource folder not found: $ExplicitPath"
        }
        return (Resolve-Path $ExplicitPath).Path
    }

    $candidates = @(
        "E:\Steam\steamapps\workshop\content\400750\3261086933\resource",
        "D:\Steam\steamapps\workshop\content\400750\3261086933\resource",
        "C:\Program Files (x86)\Steam\steamapps\workshop\content\400750\3261086933\resource",
        "C:\Steam\steamapps\workshop\content\400750\3261086933\resource"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return (Resolve-Path $candidate).Path
        }
    }

    throw "Could not locate Code:X workshop resource folder. Re-run with -CodeXResource '<path>\resource'."
}

function Convert-ToUnitIcon {
    param(
        [Parameter(Mandatory = $true)][string]$SourcePath,
        [Parameter(Mandatory = $true)][string]$TargetPath
    )

    $source = [System.Drawing.Image]::FromFile($SourcePath)
    try {
        $targetWidth = 144
        $targetHeight = 72
        $targetAspect = $targetWidth / $targetHeight
        $sourceAspect = $source.Width / $source.Height

        if ($sourceAspect -gt $targetAspect) {
            $cropHeight = $source.Height
            $cropWidth = [int][Math]::Round($cropHeight * $targetAspect)
            $cropX = [int][Math]::Round(($source.Width - $cropWidth) / 2)
            $cropY = 0
        }
        else {
            $cropWidth = $source.Width
            $cropHeight = [int][Math]::Round($cropWidth / $targetAspect)
            $cropX = 0
            $cropY = [int][Math]::Round(($source.Height - $cropHeight) * 0.40)
            $cropY = [Math]::Max(0, [Math]::Min($cropY, $source.Height - $cropHeight))
        }

        $bitmap = New-Object System.Drawing.Bitmap($targetWidth, $targetHeight)
        try {
            $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
            try {
                $graphics.CompositingMode = [System.Drawing.Drawing2D.CompositingMode]::SourceCopy
                $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
                $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
                $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality

                $destination = New-Object System.Drawing.Rectangle(0, 0, $targetWidth, $targetHeight)
                $sourceRectangle = New-Object System.Drawing.Rectangle($cropX, $cropY, $cropWidth, $cropHeight)
                $graphics.DrawImage($source, $destination, $sourceRectangle, [System.Drawing.GraphicsUnit]::Pixel)
            }
            finally {
                $graphics.Dispose()
            }

            $targetDirectory = Split-Path $TargetPath -Parent
            New-Item -ItemType Directory -Path $targetDirectory -Force | Out-Null
            $bitmap.Save($TargetPath, [System.Drawing.Imaging.ImageFormat]::Png)
        }
        finally {
            $bitmap.Dispose()
        }
    }
    finally {
        $source.Dispose()
    }
}

$resolvedCodeXResource = Resolve-CodeXResource -ExplicitPath $CodeXResource
$sourceDirectory = Join-Path $resolvedCodeXResource "interface\scene\portrait_squad"
$targetDirectory = Join-Path $OutputResource "interface\scene\unit_icon"

$mappings = @(
    @{
        Source = "squad_pla112_rifle(prc)"
        Target = "doctrine_squad_skirmish_prc(prc)"
    },
    @{
        Source = "squad_pla139_rifle(prc)"
        Target = "doctrine_squad_skirmish_prc_139(prc)"
    }
)

foreach ($mapping in $mappings) {
    foreach ($state in 0..3) {
        $suffix = "_{0:D2}.png" -f $state
        $sourcePath = Join-Path $sourceDirectory ($mapping.Source + $suffix)
        $targetPath = Join-Path $targetDirectory ($mapping.Target + $suffix)

        if (-not (Test-Path $sourcePath)) {
            throw "Missing Code:X portrait source: $sourcePath"
        }

        Convert-ToUnitIcon -SourcePath $sourcePath -TargetPath $targetPath
        Write-Host "Created $targetPath"
    }
}

Write-Host "Installed exact-name PRC doctrine portraits from Code:X artwork."
