ï»¿/**
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 * WAVEPWN - Terminal Completo com 100+ Comandos
 * Terminal interativo com execuÃÂ§ÃÂ£o de comandos reais no ESP32
 * ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
 */

// Terminal instance
let terminal = null;
let terminalFitAddon = null;
const terminalHistory = [];
let historyIndex = -1;

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// COMANDOS ORGANIZADOS POR CATEGORIA
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â

const COMMAND_CATEGORIES = {
    'WiFi Attacks': {
        scan: { desc: 'Escanear redes WiFi', handler: () => sendTerminalCommandToESP('wifi_scan') },
        deauth: { desc: 'Deauth flood [bssid]', handler: (a) => sendTerminalCommandToESP('deauth', { target: a[0] }) },
        beacon: { desc: 'Beacon spam [count]', handler: (a) => sendTerminalCommandToESP('beacon', { count: a[0] || 50 }) },
        handshake: { desc: 'Capturar handshake WPA2', handler: () => sendTerminalCommand('handshake') },
        pmkid: { desc: 'Capturar PMKID', handler: () => sendTerminalCommand('pmkid') },
        evil_twin: { desc: 'Clonar rede [ssid]', handler: (a) => sendTerminalCommand('evil_twin', { ssid: a.join(' ') }) },
        karma: { desc: 'Ativar Karma attack', handler: () => sendTerminalCommand('karma') },
        probe_flood: { desc: 'Inundar com probe requests', handler: () => sendTerminalCommand('probe_flood') },
        auth_flood: { desc: 'Flood de autenticaÃÂ§ÃÂ£o', handler: () => sendTerminalCommand('auth_flood') },
        deassoc: { desc: 'Desassociar clientes', handler: () => sendTerminalCommand('deassoc') },
        channel_hop: { desc: 'Alternar canais WiFi', handler: () => sendTerminalCommand('channel_hop') },
        hidden_reveal: { desc: 'Revelar SSIDs ocultos', handler: () => sendTerminalCommand('hidden_reveal') },
        wps_pixie: { desc: 'Ataque WPS Pixie Dust', handler: () => sendTerminalCommand('wps_pixie') },
        wps_brute: { desc: 'ForÃÂ§a bruta WPS PIN', handler: () => sendTerminalCommand('wps_bruteforce') },
        hcxdump: { desc: 'Modo caÃÂ§ador HCX', handler: () => sendTerminalCommand('hcxdump') },
        stop_wifi: { desc: 'Parar ataques WiFi', handler: () => sendTerminalCommand('stop_wifi') },
        networks: { desc: 'Listar redes encontradas', handler: () => sendTerminalCommand('networks') },
        clients: { desc: 'Listar clientes conectados', handler: () => sendTerminalCommand('clients') },
        channel: { desc: 'Mudar/ver canal [ch]', handler: (a) => sendTerminalCommand('channel', { ch: a[0] }) }
    },
    'BLE Attacks': {
        ble_scan: { desc: 'Escanear dispositivos BLE', handler: () => sendTerminalCommand('ble_scan') },
        ble_spam: { desc: 'BLE spam genÃÂ©rico [type]', handler: (a) => sendTerminalCommand('ble_spam', { type: a[0] || 'apple' }) },
        ble_apple: { desc: 'Spam Apple AirPods', handler: () => sendTerminalCommand('ble_spam', { type: 'apple' }) },
        ble_windows: { desc: 'Spam Windows Swift Pair', handler: () => sendTerminalCommand('ble_spam', { type: 'windows' }) },
        ble_samsung: { desc: 'Spam Samsung Buds', handler: () => sendTerminalCommand('ble_spam', { type: 'samsung' }) },
        ble_google: { desc: 'Spam Google Fast Pair', handler: () => sendTerminalCommand('ble_spam', { type: 'google' }) },
        ble_airtag: { desc: 'Spam AirTag falso', handler: () => sendTerminalCommand('ble_spam', { type: 'airtag' }) },
        ble_skimmer: { desc: 'Detectar skimmers BLE', handler: () => sendTerminalCommand('ble_skimmer') },
        ble_tracker: { desc: 'CaÃÂ§ar AirTags/trackers', handler: () => sendTerminalCommand('ble_tracker') },
        ble_clone: { desc: 'Clonar dispositivo BLE', handler: () => sendTerminalCommand('ble_clone') },
        ble_flood: { desc: 'Flood de conexÃÂµes BLE', handler: () => sendTerminalCommand('ble_flood') },
        stop_ble: { desc: 'Parar ataques BLE', handler: () => sendTerminalCommand('stop_ble') }
    },
    'Sistema': {
        status: { desc: 'Status completo do sistema', handler: () => sendTerminalCommand('status') },
        reboot: { desc: 'Reiniciar dispositivo', handler: () => { terminalPrint('\x1b[33mâÅ¡Â Ã¯Â¸Â Reiniciando...\x1b[0m'); setTimeout(() => sendTerminalCommand('reboot'), 2000); } },
        uptime: { desc: 'Tempo ligado', handler: showUptime },
        heap: { desc: 'MemÃÂ³ria heap livre', handler: () => sendTerminalCommand('heap') },
        psram: { desc: 'Status PSRAM', handler: () => sendTerminalCommand('psram') },
        temp: { desc: 'Temperatura CPU', handler: () => sendTerminalCommand('temp') },
        battery: { desc: 'NÃÂ­vel da bateria', handler: () => sendTerminalCommand('battery') },
        version: { desc: 'VersÃÂ£o do firmware', handler: showVersion },
        mac: { desc: 'EndereÃÂ§os MAC', handler: () => sendTerminalCommand('mac') },
        ip: { desc: 'EndereÃÂ§o IP atual', handler: () => sendTerminalCommand('ip') },
        hostname: { desc: 'Nome do host', handler: () => terminalPrint('\x1b[32mHostname: Lele\x1b[0m') },
        factory_reset: { desc: 'Reset para fÃÂ¡brica', handler: () => { terminalPrint('\x1b[31mâÅ¡Â Ã¯Â¸Â USE COM CUIDADO!\x1b[0m'); sendTerminalCommand('factory_reset'); } },
        backup: { desc: 'Fazer backup config', handler: () => sendTerminalCommand('backup') },
        restore: { desc: 'Restaurar backup', handler: () => sendTerminalCommand('restore') },
        deep_sleep: { desc: 'Entrar em deep sleep', handler: () => sendTerminalCommand('deep_sleep') },
        cpu_freq: { desc: 'FrequÃÂªncia da CPU', handler: () => sendTerminalCommand('cpu_freq') },
        sdk_version: { desc: 'VersÃÂ£o do SDK ESP', handler: () => sendTerminalCommand('sdk_version') }
    },
    'Arquivos': {
        ls: { desc: 'Listar arquivos [dir]', handler: (a) => sendTerminalCommand('ls', { path: a[0] || '/' }) },
        cd: { desc: 'Mudar diretÃÂ³rio [dir]', handler: (a) => sendTerminalCommand('cd', { path: a[0] }) },
        pwd: { desc: 'DiretÃÂ³rio atual', handler: () => sendTerminalCommand('pwd') },
        cat: { desc: 'Ver conteÃÂºdo [arquivo]', handler: (a) => sendTerminalCommand('cat', { file: a[0] }) },
        rm: { desc: 'Remover arquivo [arquivo]', handler: (a) => sendTerminalCommand('rm', { file: a[0] }) },
        mkdir: { desc: 'Criar pasta [nome]', handler: (a) => sendTerminalCommand('mkdir', { name: a[0] }) },
        touch: { desc: 'Criar arquivo [nome]', handler: (a) => sendTerminalCommand('touch', { file: a[0] }) },
        mv: { desc: 'Mover [origem] [destino]', handler: (a) => sendTerminalCommand('mv', { src: a[0], dst: a[1] }) },
        cp: { desc: 'Copiar [origem] [destino]', handler: (a) => sendTerminalCommand('cp', { src: a[0], dst: a[1] }) },
        df: { desc: 'EspaÃÂ§o em disco', handler: () => sendTerminalCommand('df') },
        du: { desc: 'Uso de disco [dir]', handler: (a) => sendTerminalCommand('du', { path: a[0] }) },
        head: { desc: 'Primeiras linhas [arquivo]', handler: (a) => sendTerminalCommand('head', { file: a[0] }) },
        tail: { desc: 'ÃÅ¡ltimas linhas [arquivo]', handler: (a) => sendTerminalCommand('tail', { file: a[0] }) }
    },
    'Rede': {
        ping: { desc: 'Ping [host]', handler: (a) => sendTerminalCommand('ping', { host: a[0] }) },
        traceroute: { desc: 'Traceroute [host]', handler: (a) => sendTerminalCommand('traceroute', { host: a[0] }) },
        dns: { desc: 'Resolver DNS [domain]', handler: (a) => sendTerminalCommand('dns', { domain: a[0] }) },
        arp: { desc: 'Tabela ARP', handler: () => sendTerminalCommand('arp') },
        netstat: { desc: 'ConexÃÂµes ativas', handler: () => sendTerminalCommand('netstat') },
        ifconfig: { desc: 'ConfiguraÃÂ§ÃÂ£o de rede', handler: () => sendTerminalCommand('ifconfig') },
        route: { desc: 'Tabela de rotas', handler: () => sendTerminalCommand('route') },
        nslookup: { desc: 'DNS lookup [domain]', handler: (a) => sendTerminalCommand('nslookup', { domain: a[0] }) },
        whois: { desc: 'WHOIS [domain]', handler: (a) => sendTerminalCommand('whois', { domain: a[0] }) },
        port_scan: { desc: 'Scan de portas [ip]', handler: (a) => sendTerminalCommand('port_scan', { ip: a[0] }) },
        http_get: { desc: 'HTTP GET [url]', handler: (a) => sendTerminalCommand('http_get', { url: a[0] }) },
        wifi_connect: { desc: 'Conectar WiFi [ssid] [pass]', handler: (a) => sendTerminalCommand('wifi_connect', { ssid: a[0], pass: a[1] }) }
    },
    'IR Control': {
        ir_send: { desc: 'Enviar cÃÂ³digo IR [nome]', handler: (a) => sendTerminalCommand('ir_send', { name: a.join(' ') }) },
        ir_learn: { desc: 'Aprender cÃÂ³digo IR', handler: () => sendTerminalCommand('ir_learn') },
        ir_list: { desc: 'Listar cÃÂ³digos salvos', handler: () => sendTerminalCommand('ir_list') },
        ir_delete: { desc: 'Deletar cÃÂ³digo [nome]', handler: (a) => sendTerminalCommand('ir_delete', { name: a[0] }) },
        ir_nuke: { desc: 'TV NUKE - todas as TVs', handler: () => { terminalPrint('\x1b[31mâËÂ Ã¯Â¸Â TV NUKE ATIVADO!\x1b[0m'); sendTerminalCommand('ir_nuke'); } },
        ir_test: { desc: 'Testar emissor IR', handler: () => sendTerminalCommand('ir_test') },
        ir_raw: { desc: 'Enviar raw [hex]', handler: (a) => sendTerminalCommand('ir_raw', { data: a[0] }) },
        ir_power: { desc: 'Power [marca]', handler: (a) => sendTerminalCommand('ir_send', { name: 'power', brand: a[0] }) }
    },
    'CÃÂ¢meras': {
        cam_scan: { desc: 'Escanear cÃÂ¢meras IP', handler: () => sendTerminalCommand('cam_scan') },
        cam_attack: { desc: 'Atacar cÃÂ¢mera [ip]', handler: (a) => sendTerminalCommand('cam_attack', { ip: a[0] }) },
        cam_rtsp: { desc: 'Capturar RTSP [ip]', handler: (a) => sendTerminalCommand('cam_rtsp', { ip: a[0] }) },
        cam_onvif: { desc: 'Descobrir ONVIF', handler: () => sendTerminalCommand('cam_onvif') },
        cam_creds: { desc: 'Testar credenciais [ip]', handler: (a) => sendTerminalCommand('cam_creds', { ip: a[0] }) },
        cam_screenshot: { desc: 'Screenshot [ip]', handler: (a) => sendTerminalCommand('cam_screenshot', { ip: a[0] }) }
    },
    'Som': {
        play: { desc: 'Tocar som [nome]', handler: (a) => sendTerminalCommand('play', { sound: a[0] }) },
        say: { desc: 'Falar texto [texto]', handler: (a) => sendTerminalCommand('say', { text: a.join(' ') }) },
        volume: { desc: 'Volume [0-100]', handler: (a) => sendTerminalCommand('volume', { level: a[0] }) },
        mute: { desc: 'Silenciar', handler: () => sendTerminalCommand('mute') },
        beep: { desc: 'Emitir beep', handler: () => sendTerminalCommand('beep') },
        sounds: { desc: 'Listar sons disponÃÂ­veis', handler: () => sendTerminalCommand('sounds') },
        song: { desc: 'Tocar mÃÂºsica [nome]', handler: (a) => sendTerminalCommand('song', { name: a[0] }) }
    },
    'Fun & Easter Eggs': {
        matrix: { desc: 'Modo Matrix', handler: () => { if (typeof setTheme === 'function') setTheme('hacker'); terminalPrint('\x1b[32mWake up, Neo...\x1b[0m'); terminalPrint('\x1b[32mThe Matrix has you...\x1b[0m'); terminalPrint('\x1b[32mFollow the white rabbit. °Å¸Ââ¡\x1b[0m'); } },
        dragon: { desc: 'DragÃÂ£o fala [msg]', handler: (a) => { const msg = a.join(' ') || 'OlÃÂ¡!'; terminalPrint(`\x1b[35m°Å¸ÂÂ² ${msg}\x1b[0m`); if (typeof dragonSay === 'function') dragonSay(msg); } },
        whoami: { desc: 'InformaÃÂ§ÃÂµes do sistema', handler: showWhoami },
        cowsay: { desc: 'Vaca diz [msg]', handler: (a) => showCowsay(a.join(' ') || 'Moo!') },
        fortune: { desc: 'Frase aleatÃÂ³ria', handler: showFortune },
        ascii_art: { desc: 'Arte ASCII [nome]', handler: (a) => showAsciiArt(a[0]) },
        banner: { desc: 'Banner Lele', handler: showBanner },
        rainbow: { desc: 'Texto colorido [msg]', handler: (a) => showRainbow(a.join(' ')) },
        hack: { desc: 'Simular hacking', handler: simulateHacking },
        neofetch: { desc: 'Info estilo neofetch', handler: showNeofetch },
        sl: { desc: 'Steam Locomotive', handler: () => terminalPrint('°Å¸Å¡â°Å¸âÂ¨°Å¸âÂ¨°Å¸âÂ¨ CHOO CHOO!') },
        rickroll: { desc: "Never gonna...", handler: () => terminalPrint('\x1b[35m°Å¸Å½Âµ Never gonna give you up, never gonna let you down...\x1b[0m') },
        lele: { desc: 'Invocar Lele', handler: () => terminalPrint('\x1b[36m°Å¸âÂ¦ Lele aparece!\x1b[0m') },
        lisa: { desc: 'Invocar Lisa', handler: () => terminalPrint('\x1b[35m°Å¸âÂ§ Lisa aparece!\x1b[0m') },
        dance: { desc: 'DanÃÂ§ar', handler: () => terminalPrint('°Å¸âÆ°Å¸â¢Âº°Å¸âÆ°Å¸â¢Âº°Å¸âÆ') },
        fire: { desc: 'Fogo!', handler: () => terminalPrint('°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥°Å¸âÂ¥') },
        explosion: { desc: 'ExplosÃÂ£o', handler: () => terminalPrint('°Å¸âÂ¥ BOOM! °Å¸âÂ¥') },
        pirate: { desc: 'Modo pirata', handler: () => terminalPrint('\x1b[33m°Å¸ÂÂ´ââ¬ÂâËÂ Ã¯Â¸Â Arr matey! Prepare the cannons!\x1b[0m') }
    },
    'Geral': {
        help: { desc: 'Lista de comandos', handler: showHelp },
        clear: { desc: 'Limpar terminal', handler: clearTerminal },
        cls: { desc: 'Limpar terminal (alias)', handler: clearTerminal },
        history: { desc: 'HistÃÂ³rico de comandos', handler: showHistory },
        alias: { desc: 'Ver aliases', handler: showAliases },
        export: { desc: 'Exportar variÃÂ¡vel', handler: () => terminalPrint('\x1b[33mUso: export VAR=value\x1b[0m') },
        env: { desc: 'VariÃÂ¡veis de ambiente', handler: showEnv },
        echo: { desc: 'Imprimir texto [msg]', handler: (a) => terminalPrint(a.join(' ')) },
        date: { desc: 'Data e hora atual', handler: () => terminalPrint(`\x1b[36m°Å¸ââ¦ ${new Date().toLocaleString('pt-BR')}\x1b[0m`) },
        time: { desc: 'Hora atual', handler: () => terminalPrint(`\x1b[36m°Å¸â¢Â ${new Date().toLocaleTimeString('pt-BR')}\x1b[0m`) },
        theme: { desc: 'Mudar tema [nome]', handler: (a) => { if (a[0] && typeof setTheme === 'function') { setTheme(a[0]); terminalPrint(`\x1b[32mâÅâ Tema: ${a[0]}\x1b[0m`); } else { terminalPrint('\x1b[33mTemas: cyberpunk, hacker, stealth, light, dracula, nord, monokai\x1b[0m'); } } },
        stop: { desc: 'Parar todos os ataques', handler: () => sendTerminalCommand('stop') },
        pwn: { desc: 'PWN EVERYTHING', handler: () => { terminalPrint('\x1b[31m°Å¸ââ¬ PWN EVERYTHING!\x1b[0m'); sendTerminalCommand('pwn_all'); } },
        cinema: { desc: 'Modo cinema', handler: () => { if (typeof toggleCinemaMode === 'function') toggleCinemaMode(); } },
        credits: { desc: 'CrÃÂ©ditos do projeto', handler: showCredits },
        about: { desc: 'Sobre o Lele', handler: showAbout },
        exit: { desc: 'Voltar ao dashboard', handler: () => { if (typeof showView === 'function') showView('dashboard'); } },
        man: { desc: 'Manual [comando]', handler: (a) => showMan(a[0]) },
        locate: { desc: 'Localizar [arquivo]', handler: (a) => sendTerminalCommand('locate', { file: a[0] }) },
        grep: { desc: 'Buscar [padrÃÂ£o] [arquivo]', handler: (a) => sendTerminalCommand('grep', { pattern: a[0], file: a[1] }) },
        find: { desc: 'Encontrar [nome]', handler: (a) => sendTerminalCommand('find', { name: a[0] }) },
        top: { desc: 'Processos (simplificado)', handler: () => sendTerminalCommand('top') },
        kill: { desc: 'Matar processo [nome]', handler: (a) => sendTerminalCommand('kill', { name: a[0] }) },
        watch_led: { desc: 'Piscar LED', handler: () => sendTerminalCommand('watch_led') },
        watch_vibrate: { desc: 'Vibrar relÃÂ³gio', handler: () => sendTerminalCommand('watch_vibrate') },
        screenshot: { desc: 'Capturar tela', handler: () => sendTerminalCommand('watch_screenshot') }
    }
};

