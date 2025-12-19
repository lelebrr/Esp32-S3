ï»¿/**
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 * Lele - WebSocket Connection Manager
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 */

// Reconnection state
let reconnectAttempts = 0;
const MAX_RECONNECT_DELAY = 30000; // 30 seconds max
let lastToastTime = 0;
const TOAST_COOLDOWN = 5000; // 5 seconds between toasts

function getReconnectDelay() {
    // Exponential backoff: 2s, 4s, 8s, 16s, 30s (max)
    return Math.min(2000 * Math.pow(2, reconnectAttempts), MAX_RECONNECT_DELAY);
}

function showRateLimitedToast(message, type) {
    const now = Date.now();
    if (now - lastToastTime > TOAST_COOLDOWN) {
        if (window.showToast) window.showToast(message, type);
        lastToastTime = now;
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// WEBSOCKET CONNECTION
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initWebSocket() {
    const wsProtocol = window.location.protocol === 'https:' ? 'wss' : 'ws';
    const wsUrl = `${wsProtocol}://${window.location.hostname}/ws`;

    // Only log on first attempt or every 5th attempt to reduce spam
    if (reconnectAttempts === 0 || reconnectAttempts % 5 === 0) {
        console.log(`[WebSocket] Connecting to ${wsUrl}... (attempt ${reconnectAttempts + 1})`);
    }

    // Update connection status to "reconnecting" if this is a retry
    if (reconnectAttempts > 0) {
        updateConnectionStatus('reconnecting');
    }

    WavePwn.ws = new WebSocket(wsUrl);

    WavePwn.ws.onopen = () => {
        console.log('[WebSocket] Connected!');
        WavePwn.state.connected = true;
        reconnectAttempts = 0; // Reset on successful connection

        updateConnectionStatus('connected');
        addLog('Conectado ao Lele Core', 'success');
        showRateLimitedToast('Conectado ao Sistema', 'success');

        // Request initial state
        WavePwn.ws.send(JSON.stringify({ cmd: 'get_state' }));
    };

    WavePwn.ws.onclose = () => {
        WavePwn.state.connected = false;
        reconnectAttempts++;

        // Only log every few attempts to reduce spam
        if (reconnectAttempts <= 3 || reconnectAttempts % 5 === 0) {
            console.log(`[WebSocket] Disconnected (attempt ${reconnectAttempts})`);
            addLog('ConexÃÂ£o perdida. Reconectando...', 'warning');
        }

        updateConnectionStatus('reconnecting');
        showRateLimitedToast('ConexÃÂ£o Perdida!', 'error');

        // Reconnect with exponential backoff
        const delay = getReconnectDelay();
        if (reconnectAttempts <= 3 || reconnectAttempts % 5 === 0) {
            console.log(`[WebSocket] Reconnecting in ${delay / 1000}s...`);
        }
        setTimeout(initWebSocket, delay);
    };

    WavePwn.ws.onerror = (error) => {
        // Only log first few errors to reduce spam
        if (reconnectAttempts <= 2) {
            console.error('[WebSocket] Error:', error);
            addLog('Erro de conexÃÂ£o WebSocket', 'error');
        }
    };

    WavePwn.ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);
            handleWebSocketMessage(data);
        } catch (e) {
            console.error('[WebSocket] Parse error:', e);
        }
    };
}

