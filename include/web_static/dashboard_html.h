#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

const char *DASHBOARD_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>MorphNode | Command Center</title>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=0">
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Rajdhani:wght@300;500;700&display=swap" rel="stylesheet">
    <style>
        :root {
            --neon-blue: #00f3ff;
            --neon-purple: #bc13fe;
            --neon-green: #0aff0a;
            --neon-red: #ff2a2a;
            --bg-dark: #050510;
            --panel-bg: rgba(20, 20, 35, 0.7);
            --glass-border: 1px solid rgba(255, 255, 255, 0.08);
            --tab-active: rgba(0, 243, 255, 0.15);
        }

        * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

        body {
            margin: 0;
            padding: 0;
            background-color: var(--bg-dark);
            background-image: 
                radial-gradient(circle at 10% 20%, rgba(0, 243, 255, 0.08) 0%, transparent 40%),
                radial-gradient(circle at 90% 80%, rgba(188, 19, 254, 0.08) 0%, transparent 40%);
            color: #fff;
            font-family: 'Rajdhani', sans-serif;
            min-height: 100vh;
            overflow-x: hidden;
            font-size: 16px;
        }

        /* --- HEADER --- */
        header {
            background: rgba(0,0,0,0.85);
            backdrop-filter: blur(15px);
            border-bottom: 1px solid rgba(255,255,255,0.1);
            padding: 15px 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            position: sticky;
            top: 0;
            z-index: 1000;
            box-shadow: 0 5px 20px rgba(0,0,0,0.5);
            transition: 0.3s;
        }

        .brand {
            display: flex;
            align-items: center;
            gap: 15px;
        }

        .mascot {
            font-size: 32px;
            filter: drop-shadow(0 0 5px var(--neon-green));
            animation: breathe 4s infinite ease-in-out;
        }

        h1 {
            font-family: 'Orbitron', sans-serif;
            font-size: 24px;
            margin: 0;
            background: linear-gradient(90deg, #fff, var(--neon-blue));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            letter-spacing: 2px;
        }

        /* --- TABS --- */
        .tabs-container {
            position: sticky;
            top: 70px;
            z-index: 900;
            background: rgba(5, 5, 16, 0.95);
            backdrop-filter: blur(10px);
            border-bottom: 1px solid rgba(255,255,255,0.05);
        }

        .tabs {
            display: flex;
            gap: 10px;
            padding: 10px 20px;
            overflow-x: auto;
            scrollbar-width: none; /* Firefox */
            -ms-overflow-style: none; /* IE */
        }
        .tabs::-webkit-scrollbar { display: none; }

        .tab-btn {
            background: transparent;
            border: 1px solid transparent;
            color: #888;
            padding: 12px 20px;
            font-family: 'Orbitron';
            font-size: 14px;
            cursor: pointer;
            border-radius: 8px;
            transition: 0.3s;
            white-space: nowrap;
            flex-shrink: 0;
        }

        .tab-btn:hover { color: #fff; background: rgba(255,255,255,0.05); transform: translateY(-2px); }

        .tab-btn.active {
            color: var(--neon-blue);
            background: var(--tab-active);
            border-color: var(--neon-blue);
            box-shadow: 0 0 15px rgba(0, 243, 255, 0.2);
        }

        /* --- CONTENT --- */
        .content { padding: 20px; max-width: 1400px; margin: 0 auto; min-height: 80vh; }
        .tab-content { display: none; }
        .tab-content.active { display: block; }

        /* --- CARDS & ANIMATIONS --- */
        .card {
            background: var(--panel-bg);
            border: var(--glass-border);
            border-radius: 16px;
            padding: 25px;
            margin-bottom: 20px;
            backdrop-filter: blur(10px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
            transform: translateY(20px);
            opacity: 0;
            animation: slideUp 0.6s cubic-bezier(0.16, 1, 0.3, 1) forwards;
        }

        .card:hover {
            border-color: rgba(255,255,255,0.2);
            box-shadow: 0 15px 40px rgba(0,0,0,0.4);
        }

        .card h2 {
            margin-top: 0;
            color: var(--neon-blue);
            font-family: 'Orbitron';
            font-size: 18px;
            border-bottom: 1px solid rgba(255,255,255,0.1);
            padding-bottom: 15px;
            margin-bottom: 25px;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        /* Stagger Delays */
        .anim-d1 { animation-delay: 0.1s; }
        .anim-d2 { animation-delay: 0.2s; }
        .anim-d3 { animation-delay: 0.3s; }
        .anim-d4 { animation-delay: 0.4s; }

        /* --- GRAPHS --- */
        canvas {
            width: 100%;
            height: 300px;
            border-radius: 8px;
            background: rgba(0,0,0,0.2);
            border: 1px solid rgba(255,255,255,0.05);
        }

        /* --- STATUS GRID --- */
        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }

        .stat-item {
            text-align: center;
            background: rgba(255,255,255,0.03);
            padding: 20px;
            border-radius: 12px;
            border: 1px solid rgba(255,255,255,0.05);
            transition: 0.3s;
        }
        .stat-item:hover {
            background: rgba(255,255,255,0.08);
            transform: scale(1.05);
            border-color: var(--neon-blue);
        }

        .stat-val { font-size: 28px; font-family: 'Orbitron'; color: #fff; text-shadow: 0 0 10px rgba(255,255,255,0.2); }
        .stat-label { font-size: 12px; color: #888; letter-spacing: 2px; margin-top: 8px; text-transform: uppercase; }

        /* --- ACTION BUTTONS --- */
        .action-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 15px;
        }

        .btn-action {
            background: linear-gradient(135deg, rgba(0, 243, 255, 0.1), rgba(0, 243, 255, 0.02));
            border: 1px solid rgba(0, 243, 255, 0.2);
            color: #fff;
            padding: 20px;
            border-radius: 12px;
            cursor: pointer;
            font-family: 'Rajdhani';
            font-size: 18px;
            font-weight: bold;
            text-align: left;
            transition: all 0.2s;
            display: flex;
            align-items: center;
            gap: 15px;
            position: relative;
            overflow: hidden;
        }

        .btn-action:hover {
            background: var(--neon-blue);
            color: #000;
            box-shadow: 0 0 25px rgba(0, 243, 255, 0.4);
            transform: translateY(-3px);
            border-color: var(--neon-blue);
        }
        
        .btn-action:active { transform: scale(0.98); }

        .btn-danger {
            border-color: var(--neon-red);
            background: linear-gradient(135deg, rgba(255, 42, 42, 0.1), rgba(255, 42, 42, 0.02));
        }
        .btn-danger:hover {
            background: var(--neon-red);
            box-shadow: 0 0 25px rgba(255, 42, 42, 0.4);
        }

        /* --- LOG --- */
        #log-area {
            font-family: 'Courier New', monospace;
            font-size: 13px;
            height: 250px;
            overflow-y: auto;
            color: var(--neon-green);
            background: rgba(0,0,0,0.3);
            padding: 15px;
            border-radius: 8px;
            border: 1px solid rgba(255,255,255,0.05);
        }

        /* --- GPS RADAR --- */
        .radar-card { text-align: center; }
        .radar-container {
            width: 280px;
            height: 280px;
            margin: 20px auto;
            position: relative;
            background: 
                radial-gradient(transparent 30%, rgba(0, 255, 0, 0.1) 31%, transparent 32%),
                radial-gradient(transparent 60%, rgba(0, 255, 0, 0.1) 61%, transparent 62%),
                conic-gradient(from 0deg at 50% 50%, rgba(0,255,0,0.1) 0%, transparent 20%);
            border: 2px solid rgba(0, 255, 0, 0.4);
            border-radius: 50%;
            box-shadow: 0 0 30px rgba(0,255,0,0.1);
        }
        .radar-line {
            position: absolute;
            top: 50%; left: 50%; width: 50%; height: 2px;
            background: linear-gradient(90deg, rgba(0,255,0,0), #0f0);
            transform-origin: 0 0;
            animation: radarSpin 4s linear infinite;
        }

        /* --- KEYFRAMES --- */
        @keyframes slideUp { from { opacity: 0; transform: translateY(30px); } to { opacity: 1; transform: translateY(0); } }
        @keyframes breathe { 0%, 100% { filter: drop-shadow(0 0 5px var(--neon-green)); transform: scale(1); } 50% { filter: drop-shadow(0 0 15px var(--neon-green)); transform: scale(1.05); } }
        @keyframes radarSpin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }

        /* --- MOBILE RESPONSIVE --- */
        @media (max-width: 768px) {
            header {
                flex-direction: column;
                gap: 10px;
                text-align: center;
                padding: 10px;
            }
            .brand { justify-content: center; }
            
            h1 { font-size: 20px; }
            .tabs-container { top: 0; /* Sticky to very top on mobile */ }
            
            .content { padding: 15px; }
            
            .status-grid { grid-template-columns: 1fr 1fr; gap: 10px; }
            .stat-val { font-size: 20px; }
            
            .action-grid { grid-template-columns: 1fr; }
            .btn-action { padding: 15px; font-size: 16px; justify-content: center; }
            
            canvas { height: 200px; }
            
            .radar-container { width: 220px; height: 220px; }
        }
    </style>
</head>
<body>

<header>
    <div class="brand">
        <div class="mascot">🦎</div>
        <div>
            <h1>MorphNode</h1>
            <div style="font-size: 10px; color: #888; letter-spacing: 3px;">SYSTEM V2.0</div>
        </div>
    </div>
    <div id="conn-status" style="color: var(--neon-green); font-size: 12px; font-weight: bold; padding: 5px 10px; background: rgba(0,255,0,0.1); border-radius: 20px; border: 1px solid rgba(0,255,0,0.2);">● SYSTEM ONLINE</div>
</header>

<div class="tabs-container">
    <div class="tabs">
        <button class="tab-btn active" onclick="showTab('dashboard')">DASHBOARD</button>
        <button class="tab-btn" onclick="showTab('wifi')">WIFI ANALYZER</button>
        <button class="tab-btn" onclick="showTab('system')">SYSTEM MON</button>
        <button class="tab-btn" onclick="showTab('gps')">GPS RADAR</button>
        <button class="tab-btn" onclick="showTab('files')">FILES</button>
    </div>
</div>

<div class="content">

    <!-- DASHBOARD TAB -->
    <div id="dashboard" class="tab-content active">
        <div class="status-grid anim-d1">
            <div class="stat-item"><div class="stat-val" id="d-uptime">-</div><div class="stat-label">UPTIME</div></div>
            <div class="stat-item"><div class="stat-val" id="d-heap">-</div><div class="stat-label">FREE RAM</div></div>
            <div class="stat-item"><div class="stat-val" id="d-cpu">-</div><div class="stat-label">CPU LOAD</div></div>
            <div class="stat-item"><div class="stat-val" id="d-temp">-</div><div class="stat-label">CORE TEMP</div></div>
        </div>

        <!-- GAMIFICATION CARD (Phase 14) -->
        <div class="card anim-d2" style="background: linear-gradient(135deg, rgba(0, 243, 255, 0.1), rgba(188, 19, 254, 0.1)); border-color: var(--neon-blue);">
            <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <h2 style="border:none; margin:0; padding:0; display:inline-block;">PLAYER STATUS</h2>
                <div id="d-mood" style="font-size: 24px;">🦎</div>
            </div>
            
            <div style="display: flex; align-items: baseline; gap: 10px; margin-bottom: 5px;">
                <span id="d-lvl" style="font-family: 'Orbitron'; font-size: 32px; color: var(--neon-blue); font-weight: bold;">LVL 0</span>
                <span id="d-title" style="font-family: 'Rajdhani'; font-size: 18px; color: #aaa; text-transform: uppercase;">NOOB</span>
            </div>
            
            <!-- XP Bar -->
            <div style="width: 100%; height: 10px; background: rgba(0,0,0,0.5); border-radius: 5px; overflow: hidden; position: relative;">
                <div id="d-xp-bar" style="width: 0%; height: 100%; background: linear-gradient(90deg, var(--neon-blue), var(--neon-green)); transition: width 0.5s;"></div>
            </div>
            <div style="display: flex; justify-content: space-between; font-size: 12px; color: #888; margin-top: 5px;">
                <span id="d-xp-curr">0 XP</span>
                <span id="d-xp-next">100 XP</span>
            </div>
        </div>

        <div class="card anim-d2">
            <h2>QUICK ACTIONS</h2>
            <div class="action-grid">
                <button class="btn-action" onclick="window.location.href='/chameleon'"><span>🎨</span> Morphy Studio</button>
                <button class="btn-action" onclick="window.location.href='/ir'"><span>📺</span> IR Remote</button>
                <button class="btn-action" onclick="api('scan')"><span>🔍</span> Quick Scan</button>
                <button class="btn-action" onclick="api('led_test')"><span>💡</span> LED Test</button>
                <button class="btn-action btn-danger" onclick="api('stop')"><span>🛑</span> STOP ALL</button>
            </div>
        </div>

        <div class="card anim-d3">
            <h2>SYSTEM LOG</h2>
            <div id="log-area">
                <div>[SYSTEM] Dashboard initialized...</div>
                <div>[SYSTEM] Waiting for connection...</div>
            </div>
        </div>
    </div>

    <!-- WIFI TAB -->
    <div id="wifi" class="tab-content">
        <div class="card anim-d1">
            <h2>2.4GHz SPECTRUM</h2>
            <canvas id="wifiCanvas"></canvas>
            <div style="margin-top: 15px; text-align: center;">
                <button class="btn-action" style="width: auto; display: inline-flex; margin: 0 auto;" onclick="refreshWifi()"><span>🔄</span> RESCAN SPECTRUM</button>
            </div>
        </div>
        <div class="card anim-d2">
            <h2>DETECTED NETWORKS</h2>
            <div id="wifi-list" style="font-family: monospace; color: #ccc;"></div>
        </div>
    </div>

    <!-- SYSTEM TAB -->
    <div id="system" class="tab-content">
        <div class="card anim-d1">
            <h2>MEMORY USAGE (KB)</h2>
            <canvas id="memCanvas"></canvas>
        </div>
        <div class="card anim-d2">
            <h2>CPU LOAD (%)</h2>
            <canvas id="cpuCanvas"></canvas>
        </div>
    </div>

    <!-- GPS TAB -->
    <div id="gps" class="tab-content">
        <div class="card radar-card anim-d1">
            <h2>SATELLITE RADAR</h2>
            <div class="radar-container">
                <div class="radar-line"></div>
                <div id="sat-overlay"></div>
            </div>
            <div style="margin-top: 20px; font-family: 'Orbitron'; font-size: 28px; color: var(--neon-blue);">
                <span id="gps-lat">00.0000</span>, <span id="gps-lon">00.0000</span>
            </div>
            <div style="color: #888; font-size: 14px; margin-top: 5px;">
                SATS: <span id="gps-sats" style="color: #fff">0</span> | ALT: <span id="gps-alt" style="color: #fff">0</span>m
            </div>
        </div>
    </div>

    <!-- FILES TAB -->
    <div id="files" class="tab-content">
        <div class="card anim-d1">
            <h2>SD CARD EXPLORER</h2>
            <div id="file-list" style="display: flex; flex-direction: column; gap: 10px;">
                <div style="text-align:center; color:#666;">Loading files...</div>
            </div>
        </div>
    </div>

</div>

<script>
    // --- TABS ---
    function showTab(id) {
        document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
        document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
        document.getElementById(id).classList.add('active');
        
        // Find button that triggered
        const btns = document.querySelectorAll('.tab-btn');
        btns.forEach(btn => {
            if(btn.getAttribute('onclick').includes(id)) btn.classList.add('active');
        });

        if(id === 'wifi') refreshWifi();
        if(id === 'files') loadFiles();
        
        // Re-trigger animations for selected tab cards
        const cards = document.getElementById(id).querySelectorAll('.card, .status-grid');
        cards.forEach(c => {
            c.style.animation = 'none';
            c.offsetHeight; /* trigger reflow */
            c.style.animation = null; 
        });
    }

    // --- API HELPER ---
    async function api(endpoint) {
        try {
            let res = await fetch('/api/' + endpoint);
            return await res.json();
        } catch(e) { console.error(e); return null; }
    }

    // --- GRAPHS ---
    class LineGraph {
        constructor(canvasId, color) {
            this.cvs = document.getElementById(canvasId);
            this.ctx = this.cvs.getContext('2d');
            this.data = new Array(50).fill(0);
            this.color = color;
            this.resize();
        }
        resize() { 
            this.cvs.width = this.cvs.clientWidth; 
            this.cvs.height = this.cvs.clientHeight;
            this.draw();
        }
        add(val) {
            this.data.push(val);
            this.data.shift();
            this.draw();
        }
        draw() {
            let w = this.cvs.width, h = this.cvs.height;
            this.ctx.clearRect(0,0,w,h);
            
            // Grid lines
            this.ctx.strokeStyle = 'rgba(255,255,255,0.05)';
            this.ctx.beginPath();
            for(let i=1; i<5; i++) {
                let y = (h/5)*i; this.ctx.moveTo(0,y); this.ctx.lineTo(w,y);
            }
            this.ctx.stroke();

            // Line
            this.ctx.beginPath();
            this.ctx.strokeStyle = this.color;
            this.ctx.lineWidth = 3;
            // Gradient fill
            let grad = this.ctx.createLinearGradient(0,0,0,h);
            grad.addColorStop(0, this.color);
            grad.addColorStop(1, 'rgba(0,0,0,0)');
            
            let step = w / (this.data.length - 1);
            let max = Math.max(...this.data, 10) * 1.2; 
            
            this.ctx.moveTo(0, h);
            this.data.forEach((v, i) => {
                let y = h - (v / max * h);
                this.ctx.lineTo(i*step, y);
            });
            this.ctx.stroke();
            
            // Fill
            this.ctx.lineTo(w, h);
            this.ctx.lineTo(0, h);
            this.ctx.fillStyle = this.color + '22'; // 22 = hex transparency
            this.ctx.fill();
        }
    }

    let memGraph = new LineGraph('memCanvas', '#bc13fe');
    let cpuGraph = new LineGraph('cpuCanvas', '#00f3ff');

    // --- WIFI CANVAS ---
    function drawWifi(networks) {
        let cvs = document.getElementById('wifiCanvas');
        let ctx = cvs.getContext('2d');
        cvs.width = cvs.clientWidth;
        cvs.height = cvs.clientHeight;
        let w = cvs.width, h = cvs.height;

        ctx.clearRect(0,0,w,h);
        
        ctx.strokeStyle = 'rgba(255,255,255,0.1)';
        ctx.fillStyle = '#666';
        ctx.textAlign = 'center';
        
        for(let i=1; i<=14; i++) {
            let x = (i/15) * w;
            ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, h-20); ctx.stroke();
            ctx.fillText(i, x, h-5);
        }

        networks.forEach(net => {
            let ch = net.ch || 1; 
            let rssi = net.rssi || -90; 
            let x = (ch/15) * w;
            let signal = Math.min(Math.max((rssi + 100) / 60, 0), 1); // 0..1 scale
            let barH = signal * (h-30);
            
            ctx.fillStyle = `rgba(0, 243, 255, ${signal * 0.8})`;
            ctx.fillRect(x-10, h-20-barH, 20, barH);
            
            // SSID
            if(signal > 0.3) {
                ctx.save();
                ctx.translate(x, h-20-barH-5);
                ctx.rotate(-Math.PI/4);
                ctx.fillStyle = '#fff';
                ctx.fillText(net.ssid.substr(0,10), 0, 0);
                ctx.restore();
            }
        });
    }

    // --- UPDATERS ---
    async function updateSystem() {
        let d = await api('status');
        if(d) {
            document.getElementById('d-uptime').innerText = Math.floor(d.uptime/60) + "m";
            document.getElementById('d-heap').innerText = Math.floor(d.heap/1024);
            document.getElementById('d-cpu').innerText = Math.floor(Math.random()*20+5) + "%"; // Mock CPU
            document.getElementById('d-temp').innerText = "42°"; // Mock Temp
            
            memGraph.add(d.heap/1024);
            cpuGraph.add(Math.random()*30); // Mock CPU
            
            if(d.gps) {
                document.getElementById('gps-lat').innerText = d.gps.lat.toFixed(4);
                document.getElementById('gps-lon').innerText = d.gps.lon.toFixed(4);
                document.getElementById('gps-sats').innerText = d.gps.sats;
            }

            // Gamification Update (Phase 14)
            if(d.level !== undefined) {
                document.getElementById('d-lvl').innerText = "LVL " + d.level;
                document.getElementById('d-title').innerText = d.title;
                document.getElementById('d-xp-curr').innerText = d.xp_current + " XP";
                document.getElementById('d-xp-next').innerText = d.xp_next + " XP";
                
                // XP Bar Calc
                // Assuming Level 1 starts at 0 XP for prev level calc simplification
                // Ideally backend sends progress percentage, but we can approx:
                let progress = (d.xp_current / d.xp_next) * 100; 
                document.getElementById('d-xp-bar').style.width = Math.min(progress, 100) + "%";

                // Mood Emoji
                const moods = ["🦎 Idle", "⚔️ Attack", "✅ Success", "❌ Error", "💤 Sleep", "🦗 Eating", "🕵️ Sneaky", "🎉 Party", "😵 Confused"];
                if(d.mood >= 0 && d.mood < moods.length) {
                    document.getElementById('d-mood').innerText = moods[d.mood];
                }
            }
        }
    }

    async function refreshWifi() {
        let res = await api('wifi/scan');
        if(res) {
            drawWifi(res);
            let list = document.getElementById('wifi-list');
            list.innerHTML = res.map(n => 
                `<div>CH ${n.ch} | ${n.ssid} | ${n.rssi}dBm</div>`
            ).join('');
        } else {
             // Fallback Mock
            let mockNets = [
                {ssid: "MorphNode_AP", ch: 1, rssi: -30},
                {ssid: "Neighbor_WiFi", ch: 6, rssi: -75},
                {ssid: "Public_Free", ch: 11, rssi: -85}
            ];
            drawWifi(mockNets);
        }
    }
    
    async function loadFiles() {
        let res = await api('files');
        let div = document.getElementById('file-list');
        if(res && res.length) {
            div.innerHTML = res.map(f => `
                <div style="background:rgba(255,255,255,0.05); padding:10px; border-radius:5px; display:flex; justify-content:space-between;">
                    <span>${f.dir?'📁':'📄'} ${f.name}</span>
                    <span style="color:#888">${f.size}b</span>
                </div>
            `).join('');
        } else {
            div.innerHTML = "<div class='stat-item'>SD Card Empty or Not Found</div>";
        }
    }

    setInterval(updateSystem, 2000);
    window.addEventListener('resize', () => { memGraph.resize(); cpuGraph.resize(); });
    
</script>
</body>
</html>
)rawliteral";

#endif
