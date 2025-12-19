/**
 * WAVEPWN - Evil Twin Builder (Dica 27)
 * Construtor avançado de Evil Twin
 */

const EvilTwinBuilder = {
    config: {
        ssid: '',
        channel: 6,
        template: 'default',
        customHtml: '',
        captureCredentials: true,
        redirectUrl: '',
        cloneTarget: null
    }
};

function initEvilTwinBuilder() {
    loadEvilTwinConfig();
}

function loadEvilTwinConfig() {
    try {
        const saved = localStorage.getItem('wavepwn-eviltwin-config');
        if (saved) EvilTwinBuilder.config = JSON.parse(saved);
    } catch (e) { }
}

function saveEvilTwinConfig() {
    localStorage.setItem('wavepwn-eviltwin-config', JSON.stringify(EvilTwinBuilder.config));
}

function renderEvilTwinBuilder(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="eviltwin-builder">
            <h3>👿 Evil Twin Builder</h3>
            
            <div class="builder-section">
                <h4>1. Selecionar Alvo</h4>
                <div class="target-select">
                    <select id="et-target" onchange="selectEvilTwinTarget(this.value)">
                        <option value="">Escolha uma rede para clonar...</option>
                    </select>
                    <button class="btn btn-sm" onclick="refreshNetworkList()">🔄</button>
                </div>
            </div>
            
            <div class="builder-section">
                <h4>2. Configuração do AP</h4>
                <div class="form-group">
                    <label>SSID</label>
                    <input type="text" id="et-ssid" value="${EvilTwinBuilder.config.ssid}" 
                           onchange="updateETConfig('ssid', this.value)" placeholder="Nome da rede">
                </div>
                <div class="form-group">
                    <label>Canal</label>
                    <select id="et-channel" onchange="updateETConfig('channel', parseInt(this.value))">
                        ${Array.from({ length: 13 }, (_, i) => `<option value="${i + 1}" ${EvilTwinBuilder.config.channel === i + 1 ? 'selected' : ''}>Canal ${i + 1}</option>`).join('')}
                    </select>
                </div>
            </div>
            
            <div class="builder-section">
                <h4>3. Portal Cativo</h4>
                <div class="template-buttons">
                    <button class="btn ${EvilTwinBuilder.config.template === 'facebook' ? 'active' : ''}" onclick="selectETTemplate('facebook')">📘 Facebook</button>
                    <button class="btn ${EvilTwinBuilder.config.template === 'google' ? 'active' : ''}" onclick="selectETTemplate('google')">🔍 Google</button>
                    <button class="btn ${EvilTwinBuilder.config.template === 'hotel' ? 'active' : ''}" onclick="selectETTemplate('hotel')">🏨 Hotel</button>
                    <button class="btn ${EvilTwinBuilder.config.template === 'custom' ? 'active' : ''}" onclick="selectETTemplate('custom')">✏️ Custom</button>
                </div>
                <div id="et-custom-html" style="display:${EvilTwinBuilder.config.template === 'custom' ? 'block' : 'none'}">
                    <textarea placeholder="HTML personalizado..." onchange="updateETConfig('customHtml', this.value)">${EvilTwinBuilder.config.customHtml}</textarea>
                </div>
            </div>
            
            <div class="builder-section">
                <h4>4. Opções</h4>
                <label class="checkbox-label">
                    <input type="checkbox" ${EvilTwinBuilder.config.captureCredentials ? 'checked' : ''} 
                           onchange="updateETConfig('captureCredentials', this.checked)">
                    Capturar credenciais
                </label>
                <div class="form-group">
                    <label>Redirect após login</label>
                    <input type="url" placeholder="https://google.com" value="${EvilTwinBuilder.config.redirectUrl}"
                           onchange="updateETConfig('redirectUrl', this.value)">
                </div>
            </div>
            
            <div class="builder-actions">
                <button class="btn btn-secondary" onclick="previewEvilTwin()">👁️ Preview</button>
                <button class="btn btn-danger" onclick="launchEvilTwin()">🚀 Lançar Evil Twin</button>
            </div>
        </div>
    `;

    refreshNetworkList();
}

function updateETConfig(key, value) {
    EvilTwinBuilder.config[key] = value;
    saveEvilTwinConfig();
}

function selectETTemplate(template) {
    EvilTwinBuilder.config.template = template;
    saveEvilTwinConfig();
    document.getElementById('et-custom-html').style.display = template === 'custom' ? 'block' : 'none';
    document.querySelectorAll('.template-buttons .btn').forEach(btn => btn.classList.remove('active'));
    event.target.classList.add('active');
}

function selectEvilTwinTarget(bssid) {
    // Clone target network settings
    const networks = window.lastScannedNetworks || [];
    const target = networks.find(n => n.bssid === bssid);
    if (target) {
        EvilTwinBuilder.config.ssid = target.ssid;
        EvilTwinBuilder.config.channel = target.channel;
        EvilTwinBuilder.config.cloneTarget = target;
        document.getElementById('et-ssid').value = target.ssid;
        document.getElementById('et-channel').value = target.channel;
        saveEvilTwinConfig();
    }
}

async function refreshNetworkList() {
    try {
        const response = await fetch('/api/networks');
        const data = await response.json();
        const networks = data.networks || [];
        window.lastScannedNetworks = networks;

        const select = document.getElementById('et-target');
        if (select) {
            select.innerHTML = '<option value="">Escolha uma rede...</option>' +
                networks.map(n => `<option value="${n.bssid}">${n.ssid || '[Hidden]'} (${n.bssid})</option>`).join('');
        }
    } catch (e) {
        console.error('[EvilTwin] Failed to load networks:', e);
    }
}

function previewEvilTwin() {
    // Open preview in modal or new window
    if (typeof selectTemplate === 'function') {
        selectTemplate(EvilTwinBuilder.config.template);
    }
}

async function launchEvilTwin() {
    if (!confirm('Iniciar Evil Twin Attack?')) return;

    try {
        const response = await fetch('/api/eviltwin/start', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(EvilTwinBuilder.config)
        });

        if (response.ok) {
            if (typeof addLog === 'function') addLog('Evil Twin iniciado!', 'success');
            if (typeof logAttack === 'function') {
                logAttack({ type: 'evil_twin', ssid: EvilTwinBuilder.config.ssid, channel: EvilTwinBuilder.config.channel, success: true });
            }
        }
    } catch (e) {
        console.error('[EvilTwin] Launch failed:', e);
    }
}

window.initEvilTwinBuilder = initEvilTwinBuilder;
window.renderEvilTwinBuilder = renderEvilTwinBuilder;
window.selectEvilTwinTarget = selectEvilTwinTarget;
window.selectETTemplate = selectETTemplate;
window.updateETConfig = updateETConfig;
window.launchEvilTwin = launchEvilTwin;
window.previewEvilTwin = previewEvilTwin;
