/**
 * WAVEPWN - Network 3D Visualization (Dica 42)
 * Visualização 3D de redes usando Three.js (lightweight version)
 */

const Network3D = {
    container: null,
    scene: null,
    camera: null,
    renderer: null,
    nodes: [],
    lines: [],
    animationId: null
};

// Check if Three.js is available
function hasThreeJS() {
    return typeof THREE !== 'undefined';
}

function initNetwork3D(containerId) {
    Network3D.container = document.getElementById(containerId);
    if (!Network3D.container) return;

    if (!hasThreeJS()) {
        // Fallback to 2D canvas visualization
        init2DFallback();
        return;
    }

    // Three.js setup
    Network3D.scene = new THREE.Scene();
    Network3D.scene.background = new THREE.Color(0x0a0a0f);

    Network3D.camera = new THREE.PerspectiveCamera(75, Network3D.container.offsetWidth / Network3D.container.offsetHeight, 0.1, 1000);
    Network3D.camera.position.z = 50;

    Network3D.renderer = new THREE.WebGLRenderer({ antialias: true });
    Network3D.renderer.setSize(Network3D.container.offsetWidth, Network3D.container.offsetHeight);
    Network3D.container.appendChild(Network3D.renderer.domElement);

    startAnimation3D();
}

function updateNetwork3DData(networks, connections) {
    if (!hasThreeJS()) {
        update2DFallback(networks);
        return;
    }

    // Clear existing nodes
    Network3D.nodes.forEach(n => Network3D.scene.remove(n));
    Network3D.lines.forEach(l => Network3D.scene.remove(l));
    Network3D.nodes = [];
    Network3D.lines = [];

    // Add network nodes
    networks.forEach((net, i) => {
        const geometry = new THREE.SphereGeometry(2, 16, 16);
        const material = new THREE.MeshBasicMaterial({
            color: net.isAP ? 0x00fff5 : 0xff00ff,
            transparent: true,
            opacity: 0.8
        });
        const sphere = new THREE.Mesh(geometry, material);

        // Position based on channel and signal
        const angle = (i / networks.length) * Math.PI * 2;
        const radius = 20 + Math.random() * 10;
        sphere.position.x = Math.cos(angle) * radius;
        sphere.position.y = Math.sin(angle) * radius;
        sphere.position.z = (net.rssi || -70) / 10;

        sphere.userData = net;
        Network3D.nodes.push(sphere);
        Network3D.scene.add(sphere);
    });

    // Add connections
    if (connections) {
        connections.forEach(conn => {
            const from = Network3D.nodes.find(n => n.userData.bssid === conn.from);
            const to = Network3D.nodes.find(n => n.userData.mac === conn.to);
            if (from && to) {
                const geometry = new THREE.BufferGeometry().setFromPoints([from.position, to.position]);
                const material = new THREE.LineBasicMaterial({ color: 0x00ff9d, opacity: 0.5, transparent: true });
                const line = new THREE.Line(geometry, material);
                Network3D.lines.push(line);
                Network3D.scene.add(line);
            }
        });
    }
}

let network3dLastDrawTime = 0;
const network3dFpsInterval = 1000 / 30; // Limit to 30 FPS

function animate3D(timestamp) {
    if (!Network3D.animationId) return;

    const elapsed = timestamp - network3dLastDrawTime;
    if (elapsed > network3dFpsInterval) {
        network3dLastDrawTime = timestamp - (elapsed % network3dFpsInterval);

        // Rotate camera around scene
        const time = Date.now() * 0.0005;
        Network3D.camera.position.x = Math.sin(time) * 60;
        Network3D.camera.position.z = Math.cos(time) * 60;
        Network3D.camera.lookAt(0, 0, 0);

        // Pulse nodes
        Network3D.nodes.forEach((node, i) => {
            node.scale.setScalar(1 + Math.sin(Date.now() * 0.003 + i) * 0.1);
        });

        Network3D.renderer.render(Network3D.scene, Network3D.camera);
    }

    Network3D.animationId = requestAnimationFrame(animate3D);
}

// 2D Fallback when Three.js is not available
function init2DFallback() {
    const canvas = document.createElement('canvas');
    canvas.width = Network3D.container.offsetWidth;
    canvas.height = Network3D.container.offsetHeight || 300;
    Network3D.container.appendChild(canvas);
    Network3D.ctx = canvas.getContext('2d');
    startAnimation2D();
}

function update2DFallback(networks) {
    Network3D.networks2D = networks;
}

function animate2D(timestamp) {
    const ctx = Network3D.ctx;
    if (!ctx) return;
    if (!Network3D.animationId) return;

    const elapsed = timestamp - network3dLastDrawTime;
    if (elapsed > network3dFpsInterval) {
        network3dLastDrawTime = timestamp - (elapsed % network3dFpsInterval);

        const w = ctx.canvas.width;
        const h = ctx.canvas.height;

        ctx.fillStyle = 'rgba(10, 10, 15, 0.1)';
        ctx.fillRect(0, 0, w, h);

        const networks = Network3D.networks2D || [];
        const centerX = w / 2;
        const centerY = h / 2;

        networks.forEach((net, i) => {
            const angle = (i / networks.length) * Math.PI * 2 + Date.now() * 0.0005;
            const radius = 80 + (net.rssi || -70) / 2;
            const x = centerX + Math.cos(angle) * radius;
            const y = centerY + Math.sin(angle) * radius;

            // Draw node
            ctx.beginPath();
            ctx.arc(x, y, 8, 0, Math.PI * 2);
            ctx.fillStyle = net.clients ? '#ff00ff' : '#00fff5';
            ctx.fill();

            // Draw connection to center
            ctx.beginPath();
            ctx.moveTo(centerX, centerY);
            ctx.lineTo(x, y);
            ctx.strokeStyle = 'rgba(0, 255, 245, 0.2)';
            ctx.stroke();

            // Label
            ctx.fillStyle = '#fff';
            ctx.font = '10px monospace';
            ctx.fillText(net.ssid || 'Hidden', x - 20, y + 20);
        });
    }

    Network3D.animationId = requestAnimationFrame(animate2D);
}

function startAnimation3D() {
    if (!Network3D.animationId) {
        network3dLastDrawTime = performance.now();
        Network3D.animationId = requestAnimationFrame(animate3D);
    }
}

function startAnimation2D() {
    if (!Network3D.animationId) {
        network3dLastDrawTime = performance.now();
        Network3D.animationId = requestAnimationFrame(animate2D);
    }
}

function stopNetwork3D() {
    if (Network3D.animationId) {
        cancelAnimationFrame(Network3D.animationId);
        Network3D.animationId = null;
    }
}

// Visibility API
document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
        stopNetwork3D();
    } else {
        if (hasThreeJS()) startAnimation3D();
        else startAnimation2D();
    }
});

window.initNetwork3D = initNetwork3D;
window.updateNetwork3DData = updateNetwork3DData;
window.stopNetwork3D = stopNetwork3D;
