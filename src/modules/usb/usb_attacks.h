/**
 * @file usb_attacks.h
 * @brief Módulo de Ataques USB (BadUSB, HID Injection)
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Este módulo implementa ataques via USB:
 * - BadUSB / Rubber Ducky: Emulação de teclado HID
 * - HID Injection: Injeção de keystroke
 * - USB Mass Storage Attack: Payloads em mass storage
 * - Keystroke Injection: Comandos rápidos
 *
 * @warning Use apenas para fins educacionais e em sistemas autorizados.
 */

#ifndef __USB_ATTACKS_H__
#define __USB_ATTACKS_H__

#include <Arduino.h>

// ============================================================================
// ENUMS E CONSTANTES
// ============================================================================

/**
 * @brief Tipos de ataque USB
 */
enum USBAttackType {
    USB_BADUSB = 0,     ///< BadUSB / Rubber Ducky
    USB_HID_INJECTION,  ///< HID Keystroke Injection
    USB_MASS_STORAGE,   ///< Mass Storage Attack
    USB_PAYLOAD_INJECT, ///< Payload Injection
    USB_KEYSTROKE_FAST  ///< Fast Keystroke
};

/**
 * @brief Payloads pré-definidos
 */
enum USBPayload {
    PAYLOAD_REVERSE_SHELL = 0, ///< Reverse Shell Windows
    PAYLOAD_WIFI_STEALER,      ///< Roubar senhas WiFi
    PAYLOAD_CREDENTIAL_DUMP,   ///< Dump de credenciais
    PAYLOAD_DISABLE_AV,        ///< Desabilitar antivírus
    PAYLOAD_EXFIL_DOCS,        ///< Exfiltrar documentos
    PAYLOAD_RANSOMWARE_SIM,    ///< Simulação de ransomware
    PAYLOAD_KEYLOGGER,         ///< Instalar keylogger
    PAYLOAD_BACKDOOR,          ///< Instalar backdoor
    PAYLOAD_BROWSER_CREDS,     ///< Credenciais do browser
    PAYLOAD_DOWNLOAD_EXEC,     ///< Download Exec
    PAYLOAD_ADD_ADMIN,         ///< Adicionar Admin
    PAYLOAD_BSOD,              ///< Tela Azul (BSOD)
    PAYLOAD_FAKE_UPDATE,       ///< Fake Update Screen
    PAYLOAD_WALLPAPER,         ///< Trocar Wallpaper
    PAYLOAD_SPEAK,             ///< Falar Texto (TTS)
    PAYLOAD_INFO_TO_NOTEPAD,   ///< Info pc para Notepad
    PAYLOAD_DISK_WIPE_WIN,     ///< Disk Wipe Windows
    PAYLOAD_DISK_WIPE_MAC,     ///< Disk Wipe Mac
    PAYLOAD_REVERSE_SHELL_PI   ///< Reverse Shell to Pi
};

/**
 * @brief Status do ataque
 */
enum USBAttackStatus {
    USB_IDLE = 0, ///< Ocioso
    USB_RUNNING,  ///< Ataque em andamento
    USB_SUCCESS,  ///< Ataque bem-sucedido
    USB_FAILED    ///< Ataque falhou
};

// ============================================================================
// FUNÇÕES PRINCIPAIS
// ============================================================================

/**
 * @brief Inicializa o módulo de ataques USB
 */
void usb_attacks_init();

/**
 * @brief Executa ataque BadUSB com script Ducky
 * @param script_path Caminho do script no SD Card
 * @return true se iniciou com sucesso
 */
bool usb_run_ducky_script(const char *script_path);

/**
 * @brief Executa payload pré-definido
 * @param payload Tipo de payload
 * @return true se iniciou com sucesso
 */
bool usb_run_payload(USBPayload payload);

/**
 * @brief Envia sequência de teclas
 * @param keys String com teclas a enviar
 * @param delay_ms Delay entre teclas em ms
 * @return true se enviou com sucesso
 */
bool usb_send_keys(const char *keys, uint32_t delay_ms);

/**
 * @brief Para ataque em andamento
 */
void usb_stop_attack();

/**
 * @brief Obtém status atual
 * @return USBAttackStatus
 */
USBAttackStatus usb_get_status();

// ============================================================================
// PAYLOADS PRÉ-DEFINIDOS
// ============================================================================

void usb_payload_reverse_shell(const char *ip, uint16_t port);
void usb_payload_wifi_stealer();
void usb_payload_browser_creds();
void usb_payload_disable_defender();
void usb_payload_powershell(const char *ps_script);

// Novos Payloads "Gigantes"
void usb_payload_download_exec(const char *url);
void usb_payload_add_admin(const char *user, const char *pass);
void usb_payload_bsod();
void usb_payload_fake_update();
void usb_payload_wallpaper(const char *img_url);
void usb_payload_speak(const char *text);
void usb_payload_info_notepad();
void usb_payload_disk_wipe_win();
void usb_payload_disk_wipe_mac();
void usb_payload_reverse_shell_pi(const char *pi_ip);

#endif // __USB_ATTACKS_H__
