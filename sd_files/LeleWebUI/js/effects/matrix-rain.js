/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Matrix Rain Effect
 * Lightweight canvas-based Matrix digital rain with customizable settings
 * ═══════════════════════════════════════════════════════════════════════════
 */

// Global variables for settings
let matrixSettings = {
    speed: 50,
    bgColor: '#0a0a0f',
    bgOpacity: 0.05,
    textColor: '#00fff5'
};

let matrixInterval = null;

function loadMatrixSettings() {
    // Load settings from localStorage or use defaults
    const savedSpeed = localStorage.getItem('matrixSpeed');
    const savedBgColor = localStorage.getItem('matrixBgColor');
    const savedBgOpacity = localStorage.getItem('matrixBgOpacity');
    const savedTextColor = localStorage.getItem('matrixTextColor');

    if (savedSpeed) matrixSettings.speed = parseInt(savedSpeed);
    if (savedBgColor) matrixSettings.bgColor = savedBgColor;
    if (savedBgOpacity) matrixSettings.bgOpacity = parseFloat(savedBgOpacity);
    if (savedTextColor) matrixSettings.textColor = savedTextColor;

    console.log('[Matrix] Loaded settings:', matrixSettings);
}

function initMatrixRain() {
    // Load settings first
    loadMatrixSettings();

    const canvas = document.getElementById('matrix-canvas');
    if (!canvas) {
        console.log('[Matrix] Canvas not found, creating...');
        const newCanvas = document.createElement('canvas');
        newCanvas.id = 'matrix-canvas';
        const container = document.getElementById('background-effects');
        if (container) {
            container.appendChild(newCanvas);
            initMatrixRain();
        }
        return;
    }

    const ctx = canvas.getContext('2d');

    // Set canvas size
    function resize() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    resize();
    window.addEventListener('resize', resize);

    // Characters to display (mix of Latin and Katakana)
    const chars = 'ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    const charArray = chars.split('');

    const fontSize = 14;
    let columns = Math.floor(canvas.width / fontSize);
    let drops = Array(columns).fill(1);

    let animationId = null;
    let lastDrawTime = 0;
    const fpsInterval = 1000 / 30; // Limit to 30 FPS

    function draw(timestamp) {
        if (!animationId) return; // Stopped

        // Calc elapsed time since last loop
        const elapsed = timestamp - lastDrawTime;

        // If enough time has passed, draw next frame
        if (elapsed > fpsInterval) {
            // Get ready for next frame by setting then=now, but...
            // Also, adjust for fpsInterval not being multiple of 16.67
            lastDrawTime = timestamp - (elapsed % fpsInterval);

            // Semi-transparent background with custom color and opacity
            const rgb = hexToRgb(matrixSettings.bgColor);
            ctx.fillStyle = `rgba(${rgb.r}, ${rgb.g}, ${rgb.b}, ${matrixSettings.bgOpacity})`;
            ctx.fillRect(0, 0, canvas.width, canvas.height);

            ctx.fillStyle = matrixSettings.textColor;
            ctx.font = `${fontSize}px 'Share Tech Mono', monospace`;

            for (let i = 0; i < drops.length; i++) {
                // Random character
                const char = charArray[Math.floor(Math.random() * charArray.length)];

                // Draw character
                ctx.fillText(char, i * fontSize, drops[i] * fontSize);

                // Reset drop to top randomly after reaching bottom
                if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
                    drops[i] = 0;
                }

                drops[i]++;
            }
        }

        // Request next frame
        animationId = requestAnimationFrame(draw);
    }

    // Handle resize
    window.addEventListener('resize', () => {
        columns = Math.floor(canvas.width / fontSize);
        drops = Array(columns).fill(1);
    });

    // Start Animation
    function startAnimation() {
        if (!animationId) {
            lastDrawTime = performance.now();
            animationId = requestAnimationFrame(draw);
        }
    }

    // Stop Animation
    function stopAnimation() {
        if (animationId) {
            cancelAnimationFrame(animationId);
            animationId = null;
        }
    }

    // Visibility API (Battery Saver)
    document.addEventListener('visibilitychange', () => {
        if (document.hidden) {
            console.log('[Matrix] Tab hidden, pausing animation...');
            stopAnimation();
        } else {
            console.log('[Matrix] Tab visible, resuming animation...');
            startAnimation();
        }
    });

    // Clear previous interval if exists (legacy cleanup)
    if (matrixInterval) {
        clearInterval(matrixInterval);
        matrixInterval = null;
    }

    // Initial Start
    startAnimation();

    // Store ID globally if needed for restart
    window.currentMatrixAnimation = { stop: stopAnimation, start: startAnimation };

    console.log('[Matrix] Rain initialized with settings (Optimized: rAF + Visibility)');
}

