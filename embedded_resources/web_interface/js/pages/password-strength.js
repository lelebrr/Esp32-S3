/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Password Strength Tester (Dica 26)
 * Teste de força de senha baseado em zxcvbn
 * ═══════════════════════════════════════════════════════════════════════════
 */

/**
 * Password strength meter using entropy-based scoring
 * Lightweight alternative to zxcvbn for embedded use
 */
const PasswordStrength = {
    // Common passwords to check against
    commonPasswords: [
        '123456', 'password', '12345678', 'qwerty', '123456789',
        '12345', '1234', '111111', '1234567', 'dragon',
        '123123', 'baseball', 'iloveyou', 'trustno1', 'sunshine',
        'master', '123321', 'welcome', 'shadow', 'ashley',
        'michael', '654321', 'passw0rd', 'admin', 'letmein',
        'senha', 'senha123', 'mudar123', 'brasil', '102030'
    ],

    // Character classes
    charsets: {
        lowercase: 'abcdefghijklmnopqrstuvwxyz',
        uppercase: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
        digits: '0123456789',
        symbols: '!@#$%^&*()_+-=[]{}|;:,.<>?/~`\'"\\',
        space: ' '
    }
};

/**
 * Analyze password strength
 */
function analyzePassword(password) {
    if (!password || password.length === 0) {
        return {
            score: 0,
            entropy: 0,
            crackTime: 'instantâneo',
            feedback: ['Digite uma senha para analisar'],
            strength: 'none'
        };
    }

    // Check common passwords
    if (PasswordStrength.commonPasswords.includes(password.toLowerCase())) {
        return {
            score: 0,
            entropy: 0,
            crackTime: 'instantâneo',
            feedback: ['Esta é uma das senhas mais comuns! Nunca use.'],
            strength: 'terrible'
        };
    }

    // Calculate character pool size
    let poolSize = 0;
    const hasLower = /[a-z]/.test(password);
    const hasUpper = /[A-Z]/.test(password);
    const hasDigit = /[0-9]/.test(password);
    const hasSymbol = /[^a-zA-Z0-9]/.test(password);

    if (hasLower) poolSize += 26;
    if (hasUpper) poolSize += 26;
    if (hasDigit) poolSize += 10;
    if (hasSymbol) poolSize += 32;

    // Calculate entropy
    const entropy = password.length * Math.log2(poolSize || 1);

    // Check for patterns
    const patterns = checkPatterns(password);
    const adjustedEntropy = entropy - (patterns.penalty * 5);

    // Calculate crack time (assuming 10 billion guesses/second)
    const guesses = Math.pow(2, Math.max(0, adjustedEntropy));
    const seconds = guesses / 10000000000;
    const crackTime = formatCrackTime(seconds);

    // Generate score (0-4)
    let score;
    if (adjustedEntropy < 20) score = 0;
    else if (adjustedEntropy < 40) score = 1;
    else if (adjustedEntropy < 60) score = 2;
    else if (adjustedEntropy < 80) score = 3;
    else score = 4;

    // Generate feedback
    const feedback = generateFeedback(password, {
        hasLower, hasUpper, hasDigit, hasSymbol,
        length: password.length,
        patterns: patterns.found
    });

    const strengthLabels = ['Muito Fraca', 'Fraca', 'Razoável', 'Forte', 'Muito Forte'];

    return {
        score,
        entropy: Math.round(adjustedEntropy),
        crackTime,
        feedback,
        strength: strengthLabels[score],
        details: {
            length: password.length,
            hasLower,
            hasUpper,
            hasDigit,
            hasSymbol,
            patterns: patterns.found
        }
    };
}

/**
 * Check for common patterns that weaken passwords
 */
function checkPatterns(password) {
    const found = [];
    let penalty = 0;

    // Sequential characters
    const sequential = /(abc|bcd|cde|def|efg|fgh|ghi|hij|ijk|jkl|klm|lmn|mno|nop|opq|pqr|qrs|rst|stu|tuv|uvw|vwx|wxy|xyz|012|123|234|345|456|567|678|789)/i;
    if (sequential.test(password)) {
        found.push('Sequência de caracteres detectada');
        penalty += 2;
    }

    // Repeated characters
    const repeated = /(.)\1{2,}/;
    if (repeated.test(password)) {
        found.push('Caracteres repetidos detectados');
        penalty += 2;
    }

    // Keyboard patterns
    const keyboard = /qwert|asdf|zxcv|1234|!@#\$/i;
    if (keyboard.test(password)) {
        found.push('Padrão de teclado detectado');
        penalty += 3;
    }

    // Only numbers
    if (/^\d+$/.test(password)) {
        found.push('Apenas números');
        penalty += 2;
    }

    // Only letters
    if (/^[a-zA-Z]+$/.test(password)) {
        found.push('Apenas letras');
        penalty += 1;
    }

    // Dates
    const datePattern = /\b(19|20)\d{2}\b|\b\d{1,2}[\/\-]\d{1,2}[\/\-]\d{2,4}\b/;
    if (datePattern.test(password)) {
        found.push('Possível data detectada');
        penalty += 2;
    }

    // L33t speak
    const leet = {
        'a': '4', 'e': '3', 'i': '1', 'o': '0', 's': '5', 't': '7'
    };
    let deleetified = password.toLowerCase();
    Object.entries(leet).forEach(([letter, num]) => {
        deleetified = deleetified.replace(new RegExp(num, 'g'), letter);
    });
    if (PasswordStrength.commonPasswords.includes(deleetified)) {
        found.push('L33t speak de senha comum');
        penalty += 3;
    }

    return { found, penalty };
}

