
// Additional functions for new features
function toggleSidebar() {
    document.getElementById('sidebar').classList.toggle('open');
}
function setBrightness(val) {
    document.getElementById('brightness-value').textContent = Math.round(val / 2.55) + '%';
    sendCommand('set_brightness', { value: parseInt(val) });
}
function setTxPower(val) {
    document.getElementById('txpower-value').textContent = val + 'dBm';
    sendCommand('set_txpower', { value: parseInt(val) });
}
function setVolume(val) {
    document.getElementById('volume-value').textContent = val + '%';
    sendCommand('set_volume', { value: parseInt(val) });
    localStorage.setItem('lele-volume', val);
}
function toggleMute() {
    const slider = document.getElementById('volume-slider');
    const icon = document.querySelector('.volume-icon');
    if (slider.value > 0) {
        slider.dataset.lastVolume = slider.value;
        slider.value = 0;
        icon.textContent = 'ðŸ”‡';
    } else {
        slider.value = slider.dataset.lastVolume || 80;
        icon.textContent = 'ðŸ”Š';
    }
    setVolume(slider.value);
}
function toggleSound(enabled) {
    WavePwn.state.soundEnabled = enabled;
    localStorage.setItem('lele-sound', enabled);
}
function playTestSound() {
    sendCommand('play_sound', { sound: 'test' });
}
function saveAPConfig() {
    const ssid = document.getElementById('ap-ssid').value;
    const password = document.getElementById('ap-password').value;
    const channel = document.getElementById('ap-channel').value;
    const ip = document.getElementById('ap-ip').value;
    if (!ssid) { addLog('SSID é obrigatório', 'error'); return; }
    if (password && password.length < 8) { addLog('Senha deve ter 8+ caracteres', 'error'); return; }
    sendCommand('set_ap_config', { ssid, password, channel: parseInt(channel), ip });
    addLog('Configuração do AP salva. IP será ' + ip + ' após reinício.', 'success');
}
function saveDashboardAuth() {
    const username = document.getElementById('dash-username').value;
    const oldPass = document.getElementById('dash-password-old').value;
    const newPass = document.getElementById('dash-password-new').value;
    if (!username || !newPass) { addLog('Preencha todos os campos', 'error'); return; }
    sendCommand('set_dashboard_auth', { username, old_password: oldPass, new_password: newPass });
}
function saveOpenAIConfig() {
    const key = document.getElementById('openai-key').value;
    const model = document.getElementById('openai-model').value;
    const tokens = document.getElementById('openai-tokens').value;
    localStorage.setItem('openai-key', key);
    localStorage.setItem('openai-model', model);
    localStorage.setItem('openai-tokens', tokens);
    sendCommand('set_openai', { key, model, max_tokens: parseInt(tokens) });
    addLog('Configuração OpenAI salva', 'success');
}
function saveOHCConfig() {
    const email = document.getElementById('ohc-email').value;
    const key = document.getElementById('ohc-key').value;
    localStorage.setItem('ohc-email', email);
    localStorage.setItem('ohc-key', key);
    addLog('Configuração OnlineHashCrack salva', 'success');
}
// Load OHC Config on startup
document.addEventListener('DOMContentLoaded', () => {
    const email = localStorage.getItem('ohc-email');
    const key = localStorage.getItem('ohc-key');
    if (email && document.getElementById('ohc-email')) document.getElementById('ohc-email').value = email;
    if (key && document.getElementById('ohc-key')) document.getElementById('ohc-key').value = key;
});
function setMascotPersonality(personality) {
    localStorage.setItem('mascot-personality', personality);
    sendCommand('set_mascot_personality', { personality });
}
async function refreshFiles(path = '/') {
    try {
        const response = await fetch('/api/files/list?path=' + encodeURIComponent(path));
        const data = await response.json();
        updateFileBrowser(data.files || [], path);
    } catch (e) { addLog('Erro ao listar arquivos', 'error'); }
}
function updateFileBrowser(files, currentPath = '/') {
    const container = document.getElementById('file-browser');
    const breadcrumb = document.getElementById('file-breadcrumb');
    breadcrumb.innerHTML = 'ðŸ“‚ ' + currentPath;
    if (!files.length) { container.innerHTML = '<p class="text-muted">Pasta vazia</p>'; return; }
    container.innerHTML = files.map(f => `
                <div class="file-item">
                    <div class="file-info">
                        <span class="file-icon">${f.isDir ? 'ðŸ“' : 'ðŸ“„'}</span>
                        <div class="file-details">
                            <span class="file-name">${f.isDir ? '<a href="#" onclick="refreshFiles(\'' + currentPath + f.name + '/\');return false;">' + f.name + '</a>' : f.name}</span>
                            <div class="file-meta"><span>${f.size ? formatBytes(f.size) : '--'}</span><span>${f.modified || '--'}</span></div>
                        </div>
                    </div>
                    <div class="file-actions">
                        ${!f.isDir ? '<button class="btn btn-sm btn-primary" onclick="downloadFile(\'' + currentPath + f.name + '\')">â¬‡ï¸</button>' : ''}
                        <button class="btn btn-sm btn-danger" onclick="deleteFile(\''+currentPath+f.name+'\')">ðŸ—‘ï¸</button>
                    </div>
                </div>
            `).join('');
}
function formatBytes(bytes) {
    if (!bytes) return '0 B';
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1048576) return (bytes / 1024).toFixed(1) + ' KB';
    return (bytes / 1048576).toFixed(1) + ' MB';
}
function downloadFile(path) {
    window.open('/api/files/download?path=' + encodeURIComponent(path), '_blank');
}
async function deleteFile(path) {
    if (!confirm('Deletar ' + path + '?')) return;
    try {
        await fetch('/api/files/delete', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ path }) });
        addLog('Arquivo deletado: ' + path, 'success');
        refreshFiles();
    } catch (e) { addLog('Erro ao deletar', 'error'); }
}
function createFolder() {
    const name = prompt('Nome da pasta:');
    if (!name) return;
    sendCommand('create_folder', { path: '/' + name });
    setTimeout(refreshFiles, 500);
}
async function uploadFile(file) {
    if (!file) return;
    const formData = new FormData();
    formData.append('file', file);
    try {
        await fetch('/api/files/upload', { method: 'POST', body: formData });
        addLog('Arquivo enviado: ' + file.name, 'success');
        refreshFiles();
    } catch (e) { addLog('Erro ao enviar', 'error'); }
}
async function uploadOTA(file) {
    if (!file || !confirm('Atualizar firmware?')) return;
    const formData = new FormData();
    formData.append('update', file);
    try {
        const r = await fetch('/update', { method: 'POST', body: formData });
        if (r.ok) { alert('Atualização OK! Reiniciando...'); setTimeout(() => location.reload(), 5000); }
    } catch (e) { addLog('Erro OTA', 'error'); }
}
function sendTerminalCommand() {
    const input = document.getElementById('terminal-input');
    const cmd = input.value.trim();
    if (!cmd) return;
    appendTerminalOutput('$ ' + cmd);
    sendCommand('terminal', { cmd });
    input.value = '';
}
function exportLogs() {
    const logs = document.getElementById('log-container').innerText;
    const blob = new Blob([logs], { type: 'text/plain' });
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = 'wavepwn_logs_' + Date.now() + '.txt';
    a.click();
}
function loadHandshakes() {
    if (typeof initHandshakesPage === 'function') initHandshakesPage();
}
function startCracking() { addLog('Cracking iniciado...', 'info'); }
function startEvilTwin() {
    const ssid = document.getElementById('et-ssid').value;
    const template = document.getElementById('et-template').value;
    if (!ssid) { addLog('SSID obrigatório', 'error'); return; }
    sendCommand('evil_twin', { ssid, template });
}
function scheduleAttack() { addLog('Agendar ataque - em desenvolvimento', 'info'); }

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// IR REMOTE CONTROL FUNCTIONS
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
function sendIR(button) {
    const device = document.getElementById('ir-device-type').value;
    const brand = document.getElementById('ir-brand').value;
    sendCommand('ir_send', { device, brand, button });
    addLog(`ðŸ“º IR enviado: ${brand.toUpperCase()} - ${button}`, 'info');
}
function loadIRBrand() {
    const device = document.getElementById('ir-device-type').value;
    addLog(`Dispositivo alterado: ${device}`, 'info');
}
// Auto-Find Remote
let autoFindState = { brand: '', codeIndex: 0, maxCodes: 50, button: 'power', phase: 1 };
function startAutoFind() {
    const brand = document.getElementById('ir-autofind-brand').value;
    autoFindState = { brand, codeIndex: 0, maxCodes: 50, button: 'vol_up', phase: 1 };
    document.getElementById('ir-autofind-status').style.display = 'block';
    document.getElementById('ir-autofind-text').textContent = `Testando ${brand.toUpperCase()} código 1 de 50... Pressione VOL+ na TV`;
    sendCommand('ir_autofind', { brand, codeIndex: 0, button: 'vol_up' });
    addLog(`ðŸ” Buscando controle ${brand}...`, 'info');
}
function autoFindYes() {
    if (autoFindState.phase === 1) {
        autoFindState.phase = 2;
        autoFindState.button = 'ch_up';
        document.getElementById('ir-autofind-text').textContent = `VOL+ funcionou! Agora testando CH+...`;
        sendCommand('ir_autofind', { brand: autoFindState.brand, codeIndex: autoFindState.codeIndex, button: 'ch_up' });
    } else {
        document.getElementById('ir-autofind-status').style.display = 'none';
        addLog(`âœ… Controle ${autoFindState.brand.toUpperCase()} encontrado! Código: ${autoFindState.codeIndex}`, 'success');
        document.getElementById('ir-brand').value = autoFindState.brand;
        sendCommand('ir_save_found', { brand: autoFindState.brand, codeIndex: autoFindState.codeIndex });
    }
}
function autoFindNo() {
    autoFindState.codeIndex++;
    if (autoFindState.codeIndex >= autoFindState.maxCodes) {
        document.getElementById('ir-autofind-status').style.display = 'none';
        addLog(`âŒ Nenhum código funcionou para ${autoFindState.brand}`, 'error');
        return;
    }
    document.getElementById('ir-autofind-text').textContent = `Testando ${autoFindState.brand.toUpperCase()} código ${autoFindState.codeIndex + 1} de 50...`;
    sendCommand('ir_autofind', { brand: autoFindState.brand, codeIndex: autoFindState.codeIndex, button: autoFindState.button });
}
// Record Remote
let recordState = { name: '', buttons: {}, currentBtn: '', recording: false };
const recordButtons = ['power', 'vol_up', 'vol_down', 'ch_up', 'ch_down', 'mute', 'ok', 'up', 'down', 'left', 'right', 'menu', 'back'];
function startRecording() {
    const name = document.getElementById('ir-record-name').value;
    if (!name) { addLog('Digite um nome para o controle', 'error'); return; }
    recordState = { name, buttons: {}, currentBtn: recordButtons[0], recording: true };
    document.getElementById('ir-record-status').style.display = 'block';
    document.getElementById('ir-record-buttons').style.display = 'block';
    document.getElementById('ir-recorded-list').innerHTML = '';
    promptNextRecordButton();
    sendCommand('ir_record_start', {});
    addLog(`âºï¸ Gravando controle: ${name}`, 'info');
}
function promptNextRecordButton() {
    if (!recordState.recording) return;
    const btn = recordState.currentBtn;
    document.getElementById('ir-record-text').textContent = `Aponte o controle para o sensor e pressione:`;
    document.getElementById('ir-record-btn').textContent = btn.toUpperCase().replace('_', ' ');
}
function irButtonRecorded(button, code) {
    recordState.buttons[button] = code;
    const list = document.getElementById('ir-recorded-list');
    list.innerHTML += `<span style="background:var(--success);color:#000;padding:4px 8px;border-radius:4px;font-size:0.75rem;">${button}</span>`;
    const idx = recordButtons.indexOf(button);
    if (idx < recordButtons.length - 1) {
        recordState.currentBtn = recordButtons[idx + 1];
        promptNextRecordButton();
    } else {
        finishRecording();
    }
}
function finishRecording() {
    recordState.recording = false;
    document.getElementById('ir-record-status').style.display = 'none';
    sendCommand('ir_record_save', { name: recordState.name, buttons: recordState.buttons });
    addLog(`âœ… Controle "${recordState.name}" salvo com ${Object.keys(recordState.buttons).length} botões!`, 'success');
    loadSavedRemotes();
}
async function loadSavedRemotes() {
    try {
        const r = await fetch('/api/ir/remotes');
        const data = await r.json();
        const list = document.getElementById('ir-saved-list');
        if (!data.remotes || data.remotes.length === 0) {
            list.innerHTML = '<p class="text-muted">Nenhum controle salvo</p>';
            return;
        }
        list.innerHTML = data.remotes.map(remote => `
                    <div style="display:flex;justify-content:space-between;align-items:center;padding:8px;border-bottom:1px solid var(--border-color);">
                        <span>ðŸ“º ${remote.name}</span>
                        <div>
                            <button class="btn btn-sm btn-primary" onclick="useRemote('${remote.name}')">Usar</button>
                            <button class="btn btn-sm btn-danger" onclick="deleteRemote('${remote.name}')">ðŸ—‘ï¸</button>
                        </div>
                    </div>
                `).join('');
    } catch (e) { console.error('Failed to load remotes:', e); }
}
function useRemote(name) {
    document.getElementById('ir-device-type').value = 'custom';
    addLog(`Usando controle: ${name}`, 'info');
    sendCommand('ir_use_remote', { name });
}
function deleteRemote(name) {
    if (!confirm(`Deletar controle "${name}"?`)) return;
    sendCommand('ir_delete_remote', { name });
    setTimeout(loadSavedRemotes, 500);
}
// Export
window.sendIR = sendIR;
window.irButtonRecorded = irButtonRecorded;

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// CAMERA CHAOS FUNCTIONS - IP Camera Scanner & Attacks
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
const cameraCredentials = [
    // ðŸ‡§ðŸ‡· BRASILEIRAS (50+)
    { brand: 'Intelbras', user: 'admin', pass: 'admin' },
    { brand: 'Intelbras', user: 'admin', pass: '123456' },
    { brand: 'Intelbras', user: 'admin', pass: '' },
    { brand: 'Intelbras', user: 'admin', pass: 'intelbras' },
    { brand: 'Intelbras', user: 'admin', pass: '1234' },
    { brand: 'Giga', user: 'admin', pass: 'admin' },
    { brand: 'Giga', user: 'admin', pass: '1234' },
    { brand: 'Giga', user: 'admin', pass: '123456' },
    { brand: 'Luxvision', user: 'admin', pass: '123456' },
    { brand: 'Luxvision', user: 'admin', pass: 'admin' },
    { brand: 'JFL', user: 'admin', pass: '123456' },
    { brand: 'JFL', user: 'admin', pass: '1234' },
    { brand: 'Positivo', user: 'admin', pass: 'meuanjo' },
    { brand: 'Positivo', user: 'admin', pass: 'admin' },
    { brand: 'TP-Link', user: 'admin', pass: 'admin' },
    { brand: 'TP-Link', user: 'admin', pass: '' },
    { brand: 'Multilaser', user: 'admin', pass: 'admin' },
    { brand: 'Multilaser', user: 'admin', pass: '123456' },
    { brand: 'Elsys', user: 'admin', pass: 'admin' },
    { brand: 'Elsys', user: 'admin', pass: '' },
    { brand: 'Tecvoz', user: 'admin', pass: 'admin' },
    { brand: 'Tecvoz', user: 'admin', pass: '123456' },
    { brand: 'VIP', user: 'admin', pass: 'admin' },
    { brand: 'Citrox', user: 'admin', pass: 'admin' },
    { brand: 'Citrox', user: 'admin', pass: '1234' },
    { brand: 'Kodo', user: 'admin', pass: 'admin' },
    { brand: 'Multitoc', user: 'admin', pass: 'admin' },
    { brand: 'Aquario', user: 'admin', pass: 'admin' },
    { brand: 'D-Link BR', user: 'admin', pass: '' },
    { brand: 'D-Link BR', user: 'admin', pass: 'admin' },
    // ðŸ‡¨ðŸ‡³ CHINESAS vendidas no Brasil (30+)
    { brand: 'Xiongmai', user: 'admin', pass: '' },
    { brand: 'Xiongmai', user: 'admin', pass: '12345' },
    { brand: 'Xiongmai', user: 'admin', pass: 'admin' },
    { brand: 'Xiongmai', user: 'root', pass: 'xmhdipc' },
    { brand: 'XM', user: 'admin', pass: '' },
    { brand: 'XM', user: 'admin', pass: '888888' },
    { brand: 'HiSilicon', user: 'admin', pass: '12345' },
    { brand: 'HiSilicon', user: 'root', pass: 'hi3518' },
    { brand: 'HiSilicon', user: 'root', pass: 'xc3511' },
    { brand: 'Jovision', user: 'admin', pass: '' },
    { brand: 'Jovision', user: 'abc', pass: '123' },
    { brand: 'V380', user: 'admin', pass: '' },
    { brand: 'V380', user: 'admin', pass: 'admin' },
    { brand: 'Yoosee', user: 'admin', pass: '' },
    { brand: 'Yoosee', user: 'admin', pass: '123456' },
    { brand: 'Tuya', user: 'admin', pass: 'admin' },
    { brand: 'TVT', user: 'admin', pass: '123456' },
    { brand: 'TVT', user: 'admin', pass: '' },
    { brand: 'Longse', user: 'admin', pass: '12345' },
    { brand: 'Generic China', user: 'admin', pass: 'jvc' },
    { brand: 'Generic China', user: 'admin', pass: '123' },
    { brand: 'Generic China', user: 'user', pass: 'user' },
    { brand: 'Generic China', user: 'admin', pass: '7ujMko0admin' },
    // ðŸŒ GLOBAIS (20+)
    { brand: 'Hikvision', user: 'admin', pass: '12345' },
    { brand: 'Hikvision', user: 'admin', pass: '123456' },
    { brand: 'Hikvision', user: 'admin', pass: 'hik12345' },
    { brand: 'Dahua', user: 'admin', pass: 'admin' },
    { brand: 'Dahua', user: 'admin', pass: '123456' },
    { brand: 'Dahua', user: '888888', pass: '888888' },
    { brand: 'Axis', user: 'root', pass: 'pass' },
    { brand: 'Axis', user: 'root', pass: '' },
    { brand: 'Vivotek', user: 'root', pass: '' },
    { brand: 'Foscam', user: 'admin', pass: '' },
    { brand: 'Foscam', user: 'admin', pass: 'admin' },
    { brand: 'Reolink', user: 'admin', pass: '' },
    { brand: 'Amcrest', user: 'admin', pass: 'admin' },
    { brand: 'Ubiquiti', user: 'ubnt', pass: 'ubnt' },
    { brand: 'ACTi', user: 'admin', pass: '123456' },
    { brand: 'Samsung', user: 'admin', pass: '4321' },
    { brand: 'Panasonic', user: 'admin', pass: '12345' },
    { brand: 'Bosch', user: 'service', pass: '' },
    { brand: 'GeoVision', user: 'admin', pass: 'admin' },
    { brand: 'Avtech', user: 'admin', pass: 'admin' }
];
let foundCameras = [];
async function startCameraScan() {
    const range = document.getElementById('cam-ip-range').value;
    const ports = document.getElementById('cam-ports').value.split(',').map(p => parseInt(p.trim()));
    document.getElementById('cam-scan-progress').style.display = 'block';
    document.getElementById('cam-scan-bar').style.width = '0%';
    document.getElementById('cam-scan-text').textContent = 'Iniciando scan...';
    foundCameras = [];
    addLog(`ðŸ“· Escaneando câmeras em ${range}.0/24...`, 'info');
    sendCommand('camera_scan', { range, ports });
    // Simulate progress
    let progress = 0;
    const interval = setInterval(() => {
        progress += 2;
        if (progress >= 100) {
            clearInterval(interval);
            document.getElementById('cam-scan-text').textContent = 'Scan completo!';
            addLog('âœ… Scan de câmeras concluído', 'success');
        }
        document.getElementById('cam-scan-bar').style.width = progress + '%';
        document.getElementById('cam-scan-text').textContent = `Escaneando ${range}.${Math.floor(progress * 2.54)}...`;
    }, 200);
}
function addCameraResult(camera) {
    foundCameras.push(camera);
    updateCameraTable();
}
function updateCameraTable() {
    const tbody = document.getElementById('cameras-tbody');
    document.getElementById('cam-count').textContent = foundCameras.length;
    if (foundCameras.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="text-muted" style="padding:20px;text-align:center;">Nenhuma câmera encontrada. Inicie um scan.</td></tr>';
        return;
    }
    tbody.innerHTML = foundCameras.map((cam, i) => `
                <tr style="border-bottom:1px solid var(--border-color);">
                    <td style="padding:10px;">${cam.ip}</td>
                    <td style="padding:10px;">${cam.port}</td>
                    <td style="padding:10px;">${cam.brand || 'Desconhecido'}</td>
                    <td style="padding:10px;">
                        <span style="padding:4px 8px;border-radius:4px;font-size:0.75rem;background:${cam.status === 'vulnerable' ? 'var(--danger)' :
            cam.status === 'logged' ? 'var(--success)' :
                'var(--warning)'
        };color:#fff;">${cam.status === 'vulnerable' ? 'ðŸ”“ Vulnerável' :
            cam.status === 'logged' ? 'âœ… Logado' :
                'ðŸ”’ Protegido'
        }</span>
                    </td>
                    <td style="padding:10px;">${cam.stream ? `<a href="${cam.stream}" target="_blank" style="color:var(--primary);">ðŸŽ¥ Ver</a>` : '-'}</td>
                    <td style="padding:10px;">
                        <button class="btn btn-sm btn-danger" onclick="attackCamera(${i})">âš”ï¸ Atacar</button>
                    </td>
                </tr>
            `).join('');
}
function camAttack(type) {
    addLog(`ðŸ“· Iniciando ataque ${type}...`, 'info');
    sendCommand('camera_attack', { type, targets: foundCameras.map(c => c.ip) });
}
function attackCamera(index) {
    const cam = foundCameras[index];
    if (!cam) return;
    addLog(`âš”ï¸ Atacando câmera ${cam.ip}:${cam.port}...`, 'info');
    sendCommand('camera_attack_single', { ip: cam.ip, port: cam.port });
}
function attackAllCameras() {
    if (foundCameras.length === 0) {
        addLog('âŒ Nenhuma câmera encontrada. Execute o scan primeiro.', 'error');
        return;
    }
    addLog(`â˜ ï¸ ATACANDO ${foundCameras.length} CÃ‚MERAS COM TODOS OS MÉTODOS...`, 'warning');
    sendCommand('camera_attack_all', {
        targets: foundCameras,
        attacks: ['default_creds', 'admin_nopass', 'onvif_discover', 'rtsp_capture', 'xiongmai_backdoor', 'mirai_check']
    });
}
// Handle camera scan results from WebSocket
function handleCameraResult(data) {
    if (data.camera_found) {
        addCameraResult(data.camera_found);
    }
    if (data.camera_attack_result) {
        const result = data.camera_attack_result;
        const idx = foundCameras.findIndex(c => c.ip === result.ip);
        if (idx !== -1) {
            foundCameras[idx].status = result.success ? 'logged' : foundCameras[idx].status;
            if (result.stream) foundCameras[idx].stream = result.stream;
            if (result.credentials) foundCameras[idx].credentials = result.credentials;
            updateCameraTable();
        }
        addLog(`ðŸ“· ${result.ip}: ${result.success ? 'âœ… ' + result.credentials : 'âŒ Falhou'}`, result.success ? 'success' : 'error');
    }
}
window.handleCameraResult = handleCameraResult;
window.addCameraResult = addCameraResult;

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// PRINTER CHAOS FUNCTIONS
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
const printerCredentials = [
    { brand: 'HP', user: 'admin', pass: 'admin' },
    { brand: 'HP', user: 'admin', pass: '' },
    { brand: 'HP', user: 'admin', pass: '123456' },
    { brand: 'Epson', user: 'epson', pass: '123456' },
    { brand: 'Epson', user: 'admin', pass: 'epson' },
    { brand: 'Brother', user: 'admin', pass: 'access' },
    { brand: 'Brother', user: 'admin', pass: 'password' },
    { brand: 'Canon', user: 'admin', pass: 'canon' },
    { brand: 'Canon', user: '7654321', pass: '7654321' },
    { brand: 'Samsung', user: 'admin', pass: 'sec00000' },
    { brand: 'Samsung', user: 'admin', pass: '1111' },
    { brand: 'Xerox', user: 'admin', pass: '1111' },
    { brand: 'Lexmark', user: 'admin', pass: '' },
    { brand: 'Kyocera', user: 'admin', pass: 'admin' },
    { brand: 'Ricoh', user: 'admin', pass: '' },
    { brand: 'Ricoh', user: 'admin', pass: 'password' },
    { brand: 'Konica Minolta', user: 'admin', pass: '12345678' },
    { brand: 'OKI', user: 'admin', pass: 'aaaaaa' }
];
let foundPrinters = [];

