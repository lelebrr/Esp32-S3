# Script to fix UTF-8 encoding issues (mojibake) in web interface files
# This script will replace corrupted characters with correct UTF-8 equivalents

$webInterfacePath = "C:\Users\leleb\OneDrive\Desktop\Cyd_Lele\embedded_resources\web_interface"

# Get all HTML, JS, and CSS files (excluding minified and gzipped)
$files = Get-ChildItem -Path $webInterfacePath -Include "*.html","*.js","*.css" -Recurse -File | 
    Where-Object { $_.Name -notmatch '\.min\.' -and $_.Name -notmatch '\.gz$' }

$fixCount = 0

foreach ($file in $files) {
    Write-Host "Processing: $($file.Name)" -ForegroundColor Cyan
    
    # Read file as bytes to preserve encoding
    $bytes = [System.IO.File]::ReadAllBytes($file.FullName)
    $content = [System.Text.Encoding]::UTF8.GetString($bytes)
    $originalContent = $content
    
    # Portuguese accented characters (mojibake from UTF-8 read as Latin-1)
    $content = $content -replace 'Ã¡', 'á'
    $content = $content -replace 'Ã©', 'é'
    $content = $content -replace 'Ã­', 'í'
    $content = $content -replace 'Ã³', 'ó'
    $content = $content -replace 'Ãº', 'ú'
    $content = $content -replace 'Ã£', 'ã'
    $content = $content -replace 'Ãµ', 'õ'
    $content = $content -replace 'Ã¢', 'â'
    $content = $content -replace 'Ãª', 'ê'
    $content = $content -replace 'Ã®', 'î'
    $content = $content -replace 'Ã´', 'ô'
    $content = $content -replace 'Ã§', 'ç'
    $content = $content -replace 'Ã ', 'à'
    $content = $content -replace 'Ã¨', 'è'
    $content = $content -replace 'Ã¬', 'ì'
    $content = $content -replace 'Ã²', 'ò'
    $content = $content -replace 'Ã¹', 'ù'
    $content = $content -replace 'Ã±', 'ñ'
    $content = $content -replace 'Ã»', 'û'
    
    # Uppercase accented
    $content = $content -replace 'Ã€', 'À'
    $content = $content -replace 'Ãˆ', 'È'
    $content = $content -replace 'ÃŒ', 'Ì'
    $content = $content -replace 'Ã'', 'Ò'
    $content = $content -replace 'Ã™', 'Ù'
    $content = $content -replace 'Ãƒ', 'Ã'
    $content = $content -replace 'Ã•', 'Õ'
    $content = $content -replace 'Ã‚', 'Â'
    $content = $content -replace 'ÃŠ', 'Ê'
    $content = $content -replace 'ÃŽ', 'Î'
    $content = $content -replace 'Ã"', 'Ô'
    $content = $content -replace 'Ã›', 'Û'
    $content = $content -replace 'Ã‡', 'Ç'
    $content = $content -replace 'Ã‰', 'É'
    $content = $content -replace 'Ãš', 'Ú'
    
    # Degree symbol
    $content = $content -replace 'Â°', '°'
    
    # Common symbols
    $content = $content -replace 'â˜°', '☰'
    $content = $content -replace 'â˜ ï¸', '☠️'
    $content = $content -replace 'âš¡', '⚡'
    $content = $content -replace 'âš™ï¸', '⚙️'
    $content = $content -replace 'âš"ï¸', '⚔️'
    $content = $content -replace 'â­', '⭐'
    $content = $content -replace 'â¬†ï¸', '⬆️'
    $content = $content -replace 'â¬‡ï¸', '⬇️'
    $content = $content -replace 'â—€', '◀'
    $content = $content -replace 'â–¶', '▶'
    $content = $content -replace 'â–²', '▲'
    $content = $content -replace 'â–¼', '▼'
    $content = $content -replace 'âœ¨', '✨'
    $content = $content -replace 'âœ‚ï¸', '✂️'
    $content = $content -replace 'âœï¸', '✏️'
    $content = $content -replace 'â„¹', 'ℹ'
    $content = $content -replace 'â†'', '→'
    $content = $content -replace 'â†•ï¸', '↕️'
    $content = $content -replace 'â†"ï¸', '↔️'
    $content = $content -replace 'â˜€ï¸', '☀️'
    $content = $content -replace 'â„ï¸', '❄️'
    $content = $content -replace 'â™¿', '♿'
    $content = $content -replace 'â»', '⻿'
    $content = $content -replace 'â®Œ', '⮌'
    $content = $content -replace 'âˆ'', '−'
    $content = $content -replace 'â±ï¸', '⏱️'
    
    # Emojis - Most common ones
    $content = $content -replace 'ðŸ"Š', '📊'
    $content = $content -replace 'ðŸ"Œ', '🔌'
    $content = $content -replace 'ðŸ"º', '📺'
    $content = $content -replace 'ðŸ–¥ï¸', '🖥️'
    $content = $content -replace 'ðŸ"', '📂'
    $content = $content -replace 'ðŸ'»', '💻'
    $content = $content -replace 'ðŸ› ï¸', '🛠️'
    $content = $content -replace 'ðŸ"š', '📚'
    $content = $content -replace 'ðŸ"‹', '📋'
    $content = $content -replace 'ðŸŒ¡ï¸', '🌡️'
    $content = $content -replace 'ðŸ"¡', '📡'
    $content = $content -replace 'ðŸ¤', '🤝'
    $content = $content -replace 'ðŸ"'', '🔑'
    $content = $content -replace 'ðŸ'¿', '💿'
    $content = $content -replace 'ðŸŽ­', '🎭'
    $content = $content -replace 'ðŸ"', '🔍'
    $content = $content -replace 'ðŸ"»', '📻'
    $content = $content -replace 'ðŸšª', '🚪'
    $content = $content -replace 'ðŸ'"', '💔'
    $content = $content -replace 'ðŸ"¢', '🔢'
    $content = $content -replace 'ðŸ–¨ï¸', '🖨️'
    $content = $content -replace 'ðŸ'£', '💣'
    $content = $content -replace 'ðŸ‰', '🐉'
    $content = $content -replace 'ðŸ–¼ï¸', '🖼️'
    $content = $content -replace 'ðŸ"„', '📄'
    $content = $content -replace 'ðŸ"›', '📛'
    $content = $content -replace 'ðŸ"'', '🔒'
    $content = $content -replace 'ðŸ"¶', '📶'
    $content = $content -replace 'ðŸ'³', '💳'
    $content = $content -replace 'ðŸ§Ÿ', '🧟'
    $content = $content -replace 'ðŸŒŠ', '🌊'
    $content = $content -replace 'ðŸŒ€', '🌀'
    $content = $content -replace 'ðŸ"—', '🔗'
    $content = $content -replace 'ðŸŽ¯', '🎯'
    $content = $content -replace 'ðŸš€', '🚀'
    $content = $content -replace 'ðŸ"¤', '📤'
    $content = $content -replace 'ðŸ"¦', '📦'
    $content = $content -replace 'ðŸ'¡', '💡'
    $content = $content -replace 'ðŸŽ¥', '🎥'
    $content = $content -replace 'ðŸ"€', '📀'
    $content = $content -replace 'ðŸŒ', '🌐'
    $content = $content -replace 'ðŸ'ï¸', '👁️'
    $content = $content -replace 'ðŸŽ®', '🎮'
    $content = $content -replace 'ðŸ¦†', '🦆'
    $content = $content -replace 'ðŸ¦', '🏦'
    $content = $content -replace 'ðŸ""', '🔓'
    $content = $content -replace 'ðŸ'¬', '💬'
    $content = $content -replace 'ðŸ"', '🔐'
    $content = $content -replace 'ðŸ'¤', '👤'
    $content = $content -replace 'ðŸ'¾', '💾'
    $content = $content -replace 'ðŸŽ«', '🎫'
    $content = $content -replace 'ðŸ"œ', '📜'
    $content = $content -replace 'ðŸ"µ', '🔵'
    $content = $content -replace 'ðŸŽ‚', '🎂'
    $content = $content -replace 'ðŸ'¦', '👦'
    $content = $content -replace 'ðŸ''', '👑'
    $content = $content -replace 'ðŸ"', '🔏'
    $content = $content -replace 'ðŸ"‚', '📂'
    $content = $content -replace 'ðŸ"„', '🔄'
    $content = $content -replace 'ðŸ'§', '💧'
    $content = $content -replace 'ðŸ˜´', '😴'
    $content = $content -replace 'ðŸŒ¿', '🌿'
    $content = $content -replace 'ðŸŒ¬ï¸', '🌬️'
    $content = $content -replace 'ðŸ"¥', '🔥'
    $content = $content -replace 'ðŸ"‡', '📇'
    $content = $content -replace 'ðŸŽµ', '🎵'
    $content = $content -replace 'ðŸ'¿', '👿'
    $content = $content -replace 'ðŸŒ™', '🌙'
    $content = $content -replace 'ðŸ–±ï¸', '🖱️'
    $content = $content -replace 'âŒ¨ï¸', '⌨️'
    $content = $content -replace 'ðŸ¤–', '🤖'
    $content = $content -replace 'ðŸ"‹', '🔋'
    $content = $content -replace 'ðŸ›'', '🛑'
    $content = $content -replace 'ðŸ'¥', '💥'
    
    # More emojis
    $content = $content -replace 'ï¸', '️'  # Clean up stray variation selectors
    
    if ($content -ne $originalContent) {
        # Write back as UTF-8 without BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBom)
        $fixCount++
        Write-Host "  FIXED: $($file.Name)" -ForegroundColor Green
    } else {
        Write-Host "  OK: $($file.Name)" -ForegroundColor Gray
    }
}

Write-Host "`n========================================" -ForegroundColor Yellow
Write-Host "Total files processed: $($files.Count)" -ForegroundColor Yellow
Write-Host "Total files fixed: $fixCount" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Yellow
