/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Sounds Page Javascript
 * Library, upload, and test playback
 * ═══════════════════════════════════════════════════════════════════════════
 */

const SoundsPage = {
    library: [],
    previewAudio: null
};

async function initSoundsPage() {
    console.log('[Sounds] Initializing...');
    await loadSoundLibrary();
}

async function loadSoundLibrary() {
    try {
        const response = await fetch('/api/sounds/list');
        const data = await response.json();
        SoundsPage.library = data.sounds || [];
        renderSoundLibrary();
    } catch (e) {
        console.error('[Sounds] Failed to load library:', e);
    }
}

function renderSoundLibrary() {
    const container = document.getElementById('sound-library');
    if (!container) return;

    container.innerHTML = '';

    // Presets section
    const presets = SoundsPage.library.filter(s => s.isPreset);
    const custom = SoundsPage.library.filter(s => !s.isPreset);

    if (presets.length > 0) {
        const presetsSection = document.createElement('div');
        presetsSection.innerHTML = '<h4 class="text-muted">🔊 Sons Pré-instalados</h4>';
        presetsSection.appendChild(createSoundGrid(presets));
        container.appendChild(presetsSection);
    }

    if (custom.length > 0) {
        const customSection = document.createElement('div');
        customSection.innerHTML = '<h4 class="text-muted mt-3">📂 Sons Customizados</h4>';
        customSection.appendChild(createSoundGrid(custom));
        container.appendChild(customSection);
    }

    if (SoundsPage.library.length === 0) {
        container.innerHTML = '<p class="text-muted">Nenhum som encontrado.</p>';
    }
}

function createSoundGrid(sounds) {
    const grid = document.createElement('div');
    grid.className = 'sound-grid';

    sounds.forEach((sound, idx) => {
        const item = document.createElement('div');
        item.className = 'sound-item';
        item.innerHTML = `
            <div class="sound-icon">🔊</div>
            <div class="sound-name">${sound.filename.replace('.wav', '')}</div>
            <div class="sound-duration">${sound.duration_ms || '?'}ms</div>
            <div class="sound-actions">
                <button class="btn btn-sm" onclick="previewSound('${sound.filename}')">▶</button>
                ${!sound.isPreset ? `<button class="btn btn-sm btn-danger" onclick="deleteSound('${sound.filename}')">🗑️</button>` : ''}
            </div>
        `;
        grid.appendChild(item);
    });

    return grid;
}

function previewSound(filename) {
    // Stop current preview
    if (SoundsPage.previewAudio) {
        SoundsPage.previewAudio.pause();
    }

    // For presets, use Web Audio API to generate tone
    if (filename.startsWith('preset_')) {
        playPresetTone(filename);
        return;
    }

    // For custom sounds, play from server
    SoundsPage.previewAudio = new Audio(`/sounds/${filename}`);
    SoundsPage.previewAudio.play().catch(e => {
        console.error('[Sounds] Playback failed:', e);
        addLog('Erro ao reproduzir som', 'error');
    });
}

function playPresetTone(presetName) {
    try {
        const ctx = new (window.AudioContext || window.webkitAudioContext)();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();

        osc.type = 'square';
        osc.frequency.value = 440;
        gain.gain.value = 0.1;

        osc.connect(gain);
        gain.connect(ctx.destination);

        osc.start();
        setTimeout(() => osc.stop(), 200);

        addLog(`Preview: ${presetName}`, 'info');
    } catch (e) {
        console.error('[Sounds] Failed to play preset:', e);
    }
}

async function deleteSound(filename) {
    if (!confirm(`Deletar som: ${filename}?`)) return;

    try {
        await fetch('/api/sounds/delete', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename })
        });
        addLog(`Som deletado: ${filename}`, 'success');
        await loadSoundLibrary();
    } catch (e) {
        addLog('Erro ao deletar som', 'error');
    }
}

async function uploadSound(file) {
    if (!file) return;
    if (!file.name.endsWith('.wav') && !file.name.endsWith('.WAV')) {
        addLog('Apenas arquivos WAV são aceitos', 'error');
        return;
    }
    if (file.size > 150 * 1024) {
        addLog('Arquivo muito grande (max 150KB)', 'error');
        return;
    }

    const formData = new FormData();
    formData.append('sound', file);

    try {
        const response = await fetch('/api/sounds/upload', {
            method: 'POST',
            body: formData
        });

        if (response.ok) {
            addLog(`Som enviado: ${file.name}`, 'success');
            await loadSoundLibrary();
        } else {
            throw new Error('Upload failed');
        }
    } catch (e) {
        addLog('Erro ao enviar som', 'error');
    }
}

// Export
window.initSoundsPage = initSoundsPage;
window.loadSoundLibrary = loadSoundLibrary;
window.previewSound = previewSound;
window.deleteSound = deleteSound;
window.uploadSound = uploadSound;
