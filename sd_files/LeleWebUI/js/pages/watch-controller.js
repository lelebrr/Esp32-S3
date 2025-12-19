/**
 * WAVEPWN - Watch Mode Web Controller
 * Controle web do modo relógio
 */

const WatchController = {
    config: null,
    watchfaces: []
};

async function initWatchController() {
    await loadWatchConfig();
    await loadWatchfaces();
    syncTime(); // Auto-sync on load
    console.log('[Watch] Controller initialized');
}

async function loadWatchConfig() {
    try {
        const response = await fetch('/api/watch/status');
        WatchController.config = await response.json();
    } catch (e) {
        WatchController.config = { active: false, watchface: 0 };
    }
}

async function loadWatchfaces() {
    try {
        const response = await fetch('/api/watch/faces');
        WatchController.watchfaces = await response.json();
    } catch (e) {
        WatchController.watchfaces = [
            { id: 0, name: 'Digital', icon: '⌚' },
            { id: 1, name: 'Analog', icon: '🕐' },
            { id: 2, name: 'Cyberpunk', icon: '📊' },
            { id: 3, name: 'Mascot', icon: '🐉' },
            { id: 4, name: 'Hacker', icon: '💀' },
            { id: 5, name: 'Minimal', icon: '🌙' }
        ];
    }
}

function renderWatchController(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    const cfg = WatchController.config || {};

    container.innerHTML = `
        <div class="watch-controller">
            <h3>⌚ Modo Relógio</h3>
            
            <div class="watch-status">
                <span class="status-indicator ${cfg.active ? 'active' : ''}">
                    ${cfg.active ? '🟢 Ativo' : '⚫ Inativo'}
                </span>
                <button class="btn ${cfg.active ? 'btn-danger' : 'btn-primary'}" 
                        onclick="${cfg.active ? 'disableWatchMode()' : 'enableWatchMode()'}">
                    ${cfg.active ? 'Desativar' : 'Ativar'}
                </button>
            </div>
            
            <h4>Mostradores</h4>
            <div class="watchface-grid">
                ${WatchController.watchfaces.map(wf => `
                    <div class="watchface-card ${wf.id === cfg.watchface ? 'selected' : ''}" 
                         onclick="selectWatchface(${wf.id})">
                        <div class="wf-icon">${wf.icon}</div>
                        <div class="wf-name">${wf.name}</div>
                    </div>
                `).join('')}
            </div>
            
            <h4>Configurações</h4>
            <div class="watch-settings">
                <label class="setting-row">
                    <span>Mostrar segundos</span>
                    <input type="checkbox" ${cfg.show_seconds ? 'checked' : ''} 
                           onchange="updateWatchSetting('show_seconds', this.checked)">
                </label>
                <label class="setting-row">
                    <span>Mostrar data</span>
                    <input type="checkbox" ${cfg.show_date ? 'checked' : ''} 
                           onchange="updateWatchSetting('show_date', this.checked)">
                </label>
                <label class="setting-row">
                    <span>Mostrar bateria</span>
                    <input type="checkbox" ${cfg.show_battery ? 'checked' : ''} 
                           onchange="updateWatchSetting('show_battery', this.checked)">
                </label>
                <label class="setting-row">
                    <span>Sniffing em background</span>
                    <input type="checkbox" ${cfg.background_sniffing ? 'checked' : ''} 
                           onchange="updateWatchSetting('background_sniffing', this.checked)">
                </label>
                <div class="setting-row">
                    <span>Brilho</span>
                    <input type="range" min="10" max="100" value="${cfg.brightness || 80}" 
                           onchange="updateWatchSetting('brightness', this.value)">
                </div>
            </div>
            
            ${cfg.stats ? `
            <h4>Estatísticas</h4>
            <div class="watch-stats">
                <div class="stat">📡 ${cfg.stats.networks || 0} redes</div>
                <div class="stat">🤝 ${cfg.stats.handshakes || 0} handshakes</div>
                <div class="stat">⏱️ ${cfg.stats.uptime || 0} min</div>
            </div>
            ` : ''}
        </div>
    `;
}

async function enableWatchMode() {
    await fetch('/api/watch/enable', { method: 'POST' });
    await loadWatchConfig();
    renderWatchController('watch-container');
}

async function disableWatchMode() {
    await fetch('/api/watch/disable', { method: 'POST' });
    await loadWatchConfig();
    renderWatchController('watch-container');
}

async function selectWatchface(id) {
    await fetch('/api/watch/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `watchface=${id}`
    });
    await loadWatchConfig();
    renderWatchController('watch-container');
}

async function syncTime() {
    try {
        const now = Math.floor(Date.now() / 1000); // Unix timestamp in seconds
        // Adjust for timezone offset (ESP32 usually needs UTC or local time depending on implementation)
        // Assuming ESP32 expects local time epoch or handles timezone internaly if NTP is used.
        // For simple sync, we send local timestamp.

        // Also send timezone offset in minutes (e.g., -180 for UTC-3)
        const offset = new Date().getTimezoneOffset();

        await fetch('/api/time', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `timestamp=${now}&offset=${offset}`
        });
        console.log(`[Watch] Time synced: ${new Date().toLocaleString()}`);
    } catch (e) {
        console.error('[Watch] Failed to sync time', e);
    }
}

async function updateWatchSetting(key, value) {
    await fetch('/api/watch/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `${key}=${value ? '1' : '0'}`
    });
}

// Inject styles
function injectWatchStyles() {
    if (document.getElementById('watch-styles')) return;
    const style = document.createElement('style');
    style.id = 'watch-styles';
    style.textContent = `
        .watch-controller h3 { margin-bottom: 15px; }
        .watch-controller h4 { margin: 20px 0 10px; color: var(--text-secondary); font-size: 0.9rem; text-transform: uppercase; }
        .watch-status { display: flex; align-items: center; justify-content: space-between; padding: 15px; background: var(--bg-card); border-radius: var(--border-radius); margin-bottom: 15px; }
        .status-indicator { font-weight: 600; }
        .status-indicator.active { color: var(--success); }
        .watchface-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; }
        .watchface-card { padding: 15px; background: var(--bg-card); border: 2px solid var(--border-color); border-radius: var(--border-radius); cursor: pointer; text-align: center; transition: all 0.2s; }
        .watchface-card:hover { border-color: var(--primary); }
        .watchface-card.selected { border-color: var(--primary); background: var(--primary-dim); }
        .wf-icon { font-size: 28px; margin-bottom: 5px; }
        .wf-name { font-size: 0.85rem; }
        .watch-settings { background: var(--bg-card); border-radius: var(--border-radius); padding: 15px; }
        .setting-row { display: flex; justify-content: space-between; align-items: center; padding: 10px 0; border-bottom: 1px solid var(--border-color); }
        .setting-row:last-child { border-bottom: none; }
        .watch-stats { display: flex; gap: 20px; padding: 15px; background: var(--bg-card); border-radius: var(--border-radius); }
        .stat { color: var(--primary); font-weight: 600; }
    `;
    document.head.appendChild(style);
}

document.addEventListener('DOMContentLoaded', injectWatchStyles);

window.initWatchController = initWatchController;
window.renderWatchController = renderWatchController;
window.enableWatchMode = enableWatchMode;
window.disableWatchMode = disableWatchMode;
window.selectWatchface = selectWatchface;
window.updateWatchSetting = updateWatchSetting;
