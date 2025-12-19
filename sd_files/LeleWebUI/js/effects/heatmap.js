/**
 * WAVEPWN - WiFi Heatmap (Dicas 9, 22)
 * Mapa de calor visual de redes WiFi por canal
 */

const WiFiHeatmap = {
    canvas: null,
    ctx: null,
    networks: [],
    animationId: null
};

function initWiFiHeatmap(canvasId) {
    WiFiHeatmap.canvas = document.getElementById(canvasId);
    if (!WiFiHeatmap.canvas) return;
    WiFiHeatmap.ctx = WiFiHeatmap.canvas.getContext('2d');
    resizeHeatmap();
    window.addEventListener('resize', resizeHeatmap);
    startHeatmapAnimation();
}

function resizeHeatmap() {
    if (!WiFiHeatmap.canvas) return;
    const parent = WiFiHeatmap.canvas.parentElement;
    WiFiHeatmap.canvas.width = parent.offsetWidth;
    WiFiHeatmap.canvas.height = parent.offsetHeight || 300;
}

function updateHeatmapData(networks) {
    WiFiHeatmap.networks = networks || [];
}

let heatmapLastDrawTime = 0;
const heatmapFpsInterval = 1000 / 30; // Limit to 30 FPS

function startHeatmapAnimation() {
    if (!WiFiHeatmap.animationId) {
        heatmapLastDrawTime = performance.now();
        animate();

        // Start polling for data
        WiFiHeatmap.pollInterval = setInterval(() => {
            if (WavePwn && WavePwn.ws && WavePwn.ws.readyState === WebSocket.OPEN) {
                WavePwn.ws.send(JSON.stringify({ cmd: 'get_heatmap' }));
            }
        }, 1000);
    }
}

function animate(timestamp) {
    if (!WiFiHeatmap.ctx) return;

    // Resume loop
    WiFiHeatmap.animationId = requestAnimationFrame(animate);

    const elapsed = timestamp - heatmapLastDrawTime;
    if (elapsed > heatmapFpsInterval) {
        heatmapLastDrawTime = timestamp - (elapsed % heatmapFpsInterval);
        drawHeatmap();
    }
}

function drawHeatmap() {
    const { canvas, ctx, networks } = WiFiHeatmap;
    if (!ctx) return;

    const w = canvas.width;
    const h = canvas.height;

    // Clear with dark background
    ctx.fillStyle = 'rgba(10, 10, 15, 0.1)';
    ctx.fillRect(0, 0, w, h);

    // Draw channel grid
    ctx.strokeStyle = 'rgba(0, 255, 245, 0.1)';
    ctx.lineWidth = 1;
    for (let i = 1; i <= 14; i++) {
        const x = (i / 15) * w;
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, h);
        ctx.stroke();
        ctx.fillStyle = 'rgba(0, 255, 245, 0.3)';
        ctx.font = '10px monospace';
        ctx.fillText(`CH${i}`, x - 10, h - 5);
    }

    // Check if we have channel counts (Array of numbers) or Network Objects
    if (networks.length > 0 && typeof networks[1] === 'number') {
        // Mode 1: Channel Activity Counts (Int Array indices 1-14)
        for (let i = 1; i <= 14; i++) {
            const count = networks[i] || 0;
            if (count > 0) {
                const x = (i / 15) * w;
                // Log scale for visual
                const intensity = Math.min(100, Math.log10(count + 1) * 30);
                const radius = 20 + intensity * 0.8;

                const gradient = ctx.createRadialGradient(x, h / 2, 0, x, h / 2, radius);
                gradient.addColorStop(0, `hsla(${200 - intensity * 2}, 100%, 50%, 0.8)`);
                gradient.addColorStop(0.5, `hsla(${200 - intensity * 2}, 100%, 50%, 0.3)`);
                gradient.addColorStop(1, `hsla(${200 - intensity * 2}, 100%, 50%, 0)`);

                ctx.fillStyle = gradient;
                ctx.beginPath();
                ctx.arc(x, h / 2, radius, 0, Math.PI * 2);
                ctx.fill();

                // Count label
                ctx.fillStyle = '#fff';
                ctx.font = '10px sans-serif';
                ctx.fillText(count, x - 5, h / 2);
            }
        }
    } else {
        // Mode 2: Network Objects List
        networks.forEach((net, idx) => {
            const channel = net.channel || 6;
            const rssi = net.rssi || -70;
            const x = (channel / 15) * w;
            const strength = Math.min(100, Math.max(0, 100 + rssi));
            const radius = 20 + (strength / 100) * 60;

            // Gradient blob
            const gradient = ctx.createRadialGradient(x, h / 2, 0, x, h / 2, radius);
            const hue = (idx * 30) % 360;
            gradient.addColorStop(0, `hsla(${hue}, 100%, 50%, 0.8)`);
            gradient.addColorStop(0.5, `hsla(${hue}, 100%, 50%, 0.3)`);
            gradient.addColorStop(1, `hsla(${hue}, 100%, 50%, 0)`);

            ctx.fillStyle = gradient;
            ctx.beginPath();
            ctx.arc(x, h / 2 + Math.sin(Date.now() / 1000 + idx) * 10, radius, 0, Math.PI * 2);
            ctx.fill();

            // SSID label
            if (net.ssid) {
                ctx.fillStyle = '#fff';
                ctx.font = '11px sans-serif';
                ctx.fillText(net.ssid.substring(0, 12), x - 30, h / 2);
            }
        });
    }
}

function stopHeatmap() {
    if (WiFiHeatmap.animationId) {
        cancelAnimationFrame(WiFiHeatmap.animationId);
        WiFiHeatmap.animationId = null;
    }
    if (WiFiHeatmap.pollInterval) {
        clearInterval(WiFiHeatmap.pollInterval);
        WiFiHeatmap.pollInterval = null;
    }
}

// Visibility API
document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
        stopHeatmap();
    } else {
        startHeatmapAnimation();
    }
});

window.initWiFiHeatmap = initWiFiHeatmap;
window.updateHeatmapData = updateHeatmapData;
window.stopHeatmap = stopHeatmap;