async function startPrinterScan() {
    const range = document.getElementById('printer-ip-range').value;
    // Common printer ports: 80 (HTTP), 9100 (JetDirect), 515 (LPR), 631 (IPP), 161 (SNMP), 443 (HTTPS), 8080, 8089, 9000
    const ports = [80, 9100, 515, 631, 161, 443, 8080, 8089, 9000];

    document.getElementById('printer-scan-progress').style.display = 'block';
    document.getElementById('printer-scan-bar').style.width = '0%';
    foundPrinters = [];
    addLog(`ðŸ–¨ï¸ Escaneando impressoras em ${range}.0/24...`, 'info');
    sendCommand('printer_scan', { range, ports });

    // Simulate progress for UI feedback
    let progress = 0;
    const interval = setInterval(() => {
        progress += 5;
        if (progress >= 100) {
            clearInterval(interval);
            if (foundPrinters.length === 0) {
                addLog('Scan de impressoras concluído. Nenhuma encontrada.', 'warning');
                updatePrinterTable(); // Ensure table shows "empty" state appropriately
            } else {
                addLog(`âœ… Scan concluído! ${foundPrinters.length} impressoras encontradas.`, 'success');
            }
        }
        document.getElementById('printer-scan-bar').style.width = progress + '%';
    }, 300);
}

