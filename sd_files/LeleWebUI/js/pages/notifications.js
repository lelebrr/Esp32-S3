/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Notifications Config Page
 * Visual notification configuration and testing
 * 
 * Efeitos de borda disponíveis:
 * - Pulse: Borda que pulsa suavemente
 * - Blink: Pisca rapidamente
 * - Fade: Fade in/out gradual
 * - Wave: Onda que percorre as bordas
 * - Rainbow: Gradiente multicolor rotativo
 * - Heartbeat: Pulsação dupla estilo batimento cardíaco
 * - Glow: Glow intenso que diminui gradualmente
 * - Neon: Efeito neon com box-shadow intenso
 * ═══════════════════════════════════════════════════════════════════════════
 */

const NotificationsPage = {
    config: {
        enabled: true,
        borderEnabled: true,  // Toggle para efeitos de borda
        effect: 0,            // Índice do efeito (0-7)
        color: '#00FFF5',
        duration: 800,
        fade: 1,              // 0=soft, 1=medium, 2=aggressive
        events: [true, true, true, true, true, true, true]
    },
    // 8 efeitos de borda disponíveis
    effects: [
        'Pulse',      // 0 - Pulsação suave
        'Blink',      // 1 - Piscar rápido
        'Fade',       // 2 - Fade gradual
        'Wave',       // 3 - Onda percorrendo bordas
        'Rainbow',    // 4 - Arco-íris
        'Heartbeat',  // 5 - Batimento cardíaco
        'Glow',       // 6 - Glow intenso
        'Neon'        // 7 - Efeito neon
    ],
    events: ['Handshake', 'WPS PIN', 'BLE Próximo', 'Bateria <20%', 'SD Cheio', 'Ataque OK', 'USB Connect'],
    colors: [
        '#00FFF5', '#FF00FF', '#BD00FF', '#00FF9D', '#FFCC00', '#FF0055', '#00A8FF', '#FF6600',
        '#FF0080', '#00FF00', '#39FF14', '#FE01B1', '#01FEFE', '#FE0101', '#FFFF00', '#7B00FF'
    ],
    intensities: ['soft', 'medium', 'aggressive']
};

async function initNotificationsPage() {
    console.log('[Notifications] Initializing...');
    await loadNotificationConfig();
    renderNotificationConfig();
}

async function loadNotificationConfig() {
    try {
        const response = await fetch('/api/notifications/config');
        const data = await response.json();
        NotificationsPage.config = { ...NotificationsPage.config, ...data };
    } catch (e) {
        console.error('[Notifications] Failed to load config:', e);
    }
}

function renderNotificationConfig() {
    const container = document.getElementById('notifications-config');
    if (!container) return;

    container.innerHTML = `
        <!-- Master Enable -->
        <div class="config-row">
            <label>Notificações Visuais</label>
            <label class="switch">
                <input type="checkbox" id="notify-enabled" ${NotificationsPage.config.enabled ? 'checked' : ''} onchange="updateNotifyConfig('enabled', this.checked)">
                <span class="slider"></span>
            </label>
        </div>
        
        <!-- Border Effects Enable -->
        <div class="config-row">
            <label>🔲 Bordas Pulsantes</label>
            <label class="switch">
                <input type="checkbox" id="notify-border-enabled" ${NotificationsPage.config.borderEnabled ? 'checked' : ''} onchange="updateNotifyConfig('borderEnabled', this.checked)">
                <span class="slider"></span>
            </label>
        </div>
        
        <!-- Effect Type -->
        <div class="config-row">
            <label>Tipo de Efeito</label>
            <select id="notify-effect" onchange="updateNotifyConfig('effect', parseInt(this.value))">
                ${NotificationsPage.effects.map((e, i) => `<option value="${i}" ${i == NotificationsPage.config.effect ? 'selected' : ''}>${e}</option>`).join('')}
            </select>
        </div>
        
        <!-- Effect Description -->
        <div id="effect-description" class="config-description" style="font-size:0.8rem;color:var(--text-muted);margin-bottom:15px;padding:10px;background:var(--bg-tertiary);border-radius:8px;">
            ${getEffectDescription(NotificationsPage.config.effect)}
        </div>
        
        <!-- Color Palette -->
        <div class="config-row">
            <label>Cor da Notificação</label>
            <div class="color-palette">
                ${NotificationsPage.colors.map(c => `<button class="color-btn ${c == NotificationsPage.config.color ? 'active' : ''}" style="background:${c}" onclick="updateNotifyConfig('color', '${c}')"></button>`).join('')}
            </div>
        </div>
        
        <!-- Duration -->
        <div class="config-row">
            <label>Duração: <span id="duration-val">${NotificationsPage.config.duration}ms</span></label>
            <input type="range" min="300" max="3000" step="100" value="${NotificationsPage.config.duration}" 
                   oninput="document.getElementById('duration-val').textContent = this.value + 'ms'; updateNotifyConfig('duration', parseInt(this.value))">
        </div>
        
        <!-- Fade Intensity -->
        <div class="config-row">
            <label>Intensidade do Glow</label>
            <select id="notify-fade" onchange="updateNotifyConfig('fade', parseInt(this.value))">
                <option value="0" ${NotificationsPage.config.fade == 0 ? 'selected' : ''}>🌙 Suave</option>
                <option value="1" ${NotificationsPage.config.fade == 1 ? 'selected' : ''}>⚡ Médio</option>
                <option value="2" ${NotificationsPage.config.fade == 2 ? 'selected' : ''}>🔥 Agressivo</option>
            </select>
        </div>
        
        <!-- Test Buttons -->
        <div class="config-row" style="flex-direction:column;gap:10px;">
            <button class="btn btn-lg btn-primary" style="width:100%; font-size:1.2rem; padding: 15px;" onclick="testNotification()">
                🔔 TESTAR NOTIFICAÇÃO
            </button>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:8px;">
                ${NotificationsPage.effects.map((e, i) => `
                    <button class="btn btn-sm btn-secondary" onclick="testSpecificEffect(${i})" title="${e}">
                        ${getEffectEmoji(i)}
                    </button>
                `).join('')}
            </div>
        </div>
        
        <!-- Event Checkboxes -->
        <h4 class="mt-3">Notificar em:</h4>
        <div class="event-grid">
            ${NotificationsPage.events.map((e, i) => `
                <label class="event-checkbox">
                    <input type="checkbox" ${NotificationsPage.config.events[i] ? 'checked' : ''} onchange="updateEventEnabled(${i}, this.checked)">
                    <span>${e}</span>
                </label>
            `).join('')}
        </div>
    `;
}

