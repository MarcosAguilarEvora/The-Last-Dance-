Add-Type -AssemblyName System.Drawing

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$itemsDir = Join-Path $root "assets\images\items"
$backupDir = Join-Path $itemsDir "_originales_antes_realistas"

if (!(Test-Path $backupDir)) {
    New-Item -ItemType Directory -Path $backupDir | Out-Null
    Get-ChildItem $itemsDir -Filter *.png | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $backupDir $_.Name)
    }
}

function New-IconCanvas {
    $bmp = New-Object System.Drawing.Bitmap 192, 192, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $g.Clear([System.Drawing.Color]::Transparent)
    return @{ Bitmap = $bmp; Graphics = $g }
}

function Save-Icon($name, $drawBlock) {
    $ctx = New-IconCanvas
    & $drawBlock $ctx.Graphics
    $path = Join-Path $itemsDir "$name.png"
    $ctx.Bitmap.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $ctx.Graphics.Dispose()
    $ctx.Bitmap.Dispose()
}

function Brush($a, $r, $g, $b) {
    return New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb($a, $r, $g, $b))
}

function Pen($a, $r, $g, $b, $w) {
    $p = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb($a, $r, $g, $b)), $w
    $p.LineJoin = [System.Drawing.Drawing2D.LineJoin]::Round
    $p.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
    $p.EndCap = [System.Drawing.Drawing2D.LineCap]::Round
    return $p
}

function LinearBrush($rect, $c1, $c2, $angle) {
    return New-Object System.Drawing.Drawing2D.LinearGradientBrush $rect, $c1, $c2, $angle
}

function Draw-SoftShadow($g, $x, $y, $w, $h) {
    $shadow = Brush 75 0 0 0
    $g.FillEllipse($shadow, $x, $y, $w, $h)
    $shadow.Dispose()
}

function Draw-Gloss($g, $x, $y, $w, $h) {
    $glossRect = New-Object System.Drawing.RectangleF ($x + $w * 0.18), ($y + $h * 0.10), ($w * 0.36), ($h * 0.20)
    $gloss = LinearBrush $glossRect ([System.Drawing.Color]::FromArgb(165,255,255,255)) ([System.Drawing.Color]::FromArgb(10,255,255,255)) 90
    $g.FillEllipse($gloss, $glossRect)
    $gloss.Dispose()
}

function RoundedRect($x, $y, $w, $h, $r) {
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $d = $r * 2
    $path.AddArc($x, $y, $d, $d, 180, 90)
    $path.AddArc($x + $w - $d, $y, $d, $d, 270, 90)
    $path.AddArc($x + $w - $d, $y + $h - $d, $d, $d, 0, 90)
    $path.AddArc($x, $y + $h - $d, $d, $d, 90, 90)
    $path.CloseFigure()
    return $path
}

Save-Icon "balon" {
    param($g)
    Draw-SoftShadow $g 44 144 104 18
    $rect = New-Object System.Drawing.Rectangle 34, 24, 124, 124
    $ballBrush = LinearBrush $rect ([System.Drawing.Color]::FromArgb(255,255,255,255)) ([System.Drawing.Color]::FromArgb(255,174,181,187)) 135
    $g.FillEllipse($ballBrush, $rect)
    $ballBrush.Dispose()
    $g.DrawEllipse((Pen 255 45 48 52 4), $rect)
    $black = Brush 255 24 26 30
    $seam = Pen 210 35 38 42 3
    $pent = New-Object System.Drawing.Drawing2D.GraphicsPath
    $pts = @(
        [System.Drawing.PointF]::new(96,57), [System.Drawing.PointF]::new(119,75),
        [System.Drawing.PointF]::new(110,103), [System.Drawing.PointF]::new(82,103),
        [System.Drawing.PointF]::new(73,75)
    )
    $pent.AddPolygon($pts)
    $g.FillPath($black, $pent)
    $g.DrawLine($seam, 96, 57, 96, 34)
    $g.DrawLine($seam, 119, 75, 145, 67)
    $g.DrawLine($seam, 110, 103, 125, 128)
    $g.DrawLine($seam, 82, 103, 65, 128)
    $g.DrawLine($seam, 73, 75, 47, 67)
    $g.FillPolygon($black, @([System.Drawing.PointF]::new(94,28), [System.Drawing.PointF]::new(111,37), [System.Drawing.PointF]::new(106,51), [System.Drawing.PointF]::new(84,51), [System.Drawing.PointF]::new(80,37)))
    $g.FillPolygon($black, @([System.Drawing.PointF]::new(142,81), [System.Drawing.PointF]::new(154,96), [System.Drawing.PointF]::new(144,116), [System.Drawing.PointF]::new(128,110), [System.Drawing.PointF]::new(128,91)))
    $g.FillPolygon($black, @([System.Drawing.PointF]::new(50,81), [System.Drawing.PointF]::new(64,91), [System.Drawing.PointF]::new(64,110), [System.Drawing.PointF]::new(48,116), [System.Drawing.PointF]::new(38,96)))
    Draw-Gloss $g 38 28 115 100
}