// Flatten all commands
const COMMANDS = {};
Object.values(COMMAND_CATEGORIES).forEach(cat => {
    Object.entries(cat).forEach(([name, cmd]) => {
        COMMANDS[name] = { description: cmd.desc, handler: cmd.handler };
    });
});

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// FUNÃâ¡Ãâ¢ES AUXILIARES DOS COMANDOS
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â

function showWhoami() {
    terminalPrint('\x1b[36mââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢â\x1b[0m');
    terminalPrint('\x1b[36mââ¢â\x1b[0m  \x1b[1m°Å¸Ââ° Lele\x1b[0m');
    terminalPrint('\x1b[36mââ¢â\x1b[0m  Hardware: ESP32-S3 + AMOLED 1.8"');
    terminalPrint('\x1b[36mââ¢â\x1b[0m  Firmware: v2.0 Cyberpunk Edition');
    terminalPrint('\x1b[36mââ¢â\x1b[0m  AI: NEURA9 TensorFlow Lite');
    terminalPrint('\x1b[36mââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â\x1b[0m');
}

function showVersion() {
    terminalPrint('\x1b[32mLele Firmware v2.0.0\x1b[0m');
    terminalPrint('\x1b[90mBuild: Cyberpunk Edition\x1b[0m');
}

function showUptime() {
    const el = document.getElementById('uptime');
    terminalPrint(`\x1b[32mUptime: ${el ? el.textContent : 'N/A'}\x1b[0m`);
}

