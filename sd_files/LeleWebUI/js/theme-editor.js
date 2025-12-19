/**
 * WAVEPWN - Theme Editor (Dica 37)
 * Editor visual de temas
 */

const ThemeEditor = {
    current: {},
    previewTimeout: null
};

function initThemeEditor() {
    loadCurrentTheme();
}

function loadCurrentTheme() {
    const styles = getComputedStyle(document.documentElement);
    ThemeEditor.current = {
        bgPrimary: styles.getPropertyValue('--bg-primary').trim(),
        bgSecondary: styles.getPropertyValue('--bg-secondary').trim(),
        primary: styles.getPropertyValue('--primary').trim(),
        secondary: styles.getPropertyValue('--secondary').trim(),
        accent: styles.getPropertyValue('--accent').trim(),
        success: styles.getPropertyValue('--success').trim(),
        danger: styles.getPropertyValue('--danger').trim(),
        textPrimary: styles.getPropertyValue('--text-primary').trim()
    };
}

function renderThemeEditor(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    const fields = [
        { key: 'bgPrimary', label: 'Fundo Principal', cssVar: '--bg-primary' },
        { key: 'bgSecondary', label: 'Fundo Secundário', cssVar: '--bg-secondary' },
        { key: 'primary', label: 'Cor Primária', cssVar: '--primary' },
        { key: 'secondary', label: 'Cor Secundária', cssVar: '--secondary' },
        { key: 'accent', label: 'Accent', cssVar: '--accent' },
        { key: 'success', label: 'Sucesso', cssVar: '--success' },
        { key: 'danger', label: 'Perigo', cssVar: '--danger' },
        { key: 'textPrimary', label: 'Texto', cssVar: '--text-primary' }
    ];

    container.innerHTML = `
        <div class="theme-editor">
            <h3>🎨 Editor de Tema</h3>
            <div class="color-grid">
                ${fields.map(f => `
                    <div class="color-field">
                        <label>${f.label}</label>
                        <div class="color-input">
                            <input type="color" id="color-${f.key}" value="${ThemeEditor.current[f.key] || '#000000'}"
                                   onchange="updateThemeColor('${f.cssVar}', this.value)">
                            <input type="text" value="${ThemeEditor.current[f.key]}" 
                                   onchange="updateThemeColor('${f.cssVar}', this.value)">
                        </div>
                    </div>
                `).join('')}
            </div>
            <div class="editor-actions">
                <button class="btn btn-secondary" onclick="resetTheme()">↩️ Resetar</button>
                <button class="btn btn-secondary" onclick="exportTheme()">📤 Exportar CSS</button>
                <button class="btn btn-primary" onclick="saveCustomTheme()">💾 Salvar Tema</button>
            </div>
            <div class="theme-presets">
                <h4>Presets Rápidos</h4>
                <div class="preset-grid">
                    <button onclick="applyPreset('cyberpunk')" style="background:linear-gradient(#0a0a0f,#00fff5)">Cyber</button>
                    <button onclick="applyPreset('neon-pink')" style="background:linear-gradient(#0a0a0f,#ff00ff)">Neon</button>
                    <button onclick="applyPreset('forest')" style="background:linear-gradient(#0a0a0f,#00ff00)">Forest</button>
                    <button onclick="applyPreset('blood')" style="background:linear-gradient(#0a0a0f,#ff0000)">Blood</button>
                    <button onclick="applyPreset('gold')" style="background:linear-gradient(#0a0a0f,#ffd700)">Gold</button>
                </div>
            </div>
        </div>
    `;
}

function updateThemeColor(cssVar, value) {
    document.documentElement.style.setProperty(cssVar, value);

    // Save to localStorage for persistence
    const custom = JSON.parse(localStorage.getItem('wavepwn-custom-theme') || '{}');
    custom[cssVar] = value;
    localStorage.setItem('wavepwn-custom-theme', JSON.stringify(custom));
}

function resetTheme() {
    localStorage.removeItem('wavepwn-custom-theme');
    document.documentElement.removeAttribute('style');
    loadCurrentTheme();
    renderThemeEditor('theme-editor-container');
}

function exportTheme() {
    const custom = JSON.parse(localStorage.getItem('wavepwn-custom-theme') || '{}');
    let css = `/* WavePwn Custom Theme - ${new Date().toLocaleDateString()} */\n:root {\n`;

    Object.entries(custom).forEach(([prop, value]) => {
        css += `    ${prop}: ${value};\n`;
    });

    css += '}\n';

    const blob = new Blob([css], { type: 'text/css' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'wavepwn-custom-theme.css';
    a.click();
    URL.revokeObjectURL(url);
}

function saveCustomTheme() {
    const name = prompt('Nome do tema:');
    if (!name) return;

    const custom = JSON.parse(localStorage.getItem('wavepwn-custom-theme') || '{}');
    const themes = JSON.parse(localStorage.getItem('wavepwn-saved-themes') || '[]');
    themes.push({ name, colors: custom, date: Date.now() });
    localStorage.setItem('wavepwn-saved-themes', JSON.stringify(themes));

    if (typeof addLog === 'function') addLog(`Tema "${name}" salvo!`, 'success');
}

function applyPreset(preset) {
    const presets = {
        'cyberpunk': { '--primary': '#00fff5', '--secondary': '#ff00ff', '--accent': '#ffcc00' },
        'neon-pink': { '--primary': '#ff00ff', '--secondary': '#00fff5', '--accent': '#ff00aa' },
        'forest': { '--primary': '#00ff00', '--secondary': '#00cc00', '--accent': '#99ff00' },
        'blood': { '--primary': '#ff0000', '--secondary': '#cc0000', '--accent': '#ff3333' },
        'gold': { '--primary': '#ffd700', '--secondary': '#ffaa00', '--accent': '#ffee00' }
    };

    if (presets[preset]) {
        Object.entries(presets[preset]).forEach(([prop, val]) => updateThemeColor(prop, val));
    }
}

function loadCustomTheme() {
    const custom = JSON.parse(localStorage.getItem('wavepwn-custom-theme') || '{}');
    Object.entries(custom).forEach(([prop, value]) => {
        document.documentElement.style.setProperty(prop, value);
    });
}

// Auto-load on page load
document.addEventListener('DOMContentLoaded', loadCustomTheme);

window.initThemeEditor = initThemeEditor;
window.renderThemeEditor = renderThemeEditor;
window.updateThemeColor = updateThemeColor;
window.resetTheme = resetTheme;
window.exportTheme = exportTheme;
window.saveCustomTheme = saveCustomTheme;
window.applyPreset = applyPreset;
