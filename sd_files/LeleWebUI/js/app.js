/**
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 * WAVEPWN - Main Application Entry Point
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 */

// Global state
const WavePwn = {
    state: {
        connected: false,
        theme: 'cyberpunk',
        soundEnabled: true,
        currentView: 'dashboard',
        stats: {
            uptime: 0,
            battery: 0,
            networks: 0,
            handshakes: 0,
            pmkids: 0,
            bleDevices: 0,
            deauthsSent: 0,
            temperature: 0
        },
        dragon: {
            name: 'Neura',
            mood: 'happy',
            personality: 'aggressive'
        }
    },
    ws: null,
    charts: {},
    sounds: {}
};

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// INITIALIZATION
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
document.addEventListener('DOMContentLoaded', () => {
    console.log('[Lele] Initializing Cyberpunk Command Center...');

    // Show boot animation
    showBootAnimation().then(() => {
        // Initialize all modules
        initTheme();
        initWebSocket();

        // Load Mascot State (Syncs Avatar/Boot/Sidebar)
        if (typeof loadMascotState === 'function') {
            loadMascotState().then(() => {
                // Only init Dragon logic if Dragon is active
                // (initDragon check inside dragon.js handles this, but we force correct order)
                initDragon();
            });
        } else {
            initDragon();
        }

        initEffects();
        initNavigation();
        initEasterEggs();
        initGlitchButtons();
        initCharts();
        initTerminal(); // NEW: Interactive Terminal
        simulateTraffic(); // NEW: Fake traffic for visuals

        // Show initial view
        showView('dashboard');

        console.log('[Lele] Initialization complete!');
    });
});

// ==========================================
// FAULT INJECTION (POCKET GLITCHER)
// ==========================================
function updateFiSettings() {
    const target = document.getElementById('fi-target').value;
    const desc = document.getElementById('fi-desc');
    const delay = document.getElementById('fi-delay');
    const pulse = document.getElementById('fi-pulse');

    // Presets based on research
    if (target === 'switch_rcm') {
        desc.innerText = "Glitch VCore durante bootrom para entrar em RCM.";
        delay.value = 100; pulse.value = 50;
    } else if (target === 'iphone_checkm8') {
        desc.innerText = "Glitch USB stack para Checkm8 exploit (A8-A11).";
        delay.value = 200; pulse.value = 40;
    } else if (target === 'car_keyless') {
        desc.innerText = "Glitch Keeloq/Rolling Code resync.";
        delay.value = 50; pulse.value = 20;
    }
}

function setFiTrigger(type) {
    alert(`Gatilho configurado para: ${type.toUpperCase()}`);
    sendCommand('fi_set_trigger', { type: type });
}

function armFaultInjection() {
    const delay = document.getElementById('fi-delay').value;
    const pulse = document.getElementById('fi-pulse').value; // Corrected: .value was missing

    document.body.classList.add('glitch-active');
    setTimeout(() => document.body.classList.remove('glitch-active'), 500);

    sendCommand('fi_arm', { delay: delay, pulse: pulse });
    alert('⚡ SISTEMA ARMADO! Cuidado com alta tensão.');
}

// ==========================================
// INTERACTIVE TERMINAL
// ==========================================
function initTerminal() {
    const input = document.getElementById('cmd-input');
    const output = document.getElementById('term-output');

    if (!input || !output) return;

    input.addEventListener('keypress', function (e) {
        if (e.key === 'Enter') {
            const cmd = this.value.trim().toLowerCase();
            this.value = '';

            // Echo command
            printTerm(`> ${cmd}`, 'cmd-echo');

            // Process
            processCommand(cmd);
        }
    });

    printTerm('LeleOS v2.0 - Terminal Access Granted', 'system');
    printTerm('Type "help" for verified commands.', 'info');
}

function printTerm(text, type = '') {
    const output = document.getElementById('term-output');
    if (!output) return;

    const line = document.createElement('div');
    line.className = `term-line ${type}`;
    line.textContent = text;
    output.appendChild(line);
    output.scrollTop = output.scrollHeight;
}