/**
 * Generate helpful feedback
 */
function generateFeedback(password, details) {
    const feedback = [];

    if (details.length < 8) {
        feedback.push('Use pelo menos 8 caracteres');
    } else if (details.length < 12) {
        feedback.push('Considere usar 12+ caracteres');
    }

    if (!details.hasUpper) {
        feedback.push('Adicione letras maiúsculas');
    }

    if (!details.hasLower) {
        feedback.push('Adicione letras minúsculas');
    }

    if (!details.hasDigit) {
        feedback.push('Adicione números');
    }

    if (!details.hasSymbol) {
        feedback.push('Adicione símbolos especiais (!@#$%)');
    }

    if (details.patterns.length > 0) {
        feedback.push(...details.patterns);
    }

    if (feedback.length === 0) {
        feedback.push('Excelente senha! 🔒');
    }

    return feedback;
}

/**
 * Format crack time in human readable format
 */
function formatCrackTime(seconds) {
    if (seconds < 1) return 'instantâneo';
    if (seconds < 60) return `${Math.round(seconds)} segundos`;
    if (seconds < 3600) return `${Math.round(seconds / 60)} minutos`;
    if (seconds < 86400) return `${Math.round(seconds / 3600)} horas`;
    if (seconds < 2592000) return `${Math.round(seconds / 86400)} dias`;
    if (seconds < 31536000) return `${Math.round(seconds / 2592000)} meses`;

    const years = seconds / 31536000;
    if (years < 1000) return `${Math.round(years)} anos`;
    if (years < 1000000) return `${Math.round(years / 1000)} mil anos`;
    if (years < 1000000000) return `${Math.round(years / 1000000)} milhões de anos`;
    return 'bilhões de anos';
}

/**
 * Render password strength meter UI
 */
