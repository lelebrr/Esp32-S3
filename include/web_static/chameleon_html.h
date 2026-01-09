#ifndef CHAMELEON_HTML_H
#define CHAMELEON_HTML_H

const char CHAMELEON_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MorphNode Studio</title>
    <style>
        :root { --bg: #050510; --panel: #151525; --text: #eee; --accent: #00f3ff; }
        body { background: var(--bg); color: var(--text); font-family: 'Segoe UI', sans-serif; margin: 0; padding: 20px; text-align: center; }
        h1 { color: var(--accent); text-shadow: 0 0 10px var(--accent); }
        .container { max-width: 800px; margin: auto; display: flex; flex-wrap: wrap; gap: 20px; justify-content: center; }
        
        /* Preview Box */
        .preview-box {
            background: var(--panel); border: 1px solid var(--accent);
            border-radius: 15px; padding: 20px; flex: 1; min-width: 300px;
            box-shadow: 0 0 20px rgba(0, 243, 255, 0.1);
        }
        canvas { background: #000; border-radius: 10px; width: 100%; max-width: 400px; height: 300px; }
        
        /* Controls Box */
        .controls-box {
            background: var(--panel); border: 1px solid #333;
            border-radius: 15px; padding: 20px; flex: 1; min-width: 300px;
            text-align: left;
        }
        .control-group { margin-bottom: 15px; border-bottom: 1px solid #333; padding-bottom: 10px; }
        label { display: block; margin-bottom: 5px; color: #aaa; }
        input[type=color] { width: 100%; height: 40px; border: none; background: none; cursor: pointer; }
        input[type=range] { width: 100%; }
        input[type=checkbox] { transform: scale(1.5); margin: 10px; }
        
        button {
            background: var(--accent); color: #000; border: none; padding: 12px 24px;
            font-size: 16px; font-weight: bold; border-radius: 5px; cursor: pointer;
            width: 100%; margin-top: 10px; transition: 0.3s;
        }
        button:hover { background: #fff; box-shadow: 0 0 15px var(--accent); }
        
        .toast {
            position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%);
            background: #333; color: #fff; padding: 10px 20px; border-radius: 5px;
            opacity: 0; transition: 0.5s; pointer-events: none;
        }
    </style>
</head>
<body>

    <h1>MorphNode Studio</h1>
    
    <div class="container">
        <!-- Preview -->
        <div class="preview-box">
            <h2>Live Preview</h2>
            <canvas id="simCanvas" width="400" height="300"></canvas>
            <div style="margin-top: 10px;">
                <label>Test Mood:</label>
                <select id="moodSelect" onchange="setSimMood(this.value)" style="width: 100%; padding: 10px; background: #222; color: #fff; border: 1px solid #444;">
                    <option value="IDLE">Idle</option>
                    <option value="ATTACK">Attack</option>
                    <option value="SUCCESS">Success</option>
                    <option value="ERROR">Error</option>
                    <option value="EATING">Eating</option>
                    <option value="SNEAKY">Sneaky</option>
                    <option value="EXCITED">Excited (Party)</option>
                    <option value="CONFUSED">Confused</option>
                </select>
            </div>
        </div>

        <!-- Controls -->
        <div class="controls-box">
            <h2>Customization</h2>
            
            <div class="control-group">
                <label>Idle Skin Color</label>
                <input type="color" id="col_idle" value="#4ecdc4" oninput="updateSim()">
            </div>
            
            <div class="control-group">
                <label>Attack Color</label>
                <input type="color" id="col_attack" value="#e94560" oninput="updateSim()">
            </div>
            
            <div class="control-group">
                <label>Success Color</label>
                <input type="color" id="col_success" value="#00ff00" oninput="updateSim()">
            </div>
            
            <div class="control-group">
                <label>Eating Color (Tongue)</label>
                <input type="color" id="col_eating" value="#ffa500" oninput="updateSim()">
            </div>

            <div class="control-group">
                <label>Sneaky Color</label>
                <input type="color" id="col_sneaky" value="#222222" oninput="updateSim()">
            </div>

            <div class="control-group">
                <label>Party Mode</label>
                <input type="checkbox" id="chk_rainbow" checked onchange="updateSim()"> Rainbow Enabled
                <input type="color" id="col_party" value="#ff00ff" oninput="updateSim()"> (Base Color)
            </div>
            
            <div class="control-group">
                <label>Animation Speed</label>
                <input type="range" id="rng_speed" min="1" max="20" value="5" oninput="updateSim()">
            </div>

            <button onclick="saveConfig()">SAVE TO DEVICE</button>
        </div>
    </div>
    
    <div id="toast" class="toast">Settings Saved!</div>

    <script>
        const canvas = document.getElementById('simCanvas');
        const ctx = canvas.getContext('2d');
        let simMood = 'IDLE';
        let time = 0;
        
        // Sim data
        let config = {
            idle: "#4ecdc4", attack: "#e94560", success: "#00ff00", error: "#ff0000",
            eating: "#ffa500", sneaky: "#222222", party: "#ff00ff", rainbow: true, speed: 5
        };

        // Load correct values from device on boot
        fetch('/api/chameleon').then(r => r.json()).then(data => {
            config = data;
            // Update inputs
            document.getElementById('col_idle').value = numToHex(config.idle);
            document.getElementById('col_attack').value = numToHex(config.attack);
            document.getElementById('col_success').value = numToHex(config.success);
            document.getElementById('col_eating').value = numToHex(config.eating);
            document.getElementById('col_sneaky').value = numToHex(config.sneaky);
            document.getElementById('col_party').value = numToHex(config.party);
            document.getElementById('chk_rainbow').checked = config.rainbow;
            document.getElementById('rng_speed').value = config.speed;
        }).catch(e => console.log('Sim mode'));

        function numToHex(num) {
            return '#' + num.toString(16).padStart(6, '0');
        }

        // --- SIMULATION LOOP ---
        function draw() {
            ctx.fillStyle = '#000';
            ctx.fillRect(0, 0, 400, 300);
            
            time += 0.1;
            
            // Determine Color
            let c = config.idle;
            if(simMood === 'ATTACK') c = config.attack;
            else if(simMood === 'SUCCESS') c = config.success;
            else if(simMood === 'ERROR') config.error;
            else if(simMood === 'SNEAKY') c = config.sneaky;
            else if(simMood === 'EATING') c = config.eating; 
            else if(simMood === 'EXCITED') {
                if(config.rainbow) {
                    let hue = (time * 10 * config.speed / 5) % 360;
                    c = `hsl(${hue}, 100%, 50%)`;
                } else {
                    c = config.party;
                }
            }

            // Draw MorphNode (Simulated)
            let yOff = Math.sin(time) * 2;
            if(simMood==='EXCITED') yOff = Math.sin(time*3) * 5;

            // Tail
            ctx.fillStyle = c;
            ctx.beginPath();
            ctx.arc(150, 150 + yOff, 20, 0, Math.PI*2);
            ctx.fill();
            
            // Body
            ctx.beginPath();
            ctx.ellipse(200, 140 + yOff, 50, 35, 0, 0, Math.PI*2);
            ctx.fill();
            
            // Head
            ctx.beginPath();
            ctx.arc(240, 125 + yOff, 30, 0, Math.PI*2);
            ctx.fill();
            
            // Eyes
            drawEye(230, 115 + yOff, time, 0); // Left
            drawEye(250, 115 + yOff, time, 1); // Right
            
            // Tongue
            if(simMood === 'EATING') {
                let ext = Math.abs(Math.sin(time * 0.5)) * 60;
                ctx.fillStyle = '#ff5555';
                ctx.fillRect(260, 130 + yOff, ext, 5);
            }

            requestAnimationFrame(draw);
        }
        
        function drawEye(x, y, t, side) {
            ctx.fillStyle = '#fff';
            ctx.beginPath();
            ctx.arc(x, y, 8, 0, Math.PI*2);
            ctx.fill();
            
            // Pupil
            let px = 0, py = 0;
            if(simMood === 'ATTACK') {
                px = Math.sin(t*(2+side)) * 3;
                py = Math.cos(t*(3-side)) * 3;
            } else if (simMood === 'CONFUSED') {
                let dir = side === 0 ? 1 : -1;
                px = Math.sin(t * dir) * 3;
                py = Math.cos(t * dir) * 3;
            }
            
            ctx.fillStyle = '#000';
            ctx.beginPath();
            ctx.arc(x + px, y + py, 3, 0, Math.PI*2);
            ctx.fill();
        }

        function updateSim() {
            config.idle = document.getElementById('col_idle').value;
            config.attack = document.getElementById('col_attack').value;
            config.success = document.getElementById('col_success').value;
            config.eating = document.getElementById('col_eating').value;
            config.sneaky = document.getElementById('col_sneaky').value;
            config.party = document.getElementById('col_party').value;
            config.rainbow = document.getElementById('chk_rainbow').checked;
            config.speed = document.getElementById('rng_speed').value;
        }

        function setSimMood(m) {
            simMood = m;
        }
        
        function saveConfig() {
            // Convert Hex to Int
            const payload = {
                idle: parseInt(config.idle.replace('#',''), 16),
                attack: parseInt(config.attack.replace('#',''), 16),
                success: parseInt(config.success.replace('#',''), 16),
                eating: parseInt(config.eating.replace('#',''), 16),
                sneaky: parseInt(config.sneaky.replace('#',''), 16),
                party: parseInt(config.party.replace('#',''), 16),
                rainbow: config.rainbow,
                speed: parseInt(config.speed)
            };
            
            fetch('/api/chameleon', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify(payload)
            }).then(r => {
                const t = document.getElementById('toast');
                t.style.opacity = 1;
                setTimeout(() => t.style.opacity = 0, 2000);
            });
        }

        draw();
    </script>
</body>
</html>
)rawliteral";

#endif