function showBanner() {
    terminalPrint('\x1b[35mââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââËââËââ¢â    ââËââËââËââËââËââËââËââ¢âââËââËââ¢â     ââËââËââËââËââËââËââËââ¢âââËââËââ¢â    ââËââËââ¢â ââËââËââËââËââËââ¢â ââËââËââËââËââËââËââËââËââ¢â ââËââËââËââËââËââËââ¢âââËââËââ¢â  ââËââËââ¢â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââËââËââ¢â    ââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â    ââËââËââ¢âââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢âââ¢Å¡ââ¢Âââ¢ÂââËââËââ¢âââ¢Âââ¢Âââ¢ÂââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â  ââËââËââ¢â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââËââËââ¢â    ââËââËââËââËââËââ¢â  ââËââËââ¢â     ââËââËââËââËââËââ¢â  ââËââËââ¢â ââËââ¢â ââËââËââ¢âââËââËââËââËââËââËââËââ¢â   ââËââËââ¢â   ââËââËââ¢â     ââËââËââËââËââËââËââËââ¢â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââËââËââ¢â    ââËââËââ¢âââ¢Âââ¢Âââ¢Â  ââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢Âââ¢Â  ââËââËââ¢âââËââËââËââ¢âââËââËââ¢âââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢â   ââËââËââ¢â   ââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââ¢Å¡ââËââËââËââ¢âââËââËââËââ¢âââ¢ÂââËââËââ¢â  ââËââËââ¢â   ââËââËââ¢â   ââ¢Å¡ââËââËââËââËââËââËââ¢âââËââËââ¢â  ââËââËââ¢â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Â ââ¢Å¡ââ¢Âââ¢Â  ââ¢Å¡ââ¢Âââ¢Â   ââ¢Å¡ââ¢Âââ¢Â    ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Â  ââ¢Å¡ââ¢Âââ¢Â  ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â\x1b[0m');
}

