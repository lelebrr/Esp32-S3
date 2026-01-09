#ifndef IR_REMOTE_HTML_H
#define IR_REMOTE_HTML_H

const char *IR_REMOTE_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>MorphNode | IR Command</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Rajdhani:wght@300;500;700&display=swap" rel="stylesheet">
    <style>
        :root {
            --neon-blue: #00f3ff;
            --neon-pink: #bc13fe;
            --neon-green: #0aff0a;
            --bg-dark: #050510;
            --panel-bg: rgba(20, 20, 35, 0.7);
            --glass-border: 1px solid rgba(255, 255, 255, 0.1);
        }

        body {
            margin: 0;
            padding: 0;
            background-color: var(--bg-dark);
            background-image: 
                radial-gradient(circle at 10% 20%, rgba(0, 243, 255, 0.1) 0%, transparent 20%),
                radial-gradient(circle at 90% 80%, rgba(188, 19, 254, 0.1) 0%, transparent 20%);
            color: #fff;
            font-family: 'Rajdhani', sans-serif;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            overflow-x: hidden;
        }

        /* HEADER */
        header {
            width: 100%;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
            background: rgba(0,0,0,0.5);
            backdrop-filter: blur(10px);
            border-bottom: 1px solid rgba(255,255,255,0.05);
            position: sticky;
            top: 0;
            z-index: 100;
            box-shadow: 0 5px 20px rgba(0,0,0,0.5);
        }

        .logo-container {
            display: flex;
            align-items: center;
            gap: 15px;
        }

        .mascot-icon {
            font-size: 40px;
            filter: drop-shadow(0 0 10px var(--neon-green));
            animation: breathe 3s infinite ease-in-out;
        }

        h1 {
            font-family: 'Orbitron', sans-serif;
            font-size: 24px;
            font-weight: 900;
            letter-spacing: 2px;
            margin: 0;
            background: linear-gradient(to right, #00f3ff, #bc13fe);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            text-transform: uppercase;
        }

        .subtitle {
            font-size: 14px;
            color: #888;
            letter-spacing: 1px;
            text-transform: uppercase;
        }

        /* CONTAINER */
        .container {
            display: flex;
            flex-wrap: wrap;
            gap: 50px;
            justify-content: center;
            padding: 40px 20px;
            max-width: 1200px;
            width: 100%;
        }

        /* REMOTE COMMON */
        .remote-wrapper {
            position: relative;
            perspective: 1000px;
        }

        .remote {
            position: relative;
            transform-style: preserve-3d;
            transition: transform 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275);
        }

        .remote:hover {
            transform: rotateX(5deg) translateY(-10px);
        }

        .ir-node {
            position: absolute;
            top: -10px;
            left: 50%;
            transform: translateX(-50%);
            width: 30px;
            height: 15px;
            background: #222;
            border-radius: 10px 10px 0 0;
            z-index: -1;
        }

        .ir-led {
            width: 8px;
            height: 8px;
            background: #444;
            border-radius: 50%;
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            transition: 0.1s;
        }

        .ir-led.flash {
            background: #ff0000;
            box-shadow: 0 0 20px #ff0000, 0 0 40px #ff0000;
        }

        /* TV REMOTE STYLING */
        .tv-remote {
            width: 200px;
            background: linear-gradient(145deg, #2a2a2a, #1a1a1a);
            border-radius: 30px;
            padding: 25px 15px;
            box-shadow: 
                20px 20px 40px #0a0a0a,
                -20px -20px 40px #303030,
                inset 0 0 0 1px rgba(255,255,255,0.05);
            display: flex;
            flex-direction: column;
            gap: 15px;
        }

        .brand-text {
            text-align: center;
            font-family: 'Orbitron', sans-serif;
            color: #444;
            font-size: 10px;
            letter-spacing: 2px;
            margin-bottom: 5px;
        }

        .btn-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 12px;
            justify-items: center;
        }

        .tv-btn {
            border: none;
            outline: none;
            cursor: pointer;
            color: #ccc;
            font-weight: bold;
            font-family: 'Rajdhani', sans-serif;
            transition: all 0.1s;
            position: relative;
            overflow: hidden;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        
        /* Rubber Button Effect */
        .btn-rubber {
            background: #252525;
            box-shadow: 
                3px 3px 6px #151515,
                -1px -1px 3px #353535,
                inset 1px 1px 1px rgba(255,255,255,0.05);
            border-radius: 8px;
            width: 45px;
            height: 35px;
            font-size: 12px;
        }

        .btn-rubber:active {
            box-shadow: inset 2px 2px 5px #000;
            transform: scale(0.95);
        }

        .btn-round {
            width: 45px;
            height: 45px;
            border-radius: 50%;
        }

        .btn-pwr {
            background: linear-gradient(145deg, #d32f2f, #b71c1c);
            color: #fff;
            box-shadow: 0 0 10px rgba(211, 47, 47, 0.3);
        }

        .btn-pwr:active {
            box-shadow: inset 2px 2px 5px rgba(0,0,0,0.5);
        }

        .btn-nav-ring {
            grid-column: span 3;
            width: 140px;
            height: 140px;
            background: #252525;
            border-radius: 50%;
            position: relative;
            margin: 10px 0;
            box-shadow: 
                5px 5px 10px #151515,
                -2px -2px 5px #353535;
        }

        .nav-btn {
            position: absolute;
            background: transparent;
            color: #888;
            font-size: 14px;
            cursor: pointer;
            border: none;
        }
        .nav-up { top: 10px; left: 50%; transform: translateX(-50%); }
        .nav-down { bottom: 10px; left: 50%; transform: translateX(-50%); }
        .nav-left { left: 10px; top: 50%; transform: translateY(-50%); }
        .nav-right { right: 10px; top: 50%; transform: translateY(-50%); }
        .nav-ok { 
            top: 50%; left: 50%; transform: translate(-50%, -50%);
            width: 50px; height: 50px; background: #1a1a1a; 
            border-radius: 50%; border: 1px solid #333;
            color: var(--neon-blue);
            box-shadow: inset 2px 2px 5px #000;
        }
        .nav-ok:active { background: var(--neon-blue); color: #000; }

        /* AC REMOTE STYLING */
        .ac-remote {
            width: 220px;
            background: #e0e5ec;
            border-radius: 30px;
            padding: 30px 20px;
            box-shadow: 
                20px 20px 60px #bec3c9,
                -20px -20px 60px #ffffff;
            color: #444;
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .ac-screen {
            background: #9ab;
            height: 90px;
            border-radius: 10px;
            box-shadow: inset 3px 3px 7px rgba(0,0,0,0.2);
            position: relative;
            font-family: 'Orbitron', monospace;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
        }

        .temp-display {
            font-size: 42px;
            color: #234;
            text-shadow: 1px 1px 0 rgba(255,255,255,0.2);
        }

        .ac-icons {
            position: absolute;
            top: 5px;
            left: 10px;
            font-size: 12px;
            color: #234;
            display: flex;
            gap: 5px;
        }

        .ac-btn {
            background: #e0e5ec;
            border: none;
            border-radius: 10px;
            padding: 15px;
            box-shadow: 
                6px 6px 12px #bec3c9,
                -6px -6px 12px #ffffff;
            color: #555;
            font-weight: bold;
            cursor: pointer;
            transition: 0.1s;
        }

        .ac-btn:active {
            box-shadow: inset 4px 4px 8px #bec3c9, inset -4px -4px 8px #ffffff;
            color: #222;
        }

        .ac-pwr { 
            background: #ff6b6b; 
            color: white; 
            grid-column: span 2;
        }
        .ac-pwr:active { background: #e05252; }

        .ac-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
        }

        /* ANIMATIONS */
        @keyframes breathe {
            0%, 100% { transform: scale(1); filter: drop-shadow(0 0 5px var(--neon-green)); }
            50% { transform: scale(1.1); filter: drop-shadow(0 0 15px var(--neon-green)); }
        }

        .toast {
            position: fixed;
            bottom: 30px;
            background: rgba(0,0,0,0.8);
            backdrop-filter: blur(10px);
            border: 1px solid var(--neon-blue);
            color: var(--neon-blue);
            padding: 15px 30px;
            border-radius: 50px;
            font-family: 'Orbitron';
            transform: translateY(100px);
            opacity: 0;
            transition: 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275);
            box-shadow: 0 0 20px rgba(0, 243, 255, 0.2);
        }

        .toast.show {
            transform: translateY(0);
            opacity: 1;
        }
    </style>
</head>
<body>

    <header>
        <div class="logo-container">
            <div class="mascot-icon">🦎</div>
            <div>
                <h1>MorphNode</h1>
                <div class="subtitle">Universal Controller</div>
            </div>
        </div>
    </header>

    <div class="container">
        
        <!-- TV REMOTE -->
        <div class="remote-wrapper">
            <div class="ir-node"><div class="ir-led" id="led-tv"></div></div>
            <div class="remote tv-remote">
                <div class="brand-text">MORPH TV</div>
                <div style="text-align: right;">
                    <button class="tv-btn btn-rubber btn-round btn-pwr" onclick="sendIR('NEC', 0x20DF10EF, 32)">⏻</button>
                </div>

                <div class="btn-grid">
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF8877, 32)">1</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF48B7, 32)">2</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DFC837, 32)">3</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF28D7, 32)">4</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DFA857, 32)">5</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF6897, 32)">6</button>
                </div>

                <div style="display: flex; justify-content: space-between; padding: 0 10px;">
                    <button class="tv-btn btn-rubber" style="height: 60px; background: #333;" onclick="sendIR('NEC', 0x20DF40BF, 32)">VOL<br>+</button>
                    <button class="tv-btn btn-rubber" style="height: 60px; background: #333;" onclick="sendIR('NEC', 0x20DF00FF, 32)">CH<br>+</button>
                </div>

                <div class="btn-nav-ring">
                    <button class="nav-btn nav-up" onclick="sendIR('NEC', 0x20DF02FD, 32)">▲</button>
                    <button class="nav-btn nav-down" onclick="sendIR('NEC', 0x20DF827D, 32)">▼</button>
                    <button class="nav-btn nav-left" onclick="sendIR('NEC', 0x20DTE01F, 32)">◄</button>
                    <button class="nav-btn nav-right" onclick="sendIR('NEC', 0x20DF609F, 32)">►</button>
                    <button class="nav-btn nav-ok" onclick="sendIR('NEC', 0x20DF22DD, 32)">OK</button>
                </div>

                <div style="display: flex; justify-content: space-between; padding: 0 10px;">
                    <button class="tv-btn btn-rubber" style="height: 60px; background: #333;" onclick="sendIR('NEC', 0x20DFC03F, 32)">VOL<br>-</button>
                    <button class="tv-btn btn-rubber" style="height: 60px; background: #333;" onclick="sendIR('NEC', 0x20DF807F, 32)">CH<br>-</button>
                </div>

                <div class="btn-grid">
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF906F, 32)">MUTE</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DFD02F, 32)">SRC</button>
                    <button class="tv-btn btn-rubber" onclick="sendIR('NEC', 0x20DF08F7, 32)">0</button>
                </div>
            </div>
        </div>

        <!-- AC REMOTE -->
        <div class="remote-wrapper">
             <div class="ir-node"><div class="ir-led" id="led-ac"></div></div>
             <div class="remote ac-remote">
                <div class="brand-text" style="color:#889;">CLIMATE</div>
                <div class="ac-screen">
                    <div class="ac-icons">❄️ 💨 AUTO</div>
                    <div class="temp-display" id="temp-val">21°</div>
                </div>
                
                <div class="ac-grid">
                    <button class="ac-btn ac-pwr" onclick="sendAC('PWR')">POWER</button>
                    <button class="ac-btn" onclick="adjustTemp(1)">TEMP ▲</button>
                    <button class="ac-btn" onclick="adjustTemp(-1)">TEMP ▼</button>
                    <button class="ac-btn" onclick="sendAC('MODE')">MODE</button>
                    <button class="ac-btn" onclick="sendAC('FAN')">FAN</button>
                    <button class="ac-btn" onclick="sendAC('SWING')">SWING</button>
                    <button class="ac-btn" onclick="sendAC('TURBO')">TURBO</button>
                </div>
             </div>
        </div>

    </div>

    <div class="toast" id="toast">Signal Transmitted</div>

    <script>
        let temp = 21;

        async function sendIR(proto, code, bits) {
            flashLed('led-tv');
            showToast(`Sending ${proto}...`);
            try {
                await fetch('/api/ir/send', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({ protocol: proto, code: code, bits: bits })
                });
            } catch(e) { console.error(e); }
        }

        async function sendAC(cmd) {
            flashLed('led-ac');
            showToast(`AC: ${cmd}`);
            // Mock AC code logic
            let code = 0x8800000 + temp;
            sendIR('NEC', code, 32);
        }

        function adjustTemp(delta) {
            temp += delta;
            if (temp > 30) temp = 30;
            if (temp < 16) temp = 16;
            document.getElementById('temp-val').innerText = temp + "°";
            sendAC('TEMP');
        }

        function flashLed(id) {
            let led = document.getElementById(id);
            led.classList.add('flash');
            setTimeout(() => led.classList.remove('flash'), 200);
        }

        function showToast(msg) {
            let t = document.getElementById('toast');
            t.innerText = msg;
            t.classList.add('show');
            setTimeout(() => t.classList.remove('show'), 2000);
        }
    </script>
</body>
</html>
)rawliteral";

#endif
