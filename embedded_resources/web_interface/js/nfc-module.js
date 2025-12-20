/**
 * NFC Module - LELE PENTEST DEVICE
 * Read, Clone, Write, and Fault Injection for NFC/RFID tags
 */

const NFCModule = {
    currentTag: null,
    tagHistory: [],

    init() {
        this.loadHistory();
        console.log('[NFC] Module initialized');
    },

    read() {
        app.cmd('nfc_read');
        app.log('[NFC] Reading tag...', 'info');

        // Simulate in preview mode
        if (app.isPreview) {
            setTimeout(() => this.onTagRead({
                uid: 'A1B2C3D4',
                type: 'MIFARE Classic 1K',
                size: 1024,
                sectors: 16,
                data: Array(64).fill(0).map(() => Math.floor(Math.random() * 256).toString(16).padStart(2, '0')).join(' ')
            }), 1500);
        }
    },

    onTagRead(tag) {
        this.currentTag = { ...tag, timestamp: Date.now() };
        this.tagHistory.unshift(this.currentTag);
        if (this.tagHistory.length > 20) this.tagHistory.pop();
        this.saveHistory();
        this.renderTagData();
        app.log(`[NFC] Tag read: ${tag.uid} (${tag.type})`, 'success');
    },

    renderTagData() {
        const container = document.getElementById('nfc-data');
        if (!container || !this.currentTag) {
            if (container) container.innerHTML = '<div class="empty-state">Aproxime um cartão NFC...</div>';
            return;
        }

        const t = this.currentTag;
        container.innerHTML = `
            <div class="nfc-tag-card">
                <div class="nfc-tag-header">
                    <span class="nfc-uid">UID: ${t.uid}</span>
                    <span class="nfc-type">${t.type}</span>
                </div>
                <div class="nfc-tag-info">
                    <span>Size: ${t.size} bytes</span>
                    <span>Sectors: ${t.sectors}</span>
                </div>
                <div class="nfc-tag-data">
                    <div class="data-label">HEX Data:</div>
                    <pre class="hex-display">${this.formatHex(t.data)}</pre>
                </div>
                <div class="nfc-tag-actions">
                    <button class="btn btn-cyan" onclick="NFCModule.clone()">📋 Clone</button>
                    <button class="btn btn-green" onclick="NFCModule.write()">✏️ Write</button>
                    <button class="btn" onclick="NFCModule.format()">🗑️ Format</button>
                </div>
            </div>
        `;
    },

    formatHex(hexString) {
        if (!hexString) return '';
        const bytes = hexString.split(' ');
        let formatted = '';
        for (let i = 0; i < bytes.length; i += 16) {
            const line = bytes.slice(i, i + 16);
            const hex = line.join(' ');
            const ascii = line.map(b => {
                const c = parseInt(b, 16);
                return (c >= 32 && c <= 126) ? String.fromCharCode(c) : '.';
            }).join('');
            formatted += `${i.toString(16).padStart(4, '0')}: ${hex.padEnd(48)} | ${ascii}\n`;
        }
        return formatted;
    },

    clone() {
        if (!this.currentTag) {
            app.log('[NFC] No tag loaded to clone', 'err');
            return;
        }
        app.cmd('nfc_clone', JSON.stringify(this.currentTag));
        app.log('[NFC] Cloning tag... Place blank card', 'warn');
    },

    write(data = null) {
        const writeData = data || this.currentTag?.data;
        if (!writeData) {
            app.log('[NFC] No data to write', 'err');
            return;
        }
        app.cmd('nfc_write', writeData);
        app.log('[NFC] Writing to tag...', 'warn');
    },

    format() {
        if (confirm('⚠️ Format will erase all data on the tag. Continue?')) {
            app.cmd('nfc_format');
            app.log('[NFC] Formatting tag...', 'warn');
        }
    },

    faultInjection() {
        app.cmd('nfc_fault');
        app.log('[NFC] Fault injection attack started', 'warn');
    },

    saveHistory() {
        try {
            localStorage.setItem('nfc_history', JSON.stringify(this.tagHistory.slice(0, 10)));
        } catch (e) { }
    },

    loadHistory() {
        try {
            const saved = localStorage.getItem('nfc_history');
            if (saved) this.tagHistory = JSON.parse(saved);
        } catch (e) { }
    },

    showHistory() {
        const container = document.getElementById('nfc-history');
        if (!container) return;

        container.innerHTML = this.tagHistory.map((t, i) => `
            <div class="history-item" onclick="NFCModule.loadFromHistory(${i})">
                <span class="uid">${t.uid}</span>
                <span class="type">${t.type}</span>
                <span class="time">${new Date(t.timestamp).toLocaleString()}</span>
            </div>
        `).join('') || '<div class="empty-state">No history</div>';
    },

    loadFromHistory(index) {
        this.currentTag = this.tagHistory[index];
        this.renderTagData();
    }
};

// Export
window.NFCModule = NFCModule;