function showCowsay(msg) {
    const len = msg.length;
    terminalPrint(' ' + '_'.repeat(len + 2));
    terminalPrint('< ' + msg + ' >');
    terminalPrint(' ' + '-'.repeat(len + 2));
    terminalPrint('        \\   ^__^');
    terminalPrint('         \\  (oo)\\_______');
    terminalPrint('            (__)\\       )\\/\\');
    terminalPrint('                ||----w |');
    terminalPrint('                ||     ||');
}

function showFortune() {
    const fortunes = [
        '°Å¸âÂ® Grande poder vem com grande responsabilidade.',
        '°Å¸âÂ® O prÃÂ³ximo handshake serÃÂ¡ o mais valioso.',
        '°Å¸âÂ® Cuidado com redes abertas...',
        '°Å¸âÂ® A paciÃÂªncia ÃÂ© a chave do hacker.',
        '°Å¸âÂ® Seu prÃÂ³ximo alvo estÃÂ¡ mais perto do que imagina.',
        '°Å¸âÂ® NÃÂ£o existe sistema inviolÃÂ¡vel. SÃÂ³ falta tentativa.',
        '°Å¸âÂ® O dragÃÂ£o sorri para vocÃÂª.',
        '°Å¸âÂ® WPA3 nÃÂ£o ÃÂ© pÃÂ¡reo para vocÃÂª.',
        '°Å¸âÂ® Os bits estÃÂ£o ao seu favor hoje.'
    ];
    terminalPrint(`\x1b[35m${fortunes[Math.floor(Math.random() * fortunes.length)]}\x1b[0m`);
}

