ï»¿/**
 * WAVEPWN - 2FA TOTP Authentication (Dica 62)
 * Google Authenticator compatible TOTP
 */

const TOTP = {
    secret: null,
    digits: 6,
    period: 30,
    algorithm: 'SHA1'
};

// Base32 alphabet
const BASE32 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567';

function initTOTP() {
    loadTOTPSecret();
}

function loadTOTPSecret() {
    TOTP.secret = localStorage.getItem('wavepwn-totp-secret');
}

function generateTOTPSecret() {
    const bytes = new Uint8Array(20);
    window.crypto.getRandomValues(bytes);

    let secret = '';
    for (let i = 0; i < bytes.length; i++) {
        secret += BASE32[bytes[i] % 32];
    }

    TOTP.secret = secret;
    localStorage.setItem('wavepwn-totp-secret', secret);
    return secret;
}

async function generateTOTPCode(secret, time) {
    time = time || Math.floor(Date.now() / 1000);
    const counter = Math.floor(time / TOTP.period);

    // Decode base32 secret
    const key = base32Decode(secret || TOTP.secret);
    if (!key) return null;

    // Create counter buffer
    const counterBuffer = new ArrayBuffer(8);
    const view = new DataView(counterBuffer);
    view.setUint32(4, counter, false);

    // Import key for HMAC
    const cryptoKey = await window.crypto.subtle.importKey(
        'raw', key, { name: 'HMAC', hash: 'SHA-1' }, false, ['sign']
    );

    // Generate HMAC
    const signature = await window.crypto.subtle.sign('HMAC', cryptoKey, counterBuffer);
    const hmac = new Uint8Array(signature);

    // Dynamic truncation
    const offset = hmac[hmac.length - 1] & 0x0f;
    const code = ((hmac[offset] & 0x7f) << 24 |
        (hmac[offset + 1] & 0xff) << 16 |
        (hmac[offset + 2] & 0xff) << 8 |
        (hmac[offset + 3] & 0xff)) % Math.pow(10, TOTP.digits);

    return code.toString().padStart(TOTP.digits, '0');
}

function base32Decode(str) {
    str = str.toUpperCase().replace(/[^A-Z2-7]/g, '');
    const bytes = [];
    let buffer = 0, bitsLeft = 0;

    for (let i = 0; i < str.length; i++) {
        const val = BASE32.indexOf(str[i]);
        if (val === -1) continue;

        buffer = (buffer << 5) | val;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            bitsLeft -= 8;
            bytes.push((buffer >> bitsLeft) & 0xff);
        }
    }

    return new Uint8Array(bytes);
}

function generateQRCodeData(secret, label) {
    const issuer = 'Lele';
    const uri = `otpauth://totp/${encodeURIComponent(label || 'Lele')}?secret=${secret}&issuer=${issuer}&algorithm=SHA1&digits=6&period=30`;
    return uri;
}

async function verifyTOTPCode(inputCode) {
    if (!TOTP.secret) return false;

    // Check current and previous/next periods for clock drift
    const now = Math.floor(Date.now() / 1000);
    const windows = [-1, 0, 1];

    for (const w of windows) {
        const code = await generateTOTPCode(TOTP.secret, now + (w * TOTP.period));
        if (code === inputCode) return true;
    }

    return false;
}

function renderTOTPSetup(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    const hasSecret = !!TOTP.secret;

    container.innerHTML = `
        <div class="totp-setup">
            <h3>°Å¸âÂ AutenticaÃÂ§ÃÂ£o 2FA</h3>
            ${hasSecret ? `
                <div class="totp-enabled">
                    <div class="status success">âÅâ 2FA Ativado</div>
                    <div class="form-group">
                        <label>CÃÂ³digo de verificaÃÂ§ÃÂ£o</label>
                        <input type="text" id="totp-verify" maxlength="6" placeholder="000000">
                    </div>
                    <button class="btn btn-primary" onclick="testTOTP()">Verificar</button>
                    <button class="btn btn-danger" onclick="disableTOTP()">Desativar 2FA</button>
                </div>
            ` : `
                <div class="totp-disabled">
                    <p>Proteja seu dashboard com Google Authenticator</p>
                    <button class="btn btn-primary" onclick="enableTOTP()">Ativar 2FA</button>
                </div>
            `}
        </div>
    `;
}