function updateConnectionStatus(status) {
    const statusEl = document.getElementById('connection-status');
    if (!statusEl) return;

    // Remove all status classes
    statusEl.classList.remove('connected', 'reconnecting');

    switch (status) {
        case 'connected':
            statusEl.classList.add('connected');
            statusEl.innerHTML = '<span class="dot"></span> Conectado';
            break;
        case 'reconnecting':
            statusEl.classList.add('reconnecting');
            const attempt = reconnectAttempts > 0 ? ` (${reconnectAttempts})` : '';
            statusEl.innerHTML = `<span class="dot"></span> Reconectando${attempt}`;
            break;
        default:
            statusEl.innerHTML = '<span class="dot"></span> Desconectado';
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// MESSAGE HANDLERS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function handleWebSocketMessage(data) {
    // Stats update (periodic)
    if (data.uptime !== undefined) {
        updateStats({
            uptime: data.uptime,
            battery: data.battery || 0,
            temperature: data.temp || 0,
            networks: data.aps || 0,
            handshakes: data.hs || 0,
            pmkids: data.pmkid || 0,
            bleDevices: data.ble_count || data.ble || 0,
            deauthsSent: data.deauths || 0,
            threat: data.ai || 'SAFE',
            // New fields
            macWifi: data.mac_wifi || '--:--:--:--:--:--',
            macBle: data.mac_ble || '--:--:--:--:--:--',
            heapFree: data.heap_free || 0,
            heapTotal: data.heap_total || 0,
            psramFree: data.psram_free || 0,
            psramTotal: data.psram_total || 0,
            sdOnline: data.sd_online || false,
            sdTotal: data.sd_total || 0,
            sdUsed: data.sd_used || 0
        });

        // Update signal chart with activity
        if (data.activity !== undefined) {
            updateChart('signal', data.activity);
        }
    }

    // Log message
    if (data.log) {
        addLog(data.log, data.logType || 'info');
    }

    // Network list update
    if (data.networks) {
        updateNetworkTable(data.networks);
    }

    // BLE devices list
    if (data.ble_devices) {
        updateBLEList(data.ble_devices);
    }

    // Dragon mascot update
    if (data.mascot) {
        updateDragonMood(data.mascot.mood, data.mascot.text);
    }

    // Terminal output
    if (data.term_out) {
        appendTerminalOutput(data.term_out);
    }

    // Attack status
    if (data.attack_status) {
        updateAttackStatus(data.attack_status);
    }

    // Channel data
    if (data.channels) {
        updateChannelChart(data.channels);
    }

    // Security types
    if (data.security_types) {
        updateSecurityChart(data.security_types);
    }

    // Achievement unlocked!
    if (data.achievement) {
        showAchievement(data.achievement);
    }

    // Handshake captured notification (Dica 16 - Confetti)
    if (data.handshake_captured) {
        showHandshakeNotification(data.handshake_captured);
        // Trigger confetti celebration!
        if (typeof celebrateHandshake === 'function') {
            celebrateHandshake();
        }
        // Trigger border notification effect
        if (typeof triggerBorderNotification === 'function') {
            triggerBorderNotification('#00FF9D'); // Verde para sucesso
        }
        // Send Discord notification if configured
        if (typeof sendDiscordNotification === 'function') {
            sendDiscordNotification(
                `°Å¸Å½â° Handshake capturado: **${data.handshake_captured.ssid}**`,
                'handshake'
            );
        }
    }

    // Mascot XP/Level update
    if (data.xp !== undefined || data.level !== undefined) {
        if (typeof updateMascotStats === 'function') {
            updateMascotStats(data);
        }
    }

    // Camera Chaos events
    if ((data.camera_found || data.camera_attack_result) && window.handleCameraResult) {
        window.handleCameraResult(data);
    }

    // Printer Chaos events
    if ((data.printer_found || data.printer_attack_log) && window.handlePrinterResult) {
        window.handlePrinterResult(data);
    }

    // [NEW] Heatmap Data (Array of ints)
    if (data.type === 'heatmap' && window.updateHeatmapData) {
        window.updateHeatmapData(data.data);
    }

    // [NEW] Mascot Events (attack_start, handshake, etc)
    if (data.type === 'mascot_event' && window.mascotReact) {
        window.mascotReact(data.event);
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// UI UPDATE FUNCTIONS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function updateNetworkTable(networks) {
    const tbody = document.querySelector('#networks-table tbody');
    if (!tbody) return;

    tbody.innerHTML = '';

    networks.forEach((net, index) => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>${escapeHtml(net.ssid) || '<hidden>'}</td>
            <td class="font-mono">${net.bssid}</td>
            <td>${net.channel}</td>
            <td>${getSignalBars(net.rssi)} ${net.rssi}dBm</td>
            <td><span class="badge badge-${getSecurityClass(net.security)}">${net.security}</span></td>
            <td>
                <button class="btn btn-sm btn-danger" onclick="startDeauth('${net.bssid}')">âÅ¡Â¡</button>
                <button class="btn btn-sm btn-primary" onclick="selectTarget('${net.bssid}', '${escapeHtml(net.ssid)}')">°Å¸Å½Â¯</button>
            </td>
        `;
        tbody.appendChild(tr);
    });

    // Update target selector
    const selector = document.getElementById('target-selector');
    if (selector) {
        selector.innerHTML = '<option value="FF:FF:FF:FF:FF:FF">Broadcast (Todos)</option>';
        networks.forEach(net => {
            const opt = document.createElement('option');
            opt.value = net.bssid;
            opt.textContent = `${net.ssid || '<hidden>'} (${net.bssid})`;
            selector.appendChild(opt);
        });
    }
}

function updateBLEList(devices) {
    const list = document.getElementById('ble-list');
    if (!list) return;

    list.innerHTML = '';

    devices.forEach(device => {
        const li = document.createElement('li');
        li.innerHTML = `
            <span class="ble-name">${escapeHtml(device.name) || 'Desconhecido'}</span>
            <span class="ble-mac font-mono">${device.mac}</span>
            <span class="ble-rssi">${device.rssi}dBm</span>
        `;
        list.appendChild(li);
    });
}

function updateChannelChart(channels) {
    if (!WavePwn.charts.channel) return;
    WavePwn.charts.channel.data.datasets[0].data = channels;
    WavePwn.charts.channel.update();
}

function updateSecurityChart(types) {
    if (!WavePwn.charts.security) return;
    WavePwn.charts.security.data.datasets[0].data = [
        types.wpa2 || 0,
        types.wpa3 || 0,
        types.wep || 0,
        types.open || 0
    ];
    WavePwn.charts.security.update();
}

function updateAttackStatus(status) {
    const indicator = document.getElementById('attack-indicator');
    if (!indicator) return;

    if (status.active) {
        indicator.classList.add('active');
        indicator.textContent = `${status.type}: ${status.progress || 'ATIVO'}`;
    } else {
        indicator.classList.remove('active');
        indicator.textContent = 'Nenhum ataque ativo';
    }
}

function appendTerminalOutput(text) {
    const terminal = document.getElementById('terminal-output');
    if (!terminal) return;

    const line = document.createElement('div');
    line.textContent = text;
    terminal.appendChild(line);
    terminal.scrollTop = terminal.scrollHeight;
}

function showAchievement(achievement) {
    const toast = document.createElement('div');
    toast.className = 'achievement-toast';
    toast.innerHTML = `
        <div class="achievement-icon">°Å¸Ââ </div>
        <div class="achievement-content">
            <div class="achievement-title">${achievement.title}</div>
            <div class="achievement-desc">${achievement.description}</div>
        </div>
    `;
    document.body.appendChild(toast);

    // Trigger border notification for achievement
    if (typeof triggerBorderNotification === 'function') {
        triggerBorderNotification('#FFCC00', 4); // Rainbow effect for achievements
    }

    setTimeout(() => {
        toast.classList.add('show');
    }, 100);

    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => toast.remove(), 500);
    }, 5000);
}

function showHandshakeNotification(data) {
    // Play sound
    playKaChing();

    // Show notification
    addLog(`°Å¸Å½â° Handshake capturado: ${data.ssid}`, 'success');

    // Flash the handshake counter
    const counter = document.getElementById('handshakes-count');
    if (counter) {
        counter.classList.add('explosion');
        setTimeout(() => counter.classList.remove('explosion'), 500);
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// HELPERS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function escapeHtml(text) {
    if (!text) return '';
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function getSignalBars(rssi) {
    if (rssi >= -50) return 'ââËââËââËââË';
    if (rssi >= -60) return 'ââËââËââËâââ';
    if (rssi >= -70) return 'ââËââËââââââ';
    if (rssi >= -80) return 'ââËâââââââââ';
    return 'ââââââââââââ';
}

function getSecurityClass(security) {
    if (!security) return 'open';
    const sec = security.toUpperCase();
    if (sec.includes('WPA3')) return 'wpa3';
    if (sec.includes('WPA2')) return 'wpa2';
    if (sec.includes('WEP')) return 'wep';
    if (sec.includes('OPEN') || sec.includes('OPN')) return 'open';
    return 'unknown';
}

function selectTarget(bssid, ssid) {
    const selector = document.getElementById('target-selector');
    if (selector) selector.value = bssid;

    addLog(`Alvo selecionado: ${ssid} (${bssid})`, 'info');
}

// Export
window.initWebSocket = initWebSocket;

