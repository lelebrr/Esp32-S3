/**
 * Hardware Glitcher Module
 * Handles MOSFET fault injection commands
 */

const HW = {
    // Send complex command helper
    send: function (cmd, params = {}) {
        if (typeof ws === 'undefined' || ws.readyState !== WebSocket.OPEN) {
            if (typeof log !== 'undefined') log('[HW] Error: WebSocket not connected', 'err');
            return;
        }

        const payload = { cmd: cmd, ...params };
        ws.send(JSON.stringify(payload));
        if (typeof log !== 'undefined') log(`> HW: ${cmd}`, 'info');
    },

    // Trigger single pulse
    triggerSingle: function () {
        const width = document.getElementById('hw-width')?.value || 100;
        this.send('hw_single', { width: parseInt(width) });
    },

    // Trigger burst
    triggerBurst: function () {
        const count = document.getElementById('hw-count')?.value || 5;
        const delay = document.getElementById('hw-delay')?.value || 100;
        this.send('hw_burst', { count: parseInt(count), delay: parseInt(delay) });
    },

    // Trigger calibration sweep
    triggerSweep: function () {
        this.send('hw_sweep');
    },

    // Start continuous glitching (if needed in future)
    startContinuous: function () {
        this.send('hw_continuous_start');
    },

    stop: function () {
        this.send('stop_hw');
    },

    // Update UI values (helper for onchange)
    updateVals: function () {
        // Here we could validate inputs or sync
        // For now, the inputs are the source of truth
    }
};

console.log("[HW] Module Loaded");
