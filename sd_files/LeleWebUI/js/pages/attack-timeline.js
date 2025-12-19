/**
 * WAVEPWN - Attack Timeline (Dica 43)
 * Histórico de ataques com replay
 */

const AttackTimeline = { attacks: [], maxHistory: 100 };

function initAttackTimeline() {
    loadAttackHistory();
}

function logAttack(attack) {
    const entry = {
        id: Date.now(),
        timestamp: new Date().toISOString(),
        type: attack.type,
        target: attack.target || null,
        ssid: attack.ssid || null,
        bssid: attack.bssid || null,
        channel: attack.channel || null,
        duration: attack.duration || 0,
        packets: attack.packets || 0,
        success: attack.success !== false,
        params: attack.params || {}
    };

    AttackTimeline.attacks.unshift(entry);
    if (AttackTimeline.attacks.length > AttackTimeline.maxHistory) {
        AttackTimeline.attacks = AttackTimeline.attacks.slice(0, AttackTimeline.maxHistory);
    }
    saveAttackHistory();
    renderTimeline();
    return entry;
}

function loadAttackHistory() {
    try {
        const saved = localStorage.getItem('wavepwn-attack-history');
        if (saved) AttackTimeline.attacks = JSON.parse(saved);
    } catch (e) { console.error('[Timeline] Load error:', e); }
}

function saveAttackHistory() {
    try {
        localStorage.setItem('wavepwn-attack-history', JSON.stringify(AttackTimeline.attacks));
    } catch (e) { console.error('[Timeline] Save error:', e); }
}

function clearAttackHistory() {
    if (!confirm('Limpar histórico?')) return;
    AttackTimeline.attacks = [];
    saveAttackHistory();
    renderTimeline();
}

function renderTimeline() {
    const container = document.getElementById('attack-timeline');
    if (!container) return;

    if (AttackTimeline.attacks.length === 0) {
        container.innerHTML = '<div class="timeline-empty"><p>Nenhum ataque registrado</p></div>';
        return;
    }

    const icons = { deauth: '💀', beacon: '📡', handshake: '🤝', pmkid: '🔑', evil_twin: '👿', ble_spam: '📶', wps_pixie: '🔓', pwn_all: '☠️' };
    const names = { deauth: 'Deauth', beacon: 'Beacon Flood', handshake: 'Handshake', pmkid: 'PMKID', evil_twin: 'Evil Twin', ble_spam: 'BLE Spam', wps_pixie: 'WPS Pixie', pwn_all: 'PWN ALL' };

    container.innerHTML = `
        <div class="timeline-header">
            <h3>📜 Histórico</h3>
            <button class="btn btn-sm btn-danger" onclick="clearAttackHistory()">🗑️</button>
        </div>
        <div class="timeline-list">
            ${AttackTimeline.attacks.slice(0, 20).map(a => `
                <div class="timeline-item ${a.success ? 'success' : 'failed'}">
                    <span class="icon">${icons[a.type] || '⚡'}</span>
                    <div class="info">
                        <strong>${names[a.type] || a.type}</strong>
                        ${a.ssid ? `<span class="ssid">${a.ssid}</span>` : ''}
                        <small>${formatTimeAgo(a.timestamp)}</small>
                    </div>
                    <button class="btn btn-sm" onclick="replayAttack('${a.id}')">🔄</button>
                </div>
            `).join('')}
        </div>
    `;
}

async function replayAttack(id) {
    const attack = AttackTimeline.attacks.find(a => a.id == id);
    if (!attack || !confirm('Repetir ataque?')) return;
    try {
        await fetch('/api/attack/replay', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(attack) });
    } catch (e) { console.error(e); }
}

function formatTimeAgo(ts) {
    const s = Math.floor((Date.now() - new Date(ts)) / 1000);
    if (s < 60) return 'agora';
    if (s < 3600) return `${Math.floor(s / 60)}m`;
    if (s < 86400) return `${Math.floor(s / 3600)}h`;
    return `${Math.floor(s / 86400)}d`;
}

window.initAttackTimeline = initAttackTimeline;
window.logAttack = logAttack;
window.replayAttack = replayAttack;
window.clearAttackHistory = clearAttackHistory;
window.renderTimeline = renderTimeline;
