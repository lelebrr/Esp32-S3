ï»¿/**
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 * WAVEPWN - Cinema Mode & Advanced Features
 * Fullscreen dragon mode, confetti effects, and more
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 */

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// CINEMA MODE (Dica 13)
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
const CinemaMode = {
    active: false,
    overlay: null,
    statsInterval: null
};

function toggleCinemaMode() {
    if (CinemaMode.active) {
        exitCinemaMode();
    } else {
        enterCinemaMode();
    }
}

function enterCinemaMode() {
    CinemaMode.active = true;

    // Create fullscreen overlay
    CinemaMode.overlay = document.createElement('div');
    CinemaMode.overlay.id = 'cinema-overlay';
    CinemaMode.overlay.innerHTML = `
        <div class="cinema-content">
            <div class="cinema-dragon">°Å¸Ââ°</div>
            <div class="cinema-stats">
                <div class="cinema-stat">
                    <span class="value" id="cinema-handshakes">0</span>
                    <span class="label">Handshakes</span>
                </div>
                <div class="cinema-stat">
                    <span class="value" id="cinema-networks">0</span>
                    <span class="label">Networks</span>
                </div>
                <div class="cinema-stat">
                    <span class="value" id="cinema-deauths">0</span>
                    <span class="label">Deauths</span>
                </div>
            </div>
            <div class="cinema-message" id="cinema-message">Hunting for handshakes...</div>
            <button class="cinema-exit" onclick="exitCinemaMode()">âÅâ¢ Exit Cinema</button>
        </div>
    `;

    // Apply styles
    CinemaMode.overlay.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: linear-gradient(135deg, #0a0a0f 0%, #1a1a24 100%);
        z-index: 10000;
        display: flex;
        align-items: center;
        justify-content: center;
        animation: cinemaFadeIn 0.5s ease-out;
    `;

    document.body.appendChild(CinemaMode.overlay);

    // Request fullscreen
    if (document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen().catch(() => { });
    }

    // Update stats periodically
    CinemaMode.statsInterval = setInterval(updateCinemaStats, 1000);
    updateCinemaStats();

    // Animate dragon messages
    startCinemaMessages();

    console.log('[Cinema] Mode activated');
}

function exitCinemaMode() {
    CinemaMode.active = false;

    if (CinemaMode.overlay) {
        CinemaMode.overlay.remove();
        CinemaMode.overlay = null;
    }

    if (CinemaMode.statsInterval) {
        clearInterval(CinemaMode.statsInterval);
    }

    // Exit fullscreen
    if (document.exitFullscreen) {
        document.exitFullscreen().catch(() => { });
    }

    console.log('[Cinema] Mode deactivated');
}

function updateCinemaStats() {
    if (!CinemaMode.overlay) return;

    const hs = document.getElementById('cinema-handshakes');
    const nets = document.getElementById('cinema-networks');
    const deauths = document.getElementById('cinema-deauths');

    if (hs) hs.textContent = WavePwn.state.stats.handshakes || 0;
    if (nets) nets.textContent = WavePwn.state.stats.networks || 0;
    if (deauths) deauths.textContent = WavePwn.state.stats.deauthsSent || 0;
}

function startCinemaMessages() {
    const messages = [
        'Hunting for handshakes...',
        'Scanning the spectrum...',
        'Detecting vulnerabilities...',
        'Lele is watching...',
        'WiFi chaos mode...',
        'Capturing packets...',
        'Finding weak targets...'
    ];

    let index = 0;
    setInterval(() => {
        if (!CinemaMode.overlay) return;
        const msgEl = document.getElementById('cinema-message');
        if (msgEl) {
            msgEl.style.opacity = '0';
            setTimeout(() => {
                msgEl.textContent = messages[index];
                msgEl.style.opacity = '1';
                index = (index + 1) % messages.length;
            }, 300);
        }
    }, 4000);
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// CONFETTI EFFECT (Dica 16)
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function triggerConfetti(options = {}) {
    const defaults = {
        particleCount: 100,
        spread: 70,
        origin: { y: 0.6 },
        colors: ['#00fff5', '#ff00ff', '#00ff9d', '#ffcc00', '#ff0055']
    };

    const settings = { ...defaults, ...options };

    // Create canvas for confetti
    const canvas = document.createElement('canvas');
    canvas.id = 'confetti-canvas';
    canvas.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        pointer-events: none;
        z-index: 10001;
    `;
    document.body.appendChild(canvas);

    const ctx = canvas.getContext('2d');
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    const particles = [];

    // Create particles
    for (let i = 0; i < settings.particleCount; i++) {
        particles.push({
            x: canvas.width * settings.origin.x || canvas.width / 2,
            y: canvas.height * settings.origin.y,
            vx: (Math.random() - 0.5) * settings.spread * 0.3,
            vy: Math.random() * -15 - 5,
            color: settings.colors[Math.floor(Math.random() * settings.colors.length)],
            size: Math.random() * 10 + 5,
            rotation: Math.random() * 360,
            rotationSpeed: (Math.random() - 0.5) * 10,
            gravity: 0.3 + Math.random() * 0.2,
            opacity: 1
        });
    }

    // Animate
    function animate() {
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        let activeParticles = 0;

        particles.forEach(p => {
            if (p.opacity <= 0) return;

            activeParticles++;

            p.x += p.vx;
            p.vy += p.gravity;
            p.y += p.vy;
            p.rotation += p.rotationSpeed;

            if (p.y > canvas.height) {
                p.opacity -= 0.02;
            }

            ctx.save();
            ctx.translate(p.x, p.y);
            ctx.rotate(p.rotation * Math.PI / 180);
            ctx.globalAlpha = p.opacity;
            ctx.fillStyle = p.color;
            ctx.fillRect(-p.size / 2, -p.size / 2, p.size, p.size / 3);
            ctx.restore();
        });

        if (activeParticles > 0) {
            requestAnimationFrame(animate);
        } else {
            canvas.remove();
        }
    }

    animate();

    // Play celebration sound
    if (WavePwn.state.soundEnabled) {
        playKaChing();
    }
}

// Trigger on handshake capture
function celebrateHandshake() {
    triggerConfetti({
        particleCount: 150,
        spread: 100,
        origin: { x: 0.5, y: 0.5 }
    });

    // Dragon reaction
    if (typeof dragonReact === 'function') {
        dragonReact('handshake');
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// AUTO DARK/LIGHT MODE (Dica 2)
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function initAutoTheme() {
    // Check localStorage for preference
    const savedTheme = localStorage.getItem('wavepwn-theme');
    const savedAutoMode = localStorage.getItem('wavepwn-auto-theme');

    if (savedAutoMode === 'true' || !savedTheme) {
        // Apply system preference
        applySystemTheme();

        // Listen for changes
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
            if (localStorage.getItem('wavepwn-auto-theme') === 'true') {
                applySystemTheme();
            }
        });
    }
}

function applySystemTheme() {
    const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
    const theme = prefersDark ? 'cyberpunk' : 'light';

    document.documentElement.setAttribute('data-theme', theme);
    console.log(`[Theme] Auto-applied: ${theme} (system prefers ${prefersDark ? 'dark' : 'light'})`);
}

function toggleAutoTheme(enabled) {
    localStorage.setItem('wavepwn-auto-theme', enabled ? 'true' : 'false');

    if (enabled) {
        applySystemTheme();
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// DISCORD WEBHOOK (Dica 30)
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
async function sendDiscordNotification(message, type = 'info') {
    const webhookUrl = localStorage.getItem('wavepwn-discord-webhook');

    if (!webhookUrl) {
        console.log('[Discord] No webhook configured');
        return false;
    }

    const colors = {
        'info': 0x00fff5,
        'success': 0x00ff9d,
        'warning': 0xffcc00,
        'danger': 0xff0055,
        'handshake': 0xff00ff
    };

    const embed = {
        title: '°Å¸Ââ° Lele Alert',
        description: message,
        color: colors[type] || colors.info,
        timestamp: new Date().toISOString(),
        footer: {
            text: 'Lele'
        }
    };

    try {
        await fetch(webhookUrl, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ embeds: [embed] })
        });
        console.log('[Discord] Notification sent');
        return true;
    } catch (error) {
        console.error('[Discord] Failed to send:', error);
        return false;
    }
}