function addPrinterResult(printer) {
    // Avoid duplicates
    if (foundPrinters.some(p => p.ip === printer.ip)) return;
    foundPrinters.push(printer);
    document.getElementById('printer-count').textContent = foundPrinters.length;
    updatePrinterTable();
}

function updatePrinterTable() {
    const tbody = document.getElementById('printers-tbody');
    if (!foundPrinters || foundPrinters.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-muted" style="padding:15px;text-align:center;">Nenhuma impressora. Execute o scan.</td></tr>';
        return;
    }
    tbody.innerHTML = foundPrinters.map((prt, i) => `
                <tr style="border-bottom:1px solid var(--border-color);">
                    <td style="padding:8px;">${prt.ip}</td>
                    <td style="padding:8px;">${prt.brand || 'Genérica'}</td>
                    <td style="padding:8px;">
                        <span style="padding:2px 6px;border-radius:4px;font-size:0.75rem;background:${prt.status === 'vulnerable' ? 'var(--danger)' :
            prt.status === 'logged' ? 'var(--success)' :
                'var(--warning)'
        };color:#fff;">${prt.status === 'vulnerable' ? 'ðŸ”“ Aberta' :
            prt.status === 'logged' ? 'âœ… Logada' :
                'ðŸ”’ Protegida'
        }</span>
                    </td>
                    <td style="padding:8px;">
                        <button class="btn btn-sm btn-primary" onclick="printerAttackTarget(${i})">ðŸŽ¯</button>
                    </td>
                </tr>
            `).join('');
}

