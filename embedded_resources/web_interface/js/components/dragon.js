/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Dragon Mascot Component
 * Interactive dragon that reacts to user actions
 * ═══════════════════════════════════════════════════════════════════════════
 */

const Dragon = {
    element: null,
    bubble: null,
    mood: 'happy',
    personality: 'aggressive',
    expressions: {
        happy: '🐲',
        excited: '🐉',
        angry: '😤🐲',
        sleepy: '😴🐲',
        hunting: '👀🐲',
        attacking: '🔥🐲',
        evil: '👿🐲',
        confused: '🤔🐲'
    },
    phrases: {
        idle: [
            'Procurando redes...',
            'Farejando handshakes...',
            'Modo caça ativo...',
            'Observando o espectro...',
            'Aguardando comandos...',
            'zzZ... WiFi... zzZ...',
            'Detectando vulnerabilidades...',
            'Scanning 2.4GHz...'
        ],
        happy: [
            'Ótimo dia para hackar! 🔥',
            'Vamos capturar uns handshakes!',
            'Estou animado!',
            'Sinto cheiro de WPA2...'
        ],
        angry: [
            'Esses APs não vão durar...',
            'Hora de destruir!',
            'Deauth TUDO!',
            'Ninguém escapa!'
        ],
        handshake: [
            'HANDSHAKE CAPTURADO! 🎉',
            'Peguei mais um!',
            'Esse foi fácil!',
            'Hashcat vai amar isso!'
        ],
        attack: [
            'Atacando...',
            'Fogo neles! 🔥',
            'Deauth enviado!',
            'Alvo travado!'
        ],
        sleepy: [
            'zzZ...',
            '5 minutos...',
            'Muito cansado...',
            'WiFi dormindo...'
        ]
    },
    blinkTimer: null,
    phraseTimer: null
};

function initDragon() {
    // Check if Dragon is selected
    if (window.MascotSystem && window.MascotSystem.currentMascot !== 'dragon') {
        return; // Abort if another mascot is active
    }

    Dragon.element = document.getElementById('dragon-avatar');
    Dragon.bubble = document.getElementById('dragon-bubble');

    if (!Dragon.element) {
        console.log('[Dragon] Avatar element not found');
        return;
    }

    // Set initial expression
    setDragonMood('happy');

    // Start idle behaviors
    startDragonBlink();
    startIdlePhrases();

    // Eye tracking (follows mouse)
    document.addEventListener('mousemove', (e) => {
        if (!Dragon.element) return;

        const rect = Dragon.element.getBoundingClientRect();
        const centerX = rect.left + rect.width / 2;
        const centerY = rect.top + rect.height / 2;

        const angleX = (e.clientX - centerX) / 50;
        const angleY = (e.clientY - centerY) / 50;

        Dragon.element.style.transform = `
            translateX(${Math.max(-5, Math.min(5, angleX))}px)
            translateY(${Math.max(-5, Math.min(5, angleY))}px)
        `;
    });

    // React to clicks
    Dragon.element.addEventListener('click', () => {
        dragonReact('clicked');
    });

    console.log('[Dragon] Mascot initialized');
}

function setDragonMood(mood) {
    if (window.MascotSystem && window.MascotSystem.currentMascot !== 'dragon') return;
    Dragon.mood = mood;

    if (Dragon.element) {
        Dragon.element.textContent = Dragon.expressions[mood] || Dragon.expressions.happy;
        Dragon.element.setAttribute('data-mood', mood);
    }

    // Update CSS mood class
    const container = document.querySelector('.dragon-container');
    if (container) {
        container.className = 'dragon-container';
        container.classList.add(`mood-${mood}`);
    }
}

function updateDragonMood(mood, text) {
    setDragonMood(mood);
    if (text) {
        dragonSay(text);
    }
}

function dragonSay(text, duration = 4000) {
    if (!Dragon.bubble) return;

    Dragon.bubble.textContent = text;
    Dragon.bubble.classList.add('show');

    clearTimeout(Dragon.bubbleTimeout);
    Dragon.bubbleTimeout = setTimeout(() => {
        Dragon.bubble.classList.remove('show');
    }, duration);
}