Save-Icon "silbato" {
    param($g)
    Draw-SoftShadow $g 30 142 135 18

    $body = New-Object System.Drawing.Drawing2D.GraphicsPath
    $body.AddBezier(35,95,42,56,75,42,112,51)
    $body.AddBezier(112,51,139,57,159,75,166,98)
    $body.AddLine(166,98,153,125)
    $body.AddBezier(153,125,116,138,62,136,35,119)
    $body.CloseFigure()

    $blackBody = LinearBrush ([System.Drawing.Rectangle]::new(35,42,132,96)) ([System.Drawing.Color]::FromArgb(255,68,70,70)) ([System.Drawing.Color]::FromArgb(255,14,15,16)) 80
    $g.FillPath($blackBody, $body)
    $g.DrawPath((Pen 255 8 9 10 4), $body)
    $blackBody.Dispose()

    $tail = RoundedRect 121 70 49 44 7
    $tailBrush = LinearBrush ([System.Drawing.Rectangle]::new(121,70,49,44)) ([System.Drawing.Color]::FromArgb(255,59,61,61)) ([System.Drawing.Color]::FromArgb(255,13,14,15)) 90
    $g.FillPath($tailBrush, $tail)
    $g.DrawPath((Pen 255 9 9 10 4), $tail)
    $tailBrush.Dispose()

    $top = RoundedRect 73 38 50 33 14
    $topBrush = LinearBrush ([System.Drawing.Rectangle]::new(73,38,50,33)) ([System.Drawing.Color]::FromArgb(255,78,80,80)) ([System.Drawing.Color]::FromArgb(255,19,20,21)) 80
    $g.FillPath($topBrush, $top)
    $g.DrawPath((Pen 255 12 13 14 3), $top)
    $topBrush.Dispose()

    $window = RoundedRect 117 60 29 23 3
    $g.FillPath((Brush 255 4 5 6), $window)
    $g.DrawPath((Pen 160 45 47 48 2), $window)

    $ringHole = RoundedRect 28 55 35 31 12
    $ringBrush = LinearBrush ([System.Drawing.Rectangle]::new(28,55,35,31)) ([System.Drawing.Color]::FromArgb(255,70,72,72)) ([System.Drawing.Color]::FromArgb(255,18,19,20)) 60
    $g.FillPath($ringBrush, $ringHole)
    $g.DrawPath((Pen 255 9 10 11 4), $ringHole)
    $ringBrush.Dispose()

    $g.DrawEllipse((Pen 235 198 198 190 4), 19, 66, 28, 31)
    $g.DrawEllipse((Pen 255 35 36 37 7), 45, 85, 52, 51)
    $g.DrawArc((Pen 130 86 88 88 3), 50, 89, 43, 42, 25, 195)
    $g.DrawLine((Pen 190 0 0 0 5), 72, 116, 155, 105)
    $g.DrawLine((Pen 90 255 255 255 5), 82, 47, 115, 49)
    $g.DrawLine((Pen 70 255 255 255 4), 54, 65, 76, 53)
    $g.DrawLine((Pen 85 120 122 122 3), 137, 74, 160, 88)
}

