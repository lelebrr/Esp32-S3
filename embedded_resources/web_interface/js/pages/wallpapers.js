/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Wallpapers Page Javascript
 * Gallery, upload, and management
 * ═══════════════════════════════════════════════════════════════════════════
 */

const WallpapersPage = {
    gallery: [],
    currentIndex: 0
};

async function initWallpapersPage() {
    console.log('[Wallpapers] Initializing...');
    await loadWallpaperGallery();
}

async function loadWallpaperGallery() {
    try {
        const response = await fetch('/api/wallpapers/list');
        const data = await response.json();
        WallpapersPage.gallery = data.wallpapers || [];
        renderWallpaperGallery();
    } catch (e) {
        console.error('[Wallpapers] Failed to load gallery:', e);
    }
}

function renderWallpaperGallery() {
    const container = document.getElementById('wallpaper-gallery');
    if (!container) return;

    container.innerHTML = '';

    if (WallpapersPage.gallery.length === 0) {
        container.innerHTML = '<p class="text-muted">Nenhum wallpaper encontrado. Faça upload!</p>';
        return;
    }

    WallpapersPage.gallery.forEach((wp, idx) => {
        const item = document.createElement('div');
        item.className = 'wallpaper-item' + (wp.current ? ' active' : '');
        item.innerHTML = `
            <img src="/wallpapers/${wp.filename}" alt="${wp.filename}" loading="lazy">
            <div class="wallpaper-info">
                <span class="name">${wp.filename}</span>
                <span class="size">${formatBytes(wp.size)}</span>
            </div>
            <div class="wallpaper-actions">
                <button class="btn btn-sm btn-primary" onclick="setWallpaper('${wp.filename}')">Usar</button>
                <button class="btn btn-sm btn-danger" onclick="deleteWallpaper('${wp.filename}')">🗑️</button>
            </div>
        `;
        container.appendChild(item);
    });
}

async function setWallpaper(filename) {
    try {
        await fetch('/api/wallpapers/set', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename })
        });
        addLog(`Wallpaper alterado: ${filename}`, 'success');
        await loadWallpaperGallery();
    } catch (e) {
        addLog('Erro ao definir wallpaper', 'error');
    }
}

async function deleteWallpaper(filename) {
    if (!confirm(`Deletar wallpaper: ${filename}?`)) return;

    try {
        await fetch('/api/wallpapers/delete', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename })
        });
        addLog(`Wallpaper deletado: ${filename}`, 'success');
        await loadWallpaperGallery();
    } catch (e) {
        addLog('Erro ao deletar wallpaper', 'error');
    }
}

async function uploadWallpaper(file) {
    if (!file) return;

    // Accept images (JPG, PNG, WebP) -> Convert to PNG/WebP
    if (!file.type.startsWith('image/')) {
        addLog('Apenas arquivos de imagem são aceitos', 'error');
        return;
    }

    const progressEl = document.getElementById('wallpaper-upload-progress');
    if (progressEl) progressEl.style.display = 'block';

    addLog('Processando imagem...', 'info');

    try {
        // 1. Client-side Resize & Compress
        // Target: 368x448 (AMOLED Resolution)
        // Format: PNG (Compatible with PNGdec on ESP32)
        const processedBlob = await processImage(file, 368, 448, 'image/png', 0.8);

        if (processedBlob.size > 200 * 1024) {
            addLog(`Imagem ainda muito grande (${formatBytes(processedBlob.size)}). Tente uma imagem mais simples.`, 'warning');
            // Proceed anyway, maybe it fits
        }

        // 2. Upload
        const formData = new FormData();
        // Rename to .png to match content
        const newFileName = file.name.replace(/\.[^/.]+$/, "") + ".png";
        formData.append('wallpaper', processedBlob, newFileName);

        const response = await fetch('/api/wallpapers/upload', {
            method: 'POST',
            body: formData
        });

        if (response.ok) {
            addLog(`Wallpaper enviado: ${newFileName}`, 'success');
            await loadWallpaperGallery();
        } else {
            throw new Error('Upload failed');
        }
    } catch (e) {
        console.error(e);
        addLog('Erro ao processar/enviar wallpaper', 'error');
    } finally {
        if (progressEl) progressEl.style.display = 'none';
    }
}

function processImage(file, targetWidth, targetHeight, format = 'image/png', quality = 0.8) {
    return new Promise((resolve, reject) => {
        const img = new Image();
        img.src = URL.createObjectURL(file);

        img.onload = () => {
            URL.revokeObjectURL(img.src);
            const canvas = document.createElement('canvas');
            canvas.width = targetWidth;
            canvas.height = targetHeight;
            const ctx = canvas.getContext('2d');

            // "Cover" fit logic
            let srcW = img.width;
            let srcH = img.height;
            let srcX = 0;
            let srcY = 0;

            const aspectTarget = targetWidth / targetHeight;
            const aspectSrc = srcW / srcH;

            if (aspectSrc > aspectTarget) {
                // Source is wider, crop transparency/sides
                const scale = targetHeight / srcH;
                const scaledW = srcW * scale;
                const cropX = (scaledW - targetWidth) / 2;
                // Draw logic: render scaled and centered
                // Easier: Calculate source rectangle to crop
                const newSrcW = srcH * aspectTarget;
                srcX = (srcW - newSrcW) / 2;
                srcW = newSrcW;
            } else {
                // Source is taller, crop top/bottom
                const newSrcH = srcW / aspectTarget;
                srcY = (srcH - newSrcH) / 2;
                srcH = newSrcH;
            }

            // Draw to canvas
            ctx.drawImage(img, srcX, srcY, srcW, srcH, 0, 0, targetWidth, targetHeight);

            // Convert to blob
            canvas.toBlob((blob) => {
                if (blob) resolve(blob);
                else reject(new Error('Canvas to Blob failed'));
            }, format, quality);
        };

        img.onerror = (err) => {
            URL.revokeObjectURL(img.src);
            reject(err);
        };
    });
}

// Drag and drop support
function initWallpaperDropZone() {
    const dropZone = document.getElementById('wallpaper-drop-zone');
    if (!dropZone) return;

    ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(evName => {
        dropZone.addEventListener(evName, e => {
            e.preventDefault();
            e.stopPropagation();
        });
    });

    ['dragenter', 'dragover'].forEach(evName => {
        dropZone.addEventListener(evName, () => dropZone.classList.add('drag-over'));
    });

    ['dragleave', 'drop'].forEach(evName => {
        dropZone.addEventListener(evName, () => dropZone.classList.remove('drag-over'));
    });

    dropZone.addEventListener('drop', e => {
        const files = e.dataTransfer.files;
        if (files.length > 0) uploadWallpaper(files[0]);
    });
}

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1048576) return (bytes / 1024).toFixed(1) + ' KB';
    return (bytes / 1048576).toFixed(1) + ' MB';
}

// Export
window.initWallpapersPage = initWallpapersPage;
window.loadWallpaperGallery = loadWallpaperGallery;
window.setWallpaper = setWallpaper;
window.deleteWallpaper = deleteWallpaper;
window.uploadWallpaper = uploadWallpaper;