// Descrições dos efeitos
function getEffectDescription(effectIndex) {
    const descriptions = [
        '💫 <b>Pulse</b>: Borda que pulsa suavemente, expandindo e contraindo',
        '⚡ <b>Blink</b>: Bordas que piscam rapidamente várias vezes',
        '🌊 <b>Fade</b>: Glow gradual que aparece e desaparece suavemente',
        '〰️ <b>Wave</b>: Onda de luz que percorre as bordas da tela',
        '🌈 <b>Rainbow</b>: Gradiente multicolor que roda pelas bordas',
        '❤️ <b>Heartbeat</b>: Pulsação dupla como batimento cardíaco',
        '✨ <b>Glow</b>: Glow intenso que ilumina toda a tela',
        '💜 <b>Neon</b>: Efeito neon estilo cyberpunk com múltiplos glows'
    ];
    return descriptions[effectIndex] || descriptions[0];
}

// Emojis para botões de teste
function getEffectEmoji(index) {
    const emojis = ['💫', '⚡', '🌊', '〰️', '🌈', '❤️', '✨', '💜'];
    return emojis[index] || '🔔';
}

function updateNotifyConfig(key, value) {
    NotificationsPage.config[key] = value;

    // Update active color button
    if (key === 'color') {
        document.querySelectorAll('.color-btn').forEach(btn => {
            btn.classList.toggle('active', btn.style.backgroundColor === value || btn.getAttribute('onclick').includes(value));
        });
    }

    // Update effect description
    if (key === 'effect') {
        const desc = document.getElementById('effect-description');
        if (desc) desc.innerHTML = getEffectDescription(value);
    }

    // Send to device
    sendNotifyConfigToDevice();
}

function updateEventEnabled(index, enabled) {
    NotificationsPage.config.events[index] = enabled;
    sendNotifyConfigToDevice();
}

async function sendNotifyConfigToDevice() {
    try {
        await fetch('/api/notifications/config', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(NotificationsPage.config)
        });
    } catch (e) {
        console.error('[Notifications] Failed to save config:', e);
    }
}

/**
 * Dispara efeito de borda de notificação
 * @param {string} color - Cor da borda (hex)
 * @param {number} effectIndex - Índice do efeito (0-7)
 * @param {number} duration - Duração em ms
 * @param {number} intensity - Intensidade (0=soft, 1=medium, 2=aggressive)
 */
function triggerBorderNotification(color = null, effectIndex = null, duration = null, intensity = null) {
    // Verificar se está habilitado
    if (!NotificationsPage.config.enabled || !NotificationsPage.config.borderEnabled) {
        return;
    }

    // Usar configurações salvas se não especificado
    color = color || NotificationsPage.config.color;
    effectIndex = effectIndex !== null ? effectIndex : NotificationsPage.config.effect;
    duration = duration || NotificationsPage.config.duration;
    intensity = intensity !== null ? intensity : NotificationsPage.config.fade;

    // Mapear índice para nome do efeito
    const effectNames = ['pulse', 'blink', 'fade', 'wave', 'rainbow', 'heartbeat', 'glow', 'neon'];
    const effectClass = effectNames[effectIndex] || 'pulse';

    // Mapear intensidade para classe
    const intensityClass = NotificationsPage.intensities[intensity] || 'medium';

    // Criar overlay
    const overlay = document.createElement('div');
    overlay.className = `notification-border-overlay effect-${effectClass} intensity-${intensityClass}`;
    overlay.style.setProperty('--notify-color', color);
    overlay.style.setProperty('--notify-duration', `${duration}ms`);

    document.body.appendChild(overlay);

    // Remover após animação
    setTimeout(() => {
        overlay.remove();
    }, duration + 100);

    console.log(`[Notification] Border effect: ${effectClass}, color: ${color}, duration: ${duration}ms`);
}

/**
 * Testa a notificação com configurações atuais
 */
async function testNotification() {
    addLog('Testando notificação...', 'info');

    // Dispara efeito de borda
    triggerBorderNotification();

    // Send test command to device
    try {
        await fetch('/api/notifications/test', { method: 'POST' });
    } catch (e) {
        console.error('[Notifications] Test failed:', e);
    }
}

/**
 * Testa um efeito específico
 */
function testSpecificEffect(effectIndex) {
    triggerBorderNotification(
        NotificationsPage.config.color,
        effectIndex,
        NotificationsPage.config.duration,
        NotificationsPage.config.fade
    );
}

// Export
window.initNotificationsPage = initNotificationsPage;
window.updateNotifyConfig = updateNotifyConfig;
window.updateEventEnabled = updateEventEnabled;
window.testNotification = testNotification;
window.testSpecificEffect = testSpecificEffect;
window.triggerBorderNotification = triggerBorderNotification;
window.NotificationsPage = NotificationsPage;