function showAsciiArt(name) {
    if (name === 'dragon' || !name) {
        terminalPrint('\x1b[32m     ____ __');
        terminalPrint('    { --.\\  |          .)%%%)%%');
        terminalPrint('     \'-._\\\\ | (\\___   %%)%%%)%%%');
        terminalPrint('         `\\|(__/  \\(_(%%%)%%)%%');
        terminalPrint('          /|       \\ \'%%)%%');
        terminalPrint('         / |        `\x1b[0m');
    } else if (name === 'skull') {
        terminalPrint('\x1b[31m     ______');
        terminalPrint('  .-"      "-.');
        terminalPrint(' /            \\');
        terminalPrint('|  O        O  |');
        terminalPrint('|      \\__/    |');
        terminalPrint(' \\   \\____/   /');
        terminalPrint('  "-._____.-"\x1b[0m');
    } else {
        terminalPrint('\x1b[33mArte nÃÂ£o encontrada. Tente: dragon, skull\x1b[0m');
    }
}

function showRainbow(msg) {
    const colors = ['\x1b[31m', '\x1b[33m', '\x1b[32m', '\x1b[36m', '\x1b[34m', '\x1b[35m'];
    let result = '';
    for (let i = 0; i < msg.length; i++) {
        result += colors[i % colors.length] + msg[i];
    }
    terminalPrint(result + '\x1b[0m');
}

