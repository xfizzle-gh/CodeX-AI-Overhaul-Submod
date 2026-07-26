param(
    [string]$CodeXResource = "E:\Steam\steamapps\workshop\content\400750\3261086933\resource",
    [string]$OutputResource = (Join-Path (Split-Path $PSScriptRoot -Parent) "resource")
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$sourceDir = Join-Path $CodeXResource "interface\scene\portrait_squad"
$targetDir = Join-Path $OutputResource "interface\scene\unit_icon"
New-Item -ItemType Directory -Path $targetDir -Force | Out-Null

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

function Convert-ToDoctrineIcon {
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
                $sourceRect = New-Object System.Drawing.Rectangle($cropX, $cropY, $cropWidth, $cropHeight)
                $graphics.DrawImage($source, $destination, $sourceRect, [System.Drawing.GraphicsUnit]::Pixel)
            }
            finally {
                $graphics.Dispose()
            }

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

foreach ($mapping in $mappings) {
    foreach ($state in 0..3) {
        $suffix = "_{0:D2}.png" -f $state
        $sourcePath = Join-Path $sourceDir ($mapping.Source + $suffix)
        $targetPath = Join-Path $targetDir ($mapping.Target + $suffix)

        if (-not (Test-Path $sourcePath)) {
            throw "Missing Code:X portrait source: $sourcePath"
        }

        Convert-ToDoctrineIcon -SourcePath $sourcePath -TargetPath $targetPath
        Write-Host "Created $targetPath"
    }
}

Write-Host "PRC doctrine portraits installed from Code:X artwork."
