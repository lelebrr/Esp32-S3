/**
 * WaveSocket - Unified WebSocket Communication Layer for WavePwn
 * Handles automatic reconnection, message dispatching, and offline queuing.
 */
class WaveSocket {
    constructor() {
        this.socket = null;
        this.isConnected = false;
        this.reconnectTimer = null;
        this.handlers = {}; // Event handlers: { 'event_name': [callbacks] }
        this.host = window.location.hostname || "192.168.4.1"; // Fallback for file://
        this.port = 81;

        // Singleton instance
        if (window.waveSocket) return window.waveSocket;
        window.waveSocket = this;
    }

    connect() {
        if (this.socket && (this.socket.readyState === WebSocket.OPEN || this.socket.readyState === WebSocket.CONNECTING)) return;

        console.log(`[WaveSocket] Connecting to ws://${this.host}:${this.port}...`);
        this.socket = new WebSocket(`ws://${this.host}:${this.port}`);

        this.socket.onopen = () => {
            console.log("[WaveSocket] Connected!");
            this.isConnected = true;

            // Sync Time with Backend (Essential for TOTP/Logs)
            this.send("sync_time", { timestamp: Date.now() });

            this.emit('connected');
            if (this.reconnectTimer) {
                clearInterval(this.reconnectTimer);
                this.reconnectTimer = null;
            }
        };

        this.socket.onclose = () => {
            console.log("[WaveSocket] Disconnected.");
            this.isConnected = false;
            this.emit('disconnected');
            this.scheduleReconnect();
        };

        this.socket.onerror = (err) => {
            console.error("[WaveSocket] Error:", err);
            this.isConnected = false;
        };

        this.socket.onmessage = (event) => {
            try {
                const msg = JSON.parse(event.data);
                this.dispatch(msg);
            } catch (e) {
                console.error("[WaveSocket] Invalid JSON:", event.data);
            }
        };
    }

    scheduleReconnect() {
        if (!this.reconnectTimer) {
            this.reconnectTimer = setInterval(() => {
                this.connect();
            }, 3000);
        }
    }

    /**
     * Send a command to the ESP32
     * @param {string} cmd - Command name (e.g., 'wifi_scan')
     * @param {object} params - Parameters object
     */
    send(cmd, params = {}) {
        if (!this.isConnected) {
            console.warn("[WaveSocket] Cannot send (disconnected):", cmd);
            this.emit('error', { message: 'Not connected' });
            return;
        }
        const payload = JSON.stringify({ cmd: cmd, ...params });
        this.socket.send(payload);
    }

    /**
     * Subscribe to a specific message type or 'all'
     * @param {string} event - Property to listen for in JSON (e.g., 'log', 'networks', 'term_out')
     * @param {function} callback - Function(data)
     */
    on(event, callback) {
        if (!this.handlers[event]) this.handlers[event] = [];
        this.handlers[event].push(callback);
    }

    emit(event, data) {
        if (this.handlers[event]) {
            this.handlers[event].forEach(cb => cb(data));
        }
    }

    dispatch(msg) {
        // Dispatch based on keys present in the message
        Object.keys(msg).forEach(key => {
            if (this.handlers[key]) {
                this.handlers[key].forEach(cb => cb(msg[key], msg));
            }
        });

        // Also emit 'message' for raw handling
        this.emit('message', msg);
    }
}

// Global accessor
const ws = new WaveSocket();
