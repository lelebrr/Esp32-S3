/**
 * WAVEPWN - Voice Control (Dica 29)
 * Controle por voz usando Web Speech API
 */

const VoiceControl = {
    recognition: null,
    isListening: false,
    commands: {}
};

function initVoiceControl() {
    if (!('webkitSpeechRecognition' in window) && !('SpeechRecognition' in window)) {
        console.log('[Voice] Speech recognition not supported');
        return false;
    }

    const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
    VoiceControl.recognition = new SpeechRecognition();
    VoiceControl.recognition.lang = 'pt-BR';
    VoiceControl.recognition.continuous = true;
    VoiceControl.recognition.interimResults = false;

    VoiceControl.recognition.onresult = handleVoiceResult;
    VoiceControl.recognition.onerror = (e) => console.error('[Voice] Error:', e.error);
    VoiceControl.recognition.onend = () => {
        if (VoiceControl.isListening) VoiceControl.recognition.start();
    };

    registerDefaultCommands();
    console.log('[Voice] Initialized');
    return true;
}

function registerDefaultCommands() {
    VoiceControl.commands = {
        'scan': () => sendCommand('scan'),
        'escanear': () => sendCommand('scan'),
        'parar': () => sendCommand('stop'),
        'stop': () => sendCommand('stop'),
        'deauth': () => sendCommand('deauth'),
        'ataque': () => sendCommand('deauth'),
        'beacon': () => sendCommand('beacon'),
        'status': () => fetch('/api/status').then(r => r.json()).then(d => speak(`${d.networks} redes, ${d.handshakes} handshakes`)),
        'reiniciar': () => { speak('Reiniciando'); sendCommand('reboot'); },
        'tema escuro': () => setTheme('cyberpunk'),
        'tema claro': () => setTheme('light'),
        'modo cinema': () => toggleCinemaMode && toggleCinemaMode(),
        'ajuda': () => speak('Comandos: scan, parar, deauth, beacon, status, reiniciar')
    };
}

function handleVoiceResult(event) {
    const results = event.results;
    const transcript = results[results.length - 1][0].transcript.toLowerCase().trim();
    console.log('[Voice] Heard:', transcript);

    for (const [cmd, action] of Object.entries(VoiceControl.commands)) {
        if (transcript.includes(cmd)) {
            showVoiceIndicator(cmd);
            action();
            return;
        }
    }
}

function startListening() {
    if (!VoiceControl.recognition) return;
    VoiceControl.isListening = true;
    VoiceControl.recognition.start();
    showVoiceIndicator('Ouvindo...');
}

function stopListening() {
    if (!VoiceControl.recognition) return;
    VoiceControl.isListening = false;
    VoiceControl.recognition.stop();
    hideVoiceIndicator();
}

function toggleVoiceControl() {
    if (VoiceControl.isListening) stopListening();
    else startListening();
}

function speak(text) {
    if (!('speechSynthesis' in window)) return;
    const utterance = new SpeechSynthesisUtterance(text);
    utterance.lang = 'pt-BR';
    utterance.rate = 1.1;
    window.speechSynthesis.speak(utterance);
}

function showVoiceIndicator(text) {
    let indicator = document.getElementById('voice-indicator');
    if (!indicator) {
        indicator = document.createElement('div');
        indicator.id = 'voice-indicator';
        indicator.style.cssText = 'position:fixed;bottom:20px;left:50%;transform:translateX(-50%);background:var(--primary);color:#000;padding:10px 20px;border-radius:20px;z-index:9999;font-weight:bold;';
        document.body.appendChild(indicator);
    }
    indicator.textContent = `🎤 ${text}`;
    indicator.style.display = 'block';
}

function hideVoiceIndicator() {
    const el = document.getElementById('voice-indicator');
    if (el) el.style.display = 'none';
}

window.initVoiceControl = initVoiceControl;
window.startListening = startListening;
window.stopListening = stopListening;
window.toggleVoiceControl = toggleVoiceControl;
window.speak = speak;