function setDiscordWebhook(url) {
    localStorage.setItem('wavepwn-discord-webhook', url);
    addLog('Discord webhook configurado', 'success');
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// CSS FOR CINEMA MODE (injected dynamically)
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function injectCinemaStyles() {
    const style = document.createElement('style');
    style.id = 'cinema-styles';
    style.textContent = `
        @keyframes cinemaFadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }
        
        .cinema-content {
            text-align: center;
            color: #fff;
        }
        
        .cinema-dragon {
            font-size: 150px;
            animation: cinemaDragonPulse 3s ease-in-out infinite;
            filter: drop-shadow(0 0 50px rgba(0, 255, 245, 0.5));
        }
        
        @keyframes cinemaDragonPulse {
            0%, 100% { transform: scale(1) translateY(0); }
            50% { transform: scale(1.05) translateY(-10px); }
        }
        
        .cinema-stats {
            display: flex;
            justify-content: center;
            gap: 60px;
            margin-top: 40px;
        }
        
        .cinema-stat {
            text-align: center;
        }
        
        .cinema-stat .value {
            display: block;
            font-family: 'Orbitron', sans-serif;
            font-size: 4rem;
            font-weight: 800;
            color: #00fff5;
            text-shadow: 0 0 30px rgba(0, 255, 245, 0.5);
        }
        
        .cinema-stat .label {
            font-size: 1rem;
            color: #606070;
            text-transform: uppercase;
            letter-spacing: 0.2em;
        }
        
        .cinema-message {
            margin-top: 40px;
            font-family: 'Share Tech Mono', monospace;
            font-size: 1.2rem;
            color: #ff00ff;
            transition: opacity 0.3s;
        }
        
        .cinema-exit {
            position: fixed;
            top: 20px;
            right: 20px;
            background: rgba(255, 0, 85, 0.3);
            border: 1px solid #ff0055;
            color: #ff0055;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-family: 'Orbitron', sans-serif;
            transition: all 0.3s;
        }
        
        .cinema-exit:hover {
            background: #ff0055;
            color: #fff;
        }
    `;
    document.head.appendChild(style);
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// IR CONTROL WITH LED INDICATOR
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
function flashIRLED() {
    const led = document.getElementById('ir-led');
    if (led) {
        led.style.background = '#ff0000';
        led.style.boxShadow = '0 0 15px #ff0000, 0 0 30px #ff0000, 0 0 45px #ff0000';
        setTimeout(() => {
            led.style.background = '#400';
            led.style.boxShadow = '0 0 5px #400';
        }, 200);
    }
}

function sendIR(command) {
    flashIRLED();
    const device = document.getElementById('ir-device-type')?.value || 'tv';
    const brand = document.getElementById('ir-brand')?.value || 'samsung';

    sendCommand('ir_send', { command, device, brand });
    addLog(`IR: ${command} (${brand})`, 'info');

    // Play click sound
    if (WavePwn.state.soundEnabled) {
        const audio = new Audio('/sounds/click.wav');
        audio.volume = 0.3;
        audio.play().catch(() => { });
    }
}

function loadIRBrand() {
    const device = document.getElementById('ir-device-type')?.value;
    const brandSelect = document.getElementById('ir-brand');
    if (!brandSelect) return;

    const brands = {
        tv: ['LG', 'Samsung', 'Sony', 'Philips', 'Panasonic', 'Toshiba', 'AOC', 'TCL', 'Philco', 'Semp'],
        tvbox: ['Xiaomi Mi Box', 'Amazon Fire TV', 'Roku', 'Apple TV', 'Chromecast', 'MXQ', 'X96', 'H96', 'T95', 'TX3'],
        ac: ['LG', 'Samsung', 'Midea', 'Springer', 'Electrolux', 'Consul', 'Carrier', 'Fujitsu', 'Daikin', 'Gree'],
        led: ['Multilaser', 'Elgin', 'Positivo', 'GenÃÂ©rica'],
        fan: ['Arno', 'Mondial', 'BritÃÂ¢nia', 'Cadence', 'GenÃÂ©rica'],
        soundbar: ['Samsung', 'LG', 'Sony', 'JBL', 'Bose', 'Yamaha', 'Philips', 'TCL'],
        projector: ['Epson', 'BenQ', 'Optoma', 'ViewSonic', 'LG', 'Sony', 'Xiaomi', 'Acer'],
        dvd: ['Samsung', 'LG', 'Sony', 'Philips', 'Panasonic', 'Pioneer'],
        receiver: ['Sky', 'Claro', 'Visionsat', 'Duosat', 'Azamerica'],
        custom: ['Custom']
    };

    brandSelect.innerHTML = '';
    (brands[device] || brands.tv).forEach(b => {
        const opt = document.createElement('option');
        opt.value = b.toLowerCase().replace(/\s+/g, '_');
        opt.textContent = b;
        brandSelect.appendChild(opt);
    });

    // Switch remote layout based on device type
    switchRemoteLayout(device);
}

// Switch remote layout display
function switchRemoteLayout(device) {
    // Map device types to layout IDs
    const layoutMap = {
        tv: 'tv',
        tvbox: 'tv',
        ac: 'ac',
        led: 'led',
        fan: 'fan',
        soundbar: 'soundbar',
        projector: 'projector',
        dvd: 'tv',
        receiver: 'receiver',
        custom: 'tv'
    };

    const targetLayout = layoutMap[device] || 'tv';

    // Hide all layouts
    document.querySelectorAll('.remote-layout').forEach(layout => {
        layout.style.display = 'none';
        layout.style.opacity = '0';
    });

    // Show target layout with animation
    const layout = document.getElementById('remote-layout-' + targetLayout);
    if (layout) {
        layout.style.display = 'block';
        setTimeout(() => {
            layout.style.opacity = '1';
        }, 50);
    }

    console.log('[IR] Switched to layout:', targetLayout);
}

// AC Temperature control
let acTemp = 22;
function adjustACTemp(delta) {
    acTemp = Math.max(16, Math.min(30, acTemp + delta));
    const display = document.getElementById('ac-temp-display');
    if (display) display.textContent = acTemp + 'Ã°C';
    sendIR('ac_temp_' + acTemp);
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// MASCOT WALLPAPER SYSTEM
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
const MascotWallpapers = {};

function uploadMascotWallpaper(file) {
    if (!file) return;

    const mascot = document.getElementById('mascot-selector')?.value || 'dragon';
    const reader = new FileReader();

    reader.onload = (e) => {
        const dataUrl = e.target.result;
        MascotWallpapers[mascot] = dataUrl;
        localStorage.setItem(`mascot-wallpaper-${mascot}`, dataUrl);
        updateMascotWallpaperPreview();
        addLog(`Wallpaper do ${mascot} atualizado!`, 'success');
    };

    reader.readAsDataURL(file);
}

function removeMascotWallpaper() {
    const mascot = document.getElementById('mascot-selector')?.value || 'dragon';
    delete MascotWallpapers[mascot];
    localStorage.removeItem(`mascot-wallpaper-${mascot}`);
    updateMascotWallpaperPreview();
    addLog(`Wallpaper do ${mascot} removido`, 'info');
}

function updateMascotWallpaperPreview() {
    const mascot = document.getElementById('mascot-selector')?.value || 'dragon';
    const preview = document.getElementById('mascot-wallpaper-preview');
    const saved = localStorage.getItem(`mascot-wallpaper-${mascot}`) || MascotWallpapers[mascot];

    if (preview) {
        if (saved) {
            preview.style.backgroundImage = `url(${saved})`;
            preview.style.backgroundSize = 'cover';
            preview.style.backgroundPosition = 'center';
            preview.innerHTML = '';
        } else {
            preview.style.backgroundImage = '';
            preview.innerHTML = '<span style="color:var(--text-muted)">Nenhum wallpaper</span>';
        }
    }

    // Update dashboard mascot box if visible
    const dragonContainer = document.querySelector('.dragon-container');
    if (dragonContainer && saved) {
        dragonContainer.style.backgroundImage = `url(${saved})`;
        dragonContainer.style.backgroundSize = 'cover';
        dragonContainer.style.backgroundPosition = 'center';
        dragonContainer.style.borderRadius = '12px';
    } else if (dragonContainer) {
        dragonContainer.style.backgroundImage = '';
    }
}

function loadMascotWallpapers() {
    ['dragon', 'lele', 'lisa', 'cat', 'robot', 'skull', 'ghost', 'phoenix'].forEach(m => {
        const saved = localStorage.getItem(`mascot-wallpaper-${m}`);
        if (saved) MascotWallpapers[m] = saved;
    });
    updateMascotWallpaperPreview();
}

// Initialize on load
document.addEventListener('DOMContentLoaded', () => {
    injectCinemaStyles();
    initAutoTheme();
    loadMascotWallpapers();

    // Listen for mascot selector changes
    const mascotSelector = document.getElementById('mascot-selector');
    if (mascotSelector) {
        mascotSelector.addEventListener('change', updateMascotWallpaperPreview);
    }
});

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// EXPORTS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
window.toggleCinemaMode = toggleCinemaMode;
window.enterCinemaMode = enterCinemaMode;
window.exitCinemaMode = exitCinemaMode;
window.triggerConfetti = triggerConfetti;
window.celebrateHandshake = celebrateHandshake;
window.initAutoTheme = initAutoTheme;
window.toggleAutoTheme = toggleAutoTheme;
window.sendDiscordNotification = sendDiscordNotification;
window.setDiscordWebhook = setDiscordWebhook;
window.sendIR = sendIR;
window.flashIRLED = flashIRLED;
window.loadIRBrand = loadIRBrand;
window.switchRemoteLayout = switchRemoteLayout;
window.adjustACTemp = adjustACTemp;
window.uploadMascotWallpaper = uploadMascotWallpaper;
window.removeMascotWallpaper = removeMascotWallpaper;
window.updateMascotWallpaperPreview = updateMascotWallpaperPreview;