function enableTOTP() {
    const secret = generateTOTPSecret();
    const qrData = generateQRCodeData(secret, 'Lele');

    // Show QR code modal
    const modal = document.createElement('div');
    modal.className = 'totp-modal';
    modal.innerHTML = `
        <div class="totp-modal-content">
            <h3>°Å¸âÂ± Configure o Google Authenticator</h3>
            <p>Escaneie o QR code ou insira a chave manualmente:</p>
            <div id="totp-qr" class="qr-container"></div>
            <div class="secret-key">
                <code>${secret}</code>
                <button class="btn btn-sm" onclick="copySecret('${secret}')">°Å¸ââ¹</button>
            </div>
            <div class="form-group">
                <label>CÃÂ³digo de verificaÃÂ§ÃÂ£o</label>
                <input type="text" id="totp-confirm" maxlength="6" placeholder="000000">
            </div>
            <div class="modal-actions">
                <button class="btn btn-secondary" onclick="cancelTOTP()">Cancelar</button>
                <button class="btn btn-primary" onclick="confirmTOTP()">Confirmar</button>
            </div>
        </div>
    `;
    document.body.appendChild(modal);

    // Generate QR using simple canvas (no external lib)
    generateSimpleQR('totp-qr', qrData);
}

async function confirmTOTP() {
    const input = document.getElementById('totp-confirm');
    if (!input) return;

    const valid = await verifyTOTPCode(input.value.trim());

    if (valid) {
        // Save to backend
        await fetch('/api/security/totp', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ secret: TOTP.secret, enabled: true })
        });

        closeTOTPModal();
        renderTOTPSetup('totp-container');
        showToast('2FA ativado com sucesso!', 'success');
    } else {
        showToast('CÃÂ³digo invÃÂ¡lido', 'error');
    }
}

function cancelTOTP() {
    TOTP.secret = null;
    localStorage.removeItem('wavepwn-totp-secret');
    closeTOTPModal();
}

function disableTOTP() {
    if (!confirm('Desativar autenticaÃÂ§ÃÂ£o 2FA?')) return;
    TOTP.secret = null;
    localStorage.removeItem('wavepwn-totp-secret');
    fetch('/api/security/totp', { method: 'DELETE' });
    renderTOTPSetup('totp-container');
}

async function testTOTP() {
    const input = document.getElementById('totp-verify');
    if (!input) return;

    const valid = await verifyTOTPCode(input.value.trim());
    showToast(valid ? 'CÃÂ³digo vÃÂ¡lido! âÅâ' : 'CÃÂ³digo invÃÂ¡lido âÅâ', valid ? 'success' : 'error');
}

function closeTOTPModal() {
    const modal = document.querySelector('.totp-modal');
    if (modal) modal.remove();
}

function copySecret(secret) {
    navigator.clipboard.writeText(secret);
    showToast('Chave copiada!', 'success');
}

function generateSimpleQR(containerId, data) {
    const container = document.getElementById(containerId);
    if (!container) return;

    // Use Google Charts API for QR code
    const img = document.createElement('img');
    img.src = `https://chart.googleapis.com/chart?chs=200x200&cht=qr&chl=${encodeURIComponent(data)}`;
    img.alt = 'QR Code';
    container.appendChild(img);
}

function showToast(msg, type) {
    if (typeof addLog === 'function') addLog(msg, type);
}

window.initTOTP = initTOTP;
window.enableTOTP = enableTOTP;
window.disableTOTP = disableTOTP;
window.testTOTP = testTOTP;
window.confirmTOTP = confirmTOTP;
window.cancelTOTP = cancelTOTP;
window.renderTOTPSetup = renderTOTPSetup;
window.verifyTOTPCode = verifyTOTPCode;