function dragonReact(event) {
    switch (event) {
        case 'handshake':
            setDragonMood('excited');
            dragonSay(randomPhrase('handshake'));
            setTimeout(() => setDragonMood('happy'), 3000);
            break;

        case 'attack_start':
            setDragonMood('attacking');
            dragonSay(randomPhrase('attack'));
            break;

        case 'attack_stop':
            setDragonMood('happy');
            dragonSay('Ataque finalizado.');
            break;

        case 'many_networks':
            setDragonMood('excited');
            dragonSay('Muitas redes! 👀');
            break;

        case 'clicked':
            const reactions = [
                { mood: 'happy', text: 'Ei! 👋' },
                { mood: 'confused', text: 'O quê?' },
                { mood: 'angry', text: 'Para de cutucar!' },
                { mood: 'sleepy', text: 'Hmm...' }
            ];
            const r = reactions[Math.floor(Math.random() * reactions.length)];
            setDragonMood(r.mood);
            dragonSay(r.text);
            setTimeout(() => setDragonMood('happy'), 2000);
            break;

        case 'error':
            setDragonMood('confused');
            dragonSay('Algo deu errado...');
            break;

        case 'sleep':
            setDragonMood('sleepy');
            dragonSay(randomPhrase('sleepy'));
            break;
    }
}

function startDragonBlink() {
    clearInterval(Dragon.blinkTimer);

    Dragon.blinkTimer = setInterval(() => {
        if (Dragon.element && Dragon.mood === 'happy') {
            // Quick blink animation
            Dragon.element.style.opacity = '0.3';
            setTimeout(() => {
                Dragon.element.style.opacity = '1';
            }, 100);
        }
    }, 3000 + Math.random() * 4000);
}

function startIdlePhrases() {
    if (window.MascotSystem && window.MascotSystem.currentMascot !== 'dragon') return;
    clearInterval(Dragon.phraseTimer);

    Dragon.phraseTimer = setInterval(() => {
        if (Dragon.mood === 'happy' || Dragon.mood === 'sleepy') {
            const phrases = Dragon.mood === 'sleepy' ? Dragon.phrases.sleepy : Dragon.phrases.idle;
            dragonSay(randomPhrase(Dragon.mood === 'sleepy' ? 'sleepy' : 'idle'));
        }
    }, 10000 + Math.random() * 10000);
}

function randomPhrase(category) {
    const phrases = Dragon.phrases[category] || Dragon.phrases.idle;
    return phrases[Math.floor(Math.random() * phrases.length)];
}

// Personality affects behavior
function setDragonPersonality(personality) {
    Dragon.personality = personality;

    switch (personality) {
        case 'aggressive':
            Dragon.phrases.idle = [
                'Hora de atacar!',
                'Vejo alvos...',
                'Ninguém está seguro!',
                'Destruição iminente!'
            ];
            break;
        case 'sleepy':
            Dragon.phrases.idle = [
                'zzZ...',
                'Tô com sono...',
                '5 minutinhos...',
                'Depois eu ataco...'
            ];
            break;
        case 'evil':
            Dragon.phrases.idle = [
                'Hehehe...',
                'Mwahahaha!',
                'Eles não suspeitam...',
                'Chaos is beautiful!'
            ];
            setDragonMood('evil');
            break;
        default:
            // Normal behavior
            break;
    }
}

// Change dragon name
function setDragonName(name) {
    WavePwn.state.dragon.name = name;
    const nameEl = document.getElementById('dragon-name');
    if (nameEl) nameEl.textContent = name;
}

// Export
window.initDragon = initDragon;
window.setDragonMood = setDragonMood;
window.updateDragonMood = updateDragonMood;
window.dragonSay = dragonSay;
window.dragonReact = dragonReact;
window.setDragonPersonality = setDragonPersonality;
window.setDragonName = setDragonName;