function processCommand(cmd) {
    switch (cmd) {
        case 'help':
            printTerm('Available Commands:', 'info');
            printTerm('  scan      - Scan for WiFi networks');
            printTerm('  status    - Show system status');
            printTerm('  clear     - Clear terminal');
            printTerm('  matrix    - Toggle Matrix mode');
            printTerm('  reboot    - Reboot system');
            break;
        case 'clear':
            document.getElementById('term-output').innerHTML = '';
            break;
        case 'scan':
            printTerm('Scanning for targets...', 'warning');
            setTimeout(() => {
                printTerm('[+] 12 targets found.', 'success');
                printTerm('    - NET_ONE (-65dBm) [WPA2]', 'info');
                printTerm('    - SKY_NET (-40dBm) [OPEN]', 'danger');
            }, 1000);
            break;
        case 'status':
            printTerm('CPU: 240MHz | RAM: 52% | BAT: 98%', 'info');
            break;
        case 'matrix':
            toggleMatrixMode();
            printTerm('The Matrix has you...', 'success');
            break;
        case 'reboot':
            printTerm('Rebooting...', 'danger');
            sendCommand('reboot');
            break;
        case 'nuke':
            printTerm('ACCESS DENIED. Are you crazy?', 'danger');
            break;
        default:
            printTerm(`Command not found: ${cmd}`, 'error');
    }
}

// ==========================================
// USB ATTACKS
// ==========================================
function usbPayload(type) {
    const script = document.getElementById('ducky-script-sel').value;

    if (type === 'disk_wipe') {
        if (!confirm('⛔ PERIGO: Isso vai simular formatação. Tem certeza absoluta?')) return;
    }

    sendCommand('usb_payload', {
        type: type,
        script: script
    });
}

function runDuckyScript() {
    const script = document.getElementById('ducky-script-sel').value;
    sendCommand('usb_ducky', { script: script });
}

// ==========================================
// GAMING & AMIIBO
// ==========================================
function loadAmiibo() {
    const amiibo = document.getElementById('amiibo-select').value;
    sendCommand('nfc_amiibo_load', { file: amiibo });
    alert(`🍄 Carregando ${amiibo}... Encoste no Joycon!`);
}

// ==========================================
// NFC ADVANCED ATTACKS
// ==========================================
function nfcAttack(type) {
    if (type.startsWith('pulse_')) {
        if (!confirm('⚡ ALERTA: Pulse Injection usa alta frequência. Pode danificar o leitor ou o ESP32. Continuar?')) return;

        // Visual feedback
        const btn = event.srcElement; // Note: event.target is safer but srcElement works in most
        if (btn) {
            btn.classList.add('glitch-active');
            setTimeout(() => btn.classList.remove('glitch-active'), 1000);
        }
    }

    sendCommand('nfc_attack', { type: type });

    if (type === 'fake_apple') {
        alert('📱 Aproxime do iPhone alvo. Ele mostrará um cartão Apple Pay falso.');
    }
}


// ==========================================
// RF GATES ATTACKS
// ==========================================
function rfAttack(type) {
    const protocol = document.getElementById('rf-protocol').value;

    // Ghost Mode special handling
    if (type === 'ghost_mode') {
        const confirmGhost = confirm('👻 GHOST MODE: Isso vai gravar o próximo sinal e ficar repetindo aleatoriamente. Tem certeza?');
        if (!confirmGhost) return;
    }

    // Jammer Warning
    if (type === 'jam_433') {
        if (!confirm('⚠️ ATENÇÃO: Jamming é ilegal. Use apenas em ambiente controlado. Continuar?')) return;
        sendCommand('rf_jammer', { freq: 433.92 });
        return;
    }

    // Standard commands
    sendCommand('rf_gate_attack', {
        type: type,
        protocol: protocol
    });
}

