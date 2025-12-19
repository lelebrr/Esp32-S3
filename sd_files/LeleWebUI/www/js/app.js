ï»¿function app() {
  return {
    loading: true,
    progress: 0,
    connected: false,
    sidebarOpen: window.innerWidth > 768,
    currentTab: 'dashboard',
    theme: localStorage.getItem('theme') || 'dark',
    status: {
      uptime: 0,
      battery: 0,
      rssi: -60,
      handshakes: 0,
      networks: 0
    },
    menuItems: [
      { id: 'dashboard', label: 'Dashboard', icon: '°Å¸âÅ ' },
      { id: 'attacks', label: 'Attacks', icon: 'âÅ¡âÃ¯Â¸Â' },
      { id: 'terminal', label: 'Terminal', icon: '°Å¸âÂ»' },
      { id: 'settings', label: 'Settings', icon: 'âÅ¡â¢Ã¯Â¸Â' }
    ],
    socket: null,
    term: null,
    dragonMessage: 'Awakening...',

    init() {
      console.log('Lele App Starting...');
      this.simulateLoading();
      this.connectWS();
      this.initTerminal();
      window.addEventListener('resize', () => {
        this.sidebarOpen = window.innerWidth > 768;
      });
    },

    simulateLoading() {
      let p = 0;
      const int = setInterval(() => {
        p += Math.random() * 10;
        if (p > 100) {
          p = 100;
          clearInterval(int);
          setTimeout(() => { this.loading = false; }, 500);
        }
        this.progress = p;
      }, 100);
    },

    connectWS() {
      const proto = window.location.protocol === 'https:' ? 'wss' : 'ws';
      const url = `${proto}://${window.location.host}/ws`;
      console.log('Connecting to WS:', url);

      this.socket = new WebSocket(url);

      this.socket.onopen = () => {
        console.log('WS Connected');
        this.connected = true;
      };

      this.socket.onclose = () => {
        console.log('WS Disconnected');
        this.connected = false;
        setTimeout(() => this.connectWS(), 3000);
      };

      this.socket.onmessage = (e) => {
        try {
          const data = JSON.parse(e.data);
          if (data.uptime) this.status.uptime = data.uptime;
          if (data.battery) this.status.battery = data.battery;
          if (data.hs) this.status.handshakes = data.hs;
          if (data.aps) this.status.networks = data.aps;
          if (data.rssi) this.status.rssi = data.rssi;
          if (data.mood) this.dragonMessage = data.mood;
        } catch (err) {
          console.error('WS Parse Error', err);
        }
      };
    },

    initTerminal() {
      if (this.term) return;
      try {
        this.term = new Terminal({
          cursorBlink: true,
          theme: {
            background: '#0a0a0a',
            foreground: '#00ff00'
          }
        });
        const fitAddon = new FitAddon.FitAddon();
        this.term.loadAddon(fitAddon);
        this.term.open(document.getElementById('terminal-container'));
        fitAddon.fit();

        this.term.writeln('Welcome to Lele Terminal v1.0');
        this.term.writeln('Type "help" for commands.');
        this.term.write('\r\n$ ');

        let currLine = '';
        this.term.onKey(e => {
          const printable = !e.domEvent.altKey && !e.domEvent.altGraphKey && !e.domEvent.ctrlKey && !e.domEvent.metaKey;

          if (e.domEvent.keyCode === 13) { // Enter
            this.term.write('\r\n');
            this.handleCommand(currLine);
            currLine = '';
            this.term.write('$ ');
          } else if (e.domEvent.keyCode === 8) { // Backspace
            if (currLine.length > 0) {
              currLine = currLine.slice(0, -1);
              this.term.write('\b \b');
            }
          } else if (printable) {
            currLine += e.key;
            this.term.write(e.key);
          }
        });
      } catch (e) {
        console.warn('Terminal init failed', e);
      }
    },

    async handleCommand(cmd) {
      console.log('CMD:', cmd);
      this.term.writeln('Sending: ' + cmd + '...');

      try {
        const res = await fetch('/api/terminal', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: `cmd=${encodeURIComponent(cmd)}`
        });
        const data = await res.json();
        this.term.writeln(data.output || 'OK');
      } catch (e) {
        this.term.writeln('Error sending command');
      }
    },

    async toggleAttack(type) {
      fetch('/api/attack/start', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `type=${type}`
      });
    },

    async stopAll() {
      fetch('/api/stop', { method: 'POST' });
    },

    setTheme(t) {
      this.theme = t;
      localStorage.setItem('theme', t);
      document.documentElement.setAttribute('data-theme', t);
    },

    formatUptime(s) {
      const h = Math.floor(s / 3600);
      const m = Math.floor((s % 3600) / 60);
      const sec = s % 60;
      return `${h}:${m.toString().padStart(2, '0')}:${sec.toString().padStart(2, '0')}`;
    }
  }
}
