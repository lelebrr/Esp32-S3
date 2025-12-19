/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Easter Eggs & Secret Features
 * Konami code, hidden modes, etc.
 * ═══════════════════════════════════════════════════════════════════════════
 */

const EasterEggs = {
    konamiSequence: ['ArrowUp', 'ArrowUp', 'ArrowDown', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ArrowLeft', 'ArrowRight', 'b', 'a'],
    currentIndex: 0,
    konamiActive: false,
    secrets: []
};

function initEasterEggs() {
    // Listen for Konami code
    document.addEventListener('keydown', (e) => {
        handleKonamiCode(e);
        handleSecretCodes(e);
    });

    // Secret click patterns
    initSecretClicks();

    console.log('[EasterEggs] Secrets loaded');
}

// ═══════════════════════════════════════════════════════════════════════════
// KONAMI CODE
// ═══════════════════════════════════════════════════════════════════════════
function handleKonamiCode(e) {
    const expected = EasterEggs.konamiSequence[EasterEggs.currentIndex];

    if (e.key.toLowerCase() === expected.toLowerCase()) {
        EasterEggs.currentIndex++;

        // Show progress indicator
        showKonamiProgress(EasterEggs.currentIndex);

        if (EasterEggs.currentIndex === EasterEggs.konamiSequence.length) {
            activateKonamiMode();
            EasterEggs.currentIndex = 0;
        }
    } else {
        EasterEggs.currentIndex = 0;
    }
}

function showKonamiProgress(count) {
    const total = EasterEggs.konamiSequence.length;
    if (count > 2 && count < total) {
        // Show subtle hint
        const hint = document.createElement('div');
        hint.className = 'konami-hint';
        hint.textContent = `${count}/${total}`;
        hint.style.cssText = `
            position: fixed;
            bottom: 20px;
            right: 20px;
            background: var(--primary-dim);
            color: var(--primary);
            padding: 5px 15px;
            border-radius: 20px;
            font-family: var(--font-mono);
            font-size: 12px;
            z-index: 9999;
            animation: fadeOut 1s forwards;
        `;
        document.body.appendChild(hint);
        setTimeout(() => hint.remove(), 1000);
    }
}

function activateKonamiMode() {
    if (EasterEggs.konamiActive) {
        deactivateKonamiMode();
        return;
    }

    EasterEggs.konamiActive = true;
    document.body.classList.add('konami-active');

    // Show notification
    const notification = document.createElement('div');
    notification.className = 'konami-notification';
    notification.innerHTML = `
        <div style="font-size: 48px; margin-bottom: 20px;">🎮</div>
        <div style="font-family: var(--font-display); font-size: 24px; color: var(--primary); margin-bottom: 10px;">
            KONAMI CODE ACTIVATED!
        </div>
        <div style="color: var(--text-secondary);">
            You found a secret! Enjoy the 8-bit vibes.
        </div>
        <button class="btn btn-primary" style="margin-top: 20px;" onclick="this.parentElement.remove()">
            AWESOME!
        </button>
    `;
    document.body.appendChild(notification);

    // Play 8-bit music if available
    play8BitMusic();

    // Add retro effects
    addRetroEffects();

    // Dragon reaction
    dragonSay('POWER UP! 🎮');
    setDragonMood('excited');

    // Log achievement
    addLog('🎮 Konami Code desbloqueado!', 'success');
}

function deactivateKonamiMode() {
    EasterEggs.konamiActive = false;
    document.body.classList.remove('konami-active');

    // Remove retro effects
    const retro = document.querySelector('.retro-overlay');
    if (retro) retro.remove();

    // Stop music
    if (EasterEggs.music) {
        EasterEggs.music.pause();
        EasterEggs.music = null;
    }
}

function addRetroEffects() {
    const overlay = document.createElement('div');
    overlay.className = 'retro-overlay';
    overlay.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        pointer-events: none;
        z-index: 9997;
        background: repeating-linear-gradient(
            0deg,
            transparent,
            transparent 2px,
            rgba(0, 255, 0, 0.03) 2px,
            rgba(0, 255, 0, 0.03) 4px
        );
        animation: retroScan 0.1s linear infinite;
    `;
    document.body.appendChild(overlay);
}

function play8BitMusic() {
    // Create simple 8-bit tune using Web Audio API
    try {
        const audioContext = new (window.AudioContext || window.webkitAudioContext)();

        const notes = [523, 587, 659, 698, 784, 880, 988, 1047]; // C5 to C6
        let time = audioContext.currentTime;

        notes.forEach((freq, i) => {
            const osc = audioContext.createOscillator();
            const gain = audioContext.createGain();

            osc.type = 'square';
            osc.frequency.value = freq;

            gain.gain.value = 0.1;
            gain.gain.exponentialRampToValueAtTime(0.001, time + 0.2 + i * 0.15);

            osc.connect(gain);
            gain.connect(audioContext.destination);

            osc.start(time + i * 0.15);
            osc.stop(time + 0.2 + i * 0.15);
        });
    } catch (e) {
        console.log('[Audio] 8-bit music failed:', e);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SECRET CODES
// ═══════════════════════════════════════════════════════════════════════════
const secretCodes = {
    buffer: '',
    codes: {
        'matrix': activateMatrixMode,
        'leet': activateLeetMode,
        'dragon': activateDragonMode,
        'god': activateGodMode,
        '1337': activateLeetMode,
        'hack': showHackerQuote
    }
};

function handleSecretCodes(e) {
    if (e.key.length === 1) {
        secretCodes.buffer += e.key.toLowerCase();

        // Keep buffer at reasonable length
        if (secretCodes.buffer.length > 20) {
            secretCodes.buffer = secretCodes.buffer.slice(-20);
        }

        // Check for codes
        Object.keys(secretCodes.codes).forEach(code => {
            if (secretCodes.buffer.includes(code)) {
                secretCodes.codes[code]();
                secretCodes.buffer = '';
            }
        });
    }
}

function activateMatrixMode() {
    setTheme('hacker');
    dragonSay('Wake up, Neo...');
    addLog('🕶️ Matrix mode ativado', 'success');
}

function activateLeetMode() {
    dragonSay('1337 H4X0R MODE!');
    document.body.style.fontFamily = "'Share Tech Mono', monospace";
    addLog('1337 mode activated', 'success');
}

function activateDragonMode() {
    setDragonMood('excited');
    dragonSay('DRAGON POWER!!! 🐉🔥');

    // Make dragon bigger temporarily
    const dragon = document.getElementById('dragon-avatar');
    if (dragon) {
        dragon.style.transition = 'transform 0.3s';
        dragon.style.transform = 'scale(2)';
        setTimeout(() => {
            dragon.style.transform = 'scale(1)';
        }, 2000);
    }
}

function activateGodMode() {
    dragonSay('UNLIMITED POWER!');
    addLog('⚡ God mode: Todos os ataques desbloqueados', 'success');

    // Visual effect
    document.body.style.boxShadow = 'inset 0 0 100px gold';
    setTimeout(() => {
        document.body.style.boxShadow = '';
    }, 3000);
}

function showHackerQuote() {
    const quotes = [
        "The quieter you become, the more you can hear.",
        "There is no patch for human stupidity.",
        "In a world of locked doors, the man with the key is king.",
        "We are all connected in the great web.",
        "Behind every elegant hack is an elegant mind."
    ];
    const quote = quotes[Math.floor(Math.random() * quotes.length)];
    dragonSay(quote, 6000);
}

// ═══════════════════════════════════════════════════════════════════════════
// SECRET CLICKS
// ═══════════════════════════════════════════════════════════════════════════
function initSecretClicks() {
    // Triple click on logo for secret menu
    let logoClicks = 0;
    let lastClick = 0;

    const logo = document.querySelector('.brand .logo');
    if (logo) {
        logo.addEventListener('click', () => {
            const now = Date.now();
            if (now - lastClick < 500) {
                logoClicks++;
                if (logoClicks >= 3) {
                    showSecretMenu();
                    logoClicks = 0;
                }
            } else {
                logoClicks = 1;
            }
            lastClick = now;
        });
    }
}

function showSecretMenu() {
    const menu = document.createElement('div');
    menu.className = 'secret-menu';
    menu.innerHTML = `
        <div class="secret-menu-content">
            <h3>🔐 Secret Lab</h3>
            <button onclick="activateMatrixMode(); this.parentElement.parentElement.remove();">Matrix Mode</button>
            <button onclick="toggleNightOps(); this.parentElement.parentElement.remove();">Night Ops</button>
            <button onclick="activateGodMode(); this.parentElement.parentElement.remove();">God Mode</button>
            <button onclick="this.parentElement.parentElement.remove();">Close</button>
        </div>
    `;
    menu.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(0,0,0,0.8);
        display: flex;
        align-items: center;
        justify-content: center;
        z-index: 10001;
    `;
    menu.querySelector('.secret-menu-content').style.cssText = `
        background: var(--bg-card);
        padding: 30px;
        border-radius: var(--border-radius);
        border: 1px solid var(--primary);
        text-align: center;
    `;
    menu.querySelectorAll('button').forEach(btn => {
        btn.style.cssText = `
            display: block;
            width: 100%;
            margin: 10px 0;
            padding: 10px 20px;
            background: var(--primary-dim);
            border: 1px solid var(--primary);
            color: var(--primary);
            border-radius: 5px;
            cursor: pointer;
        `;
    });
    document.body.appendChild(menu);
}

// Export
window.initEasterEggs = initEasterEggs;
window.activateKonamiMode = activateKonamiMode;