function simulateHacking() {
    const lines = [
        '\x1b[32m[*] Iniciando varredura...\x1b[0m',
        '\x1b[32m[*] Detectando vulnerabilidades...\x1b[0m',
        '\x1b[33m[!] Firewall detectado... bypassing...\x1b[0m',
        '\x1b[32m[*] Injetando payload...\x1b[0m',
        '\x1b[32m[*] Escalando privilÃÂ©gios...\x1b[0m',
        '\x1b[32m[+] ROOT ACCESS OBTAINED!\x1b[0m',
        '\x1b[31m°Å¸ââ¬ SISTEMA COMPROMETIDO °Å¸ââ¬\x1b[0m'
    ];
    let i = 0;
    const interval = setInterval(() => {
        if (i < lines.length) {
            terminalPrint(lines[i++]);
        } else {
            clearInterval(interval);
        }
    }, 500);
}

function showNeofetch() {
    terminalPrint('\x1b[36m       _____      \x1b[0m  \x1b[33mLele\x1b[0m');
    terminalPrint('\x1b[36m      /     \\     \x1b[0m  âââ¬âââ¬âââ¬âââ¬âââ¬âââ¬âââ¬âââ¬âââ¬');
    terminalPrint('\x1b[36m     | () () |    \x1b[0m  \x1b[36mOS:\x1b[0m FreeRTOS');
    terminalPrint('\x1b[36m      \\  ââÂ½  /     \x1b[0m  \x1b[36mHost:\x1b[0m ESP32-S3');
    terminalPrint('\x1b[36m       \\___/      \x1b[0m  \x1b[36mKernel:\x1b[0m ESP-IDF 5.x');
    terminalPrint('\x1b[36m        | |       \x1b[0m  \x1b[36mUptime:\x1b[0m ' + (document.getElementById('uptime')?.textContent || 'N/A'));
    terminalPrint('\x1b[36m       (_____)    \x1b[0m  \x1b[36mRAM:\x1b[0m ' + (document.getElementById('ram-used')?.textContent || 'N/A'));
}

function showHistory() {
    if (terminalHistory.length === 0) {
        terminalPrint('\x1b[33mHistÃÂ³rico vazio\x1b[0m');
        return;
    }
    terminalPrint('\x1b[36m°Å¸âÅ HistÃÂ³rico de Comandos:\x1b[0m');
    terminalHistory.slice(-20).forEach((cmd, i) => {
        terminalPrint(`  ${i + 1}. ${cmd}`);
    });
}

function showAliases() {
    terminalPrint('\x1b[36m°Å¸ââ Aliases:\x1b[0m');
    terminalPrint('  cls ââ â clear');
    terminalPrint('  q ââ â exit');
    terminalPrint('  scan ââ â wifi_scan');
    terminalPrint('  nuke ââ â ir_nuke');
}

function showEnv() {
    terminalPrint('\x1b[36m°Å¸ÅÂ Environment:\x1b[0m');
    terminalPrint('  SHELL=/bin/leleterm');
    terminalPrint('  USER=dragon');
    terminalPrint('  HOME=/sd');
    terminalPrint('  LANG=pt_BR.UTF-8');
    terminalPrint('  THEME=' + (document.documentElement.getAttribute('data-theme') || 'cyberpunk'));
}