Save-Icon "dinero" {
    param($g)
    Draw-SoftShadow $g 35 141 122 19
    for ($i = 0; $i -lt 4; $i++) {
        $x = 37 + $i * 7
        $y = 70 - $i * 5
        $card = RoundedRect $x $y 103 58 7
        $brush = LinearBrush ([System.Drawing.Rectangle]::new($x,$y,103,58)) ([System.Drawing.Color]::FromArgb(255,169,223,149)) ([System.Drawing.Color]::FromArgb(255,57,137,77)) 35
        $g.FillPath($brush, $card)
        $g.DrawPath((Pen 255 30 92 49 3), $card)
        $g.DrawEllipse((Pen 180 235 255 214 3), ($x + 38), ($y + 13), 27, 27)
        $g.DrawLine((Pen 130 245 255 226 2), ($x + 13), ($y + 16), ($x + 90), ($y + 16))
        $g.DrawLine((Pen 90 16 82 36 2), ($x + 12), ($y + 45), ($x + 88), ($y + 45))
        $brush.Dispose()
    }
    $band = RoundedRect 75 52 27 94 6
    $g.FillPath((Brush 230 245 215 78), $band)
    $g.DrawPath((Pen 255 112 83 20 2), $band)
}

Save-Icon "flecha" {
    param($g)
    Draw-SoftShadow $g 38 139 118 18
    $shaft = Pen 255 104 81 56 18
    $highlight = Pen 150 255 241 210 4
    $g.DrawLine($shaft, 48, 126, 129, 45)
    $g.DrawLine($highlight, 54, 119, 118, 55)
    $head = @([System.Drawing.PointF]::new(136,35), [System.Drawing.PointF]::new(150,82), [System.Drawing.PointF]::new(101,68))
    $headBrush = LinearBrush ([System.Drawing.Rectangle]::new(101,35,49,47)) ([System.Drawing.Color]::FromArgb(255,225,55,44)) ([System.Drawing.Color]::FromArgb(255,117,24,22)) 45
    $g.FillPolygon($headBrush, $head)
    $g.DrawPolygon((Pen 255 73 18 18 4), $head)
    $g.FillPolygon((Brush 255 238 238 228), @([System.Drawing.PointF]::new(43,129), [System.Drawing.PointF]::new(30,153), [System.Drawing.PointF]::new(62,143)))
    $g.DrawLine((Pen 255 50 45 42 4), 43, 129, 30, 153)
    $g.DrawLine((Pen 255 50 45 42 4), 43, 129, 62, 143)
}

Save-Icon "taquetes" {
    param($g)
    Draw-SoftShadow $g 29 141 132 21
    $shoe = New-Object System.Drawing.Drawing2D.GraphicsPath
    $shoe.AddBezier(39,107,62,73,103,86,119,106)
    $shoe.AddBezier(119,106,138,105,153,112,158,126)
    $shoe.AddBezier(158,126,126,141,63,139,35,128)
    $shoe.CloseFigure()
    $leather = LinearBrush ([System.Drawing.Rectangle]::new(35,76,124,65)) ([System.Drawing.Color]::FromArgb(255,238,54,61)) ([System.Drawing.Color]::FromArgb(255,100,21,30)) 80
    $g.FillPath($leather, $shoe)
    $g.DrawPath((Pen 255 60 18 24 4), $shoe)
    $g.DrawLine((Pen 180 255 255 255 3), 58, 101, 112, 103)
    $g.DrawLine((Pen 220 32 36 40 5), 48, 127, 150, 127)
    foreach ($x in @(55,78,103,130)) {
        $g.FillEllipse((Brush 255 220 220 210), $x, 135, 12, 9)
        $g.DrawEllipse((Pen 255 72 72 70 1), $x, 135, 12, 9)
    }
    for ($i = 0; $i -lt 4; $i++) {
        $g.DrawLine((Pen 220 245 245 245 3), (76 + $i * 13), 93, (68 + $i * 13), 111)
    }
}

Save-Icon "espinilleras" {
    param($g)
    Draw-SoftShadow $g 45 144 102 18
    foreach ($x in @(52,98)) {
        $guard = RoundedRect $x 42 42 98 18
        $blue = LinearBrush ([System.Drawing.Rectangle]::new($x,42,42,98)) ([System.Drawing.Color]::FromArgb(255,105,208,255)) ([System.Drawing.Color]::FromArgb(255,24,78,160)) 90
        $g.FillPath($blue, $guard)
        $g.DrawPath((Pen 255 11 45 96 4), $guard)
        $g.DrawLine((Pen 140 255 255 255 4), ($x + 9), 55, ($x + 9), 113)
        $g.DrawLine((Pen 120 7 31 75 3), ($x + 30), 55, ($x + 29), 126)
        $blue.Dispose()
    }
    $g.DrawLine((Pen 230 30 32 38 8), 48, 75, 142, 70)
    $g.DrawLine((Pen 230 30 32 38 8), 50, 112, 139, 117)
}

