// WavePwn Web App
// WavePwn Web App
let trafficChart;
let trafficData = new Array(30).fill(0); // Last 30 seconds

// Init
document.addEventListener("DOMContentLoaded", () => {
    initChart();
    // WaveSocket auto-connects
    setupSocketListeners();
    showView('dashboard');
});

// View Navigation
function showView(viewId) {
    document.querySelectorAll('.view').forEach(el => el.classList.remove('active'));
    document.querySelectorAll('.nav-links li').forEach(el => el.classList.remove('active'));

    document.getElementById(viewId).classList.add('active');
    // Highlight sidebar (approximate matching)
    const navItems = document.querySelectorAll('.nav-links li');
    if (viewId === 'dashboard') navItems[0].classList.add('active');
    else if (viewId === 'attacks') navItems[1]?.classList.add('active'); // Fixed index match
    else if (viewId === 'usb') navItems[2]?.classList.add('active');
    else if (viewId === 'cameras') navItems[3]?.classList.add('active');
    else if (viewId === 'ir-remote') navItems[4]?.classList.add('active');
    else if (viewId === 'remote-access') navItems[5]?.classList.add('active');
    else if (viewId === 'files') navItems[6]?.classList.add('active');
    else if (viewId === 'terminal') navItems[7]?.classList.add('active');
    else if (viewId === 'tools') navItems[8]?.classList.add('active');
    else if (viewId === 'config') navItems[9]?.classList.add('active');
}

// WebSocket Listeners
function setupSocketListeners() {
    ws.connect(); // Start connection

    ws.on('connected', () => updateStatusDisplay(true));
    ws.on('disconnected', () => updateStatusDisplay(false));

    ws.on('message', (msg) => {
        handleMessage(msg);
    });
}

function updateStatusDisplay(connected) {
    const el = document.getElementById('connection-status'); // Fixed ID from index.html (was ws-status)
    // index.html line 78 says id="connection-status".
    if (!el) return;

    if (connected) {
        el.innerHTML = '<span class="dot" style="background-color: var(--primary); box-shadow: 0 0 8px var(--primary);"></span> Conectado';
        el.classList.add('connected');
    } else {
        el.innerHTML = '<span class="dot" style="background-color: var(--danger);"></span> Desconectado';
        el.classList.remove('connected');
    }
}

// Message Handler
function handleMessage(msg) {
    // 1. Stats Update (Periodic)
    if (msg.uptime !== undefined) {
        document.getElementById('uptime').innerText = formatTime(msg.uptime);
        document.getElementById('bat-level').innerText = msg.battery + '%';
        document.getElementById('stat-aps').innerText = msg.aps;
        document.getElementById('stat-handshakes').innerText = msg.hs;
        document.getElementById('stat-threat').innerText = msg.ai;

        // Update Chart with dummy activity for now or real if available
        // Assuming msg.activity is packets/sec
        const activity = msg.activity || Math.floor(Math.random() * 10);
        updateChart(activity);
    }

    // 2. Log Line
    if (msg.log) {
        addLog(msg.log);
    }

    // 3. Network List
    if (msg.networks) {
        updateNetworkTable(msg.networks);
    }

    // 4. Terminal Output
    if (msg.term_out) {
        logToTerminal(msg.term_out);
    }
}

// Commands
function sendCommand(cmd, params = {}) {
    ws.send(cmd, params);
}

// Handlers for specific UI actions
function toggleMonitor(checkbox) {
    sendCommand(checkbox.checked ? 'wifi_monitor_on' : 'wifi_monitor_off');
}

function startDeauth() {
    const target = document.getElementById('deauth-target').value;
    sendCommand('wifi_deauth', { target: target });
}

function startBeacon() {
    const ssid = document.getElementById('beacon-ssid').value;
    sendCommand('wifi_beacon_spam', { ssid: ssid });
}

function handleCmdInput(e) {
    if (e.key === 'Enter') {
        const input = document.getElementById('cmd-input');
        const cmd = input.value;
        logToTerminal("$ " + cmd);
        sendCommand('term_input', { line: cmd });
        input.value = '';
    }
}

// UI Updaters
function addLog(text) {
    const container = document.getElementById('mini-log');
    const div = document.createElement('div');
    div.innerText = `[${new Date().toLocaleTimeString()}] ${text}`;
    div.style.borderBottom = '1px solid #333';
    div.style.padding = '2px 0';
    div.style.fontSize = '12px';
    container.appendChild(div);
    if (container.childElementCount > 50) container.removeChild(container.firstChild);
    container.scrollTop = container.scrollHeight;
}

function logToTerminal(text) {
    const term = document.getElementById('full-terminal');
    const div = document.createElement('div');
    div.innerText = text;
    term.appendChild(div);
    term.scrollTop = term.scrollHeight;
}

function updateNetworkTable(networks) {
    const tbody = document.querySelector('#wifi-table tbody');
    tbody.innerHTML = '';
    const select = document.getElementById('deauth-target');

    // Keep 'Broadcast'
    select.innerHTML = '<option value="FF:FF:FF:FF:FF:FF">Broadcast (Todos)</option>';

    networks.forEach(net => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>${net.ssid}</td>
            <td>${net.bssid}</td>
            <td>${net.ch}</td>
            <td>${net.rssi}</td>
            <td>${net.sec}</td>
            <td><button class="btn-small btn-danger" onclick="selectTarget('${net.bssid}')">🎯</button></td>
        `;
        tbody.appendChild(tr);

        const opt = document.createElement('option');
        opt.value = net.bssid;
        opt.innerText = `${net.ssid} (${net.bssid})`;
        select.appendChild(opt);
    });
}

function selectTarget(bssid) {
    document.getElementById('deauth-target').value = bssid;
    showView('wifi'); // Switch back if not already
}

// Helpers
function formatTime(seconds) {
    const h = Math.floor(seconds / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;
    return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
}

// Simple Canvas Chart
function initChart() {
    const canvas = document.getElementById('trafficChart');
    const ctx = canvas.getContext('2d');

    // Responsive canvas
    const resize = () => {
        canvas.width = canvas.parentElement.clientWidth;
        canvas.height = 200;
    };
    window.addEventListener('resize', resize);
    resize();

    function draw() {
        ctx.fillStyle = '#14161f'; // bg-panel
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        const w = canvas.width / trafficData.length;
        const maxVal = 50; // max scale

        ctx.strokeStyle = '#00ff9d';
        ctx.lineWidth = 2;
        ctx.beginPath();

        for (let i = 0; i < trafficData.length; i++) {
            const h = (trafficData[i] / maxVal) * canvas.height;
            const x = i * w;
            const y = canvas.height - h;

            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        }
        ctx.stroke();

        // Fill
        ctx.lineTo(canvas.width, canvas.height);
        ctx.lineTo(0, canvas.height);
        ctx.fillStyle = '#00ff9d20';
        ctx.fill();

        requestAnimationFrame(draw);
    }
    draw();
}

function updateChart(newValue) {
    trafficData.push(newValue);
    trafficData.shift();
}