function showCredits() {
    terminalPrint('\x1b[35mââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â       Lele CREDITS         ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢Â ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â£\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  Developer: Lele              ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  AI Assistant: Antigravity     ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢â  Mascots: Neura, Lele, Lisa    ââ¢â\x1b[0m');
    terminalPrint('\x1b[35mââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â\x1b[0m');
}

function showAbout() {
    terminalPrint('\x1b[36m°Å¸Ââ° Lele v2.0 - Cyberpunk Edition\x1b[0m');
    terminalPrint('');
    terminalPrint('Dispositivo multifunÃÂ§ÃÂ£o para anÃÂ¡lise de seguranÃÂ§a');
    terminalPrint('WiFi ââ¬Â¢ BLE ââ¬Â¢ IR ââ¬Â¢ CÃÂ¢meras ââ¬Â¢ AI');
    terminalPrint('');
    terminalPrint('\x1b[90mPowered by ESP32-S3 + LVGL + TensorFlow Lite\x1b[0m');
}

function showMan(cmd) {
    if (!cmd) {
        terminalPrint('\x1b[33mUso: man [comando]\x1b[0m');
        return;
    }
    if (COMMANDS[cmd]) {
        terminalPrint(`\x1b[36m${cmd.toUpperCase()}(1)\x1b[0m`);
        terminalPrint('');
        terminalPrint(`  \x1b[1mNOME\x1b[0m`);
        terminalPrint(`    ${cmd} - ${COMMANDS[cmd].description}`);
        terminalPrint('');
        terminalPrint(`  \x1b[1mUSO\x1b[0m`);
        terminalPrint(`    ${cmd} [argumentos...]`);
    } else {
        terminalPrint(`\x1b[31mComando nÃÂ£o encontrado: ${cmd}\x1b[0m`);
    }
}

// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â
// TERMINAL CORE
// ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â

function initTerminal() {
    const terminalOutput = document.getElementById('terminal-output');
    if (!terminalOutput) return;
    showWelcome();
}

function showWelcome() {
    const output = document.getElementById('terminal-output');
    if (!output) return;
    output.innerHTML = `
<div style="color:#00fff5">ââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââËââËââ¢â    ââËââËââËââËââËââËââËââ¢âââËââËââ¢â     ââËââËââËââËââËââËââËââ¢âââËââËââ¢â    ââËââËââ¢â ââËââËââËââËââËââ¢â ââËââËââËââËââËââËââËââËââ¢â ââËââËââËââËââËââËââ¢âââËââËââ¢â  ââËââËââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââËââËââ¢â    ââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â    ââËââËââ¢âââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢âââ¢Å¡ââ¢Âââ¢ÂââËââËââ¢âââ¢Âââ¢Âââ¢ÂââËââËââ¢âââ¢Âââ¢Âââ¢Âââ¢Âââ¢ÂââËââËââ¢â  ââËââËââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââËââËââ¢â    ââËââËââËââËââËââ¢â  ââËââËââ¢â     ââËââËââËââËââËââ¢â  ââËââËââ¢â ââËââ¢â ââËââËââ¢âââËââËââËââËââËââËââËââ¢â   ââËââËââ¢â   ââËââËââ¢â     ââËââËââËââËââËââËââËââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââËââËââ¢â    ââËââËââ¢âââ¢Âââ¢Âââ¢Â  ââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢Âââ¢Â  ââËââËââ¢âââËââËââËââ¢âââËââËââ¢âââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢â   ââËââËââ¢â   ââËââËââ¢â     ââËââËââ¢âââ¢Âââ¢ÂââËââËââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââËââËââËââËââËââËââËââ¢âââ¢Å¡ââËââËââËââ¢âââËââËââËââ¢âââ¢ÂââËââËââ¢â  ââËââËââ¢â   ââËââËââ¢â   ââ¢Å¡ââËââËââËââËââËââËââ¢âââËââËââ¢â  ââËââËââ¢â</div>
<div style="color:#ff00ff">ââ¢â  ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Âââ¢Â ââ¢Å¡ââ¢Âââ¢Â  ââ¢Å¡ââ¢Âââ¢Â   ââ¢Å¡ââ¢Âââ¢Â    ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Å¡ââ¢Âââ¢Â  ââ¢Å¡ââ¢Âââ¢Â</div>
<div style="color:#00fff5">ââ¢Å¡ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â</div>
<div></div>
<div style="color:#ffcc00">  °Å¸Ââ° Lele Terminal v2.0 - ${Object.keys(COMMANDS).length} comandos disponÃÂ­veis</div>
<div style="color:#a0a0b0">  Digite "help" para ver todos os comandos</div>
<div>---</div>`;
}