// ==========================================
// ETHERNET ATTACKS
// ==========================================
function ethAttack(type) {
    const output = document.getElementById('eth-output');
    output.innerHTML += `<div>[>] Iniciando ${type}...</div>`;

    // Send command
    sendCommand('eth_attack', { type: type });

    // Simulate initial feedback
    setTimeout(() => {
        if (type === 'arp_scan') {
            output.innerHTML += `<div>[+] Scan iniciado. Aguardando hosts...</div>`;
        }
    }, 500);
}
// ==========================================
// EASTER EGGS
// ==========================================
function initEasterEggs() {
    let konami = ['ArrowUp', 'ArrowUp', 'ArrowDown', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ArrowLeft', 'ArrowRight', 'b', 'a'];
    let current = 0;

    document.addEventListener('keydown', (e) => {
        if (e.key === konami[current]) {
            current++;
            if (current === konami.length) {
                activateKonamiMode();
                current = 0;
            }
        } else {
            current = 0;
        }
    });
}

function activateKonamiMode() {
    alert('ðŸ‰ KONAMI CODE ACTIVATED! SUPER ADMIN MODE ENABLED ðŸ‰');
    document.body.classList.add('super-admin');
    playSound('kaChng');
    // Unleash all particles
    for (let i = 0; i < 50; i++) {
        createParticle(window.innerWidth / 2, window.innerHeight / 2);
    }
}

// Simulated Particles (Simple version if initParticles is missing or complex)
function createParticle(x, y) {
    const p = document.createElement('div');
    p.style.position = 'fixed';
    p.style.left = x + 'px';
    p.style.top = y + 'px';
    p.style.width = '4px';
    p.style.height = '4px';
    p.style.background = `hsl(${Math.random() * 360}, 100%, 50%)`;
    p.style.borderRadius = '50%';
    p.style.pointerEvents = 'none';
    p.style.zIndex = '9999';
    document.body.appendChild(p);

    const destX = (Math.random() - 0.5) * 500;
    const destY = (Math.random() - 0.5) * 500;

    p.animate([
        { transform: `translate(0,0)`, opacity: 1 },
        { transform: `translate(${destX}px, ${destY}px)`, opacity: 0 }
    ], {
        duration: 1000,
        easing: 'ease-out'
    }).onfinish = () => p.remove();
}

function playSound(name) {
    if (WavePwn && WavePwn.sounds && WavePwn.sounds[name]) {
        WavePwn.sounds[name].currentTime = 0;
        WavePwn.sounds[name].play().catch(e => { });
    }
}


// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// BOOT ANIMATION
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
async function showBootAnimation() {
    return new Promise((resolve) => {
        const overlay = document.getElementById('boot-overlay');
        if (!overlay) {
            resolve();
            return;
        }

        const bootText = overlay.querySelector('.boot-text');
        const bootLines = [
            'Lele SYSTEM INITIALIZING...',
            'LOADING NEURAL NETWORKS...',
            'CONNECTING TO Lele CORE...',
            'ESTABLISHING SECURE CHANNEL...',
            'SYSTEM READY.'
        ];

        let lineIndex = 0;
        const lineInterval = setInterval(() => {
            if (lineIndex < bootLines.length) {
                bootText.innerHTML += bootLines[lineIndex] + '<br>';
                lineIndex++;
            } else {
                clearInterval(lineInterval);
            }
        }, 400);

        // Fade out after boot completes
        setTimeout(() => {
            overlay.classList.add('fade-out');
            setTimeout(() => {
                overlay.style.display = 'none';
                resolve();
            }, 1000);
        }, 2500);
    });
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// THEME SYSTEM
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initTheme() {
    const savedTheme = localStorage.getItem('wavepwn-theme') || 'cyberpunk';
    setTheme(savedTheme);
}

function setTheme(themeName) {
    document.documentElement.setAttribute('data-theme', themeName);
    WavePwn.state.theme = themeName;
    localStorage.setItem('wavepwn-theme', themeName);

    // Update theme selector if exists
    const selector = document.getElementById('theme-selector');
    if (selector) selector.value = themeName;

    console.log(`[Theme] Switched to: ${themeName}`);
}

function toggleNightOps() {
    document.body.classList.toggle('night-ops');

    // Create night ops dot if needed
    if (document.body.classList.contains('night-ops')) {
        if (!document.querySelector('.night-ops-dot')) {
            const dot = document.createElement('div');
            dot.classList.add('night-ops-dot');
            document.body.appendChild(dot);
        }
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// MOBILE SIDEBAR - Definido primeiro para garantir disponibilidade
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â

// Mobile sidebar toggle function (global) - DEVE SER DEFINIDA ANTES DE SER USADA
function toggleSidebar() {
    console.log('[MENU] toggleSidebar chamado');
    const sidebar = document.getElementById('sidebar');
    const overlay = document.querySelector('.sidebar-overlay');

    if (!sidebar) {
        console.error('[MENU] ERRO: elemento #sidebar nÃÂ£o encontrado!');
        return;
    }

    const wasOpen = sidebar.classList.contains('open');
    console.log('[MENU] Estado antes:', wasOpen ? 'ABERTO' : 'FECHADO');

    if (wasOpen) {
        sidebar.classList.remove('open');
        if (overlay) overlay.classList.remove('active');
        console.log('[MENU] Fechando sidebar');
    } else {
        sidebar.classList.add('open');
        if (overlay) overlay.classList.add('active');
        console.log('[MENU] Abrindo sidebar');
    }

    console.log('[MENU] Estado depois:', sidebar.classList.contains('open') ? 'ABERTO' : 'FECHADO');
}

// Fechar sidebar ao clicar em um item do menu (mobile)
function closeSidebarOnNavClick() {
    if (window.innerWidth <= 768) {
        const sidebar = document.getElementById('sidebar');
        const overlay = document.querySelector('.sidebar-overlay');

        if (sidebar) sidebar.classList.remove('open');
        if (overlay) overlay.classList.remove('active');
    }
}

// Expor toggleSidebar globalmente IMEDIATAMENTE
window.toggleSidebar = toggleSidebar;

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// NAVIGATION
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initNavigation() {
    const navLinks = document.querySelectorAll('.nav-links a, .nav-links button');

    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const viewId = link.dataset.view || link.getAttribute('href')?.replace('#', '');
            if (viewId) {
                showView(viewId);

                // Update active state
                navLinks.forEach(l => l.classList.remove('active'));
                link.classList.add('active');

                // Fechar sidebar em mobile apÃÂ³s clicar em um item do menu
                closeSidebarOnNavClick();
            }
        });
    });

    // Mobile sidebar toggle - usar toggleSidebar para consistÃÂªncia
    const menuToggle = document.getElementById('mobile-menu-btn');
    if (menuToggle) {
        menuToggle.addEventListener('click', toggleSidebar);
    }

    // Overlay click para fechar sidebar
    const overlay = document.querySelector('.sidebar-overlay');
    if (overlay) {
        overlay.addEventListener('click', toggleSidebar);
    }
}

