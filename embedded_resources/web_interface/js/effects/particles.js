/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Floating Particles Effect
 * Mouse-following particles for interactive background
 * ═══════════════════════════════════════════════════════════════════════════
 */

const Particles = {
    canvas: null,
    ctx: null,
    particles: [],
    mouse: { x: 0, y: 0 },
    maxParticles: 50,
    initialized: false
};

function initParticles() {
    Particles.canvas = document.getElementById('particles-canvas');
    if (!Particles.canvas) {
        const container = document.getElementById('background-effects');
        if (!container) return;

        Particles.canvas = document.createElement('canvas');
        Particles.canvas.id = 'particles-canvas';
        container.appendChild(Particles.canvas);
    }

    Particles.ctx = Particles.canvas.getContext('2d');

    function resize() {
        Particles.canvas.width = window.innerWidth;
        Particles.canvas.height = window.innerHeight;
    }
    resize();
    window.addEventListener('resize', resize);

    // Track mouse
    document.addEventListener('mousemove', (e) => {
        Particles.mouse.x = e.clientX;
        Particles.mouse.y = e.clientY;

        // Spawn particle on mouse move
        if (Particles.particles.length < Particles.maxParticles && Math.random() > 0.7) {
            spawnParticle(e.clientX, e.clientY);
        }
    });

    Particles.initialized = true;
    startAnimation();

    console.log('[Particles] Effect initialized (Optimized: rAF + Visibility)');
}

function spawnParticle(x, y) {
    const colors = ['#00fff5', '#ff00ff', '#bd00ff', '#00ff9d'];

    Particles.particles.push({
        x: x,
        y: y,
        vx: (Math.random() - 0.5) * 2,
        vy: (Math.random() - 0.5) * 2 - 1,
        size: Math.random() * 4 + 2,
        color: colors[Math.floor(Math.random() * colors.length)],
        alpha: 1,
        decay: 0.01 + Math.random() * 0.02
    });
}

let animationId = null;
let lastDrawTime = 0;
const fpsInterval = 1000 / 30; // Limit to 30 FPS

function animateParticles(timestamp) {
    if (!Particles.ctx) return;
    if (!animationId) return; // Stopped

    // Calc elapsed time since last loop
    const elapsed = timestamp - lastDrawTime;

    // If enough time has passed, draw next frame
    if (elapsed > fpsInterval) {
        lastDrawTime = timestamp - (elapsed % fpsInterval);

        // Clear canvas
        Particles.ctx.clearRect(0, 0, Particles.canvas.width, Particles.canvas.height);

        // Update and draw particles
        for (let i = Particles.particles.length - 1; i >= 0; i--) {
            const p = Particles.particles[i];

            // Update position
            p.x += p.vx;
            p.y += p.vy;

            // Fade out
            p.alpha -= p.decay;

            // Remove dead particles
            if (p.alpha <= 0) {
                Particles.particles.splice(i, 1);
                continue;
            }

            // Draw particle
            Particles.ctx.save();
            Particles.ctx.globalAlpha = p.alpha;
            Particles.ctx.fillStyle = p.color;
            Particles.ctx.shadowColor = p.color;
            Particles.ctx.shadowBlur = 10;

            Particles.ctx.beginPath();
            Particles.ctx.arc(p.x, p.y, p.size, 0, Math.PI * 2);
            Particles.ctx.fill();

            Particles.ctx.restore();
        }

        // Draw connection lines to mouse
        if (Particles.particles.length > 0) {
            Particles.ctx.strokeStyle = 'rgba(0, 255, 245, 0.1)';
            Particles.ctx.lineWidth = 1;

            Particles.particles.slice(0, 10).forEach(p => {
                const dist = Math.hypot(p.x - Particles.mouse.x, p.y - Particles.mouse.y);
                if (dist < 150) {
                    Particles.ctx.beginPath();
                    Particles.ctx.moveTo(p.x, p.y);
                    Particles.ctx.lineTo(Particles.mouse.x, Particles.mouse.y);
                    Particles.ctx.stroke();
                }
            });
        }
    }

    requestAnimationFrame(animateParticles);
}

function startAnimation() {
    if (!animationId) {
        lastDrawTime = performance.now();
        animationId = requestAnimationFrame(animateParticles);
    }
}

function stopAnimation() {
    if (animationId) {
        cancelAnimationFrame(animationId);
        animationId = null;
    }
}

// Visibility API
document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
        stopAnimation();
    } else {
        startAnimation();
    }
});

// Export
window.initParticles = initParticles;