function renderPasswordMeter(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="password-tester">
            <div class="form-group">
                <label>🔐 Teste sua senha</label>
                <input type="text" id="password-input" placeholder="Digite a senha para analisar"
                       autocomplete="off" oninput="updatePasswordStrength(this.value)">
            </div>
            
            <div class="strength-meter">
                <div class="strength-bar">
                    <div id="strength-fill" class="strength-fill"></div>
                </div>
                <div id="strength-label" class="strength-label">-</div>
            </div>
            
            <div class="strength-details">
                <div class="detail-item">
                    <span class="detail-label">Entropia</span>
                    <span id="entropy-value" class="detail-value">0 bits</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Tempo para quebrar</span>
                    <span id="crack-time" class="detail-value">-</span>
                </div>
            </div>
            
            <div id="strength-feedback" class="strength-feedback"></div>
            
            <div class="password-generator">
                <button class="btn btn-secondary" onclick="generateSecurePassword()">
                    🎲 Gerar Senha Segura
                </button>
                <button class="btn btn-secondary" onclick="generatePassphrase()">
                    📝 Gerar Frase-Senha
                </button>
            </div>
        </div>
    `;

    injectPasswordStyles();
}

/**
 * Update password strength display
 */
function updatePasswordStrength(password) {
    const analysis = analyzePassword(password);

    // Update strength bar
    const fill = document.getElementById('strength-fill');
    if (fill) {
        const width = (analysis.score + 1) * 20;
        fill.style.width = `${width}%`;
        fill.className = `strength-fill score-${analysis.score}`;
    }

    // Update label
    const label = document.getElementById('strength-label');
    if (label) {
        label.textContent = analysis.strength || '-';
        label.className = `strength-label score-${analysis.score}`;
    }

    // Update entropy
    const entropy = document.getElementById('entropy-value');
    if (entropy) {
        entropy.textContent = `${analysis.entropy} bits`;
    }

    // Update crack time
    const crackTime = document.getElementById('crack-time');
    if (crackTime) {
        crackTime.textContent = analysis.crackTime;
    }

    // Update feedback
    const feedback = document.getElementById('strength-feedback');
    if (feedback) {
        feedback.innerHTML = analysis.feedback
            .map(msg => `<div class="feedback-item">• ${msg}</div>`)
            .join('');
    }
}

/**
 * Generate a secure random password
 */
function generateSecurePassword(length = 16) {
    const chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}';
    let password = '';

    // Use crypto API if available
    if (window.crypto && window.crypto.getRandomValues) {
        const array = new Uint32Array(length);
        window.crypto.getRandomValues(array);
        for (let i = 0; i < length; i++) {
            password += chars[array[i] % chars.length];
        }
    } else {
        for (let i = 0; i < length; i++) {
            password += chars[Math.floor(Math.random() * chars.length)];
        }
    }

    // Put in input and analyze
    const input = document.getElementById('password-input');
    if (input) {
        input.value = password;
        updatePasswordStrength(password);
    }

    return password;
}

/**
 * Generate a memorable passphrase
 */
function generatePassphrase(wordCount = 4) {
    const words = [
        'dragao', 'cyber', 'hacker', 'matrix', 'neura', 'pulse', 'wave',
        'shadow', 'ghost', 'ninja', 'storm', 'blade', 'phoenix', 'viper',
        'cobra', 'wolf', 'hawk', 'tiger', 'eagle', 'falcon', 'raven',
        'fire', 'ice', 'thunder', 'laser', 'plasma', 'neon', 'crypto',
        'quantum', 'neural', 'binary', 'digital', 'virtual', 'cyber'
    ];

    const separators = ['-', '_', '.', '@', '#'];

    let passphrase = '';
    for (let i = 0; i < wordCount; i++) {
        const wordIndex = Math.floor(Math.random() * words.length);
        const word = words[wordIndex];

        // Capitalize first letter sometimes
        const capitalized = Math.random() > 0.5
            ? word.charAt(0).toUpperCase() + word.slice(1)
            : word;

        passphrase += capitalized;

        if (i < wordCount - 1) {
            const sepIndex = Math.floor(Math.random() * separators.length);
            passphrase += separators[sepIndex];
        }
    }

    // Add a number at the end
    passphrase += Math.floor(Math.random() * 100);

    const input = document.getElementById('password-input');
    if (input) {
        input.value = passphrase;
        updatePasswordStrength(passphrase);
    }

    return passphrase;
}

/**
 * Inject CSS styles
 */
function injectPasswordStyles() {
    if (document.getElementById('password-styles')) return;

    const style = document.createElement('style');
    style.id = 'password-styles';
    style.textContent = `
        .password-tester {
            max-width: 500px;
        }
        
        .strength-meter {
            display: flex;
            align-items: center;
            gap: 15px;
            margin: 20px 0;
        }
        
        .strength-bar {
            flex: 1;
            height: 8px;
            background: var(--bg-tertiary);
            border-radius: 4px;
            overflow: hidden;
        }
        
        .strength-fill {
            height: 100%;
            width: 0%;
            transition: all 0.3s ease;
            border-radius: 4px;
        }
        
        .strength-fill.score-0 { background: #ff0055; width: 20%; }
        .strength-fill.score-1 { background: #ff6600; width: 40%; }
        .strength-fill.score-2 { background: #ffcc00; width: 60%; }
        .strength-fill.score-3 { background: #99cc00; width: 80%; }
        .strength-fill.score-4 { background: #00ff9d; width: 100%; }
        
        .strength-label {
            min-width: 100px;
            text-align: center;
            font-weight: 600;
            padding: 5px 10px;
            border-radius: 5px;
            font-size: 0.85rem;
        }
        
        .strength-label.score-0 { color: #ff0055; background: rgba(255,0,85,0.1); }
        .strength-label.score-1 { color: #ff6600; background: rgba(255,102,0,0.1); }
        .strength-label.score-2 { color: #ffcc00; background: rgba(255,204,0,0.1); }
        .strength-label.score-3 { color: #99cc00; background: rgba(153,204,0,0.1); }
        .strength-label.score-4 { color: #00ff9d; background: rgba(0,255,157,0.1); }
        
        .strength-details {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .detail-item {
            background: var(--bg-tertiary);
            padding: 12px;
            border-radius: var(--border-radius-sm);
        }
        
        .detail-label {
            display: block;
            font-size: 0.75rem;
            color: var(--text-muted);
            text-transform: uppercase;
            margin-bottom: 5px;
        }
        
        .detail-value {
            color: var(--primary);
            font-family: var(--font-mono);
            font-weight: 600;
        }
        
        .strength-feedback {
            background: var(--bg-card);
            border: 1px solid var(--border-color);
            border-radius: var(--border-radius);
            padding: 15px;
            margin-bottom: 20px;
        }
        
        .feedback-item {
            color: var(--text-secondary);
            padding: 5px 0;
            font-size: 0.9rem;
        }
        
        .password-generator {
            display: flex;
            gap: 10px;
        }
    `;
    document.head.appendChild(style);
}

// Exports
window.analyzePassword = analyzePassword;
window.renderPasswordMeter = renderPasswordMeter;
window.updatePasswordStrength = updatePasswordStrength;
window.generateSecurePassword = generateSecurePassword;
window.generatePassphrase = generatePassphrase;