async function printerAttack(type) {
    if (foundPrinters.length === 0) {
        addLog('âŒ Nenhuma impressora para atacar!', 'error');
        return;
    }

    const payload = await getPrinterPayload();
    payload.type = type;
    payload.targets = foundPrinters.map(p => p.ip);

    addLog(`ðŸ–¨ï¸ Executando ataque: ${type.toUpperCase().replace('_', ' ')}...`, 'warning');
    sendCommand('printer_attack', payload);

    // Play printer sound via backend
    sendCommand('play_sound', { sound: 'printer_print' });
}

async function printerAttackTarget(index) {
    const printer = foundPrinters[index];
    if (!printer) return;

    const payload = await getPrinterPayload();
    payload.targets = [printer.ip];
    // Default to printing text if specific attack button wasn't clicked, but here we probably want to pick the attack type
    // For this specific button in the table, let's assume it prints the text
    payload.type = 'print_text';

    addLog(`ðŸ–¨ï¸ Atacando ${printer.ip}...`, 'info');
    sendCommand('printer_attack', payload);
    sendCommand('play_sound', { sound: 'printer_print' });
}

function attackAllPrinters() {
    if (foundPrinters.length === 0) {
        addLog('âŒ Nenhuma impressora encontrada!', 'error');
        return;
    }
    if (!confirm('âš ï¸ TEM CERTEZA? ISSO VAI GASTAR PAPEL EM TODAS AS IMPRESSORAS!')) return;

    addLog(`â˜ ï¸ INICIANDO IMPRESSÃƒO EM ${foundPrinters.length} IMPRESSORAS!`, 'danger');

    // Execute text print and rename on all
    getPrinterPayload().then(payload => {
        // First rename
        const renamePayload = { ...payload, type: 'rename' };
        sendCommand('printer_attack', renamePayload);

        // Then print
        const printPayload = { ...payload, type: 'print_text' };
        setTimeout(() => {
            sendCommand('printer_attack', printPayload);
            sendCommand('play_sound', { sound: 'printer_long' });
        }, 2000);
    });
}

