/**
 * CYD DASHBOARD 3000 CORE LOGIC
 * Powered by Three.js + Vanilla JS
 */

const app = {
    ws: null,
    currentView: 'dashboard',

    init: function () {
        this.init3D();
        this.initWS();
        this.log("[SYS] SYSTEM READY. INTERFACE LOADED.", "success");
    },

    // --- 3D ENGINE ---
    init3D: function () {
        if (typeof THREE === 'undefined') {
            console.error("Three.js not loaded");
            return;
        }

        const container = document.getElementById('canvas-container');
        const scene = new THREE.Scene();
        // Fog for depth
        scene.fog = new THREE.FogExp2(0x050510, 0.002);

        const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        camera.position.z = 5;

        const renderer = new THREE.WebGLRenderer({ alpha: true, antialias: true });
        renderer.setSize(window.innerWidth, window.innerHeight);
        renderer.setPixelRatio(window.devicePixelRatio);
        container.appendChild(renderer.domElement);

        // PARTICLE SYSTEM (Space Background)
        const starGeo = new THREE.BufferGeometry();
        const starCount = 2000;
        const starPos = new Float32Array(starCount * 3);
        const starVel = []; // velocity for animation

        for (let i = 0; i < starCount * 3; i++) {
            starPos[i] = (Math.random() - 0.5) * 100;
            if (i % 3 === 2) starVel.push((Math.random() * 0.01) + 0.01); // Z velocity
        }
        starGeo.setAttribute('position', new THREE.BufferAttribute(starPos, 3));
        const starMat = new THREE.PointsMaterial({ color: 0x00f3ff, size: 0.1, transparent: true });
        const stars = new THREE.Points(starGeo, starMat);
        scene.add(stars);

        // HOLOGRAM (ESP32 Chip Representation)
        const chipGroup = new THREE.Group();

        // Main Board
        const boardGeo = new THREE.BoxGeometry(2, 0.1, 3);
        const boardEdge = new THREE.EdgesGeometry(boardGeo);
        const boardMat = new THREE.LineBasicMaterial({ color: 0x00f3ff, transparent: true, opacity: 0.5 });
        const board = new THREE.LineSegments(boardEdge, boardMat);
        chipGroup.add(board);

        // Metal Shield (center)
        const shieldGeo = new THREE.BoxGeometry(1.2, 0.2, 1.2);
        const shieldEdge = new THREE.EdgesGeometry(shieldGeo);
        const shieldMat = new THREE.LineBasicMaterial({ color: 0xff00ff, transparent: true, opacity: 0.8 });
        const shield = new THREE.LineSegments(shieldEdge, shieldMat);
        shield.position.y = 0.15;
        shield.position.z = -0.5;
        chipGroup.add(shield);

        // Antenna trace (squiggly line)
        // (Simplified as a box for now to save code size)
        const antGeo = new THREE.BoxGeometry(1, 0.05, 0.5);
        const antMat = new THREE.MeshBasicMaterial({ color: 0xffff00, wireframe: true });
        const ant = new THREE.Mesh(antGeo, antMat);
        ant.position.z = 1.6;
        chipGroup.add(ant);

        scene.add(chipGroup);

        // ANIMATION LOOP
        const animate = () => {
            requestAnimationFrame(animate);

            // Rotate Hologram
            chipGroup.rotation.y += 0.005;
            chipGroup.rotation.x = Math.sin(Date.now() * 0.001) * 0.1;

            // Move Stars
            const positions = stars.geometry.attributes.position.array;
            for (let i = 2; i < starCount * 3; i += 3) {
                positions[i] += 0.05; // Move towards camera
                if (positions[i] > 10) {
                    positions[i] = -50; // Reset far back
                }
            }
            stars.geometry.attributes.position.needsUpdate = true;

            // Mouse/Touch Interaction (Parallax)
            // (Simplified: just auto rotate for "Zero Lag" feel)

            renderer.render(scene, camera);
        };

        animate();

        // Resize
        window.addEventListener('resize', () => {
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(window.innerWidth, window.innerHeight);
        });
    },

    // --- NETWORK ---
    initWS: function () {
        const host = window.location.hostname || '192.168.4.1'; // Default fallback
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        this.ws = new WebSocket(`${protocol}//${host}/ws`);

        this.ws.onopen = () => {
            this.log("WEBSOCKET CONNECTED", "success");
            document.querySelector('.status-item span').style.color = '#00ff41'; // Green dot
        };

        this.ws.onclose = () => {
            this.log("CONNECTION LOST - RECONNECTING...", "err");
            setTimeout(() => this.initWS(), 3000);
        };

        this.ws.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                this.handleData(data);
            } catch (e) {
                console.error("Parse Error", e);
            }
        };
    },

    handleData: function (data) {
        if (data.type === 'status') {
            if (data.bat) document.getElementById('bat-val').innerText = data.bat + '%';
            if (data.temp) document.getElementById('temp-val').innerText = data.temp + '°C';
            if (data.pps) document.getElementById('pps-val').innerText = data.pps + ' PPS';
        }
        else if (data.type === 'wifi_scan') {
            this.renderWifiList(data.networks);
        }
        else if (data.type === 'log') {
            this.log(data.msg, data.level || 'info');
        }
    },

    // --- ACTIONS ---
    cmd: function (command, param = null) {
        this.log(`> CMD: ${command} ${param ? param : ''}`);
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify({ cmd: command, value: param }));
        } else {
            this.log("ERROR: OFFLINE", "err");
        }
    },

    nav: function (viewId) {
        // Hide all views
        document.querySelectorAll('.view-section').forEach(el => {
            el.style.display = 'none';
        });

        // Show target
        const target = document.getElementById('view-' + viewId);
        if (target) {
            target.style.display = 'flex'; // Use flex for some, block for others?
            if (viewId === 'wifi') target.style.display = 'block'; // Wifi view needs block
        }

        this.currentView = viewId;
    },

    // --- UI RENDERERS ---
    log: function (msg, type = 'info') {
        const consoleEl = document.getElementById('console-output');
        const div = document.createElement('div');
        div.className = `log-line ${type}`;
        div.innerText = `[${new Date().toLocaleTimeString()}] ${msg}`;
        consoleEl.prepend(div);
        if (consoleEl.children.length > 50) consoleEl.lastChild.remove();
    },

    renderWifiList: function (networks) {
        const tbody = document.getElementById('wifi-list');
        tbody.innerHTML = '';
        networks.forEach(net => {
            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td style="color:var(--neon-cyan); font-weight:bold;">${net.ssid}</td>
                <td style="font-family:monospace; color:#888;">${net.bssid}</td>
                <td style="color:${net.rssi > -60 ? '#0f0' : 'orange'}">${net.rssi}</td>
                <td>${net.ch}</td>
                <td><button onclick="app.cmd('deauth', '${net.bssid}')">⚡</button></td>
            `;
            tbody.appendChild(tr);
        });
    }
};

// Start
window.onload = () => app.init();