// Helper function to convert hex to RGB
function hexToRgb(hex) {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : { r: 10, g: 10, b: 15 };
}

// Functions to update settings in real-time (called from Config page)
function updateMatrixSpeed(value) {
    matrixSettings.speed = parseInt(value);
    document.getElementById('matrix-speed-value').textContent = value + 'ms';
    // Restart animation with new speed
    if (matrixInterval) {
        clearInterval(matrixInterval);
        initMatrixRain();
    }
}

function updateMatrixBgColor(value) {
    matrixSettings.bgColor = value;
}

function updateMatrixBgOpacity(value) {
    matrixSettings.bgOpacity = parseInt(value) / 100;
    document.getElementById('matrix-bg-opacity-value').textContent = value + '%';
}

function updateMatrixTextColor(value) {
    matrixSettings.textColor = value;
}

function setMatrixTextColor(color) {
    document.getElementById('matrix-text-color').value = color;
    updateMatrixTextColor(color);
}

function saveMatrixSettings() {
    localStorage.setItem('matrixSpeed', matrixSettings.speed);
    localStorage.setItem('matrixBgColor', matrixSettings.bgColor);
    localStorage.setItem('matrixBgOpacity', matrixSettings.bgOpacity);
    localStorage.setItem('matrixTextColor', matrixSettings.textColor);

    if (typeof addLog === 'function') {
        addLog('💾 Configurações do Matrix salvas', 'success');
    }
    alert('✅ Configurações do Matrix salvas com sucesso!');
}

function resetMatrixSettings() {
    // Reset to defaults
    matrixSettings = {
        speed: 50,
        bgColor: '#0a0a0f',
        bgOpacity: 0.05,
        textColor: '#00fff5'
    };

    // Clear localStorage
    localStorage.removeItem('matrixSpeed');
    localStorage.removeItem('matrixBgColor');
    localStorage.removeItem('matrixBgOpacity');
    localStorage.removeItem('matrixTextColor');

    // Update UI
    document.getElementById('matrix-speed-slider').value = 50;
    document.getElementById('matrix-speed-value').textContent = '50ms';
    document.getElementById('matrix-bg-color').value = '#0a0a0f';
    document.getElementById('matrix-bg-opacity').value = 5;
    document.getElementById('matrix-bg-opacity-value').textContent = '5%';
    document.getElementById('matrix-text-color').value = '#00fff5';

    // Restart animation
    if (matrixInterval) {
        clearInterval(matrixInterval);
        initMatrixRain();
    }

    if (typeof addLog === 'function') {
        addLog('🔄 Configurações do Matrix restauradas', 'info');
    }
    alert('✅ Configurações restauradas para o padrão!');
}

// Export functions
window.initMatrixRain = initMatrixRain;
window.updateMatrixSpeed = updateMatrixSpeed;
window.updateMatrixBgColor = updateMatrixBgColor;
window.updateMatrixBgOpacity = updateMatrixBgOpacity;
window.updateMatrixTextColor = updateMatrixTextColor;
window.setMatrixTextColor = setMatrixTextColor;
window.saveMatrixSettings = saveMatrixSettings;
window.resetMatrixSettings = resetMatrixSettings;

