/**
 * RF/SubGHz Module - LELE PENTEST DEVICE
 * Spectrum analyzer, signal capture/replay, jammer controls
 */

const RFModule = {
    signals: [],
    scanning: false,
    jamming: false,
    currentFreq: 433.92,

    frequencies: {
        '315': { name: '315 MHz', color: '#ff003c' },
        '433': { name: '433.92 MHz', color: '#00f3ff' },
        '868': { name: '868 MHz', color: '#ff00ff' },
        '915': { name: '915 MHz', color: '#00ff41' }
    },

    init() {
        this.renderSignalList();
        console.log('[RF] Module initialized');
    },

    startScan(freq = 433) {
        this.scanning = true;
        this.currentFreq = freq;
        app.cmd('rf_scan', freq);
        app.log(`[RF] Scanning ${freq} MHz...`, 'info');

        document.getElementById('rf-scan-btn')?.classList.add('active');

        // Simulate in preview mode
        if (app.isPreview) {
            setTimeout(() => this.onSignalCaptured({
                freq: freq,
                protocol: 'AM',
                data: 'A5F0C3',
                rssi: -45,
                timestamp: Date.now()
            }), 2000);
        }
    },

    stopScan() {
        this.scanning = false;
        app.cmd('rf_stop');
        document.getElementById('rf-scan-btn')?.classList.remove('active');
        app.log('[RF] Scan stopped', 'info');
    },

    onSignalCaptured(signal) {
        this.signals.unshift({
            id: Date.now(),
            ...signal,
            time: new Date().toLocaleTimeString()
        });

        if (this.signals.length > 50) this.signals.pop();
        this.renderSignalList();
        app.log(`[RF] Signal captured: ${signal.data}`, 'success');
    },

    renderSignalList() {
        const container = document.getElementById('rf-signals');
        if (!container) return;

        if (this.signals.length === 0) {
            container.innerHTML = '<div class="empty-state">No signals captured yet. Start scanning!</div>';
            return;
        }

        container.innerHTML = this.signals.map(s => `
            <div class="signal-item" data-id="${s.id}">
                <div class="signal-freq" style="color:${this.frequencies[String(Math.floor(s.freq))]?.color || '#fff'}">
                    ${s.freq} MHz
                </div>
                <div class="signal-data">${s.data}</div>
                <div class="signal-meta">${s.protocol} | ${s.rssi}dBm | ${s.time}</div>
                <div class="signal-actions">
                    <button class="btn-mini" onclick="RFModule.replay(${s.id})">▶️</button>
                    <button class="btn-mini" onclick="RFModule.deleteSignal(${s.id})">🗑️</button>
                </div>
            </div>
        `).join('');
    },

    replay(signalId) {
        const signal = this.signals.find(s => s.id === signalId);
        if (!signal) return;

        app.cmd('rf_replay', JSON.stringify(signal));
        app.log(`[RF] Replaying signal: ${signal.data}`, 'success');
    },

    deleteSignal(signalId) {
        this.signals = this.signals.filter(s => s.id !== signalId);
        this.renderSignalList();
    },

    startJammer(freq, mode = 'continuous') {
        this.jamming = true;
        app.cmd('rf_jam', JSON.stringify({ freq, mode }));
        app.log(`[RF] Jammer started: ${freq} MHz (${mode})`, 'warn');

        document.getElementById('rf-jam-btn')?.classList.add('active', 'danger');
    },

    stopJammer() {
        this.jamming = false;
        app.cmd('rf_jam_stop');
        document.getElementById('rf-jam-btn')?.classList.remove('active', 'danger');
        app.log('[RF] Jammer stopped', 'info');
    },

    bruteForce(protocol, startCode = '000000') {
        app.cmd('rf_brute', JSON.stringify({ protocol, start: startCode }));
        app.log(`[RF] Brute force started: ${protocol}`, 'warn');
    },

    // Spectrum visualization (simplified)
    drawSpectrum(data) {
        const canvas = document.getElementById('rf-spectrum-canvas');
        if (!canvas) return;

        const ctx = canvas.getContext('2d');
        const w = canvas.width;
        const h = canvas.height;

        ctx.clearRect(0, 0, w, h);
        ctx.strokeStyle = '#00f3ff';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(0, h);

        for (let i = 0; i < data.length; i++) {
            const x = (i / data.length) * w;
            const y = h - (data[i] / 100) * h;
            ctx.lineTo(x, y);
        }

        ctx.stroke();

        // Glow effect
        ctx.shadowColor = '#00f3ff';
        ctx.shadowBlur = 10;
        ctx.stroke();
    }
};

// Export
window.RFModule = RFModule;