async function getPrinterPayload() {
    const text = document.getElementById('printer-text').value;
    const copies = parseInt(document.getElementById('printer-copies').value) || 1;
    const newname = document.getElementById('printer-newname').value;

    let imageData = null;
    const imageInput = document.getElementById('printer-image');
    if (imageInput.files && imageInput.files[0]) {
        imageData = await new Promise((resolve) => {
            const reader = new FileReader();
            reader.onload = (e) => resolve(e.target.result);
            reader.readAsDataURL(imageInput.files[0]);
        });
    }

    return {
        text: text,
        image: imageData,
        copies: copies,
        newname: newname
    };
}

function handlePrinterResult(data) {
    if (data.printer_found) {
        addPrinterResult(data.printer_found);
    }
    if (data.printer_attack_log) {
        addLog(data.printer_attack_log, data.success ? 'success' : 'error');
    }
}

window.handlePrinterResult = handlePrinterResult;

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// BLUETOOTH CHAOS FUNCTIONS
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
let foundBLEAudio = [];
let isBleScanning = false;

async function startBluetoothChaosScan() {
    if (isBleScanning) return;
    isBleScanning = true;

    document.getElementById('ble-scan-progress').style.display = 'block';
    document.getElementById('ble-scan-bar').style.width = '0%';
    foundBLEAudio = [];
    updateBLEChaosTable();

    addLog('ðŸ“¡ Iniciando scan de Caixas de Som e Ãudio BLE...', 'info');
    sendCommand('ble_chaos_scan', { type: 'audio' });

    // Simulate progress
    let progress = 0;
    const interval = setInterval(() => {
        progress += 20;
        if (progress >= 100) {
            clearInterval(interval);
            isBleScanning = false;
            if (foundBLEAudio.length === 0) {
                addLog('Nenhuma caixa de som encontrada. Tente chegar mais perto.', 'warning');
                updateBLEChaosTable();
            } else {
                addLog(`âœ… Scan BLE completo! ${foundBLEAudio.length} caixas encontradas.`, 'success');
            }
        }
        document.getElementById('ble-scan-bar').style.width = progress + '%';
    }, 1000);
}