function terminalPrint(text) {
    const output = document.getElementById('terminal-output');
    if (!output) return;
    const div = document.createElement('div');
    div.innerHTML = text.replace(/\x1b\[(\d+)m/g, (m, code) => {
        const colors = { '0': '', '31': 'color:#ff0055', '32': 'color:#00ff9d', '33': 'color:#ffcc00', '34': 'color:#00a8ff', '35': 'color:#ff00ff', '36': 'color:#00fff5', '90': 'color:#606070', '1': 'font-weight:bold' };
        return colors[code] ? `</span><span style="${colors[code]}">` : '';
    });
    output.appendChild(div);
    output.scrollTop = output.scrollHeight;
}

function clearTerminal() {
    const output = document.getElementById('terminal-output');
    if (output) output.innerHTML = '';
    showWelcome();
}

function sendTerminalCommand() {
    const input = document.getElementById('terminal-input');
    if (!input) return;

    const cmdLine = input.value.trim();
    input.value = '';

    if (!cmdLine) return;

    // Add to history
    terminalHistory.push(cmdLine);
    historyIndex = terminalHistory.length;

    // Echo command
    terminalPrint(`<span style="color:#00ff9d">$</span> ${cmdLine}`);

    // Parse command
    const parts = cmdLine.split(/\s+/);
    const cmd = parts[0].toLowerCase();
    const args = parts.slice(1);

    // Execute
    if (COMMANDS[cmd]) {
        try {
            COMMANDS[cmd].handler(args);
        } catch (e) {
            terminalPrint(`<span style="color:#ff0055">Erro: ${e.message}</span>`);
        }
    } else {
        // If not a local UI command, send directly to ESP32
        sendTerminalCommandToESP(cmd, { args: args.join(' ') });
    }
}

// WaveSocket Integration
function initSocketTerminal() {
    // Listen for terminal output from backend
    ws.on('term_out', (line) => {
        // Handle ANSI colors if backend sends them, or plain text
        terminalPrint(line);
    });
}

function sendTerminalCommandToESP(command, params = {}) {
    // Send via WebSocket instead of HTTP POST
    // Backend expects { cmd: 'term_input', line: 'command args...' } for shell
    // OR we can map specific commands to their handlers if the backend supports it.

    // For general terminal input simulation:
    const fullCmd = params.line || (command + (params ? ' ' + Object.values(params).join(' ') : ''));

    // Echo locally is already done by sendTerminalCommand, but good to be sure.
    // terminalPrint(`[WS] > ${fullCmd}`); 

    ws.send('term_input', { line: fullCmd });
}

// Initialize socket listeners
document.addEventListener('DOMContentLoaded', initSocketTerminal);

function showHelp() {
    terminalPrint('');
    terminalPrint(`<span style="color:#00fff5;font-weight:bold">°Å¸ââ¹ COMANDOS DISPONÃÂVEIS (${Object.keys(COMMANDS).length} total)</span>`);
    terminalPrint('<span style="color:#00fff5">ââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Âââ¢Â</span>');

    Object.entries(COMMAND_CATEGORIES).forEach(([category, cmds]) => {
        terminalPrint('');
        terminalPrint(`<span style="color:#ff00ff;font-weight:bold">ââÂ¸ ${category.toUpperCase()}</span>`);
        Object.entries(cmds).forEach(([name, cmd]) => {
            terminalPrint(`  <span style="color:#00ff9d">${name.padEnd(16)}</span> ${cmd.desc}`);
        });
    });

    terminalPrint('');
    terminalPrint('<span style="color:#ffcc00">°Å¸âÂ¡ Use TAB para auto-completar, ââ âââ â para histÃÂ³rico</span>');
}

// Handle keyboard events
document.addEventListener('keydown', (e) => {
    const input = document.getElementById('terminal-input');
    if (!input || document.activeElement !== input) return;

    if (e.key === 'ArrowUp') {
        e.preventDefault();
        if (historyIndex > 0) {
            historyIndex--;
            input.value = terminalHistory[historyIndex] || '';
        }
    } else if (e.key === 'ArrowDown') {
        e.preventDefault();
        if (historyIndex < terminalHistory.length - 1) {
            historyIndex++;
            input.value = terminalHistory[historyIndex] || '';
        } else {
            historyIndex = terminalHistory.length;
            input.value = '';
        }
    } else if (e.key === 'Tab') {
        e.preventDefault();
        const val = input.value.toLowerCase();
        const matches = Object.keys(COMMANDS).filter(c => c.startsWith(val));
        if (matches.length === 1) {
            input.value = matches[0] + ' ';
        } else if (matches.length > 1) {
            terminalPrint(`<span style="color:#a0a0b0">${matches.join('  ')}</span>`);
        }
    }
});

// Initialize on page load
document.addEventListener('DOMContentLoaded', initTerminal);

// Export functions
window.sendTerminalCommand = sendTerminalCommand;
window.terminalPrint = terminalPrint;
window.clearTerminal = clearTerminal;
window.showHelp = showHelp;