// Placeholder for init() and loadMascotState() from the instruction,
// assuming they are methods of a global app object (e.g., WavePwn)
// and the instruction snippet was incomplete or out of context for top-level functions.
// Given the existing structure, these would likely be top-level functions or
// methods of a specific module. For now, I'll place them as top-level functions
// as the instruction implies they are new functions to be added.
// Note: The `this` references in the instruction's `init()` suggest it's a method
// of an object, but without that object's definition, it's hard to place correctly.
// I will define them as standalone functions for syntactic correctness based on the surrounding code.

// Assuming init() and loadMascotState() are part of a larger app object,
// but for this edit, they are placed as standalone functions as per the instruction's context.
// The `this` references in the instruction's `init()` would require a different
// code structure (e.g., a class or an object literal for the main app).
// For now, I'll define the `loadMascotState` function as requested.
// The `init()` function from the instruction seems to be a different initialization
// routine than the existing `DOMContentLoaded` one, and contains `this` references
// that don't fit the current global function pattern. I will only add `loadMascotState`
// as it's a complete, self-contained function.

async function loadMascotState() {
    try {
        const res = await fetch('/api/mascot/current');
        const data = await res.json();
        let mascotId = 'dragon';
        if (data.id === 1) mascotId = 'lele';
        if (data.id === 2) mascotId = 'lisa';

        if (window.MascotSystem) {
            window.MascotSystem.setMascot(mascotId);
            // Force UI update
            // const bootMascot = document.getElementById('boot-mascot'); // Removido: Agora usamos logo.png estÃÂ¡tico
            const sidebarMascot = document.getElementById('sidebar-mascot');
            // if (bootMascot) bootMascot.textContent = window.MascotSystem.mascots[mascotId].emoji;
            if (sidebarMascot) sidebarMascot.textContent = window.MascotSystem.mascots[mascotId].emoji;
        }
    } catch (e) { console.warn("Mascot sync failed", e); }
}


function showView(viewId) {
    // Glitch-scan transition effect (hacker style)
    const transition = document.createElement('div');
    transition.className = 'glitch-scan-transition';
    transition.innerHTML = `
        <div class="scan-line"></div>
        <div class="glitch-bars">
            <div class="glitch-bar"></div>
            <div class="glitch-bar"></div>
            <div class="glitch-bar"></div>
        </div>
    `;
    document.body.appendChild(transition);

    // Phase 1: Glitch scan in
    setTimeout(() => {
        // Hide all views
        document.querySelectorAll('.view').forEach(view => {
            view.classList.remove('active');
        });

        // Show target view
        const targetView = document.getElementById(viewId);
        if (targetView) {
            targetView.classList.add('active');
            WavePwn.state.currentView = viewId;
        }

        // Phase 2: Scan out
        transition.classList.add('scan-out');

        setTimeout(() => {
            transition.remove();
        }, 300);
    }, 250);
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

function showToast(message, type = 'info', duration = 3000) {
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    toast.textContent = message;

    // Add icon based on type
    if (type === 'error') toast.innerHTML = 'âÂÅ ' + message;
    if (type === 'success') toast.innerHTML = 'âÅâ¦ ' + message;
    if (type === 'warning') toast.innerHTML = 'âÅ¡Â Ã¯Â¸Â ' + message;
    if (type === 'info') toast.innerHTML = 'ââÂ¹Ã¯Â¸Â ' + message;

    document.body.appendChild(toast);

    setTimeout(() => {
        toast.classList.add('show');
    }, 100);

    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => toast.remove(), 500);
    }, duration);
}