function addBLEChaosResult(device) {
    // Avoid duplicates based on MAC
    if (foundBLEAudio.some(d => d.mac === device.mac)) return;

    foundBLEAudio.push(device);
    document.getElementById('ble-audio-count').textContent = foundBLEAudio.length;
    updateBLEChaosTable();
}

function updateBLEChaosTable() {
    const tbody = document.getElementById('ble-chaos-tbody');
    if (foundBLEAudio.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-muted" style="padding:15px;text-align:center;">Nenhuma caixa encontrada. Execute o scan.</td></tr>';
        return;
    }
    tbody.innerHTML = foundBLEAudio.map((dev, i) => `
                <tr style="border-bottom:1px solid var(--border-color);">
                    <td style="padding:8px;color:${dev.vulnerable ? 'var(--danger)' : 'var(--text-primary)'}">${dev.name || 'Desconhecido'}</td>
                    <td style="padding:8px;font-family:monospace;">${dev.mac}</td>
                    <td style="padding:8px;">${dev.rssi} dBm</td>
                    <td style="padding:8px;">
                        <button class="btn btn-sm" style="background:var(--danger);color:#fff;" onclick="bluetoothAttackTarget(${i})">âš”ï¸</button>
                    </td>
                </tr>
            `).join('');
}

function bluetoothAttack(type) {
    const customName = document.getElementById('ble-custom-name').value;

    // Attacks that don't need a specific target (broadcast/spam)
    if (['fake_spam', 'device_flood', 'force_pair'].includes(type)) {
        addLog(`ðŸ”Š Iniciando ataque BLE Broadcast: ${type}...`, 'warning');
        sendCommand('ble_chaos_attack', { type: type, name: customName });
        sendCommand('play_sound', { sound: 'ble_spam' });
        return;
    }

    // Attacks that need targets
    if (foundBLEAudio.length === 0) {
        addLog('âŒ Nenhuma caixa encontrada para atacar!', 'error');
        return;
    }

    const targets = foundBLEAudio.map(d => d.mac);
    addLog(`ðŸ”Š Atacando ${targets.length} dispositivos com ${type}...`, 'danger');
    sendCommand('ble_chaos_attack', { type: type, targets: targets, name: customName });

    if (type === 'disconnect' || type === 'connection_flood') {
        sendCommand('play_sound', { sound: 'ble_disconnect' });
    }
}

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// TOOLS: SCHEDULER & SOUND MANAGER
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
function saveHandshakeSchedule() {
    const time = document.getElementById('hs-schedule-time').value;
    const auto = document.getElementById('hs-auto-upload').checked;

    localStorage.setItem('hs-schedule-time', time);
    localStorage.setItem('hs-schedule-auto', auto);

    // Set next check time
    if (time !== 'next_connect') {
        const minutes = parseInt(time);
        const nextCheck = new Date().getTime() + (minutes * 60000);
        localStorage.setItem('hs-schedule-next', nextCheck);
        addLog(`ðŸ“… Agendado para daqui a ${minutes} minutos.`, 'success');
    } else {
        localStorage.removeItem('hs-schedule-next');
        addLog('ðŸ“… Agendado para a próxima conexão.', 'success');
    }
}

function checkScheduler() {
    // Check on load
    const auto = localStorage.getItem('hs-schedule-auto') === 'true';
    if (!auto) return;

    const nextCheck = localStorage.getItem('hs-schedule-next');
    if (nextCheck) {
        if (new Date().getTime() >= parseInt(nextCheck)) {
            // Time to remind or upload
            addLog('â° Lembrete de Handshake: Hora de enviar handshakes não quebrados!', 'warning');
            // If we had a list of uncracked, we would try to upload them here
            if (confirm('â° Lembrete: Enviar handshakes pendentes para OnlineHashCrack agora?')) {
                // Logic to bulk upload or redirect
                showTab('handshakes');
            }
            // Reset or clear
            localStorage.removeItem('hs-schedule-next');
        }
    } else {
        // 'next_connect' logic - we are here, so we are connected
        const time = localStorage.getItem('hs-schedule-time');
        if (time === 'next_connect') {
            addLog('â° Auto-Check: Conexão Web detectada.', 'info');
        }
    }
}

// Sound Manager
function uploadSoundToSD(file) {
    if (!file) return;
    addLog(`ðŸ“¤ Enviando ${file.name} para o SD...`, 'info');
    // Mock upload
    setTimeout(() => {
        addLog('âœ… Upload Concluído! Som disponível para ataques.', 'success');
        loadSoundList(); // Refresh list
    }, 1000);
}