Save-Icon "guantes" {
    param($g)
    Draw-SoftShadow $g 35 144 122 18
    foreach ($x in @(45,96)) {
        $palm = RoundedRect $x 69 46 62 18
        $white = LinearBrush ([System.Drawing.Rectangle]::new($x,69,46,62)) ([System.Drawing.Color]::FromArgb(255,255,255,255)) ([System.Drawing.Color]::FromArgb(255,186,193,206)) 90
        $g.FillPath($white, $palm)
        $g.DrawPath((Pen 255 70 62 122 4), $palm)
        for ($i = 0; $i -lt 4; $i++) {
            $g.FillRectangle((Brush 255 106 74 215), ($x + 4 + $i * 10), 44, 8, 41)
            $g.DrawRectangle((Pen 255 55 42 126 2), ($x + 4 + $i * 10), 44, 8, 41)
        }
        $g.FillRectangle((Brush 255 245 214 66), $x, 120, 46, 15)
        $g.DrawLine((Pen 160 255 255 255 3), ($x + 10), 78, ($x + 34), 78)
    }
}

Save-Icon "banda" {
    param($g)
    Draw-SoftShadow $g 35 143 122 20
    $band = RoundedRect 37 65 119 62 20
    $yellow = LinearBrush ([System.Drawing.Rectangle]::new(37,65,119,62)) ([System.Drawing.Color]::FromArgb(255,255,231,70)) ([System.Drawing.Color]::FromArgb(255,181,113,25)) 80
    $g.FillPath($yellow, $band)
    $g.DrawPath((Pen 255 91 61 18 5), $band)
    $g.DrawLine((Pen 170 255 255 255 5), 52, 73, 139, 74)
    $font = New-Object System.Drawing.Font "Arial", 38, ([System.Drawing.FontStyle]::Bold)
    $g.DrawString("C", $font, (Brush 235 50 42 34), 79, 72)
    $font.Dispose()
}

function Draw-Card($g, $color1, $color2) {
    Draw-SoftShadow $g 48 143 96 18
    $card = RoundedRect 58 30 76 116 8
    $brush = LinearBrush ([System.Drawing.Rectangle]::new(58,30,76,116)) $color1 $color2 105
    $g.FillPath($brush, $card)
    $g.DrawPath((Pen 255 35 35 35 4), $card)
    $g.DrawLine((Pen 155 255 255 255 5), 70, 42, 122, 42)
    $g.DrawLine((Pen 80 0 0 0 4), 67, 134, 124, 134)
    $brush.Dispose()
}

Save-Icon "amarilla" {
    param($g)
    Draw-Card $g ([System.Drawing.Color]::FromArgb(255,255,235,66)) ([System.Drawing.Color]::FromArgb(255,213,152,20))
}

Save-Icon "roja" {
    param($g)
    Draw-Card $g ([System.Drawing.Color]::FromArgb(255,255,82,78)) ([System.Drawing.Color]::FromArgb(255,156,18,27))
}

Save-Icon "corazon" {
    param($g)
    Draw-SoftShadow $g 42 145 110 18
    $heart = New-Object System.Drawing.Drawing2D.GraphicsPath
    $heart.AddBezier(96,145,28,98,43,43,85,61)
    $heart.AddBezier(85,61,96,28,146,43,146,82)
    $heart.AddBezier(146,82,146,110,123,128,96,145)
    $red = LinearBrush ([System.Drawing.Rectangle]::new(42,39,108,106)) ([System.Drawing.Color]::FromArgb(255,255,80,126)) ([System.Drawing.Color]::FromArgb(255,141,9,47)) 90
    $g.FillPath($red, $heart)
    $g.DrawPath((Pen 255 92 8 37 4), $heart)
    Draw-Gloss $g 52 47 72 58
}

Write-Host "Sprites realistas generados en $itemsDir"