window.showToast = showToast;

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// GLITCH BUTTON EFFECT
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initGlitchButtons() {
    document.querySelectorAll('.btn, button').forEach(btn => {
        btn.addEventListener('click', function () {
            this.classList.add('glitch-click-active');

            // Play click sound if enabled
            if (WavePwn.state.soundEnabled && WavePwn.sounds.click) {
                WavePwn.sounds.click.play();
            }

            setTimeout(() => {
                this.classList.remove('glitch-click-active');
            }, 300);
        });
    });
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// EFFECTS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initEffects() {
    // Initialize Matrix rain
    initMatrixRain();

    // Initialize floating particles
    initParticles();

    // Add scanlines overlay
    if (!document.querySelector('.scanlines-overlay')) {
        const scanlines = document.createElement('div');
        scanlines.classList.add('scanlines-overlay');
        document.body.appendChild(scanlines);
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// CHARTS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initCharts() {
    // Signal strength chart
    const signalCanvas = document.getElementById('signal-chart');
    if (signalCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.signal = new Chart(signalCanvas, {
            type: 'line',
            data: {
                labels: Array(30).fill(''),
                datasets: [{
                    label: 'Atividade WiFi',
                    data: Array(30).fill(0),
                    borderColor: getComputedStyle(document.documentElement).getPropertyValue('--primary').trim(),
                    backgroundColor: 'rgba(0, 255, 245, 0.1)',
                    tension: 0.4,
                    fill: true,
                    pointRadius: 0
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: { legend: { display: false } },
                scales: {
                    y: {
                        beginAtZero: true,
                        grid: { color: 'rgba(255,255,255,0.05)' },
                        ticks: { color: '#606070' }
                    },
                    x: { display: false }
                },
                animation: { duration: 0 }
            }
        });
    }

    // Channel usage chart
    const channelCanvas = document.getElementById('channel-chart');
    if (channelCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.channel = new Chart(channelCanvas, {
            type: 'bar',
            data: {
                labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13'],
                datasets: [{
                    label: 'APs por Canal',
                    data: Array(13).fill(0),
                    backgroundColor: 'rgba(0, 255, 245, 0.6)',
                    borderColor: '#00fff5',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: { legend: { display: false } },
                scales: {
                    y: {
                        beginAtZero: true,
                        grid: { color: 'rgba(255,255,255,0.05)' },
                        ticks: { color: '#606070' }
                    },
                    x: {
                        grid: { display: false },
                        ticks: { color: '#606070' }
                    }
                }
            }
        });
    }

    // Security type pie chart
    const securityCanvas = document.getElementById('security-chart');
    if (securityCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.security = new Chart(securityCanvas, {
            type: 'doughnut',
            data: {
                labels: ['WPA2', 'WPA3', 'WEP', 'Open'],
                datasets: [{
                    data: [0, 0, 0, 0],
                    backgroundColor: ['#00fff5', '#ff00ff', '#ffcc00', '#ff0055'],
                    borderWidth: 0
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: { color: '#a0a0b0' }
                    }
                }
            }
        });
    }
}

function updateChart(chartName, newValue) {
    const chart = WavePwn.charts[chartName];
    if (!chart) return;

    if (chartName === 'signal') {
        chart.data.datasets[0].data.push(newValue);
        chart.data.datasets[0].data.shift();
        chart.update('none');
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// SOUND EFFECTS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initSounds() {
    // Only load if sounds are enabled and available
    try {
        WavePwn.sounds.kaChng = new Audio('/assets/sounds/ka-ching.mp3');
        WavePwn.sounds.click = new Audio('/assets/sounds/click.mp3');
        WavePwn.sounds.alert = new Audio('/assets/sounds/alert.mp3');
        WavePwn.sounds.ambient = new Audio('/assets/sounds/ambient.mp3');
        WavePwn.sounds.ambient.loop = true;
        WavePwn.sounds.ambient.volume = 0.2;
    } catch (e) {
        console.log('[Sound] Audio not available');
    }
}

function playKaChing() {
    if (WavePwn.state.soundEnabled && WavePwn.sounds.kaChng) {
        WavePwn.sounds.kaChng.currentTime = 0;
        WavePwn.sounds.kaChng.play().catch(() => { });
    }
}

function toggleAmbientSound() {
    if (!WavePwn.sounds.ambient) return;

    if (WavePwn.sounds.ambient.paused) {
        WavePwn.sounds.ambient.play().catch(() => { });
    } else {
        WavePwn.sounds.ambient.pause();
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// UI UPDATE HELPERS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function updateStats(data) {
    // Update internal state
    Object.assign(WavePwn.state.stats, data);

    // Update DOM elements
    updateElement('uptime', formatUptime(data.uptime));
    updateElement('battery', data.battery + '%');
    updateElement('temperature', data.temperature + 'Ã°C');
    updateElement('networks-count', data.networks);
    updateElement('handshakes-count', data.handshakes, true);
    updateElement('pmkid-count', data.pmkids);
    updateElement('ble-count', data.bleDevices);
    updateElement('deauths-count', data.deauthsSent);

    // New system info fields
    updateElement('mac-wifi', data.macWifi);
    updateElement('mac-ble', data.macBle);

    // RAM/Heap info (in KB, display as formatted)
    if (data.heapTotal > 0) {
        const heapUsed = data.heapTotal - data.heapFree;
        updateElement('heap-free', data.heapFree + ' KB');
        updateElement('ram-used', Math.round((heapUsed / data.heapTotal) * 100) + '%');
    }

    // SD Card info
    if (data.sdOnline) {
        const sdFree = data.sdTotal - data.sdUsed;
        updateElement('sd-space', sdFree + ' / ' + data.sdTotal + ' MB');
    } else {
        updateElement('sd-space', 'NÃÂ£o inserido');
    }

    // Update threat indicator
    const threatEl = document.getElementById('threat-level');
    if (threatEl && data.threat) {
        threatEl.textContent = data.threat;
        threatEl.className = `threat-badge threat-${data.threat.toLowerCase()}`;
    }
}

function updateElement(id, value, animate = false) {
    const el = document.getElementById(id);
    if (!el) return;

    const oldValue = el.textContent;
    el.textContent = value;

    // Animate if value changed
    if (animate && oldValue !== String(value)) {
        el.classList.add('counter-update');
        setTimeout(() => el.classList.remove('counter-update'), 300);

        // Play ka-ching for handshakes
        if (id === 'handshakes-count' && parseInt(value) > parseInt(oldValue)) {
            playKaChing();
            el.classList.add('ka-ching');
            setTimeout(() => el.classList.remove('ka-ching'), 1000);
        }
    }
}

function formatUptime(seconds) {
    const h = Math.floor(seconds / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;
    return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
}

function addLog(message, type = 'info') {
    const logContainer = document.getElementById('log-container');
    if (!logContainer) return;

    const logLine = document.createElement('div');
    logLine.classList.add('log-line', `log-${type}`);
    const time = new Date().toLocaleTimeString();
    logLine.innerHTML = `<span class="log-time">[${time}]</span> <span class="log-msg">${message}</span>`;

    logContainer.insertBefore(logLine, logContainer.firstChild);

    // Keep only last 100 logs
    while (logContainer.children.length > 100) {
        logContainer.removeChild(logContainer.lastChild);
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// API COMMANDS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
async function sendCommand(command, params = {}) {
    console.log(`[CMD] Executing: ${command}`, params);

    // Send via WebSocket if connected
    if (WavePwn.ws && WavePwn.ws.readyState === WebSocket.OPEN) {
        WavePwn.ws.send(JSON.stringify({ cmd: command, ...params }));
        addLog(`Comando enviado: ${command}`, 'info');
        console.log(`[WS] Command sent via WebSocket: ${command}`);
        return;
    }

    // Log warning if WebSocket not connected
    console.warn(`[WS] WebSocket not connected (state: ${WavePwn.ws?.readyState ?? 'null'}), using HTTP fallback`);

    // Fallback to HTTP API
    try {
        const response = await fetch(`/api/${command}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(params)
        });

        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }

        const data = await response.json();
        addLog(`${command}: ${data.status || 'OK'}`, 'success');
        console.log(`[API] HTTP response for ${command}:`, data);
        return data;
    } catch (error) {
        addLog(`Erro: ${error.message}`, 'error');
        console.error(`[API] Failed to execute ${command}:`, error);
        throw error;
    }
}

// Attack shortcuts
function startDeauth(target) {
    sendCommand('deauth', { target });
}

function startBeaconFlood() {
    sendCommand('beacon');
}

function startHandshakeCapture() {
    sendCommand('handshake');
}

function stopAllAttacks() {
    sendCommand('stop');
}

function pwnEverything() {
    const btn = document.getElementById('pwn-btn');
    if (!btn) return;

    // Countdown
    let countdown = 60;
    btn.disabled = true;
    btn.textContent = `PWN ATIVO: ${countdown}s`;
    btn.classList.add('active');

    // Start all attacks
    sendCommand('pwn_all');

    const interval = setInterval(() => {
        countdown--;
        btn.textContent = `PWN ATIVO: ${countdown}s`;

        if (countdown <= 0) {
            clearInterval(interval);
            sendCommand('stop');
            btn.disabled = false;
            btn.textContent = 'âËÂ Ã¯Â¸Â PWN EVERYTHING âËÂ Ã¯Â¸Â';
            btn.classList.remove('active');
        }
    }, 1000);
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// ==========================================
// SPA INTEGRATION LOGIC (Remote, Update, Theme)
// ==========================================

// --- REMOTE STREAM ---
let streamInterval = null;
const streamImg = document.getElementById('stream-screen');
const streamCanvas = document.getElementById('stream-overlay');

function toggleRemoteStream(enable) {
    const wrapper = document.getElementById('remote-stream-wrapper');
    const startBtn = document.getElementById('btn-start-stream');

    if (enable) {
        if (startBtn) startBtn.style.display = 'none';
        if (wrapper) wrapper.style.display = 'block';
        startStream();
    } else {
        if (startBtn) startBtn.style.display = 'block';
        if (wrapper) wrapper.style.display = 'none';
        stopStream();
    }
}

function startStream() {
    if (streamInterval) clearInterval(streamInterval);
    const updateFrame = () => {
        const streamImg = document.getElementById('stream-screen');
        const remoteView = document.getElementById('remote-access');
        const streamWrapper = document.getElementById('remote-stream-wrapper');

        if (remoteView && remoteView.classList.contains('active') && streamWrapper && streamWrapper.style.display === 'block') {
            if (streamImg) streamImg.src = '/stream?' + Date.now();
        } else {
            stopStream(); // Auto stop if view hidden
        }
    };

    // Initial call
    if (streamImg) {
        streamImg.onload = () => setTimeout(updateFrame, 100); // 10 FPS target
        streamImg.onerror = () => setTimeout(updateFrame, 1000); // Retry slow on error
        // Trigger first load
        streamImg.src = '/stream?' + Date.now();
    }
}

function stopStream() {
    const streamImg = document.getElementById('stream-screen');
    if (streamImg) {
        streamImg.onload = null;
        streamImg.onerror = null;
        streamImg.src = '';
    }
}

// Touch Handling
if (streamCanvas) {
    streamCanvas.addEventListener('click', (e) => {
        const rect = streamCanvas.getBoundingClientRect();
        // Map click to 320x240 or whatever resolution the ESP32 uses (adjust 368/448 if needed based on original remote.html)
        // Original remote.html used normalized coordinates? No, it used exact pixels. 
        // Assuming standard ESP32-S3 touch resolution.
        const x = Math.floor((e.clientX - rect.left) / rect.width * 320);
        const y = Math.floor((e.clientY - rect.top) / rect.height * 240);

        fetch(`/touch?x=${x}&y=${y}`);

        // Visual feedback
        const ripple = document.createElement('div');
        ripple.style.position = 'absolute';
        ripple.style.left = (e.clientX - rect.left) + 'px';
        ripple.style.top = (e.clientY - rect.top) + 'px';
        ripple.style.width = '20px';
        ripple.style.height = '20px';
        ripple.style.background = 'rgba(0,255,255,0.5)';
        ripple.style.borderRadius = '50%';
        ripple.style.transform = 'translate(-50%, -50%)';
        ripple.style.pointerEvents = 'none';
        ripple.style.animation = 'ripple 0.5s linear'; // Needs CSS for ripple
        streamCanvas.parentElement.appendChild(ripple);
        setTimeout(() => ripple.remove(), 500);
    });
}

function sendTouch(action) {
    // Helper for buttons below stream
    fetch(`/touch_action?action=${action}`);
}

// --- IR REMOTE ---
function changeDeviceType(type) {
    // Hide all remotes
    document.querySelectorAll('.ir-controls').forEach(el => el.style.display = 'none');

    // Show selected
    const selected = document.getElementById(`remote-${type}`);
    if (selected) {
        selected.style.display = 'block';
    } else {
        // Fallback to TV if type not found (e.g. fan/projector generic)
        const tv = document.getElementById('remote-tv');
        if (tv) tv.style.display = 'block';
    }

    console.log("Changed IR device to:", type);
}

function changeBrand(brand) {
    console.log("Changed IR brand to:", brand);
}

function sendIR(cmd) {
    sendCommand('ir', { command: cmd }); // Assuming backend support
}


// --- FIRMWARE UPDATE ---
function uploadFirmware() {
    const fileInput = document.getElementById('ota-file');
    const file = fileInput.files[0];
    if (!file) {
        alert("Selecione um arquivo .bin primeiro!");
        return;
    }

    const formData = new FormData();
    formData.append("update", file);

    const xhr = new XMLHttpRequest();
    const progressBar = document.getElementById('ota-bar');
    const statusDiv = document.getElementById('ota-status');
    const progressWrapper = document.getElementById('ota-progress-wrapper');

    if (progressWrapper) progressWrapper.style.display = 'block';
    if (statusDiv) statusDiv.textContent = "Iniciando upload...";

    xhr.upload.addEventListener("progress", (e) => {
        if (e.lengthComputable && progressBar && statusDiv) {
            const percent = Math.round((e.loaded / e.total) * 100);
            progressBar.style.width = percent + "%";
            statusDiv.textContent = `Enviando: ${percent}%`;
        }
    });

    xhr.addEventListener("load", () => {
        if (xhr.status === 200) {
            if (statusDiv) statusDiv.textContent = "Sucesso! Reiniciando...";
            if (progressBar) progressBar.style.background = "#00ff00";
            setTimeout(() => location.reload(), 10000);
        } else {
            if (statusDiv) statusDiv.textContent = "Erro no upload.";
            if (progressBar) progressBar.style.background = "#ff0000";
        }
    });

    xhr.addEventListener("error", () => {
        if (statusDiv) statusDiv.textContent = "Erro de conexão.";
        if (progressBar) progressBar.style.background = "#ff0000";
    });

    xhr.open("POST", "/update"); // Usually /update endpoint for ESP32 OTA
    xhr.send(formData);
}


// --- THEME EDITOR ---
function updateLiveTheme() {
    const root = document.documentElement;
    const p = document.getElementById('theme-primary').value;
    const s = document.getElementById('theme-secondary').value;
    const bg = document.getElementById('theme-bg').value;
    const txt = document.getElementById('theme-text').value;

    root.style.setProperty('--primary', p);
    root.style.setProperty('--secondary', s);
    root.style.setProperty('--bg-dark', bg);
    root.style.setProperty('--text-main', txt);

    // Update preview
    const mascot = document.getElementById('preview-mascot');
    if (mascot) mascot.style.color = p;
}

function loadThemePreset(preset) {
    const presets = {
        cyberpunk: { p: '#00fff5', s: '#bd00ff', bg: '#0a0b10', t: '#e0e0e0' },
        matrix: { p: '#00ff00', s: '#008800', bg: '#001100', t: '#00ff00' },
        dracula: { p: '#ff79c6', s: '#bd93f9', bg: '#282a36', t: '#f8f8f2' },
        neon: { p: '#ff0099', s: '#ffff00', bg: '#111111', t: '#ffffff' }
    };

    const colors = presets[preset];
    if (colors) {
        const elP = document.getElementById('theme-primary');
        const elS = document.getElementById('theme-secondary');
        const elBg = document.getElementById('theme-bg');
        const elT = document.getElementById('theme-text');

        if (elP) elP.value = colors.p;
        if (elS) elS.value = colors.s;
        if (elBg) elBg.value = colors.bg;
        if (elT) elT.value = colors.t;

        updateLiveTheme();
    }
}

function saveCustomTheme() {
    const theme = {
        primary: document.getElementById('theme-primary').value,
        secondary: document.getElementById('theme-secondary').value,
        bg: document.getElementById('theme-bg').value,
        text: document.getElementById('theme-text').value
    };
    // In a real app, send to backend. For now, save to local storage
    localStorage.setItem('custom_theme', JSON.stringify(theme));
    alert('Tema salvo (Local Storage)!');
}

function resetTheme() {
    loadThemePreset('cyberpunk');
}


// EXPORTS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// Export functions to window for HTML buttons
window.WavePwn = WavePwn;
window.showView = showView;
window.setTheme = setTheme;
window.toggleNightOps = toggleNightOps;
window.toggleSidebar = toggleSidebar;
window.loadView = loadView;
window.sendCommand = sendCommand;
window.rfAttack = rfAttack;
window.ethAttack = ethAttack;
window.usbPayload = usbPayload;
window.runDuckyScript = runDuckyScript;
window.loadAmiibo = loadAmiibo;
window.armFaultInjection = armFaultInjection;
window.setFiTrigger = setFiTrigger;
window.updateFiSettings = updateFiSettings;
window.nfcAttack = nfcAttack;
window.startDeauth = startDeauth;
window.startBeaconFlood = startBeaconFlood;
window.startHandshakeCapture = startHandshakeCapture;
window.stopAllAttacks = stopAllAttacks;
window.pwnEverything = pwnEverything;
window.toggleAmbientSound = toggleAmbientSound;
window.toggleSidebar = toggleSidebar;
window.closeSidebarOnNavClick = closeSidebarOnNavClick;
window.toggleRemoteStream = toggleRemoteStream;
window.uploadFirmware = uploadFirmware;
window.updateLiveTheme = updateLiveTheme;
window.loadThemePreset = loadThemePreset;
window.saveCustomTheme = saveCustomTheme;
window.resetTheme = resetTheme;
window.sendTouch = sendTouch;
window.sendIR = sendIR;
window.changeDeviceType = changeDeviceType;
window.changeBrand = changeBrand;