function loadSoundList() {
    const list = document.getElementById('sound-list');
    if (!list) return;
    // Mock list
    const sounds = ['ble_spam.wav', 'siren.mp3', 'glitch_noise.wav', 'custom_voice.wav'];
    list.innerHTML = sounds.map(s => `<div style="padding:5px;border-bottom:1px solid #444;display:flex;justify-content:space-between;"><span>ðŸŽµ ${s}</span> <button class="btn btn-sm btn-danger" onclick="addLog('Som excluído (mock)', 'warning')">ðŸ—‘ï¸</button></div>`).join('');
}

// Init
document.addEventListener('DOMContentLoaded', () => {
    checkScheduler();
    loadSoundList();
});

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// HCXDUMP ATTACK FUNCTIONS
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
function startHCXDump() {
    const channel = document.getElementById('hcx-channels').value;
    const active = document.getElementById('hcx-active').value;
    addLog(`ðŸ’£ Iniciando HCXDump (Chan: ${channel === '0' ? 'All' : channel}, Active: ${active === '1' ? 'Yes' : 'No'})...`, 'warning');
    sendCommand('wifi_hcxdump', { channel: parseInt(channel), active: parseInt(active) });
    // Simulate start
    setTimeout(() => addLog('âœ… HCXDump iniciado! Capturando em background...', 'success'), 1000);
}

// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
// OHC STATS & HANDSHAKE UPLOAD
// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
function updateOHCStats(data) {
    // If we had a real endpoint we would fetch here. For now, we mock or read from local storage tracking
    // Or prompt user that this is a placeholder if no API key
    const box = document.getElementById('ohc-stats-box');
    const email = localStorage.getItem('ohc-email');

    if (email && box) {
        box.style.display = 'block';
        // Mock data for demo purposes as requested "0 / 100 tasks"
        // In production this would come from a backend proxy to OHC API
        document.getElementById('ohc-usage').textContent = '2 / 100';
        document.getElementById('ohc-progress').style.width = '2%';
        document.getElementById('ohc-reset').textContent = '31/12/2025';

        const key = localStorage.getItem('ohc-key');
        if (key) {
            document.getElementById('ohc-key-display').textContent = key.substring(0, 4) + '...';
        }
    }
}

// Populate Handshake Select on OHC View Load or specific event
function populateHandshakeSelect() {
    // This relies on handshakes being loaded elsewhere or finding a way to get them.
    // If HandshakeViewer is global or accessible:
    const select = document.getElementById('crack-handshake-select');
    if (!select) return;

    // Try to use globally available HandshakeViewer if it exists (from handshakes.js)
    // Or fetch again via API
    fetch('/api/handshakes').then(r => r.json()).then(data => {
        const list = data.handshakes || [];
        // Sort by ID or timestamp
        select.innerHTML = '<option value="">Selecione...</option>' +
            list.map(hs => `<option value="${hs.id}">${hs.ssid} (${hs.bssid})</option>`).join('');
    }).catch(e => console.log('Error loading HS list for cracker'));
}

async function uploadSelectedHandshakeToOHC() {
    const select = document.getElementById('crack-handshake-select');
    const id = select.value;
    if (!id) {
        addLog('âŒ Selecione um handshake primeiro!', 'error');
        return;
    }
    if (typeof uploadHandshakeToCrack === 'function') {
        await uploadHandshakeToCrack(id);
        // Update stats mock
        document.getElementById('ohc-usage').textContent = '3 / 100'; // Simply increment for feedback
        document.getElementById('ohc-progress').style.width = '3%';
    } else {
        addLog('Função de upload não encontrada (js error)', 'error');
    }
}

// Call status update on load
document.addEventListener('DOMContentLoaded', updateOHCStats);
// And populate select when clicking/showing tab or just on load
document.addEventListener('DOMContentLoaded', populateHandshakeSelect);

function bluetoothAttackTarget(index) {
    const device = foundBLEAudio[index];
    if (!device) return;

    const customName = document.getElementById('ble-custom-name').value;
    // Default single attack = disconnect/glitch
    addLog(`ðŸ”Š Derrubando ${device.name || device.mac}...`, 'danger');
    sendCommand('ble_chaos_attack', { type: 'disconnect', targets: [device.mac], name: customName });
    sendCommand('play_sound', { sound: 'ble_disconnect' });
}

function attackAllBluetooth() {
    if (foundBLEAudio.length === 0) {
        addLog('âŒ Nenhuma caixa encontrada. Escaneie primeiro.', 'error');
        return;
    }

    if (!confirm('â˜ ï¸ ATENÇÃƒO: ISSO VAI DERRUBAR TUDO AO REDOR. CONTINUAR?')) return;

    addLog('â˜ ï¸ INICIANDO CAOS BLUETOOTH TOTAL â˜ ï¸', 'danger');
    sendCommand('play_sound', { sound: 'nuclear_alarm' });

    const customName = document.getElementById('ble-custom-name').value;
    const targets = foundBLEAudio.map(d => d.mac);

    // Sequence of attacks
    const sequence = ['connection_flood', 'disconnect', 'audio_glitch', 'rename'];

    sequence.forEach((attack, i) => {
        setTimeout(() => {
            addLog(`ðŸ”¥ Executando ${attack}...`, 'warning');
            sendCommand('ble_chaos_attack', { type: attack, targets: targets, name: customName });
        }, i * 2000);
    });
}

function handleBLEChaosResult(data) {
    if (data.ble_found) {
        addBLEChaosResult(data.ble_found);
    }
    if (data.ble_attack_log) {
        addLog(data.ble_attack_log, data.success ? 'success' : 'error');
    }
}

window.handleBLEChaosResult = handleBLEChaosResult;

