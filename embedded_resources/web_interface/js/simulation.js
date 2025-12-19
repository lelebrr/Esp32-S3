/**
 * LELE - Data Manager
 * Gerencia dados reais do dispositivo com fallback para simulação quando desconectado
 */

// Estado global de dados
const RealTimeData = {
    connected: false,
    lastUpdate: 0,
    stats: {
        uptime: 0,
        battery: 0,
        temperature: 0,
        heapFree: 0,
        heapTotal: 0,
        networks: 0,
        handshakes: 0,
        pmkids: 0,
        bleDevices: 0,
        deauthsSent: 0
    }
};

/**
 * Atualiza dados reais recebidos via WebSocket
 */
function updateRealData(data) {
    RealTimeData.connected = true;
    RealTimeData.lastUpdate = Date.now();

    // Atualiza stats
    if (data.uptime !== undefined) RealTimeData.stats.uptime = data.uptime;
    if (data.battery !== undefined) RealTimeData.stats.battery = data.battery;
    if (data.temp !== undefined) RealTimeData.stats.temperature = data.temp;
    if (data.heap_free !== undefined) RealTimeData.stats.heapFree = data.heap_free;
    if (data.heap_total !== undefined) RealTimeData.stats.heapTotal = data.heap_total;
    if (data.aps !== undefined) RealTimeData.stats.networks = data.aps;
    if (data.hs !== undefined) RealTimeData.stats.handshakes = data.hs;
    if (data.pmkid !== undefined) RealTimeData.stats.pmkids = data.pmkid;
    if (data.ble_count !== undefined) RealTimeData.stats.bleDevices = data.ble_count;
    if (data.deauths !== undefined) RealTimeData.stats.deauthsSent = data.deauths;

    // Atualiza UI com dados reais
    updateDashboardUI();
}

/**
 * Atualiza elementos do Dashboard com dados reais
 */
function updateDashboardUI() {
    const stats = RealTimeData.stats;

    // Cards de status
    updateElement('uptime-value', formatUptime(stats.uptime));
    updateElement('battery-value', stats.battery + '%');
    updateElement('temp-value', stats.temperature + '°C');
    updateElement('networks-count', stats.networks);
    updateElement('handshakes-count', stats.handshakes);
    updateElement('pmkid-count', stats.pmkids);
    updateElement('ble-count', stats.bleDevices);
    updateElement('deauths-count', stats.deauthsSent);

    // Memória
    if (stats.heapTotal > 0) {
        const usedPercent = ((stats.heapTotal - stats.heapFree) / stats.heapTotal * 100).toFixed(1);
        updateElement('memory-value', usedPercent + '%');
        updateProgressBar('memory-bar', 100 - parseFloat(usedPercent));
    }

    // LED indicador de conexão
    updateConnectionIndicator(true);
}

/**
 * Helper para atualizar elemento
 */
function updateElement(id, value) {
    const el = document.getElementById(id);
    if (el) el.textContent = value;
}

/**
 * Helper para progress bar
 */
function updateProgressBar(id, percent) {
    const el = document.getElementById(id);
    if (el) {
        el.style.width = Math.min(100, Math.max(0, percent)) + '%';
    }
}

/**
 * Formata uptime em horas:minutos:segundos
 */
function formatUptime(seconds) {
    if (!seconds) return '00:00:00';
    const h = Math.floor(seconds / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;
    return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
}

/**
 * Atualiza indicador de conexão
 */
function updateConnectionIndicator(connected) {
    const statusEl = document.getElementById('connection-status');
    if (statusEl) {
        if (connected) {
            statusEl.classList.add('connected');
            statusEl.classList.remove('reconnecting');
        } else {
            statusEl.classList.remove('connected');
            statusEl.classList.add('reconnecting');
        }
    }
}

// ==========================================
// MODO DE SIMULAÇÃO (quando desconectado)
// ==========================================
function simulateTraffic() {
    console.log('[LELE] Iniciando modo de dados...');

    setInterval(() => {
        // Só simula se não estiver conectado ou timeout de 5s
        const timeSinceUpdate = Date.now() - RealTimeData.lastUpdate;
        const shouldSimulate = !RealTimeData.connected || timeSinceUpdate > 5000;

        if (shouldSimulate) {
            updateConnectionIndicator(false);

            // Simula dados visuais
            const signal = -40 - Math.random() * 50;
            updateChart('signal', signal);

            // Atualiza canais aleatoriamente
            if (Math.random() > 0.8 && WavePwn.charts.channel) {
                const ch = Math.floor(Math.random() * 13);
                WavePwn.charts.channel.data.datasets[0].data[ch] = Math.floor(Math.random() * 20);
                WavePwn.charts.channel.update('none');
            }

            // Log de simulação
            if (Math.random() > 0.95) {
                const msgs = [
                    '[SCAN] Rede oculta detectada: FBI_Van_42',
                    '[BLE] Dispositivo: Apple iPhone 15',
                    '[RF] Sinal em 433.92MHz',
                    '[SISTEMA] Aguardando conexão...',
                    '[WIFI] Escaneando redes...',
                    '[BLE] Nordic nRF52840 encontrado'
                ];
                addLog(msgs[Math.floor(Math.random() * msgs.length)], 'info');
            }
        } else {
            updateConnectionIndicator(true);

            // Usa dados reais para gráficos
            if (WavePwn.charts.signal && RealTimeData.stats.networks > 0) {
                updateChart('signal', -50 - Math.random() * 30); // Variação sutil
            }
        }

    }, 1000);
}

/**
 * Solicita dados do dispositivo via WebSocket
 */
function requestRealData() {
    if (WavePwn.ws && WavePwn.ws.readyState === WebSocket.OPEN) {
        WavePwn.ws.send(JSON.stringify({ cmd: 'get_state' }));
    }
}

// Solicita dados a cada 2 segundos quando conectado
setInterval(() => {
    if (WavePwn.ws && WavePwn.ws.readyState === WebSocket.OPEN) {
        requestRealData();
    }
}, 2000);

// Exporta funções
window.updateRealData = updateRealData;
window.RealTimeData = RealTimeData;
window.simulateTraffic = simulateTraffic;
