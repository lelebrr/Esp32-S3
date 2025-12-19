# 🌐 **WEB INTERFACE 3.0: CYBERPUNK 3D DASHBOARD**

**Status:** ✅ **IMPLANTADO E OPERACIONAL**
**Tech Stack:** Three.js r165, WebSocket, Vanilla CSS (Glassmorphism), ESPAsyncWebServer

---

## 🚀 **VISÃO GERAL**

A nova interface web (WebUI 3.0) transforma o ESP32 em um centro de comando futurista, inspirado em "Black Mirror" e interfaces Cyberpunk. Diferente de dashboards web comuns, esta interface utiliza aceleração 3D (via Three.js no cliente) para renderizar um ambiente imersivo sem sobrecarregar o microcontrolador.

### **Principais Recursos:**

- **Visual 3D Imersivo:** Fundo espacial com 2000+ partículas animadas e um holograma procedural do ESP32 girando no centro.
- **Zero-Lag Communication:** Protocolo WebSocket Full-Duplex para atualizações de estado em tempo real (<10ms latência).
- **Glassmorphism UI:** Painéis flutuantes translúcidos com efeitos de blur e bordas neon.
- **Responsividade Total:** Funciona perfeitamente em Desktop e Mobile (Touch friendly).
- **Modo Stealth:** Ocultação de interface e camuflagem de tráfego.

---

## 🏗️ **ARQUITETURA TÉCNICA**

### **Frontend (Client-Side)**

Todo o processamento pesado (renderização 3D, animações) é feito no **navegador do cliente**, liberando o ESP32 para focar nos ataques.

1. **`index.html`**: Estrutura DOM mínima. Carrega o canvas 3D no fundo e sobrepõe a UI em HTML/CSS.
2. **`style.css`**: Define o tema "Cyberpunk". Uso intensivo de variáveis CSS (`--neon-cyan`, `--glass-bg`), animações CSS (`@keyframes scan`) e filtros (`backdrop-filter`).
3. **`script.js`**:
    - **Three.js Engine:** Inicializa a cena, câmera e renderizador WebGL. Cria as partículas e o holograma wireframe.
    - **WebSocket Client:** Conecta em `ws://192.168.4.1/ws`. Gerencia reconexão automática.
    - **Command Dispatcher:** Envia comandos JSON `{'cmd': '...', 'value': '...'}` para o ESP32.

### **Backend (ESP32-Side)**

Otimizado para throughput e baixa latência.

1. **`webInterface.cpp`**:
    - **AsyncWebSocket:** Endpoint `/ws` para comunicação bidirecional.
    - **JSON Parsing:** ArduinoJson V6 para decodificar comandos complexos eficiente.
    - **Status Loop:** Tarefa dedicada que envia telemetria (Bateria, Temp, PPS) a 10Hz.
    - **File Server:** Serve os arquivos estáticos (HTML/JS/CSS) do cartão SD ou LittleFS com Cache-Control agressivo.

---

## 🎮 **FUNCIONALIDADES E COMANDOS**

A interface expõe os seguintes módulos de controle:

### **1. Dashboard (HUD)**

* **Holograma Central:** Visualização 3D do estado do dispositivo.
- **Status Bar:** Bateria, Temperatura, PPS (Pacotes por Segundo) e Uptime.
- **Terminal:** Logs do sistema em tempo real com estilo retro.

### **2. WiFi Attacks (`/view-wifi`)**

* **Scan Networks:** Varredura assíncrona de redes, retornando SSID, BSSID, RSSI e Canal.
- **Deauth All:** Envia pacotes de desautenticação para todos os clientes da rede alvo.
- **Beacon Flood:** Inunda o espectro com APs falsos.
- **Handshake Capture:** Monitora o canal para capturar 4-way handshakes WPA2.
- **NUKE Mode:** Ativa múltiplos vetores de ataque simultaneamente.

### **3. BLE Radar (`/view-ble`)**

* **Radar 3D:** Visualização polar de dispositivos Bluetooth Low Energy próximos.
- **Spam Attacks:** Injeção de pacotes para iOS (Apple Continuity) e Windows (Pairing).

### **4. System Core (`/view-settings`)**

* **Clock Speed:** Slider para Overclock/Underclock dinâmico (80MHz - 240MHz).
- **Stealth Mode:** Desativa LEDs e Serial.
- **Self Destruct:** Comando de emergência para limpar dados sensíveis e reiniciar.

---

## 🔧 **INSTALAÇÃO E USO**

1. **Flash:** Compile e grave o firmware atualizado.
2. **SD Card:** Copie a pasta `embedded_resources/web_interface` para a raiz do Cartão SD, renomeando para `LeleWebUI`.
    - *Nota:* Certifique-se de baixar o `three.min.js` (r165) e colocá-lo na pasta `js/` do SD, pois ele não é incluído no repo para economizar espaço no Git.
3. **Acesso:**
    - Conecte-se ao WiFi `LeleNet` (Senha: `lelenet`).
    - Acesse `http://192.168.4.1` ou `http://lele.local`.
4. **Login:** Usuário `admin`, Senha `lele` (padrão).

---

## ⚠️ **NOTAS DE SEGURANÇA**

- **Autenticação:** A WebUI é protegida por login e token de sessão.
- **Isolamento:** Comandos críticos são validados antes da execução.
- **Zero-Trace:** Em modo Stealth, a interface tenta mascarar sua assinatura de servidor web.

---

**Desenvolvido por Lele Team - 2025**