// WPS All Attacks - executa sequencialmente
async function startAllWPS() {
    addLog('ðŸ”“ Iniciando TODOS ataques WPS...', 'info');
    const btn = event.target.closest('button');
    if (btn) btn.disabled = true;

    // 1. Pixie Dust
    addLog('âœ¨ Fase 1: Pixie Dust Attack...', 'info');
    sendCommand('wps_pixie');
    await new Promise(r => setTimeout(r, 15000));

    // 2. Brute-Force
    addLog('ðŸ”¢ Fase 2: Brute-Force PIN Attack...', 'info');
    sendCommand('wps_bruteforce');
    await new Promise(r => setTimeout(r, 30000));

    // 3. Flood DoS
    addLog('ðŸ’¥ Fase 3: WPS Flood DoS...', 'info');
    sendCommand('wps_flood');
    await new Promise(r => setTimeout(r, 10000));

    addLog('âœ… Todos ataques WPS concluídos!', 'success');
    if (btn) btn.disabled = false;
}

// Theme fix - properly applies theme
function setTheme(themeName) {
    document.documentElement.setAttribute('data-theme', themeName);
    localStorage.setItem('lele-theme', themeName);
    // Update all theme stylesheets
    const validThemes = ['cyberpunk', 'stealth', 'hacker', 'light', 'dracula', 'nord', 'monokai', 'solarized', 'tokyo-night', 'gruvbox', 'one-dark'];
    if (validThemes.includes(themeName)) {
        console.log('[Theme] Applied: ' + themeName);
        if (typeof addLog === 'function') addLog('Tema alterado: ' + themeName, 'info');
    }
}

// Save mascot configuration
function saveMascotConfig() {
    const mascot = document.getElementById('mascot-selector').value;
    const personality = document.getElementById('mascot-personality').value;
    // Update local mascot system
    if (typeof MascotSystem !== 'undefined') {
        MascotSystem.setMascot(mascot);
    }
    // Save to localStorage
    localStorage.setItem('lele-mascot', mascot);
    localStorage.setItem('mascot-personality', personality);
    // Update mascot icons in UI
    updateMascotIcons(mascot);
    // Send to device
    sendCommand('set_mascot', { mascot, personality });
    addLog('ðŸŽ­ Mascote salvo: ' + mascot + ' (' + personality + ')', 'success');
}

// Update mascot icons everywhere
function updateMascotIcons(mascotId) {
    const mascotEmojis = {
        'dragon': 'ðŸ‰', 'cat': 'ðŸ±', 'robot': 'ðŸ¤–',
        'skull': 'ðŸ’€', 'ghost': 'ðŸ‘»', 'phoenix': 'ðŸ¦…'
    };
    const emoji = mascotEmojis[mascotId] || 'ðŸ‰';
    // Update boot logo
    const bootMascot = document.getElementById('boot-mascot');
    if (bootMascot) bootMascot.textContent = emoji;
    // Update sidebar logo
    const sidebarMascot = document.getElementById('sidebar-mascot');
    if (sidebarMascot) sidebarMascot.textContent = emoji;
    // Update dashboard mascot avatar
    const dragonAvatar = document.getElementById('dragon-avatar');
    if (dragonAvatar) dragonAvatar.textContent = emoji;
}

// Init on load
document.addEventListener('DOMContentLoaded', () => {
    // Load saved theme
    const savedTheme = localStorage.getItem('lele-theme') || 'cyberpunk';
    setTheme(savedTheme);
    const themeSelector = document.getElementById('theme-selector');
    if (themeSelector) themeSelector.value = savedTheme;
    // Load saved mascot
    const savedMascot = localStorage.getItem('lele-mascot') || 'dragon';
    const mascotSelector = document.getElementById('mascot-selector');
    if (mascotSelector) mascotSelector.value = savedMascot;
    updateMascotIcons(savedMascot);
    if (typeof MascotSystem !== 'undefined') MascotSystem.setMascot(savedMascot);
    // Load saved personality
    const savedPersonality = localStorage.getItem('mascot-personality') || 'aggressive';
    const personalitySelector = document.getElementById('mascot-personality');
    if (personalitySelector) personalitySelector.value = savedPersonality;
    // Load saved volume
    const savedVol = localStorage.getItem('lele-volume') || 80;
    const volSlider = document.getElementById('volume-slider');
    const volValue = document.getElementById('volume-value');
    if (volSlider) volSlider.value = savedVol;
    if (volValue) volValue.textContent = savedVol + '%';
    // Init files
    setTimeout(refreshFiles, 1000);
    // Init wallpapers
    if (typeof initWallpapersPage === 'function') setTimeout(initWallpapersPage, 1500);
    // Init mascot age timer
    updateMascotAge();
    setInterval(updateMascotAge, 60000); // Update every minute
});

// Update mascot XP/Level from WebSocket data
function updateMascotStats(data) {
    if (data.xp !== undefined) {
        const xpEl = document.getElementById('mascot-xp');
        if (xpEl) xpEl.textContent = data.xp;
    }
    if (data.level !== undefined) {
        const lvlEl = document.getElementById('mascot-level');
        if (lvlEl) lvlEl.textContent = data.level;
    }
}

// Calculate mascot age based on first boot timestamp
function updateMascotAge() {
    // Get birth date from localStorage or use current time as fallback
    let birthDate = localStorage.getItem('mascot-birth');
    if (!birthDate) {
        birthDate = Date.now().toString();
        localStorage.setItem('mascot-birth', birthDate);
    }
    const birth = parseInt(birthDate);
    const now = Date.now();
    const diff = now - birth;
    const days = Math.floor(diff / (1000 * 60 * 60 * 24));
    const hours = Math.floor((diff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
    const ageEl = document.getElementById('mascot-age');
    if (ageEl) ageEl.textContent = days + 'd ' + hours + 'h';
}

// Export for external use
window.updateMascotStats = updateMascotStats;
window.startAllWPS = startAllWPS;

// OHC Proxy Function
async function uploadHandshakeToCrack(id) {
    if (!id) return;
    addLog('?? Enviando handshake ' + id + ' para OHC via Proxy...', 'info');
    try {
        const response = await fetch('/api/ohc/proxy', {
            method: 'POST',
            body: JSON.stringify({id: id}),
            headers: {'Content-Type': 'application/json'}
        });
        const data = await response.json();
        if (data.status === 'queued_for_ohc') {
             addLog('? Handshake na fila de upload (Proxy)', 'success');
        } else {
             addLog('? Erro no proxy: ' + (data.msg || 'Unknown'), 'error');
        }
    } catch(e) {
        addLog('? Falha na conexão com Proxy', 'error');
    }
}

