/**
 * @file pmkid_capture.h
 * @brief PMKID Capture com EAPOL Monitoring
 * 
 * Captura PMKID de redes WPA2 sem precisar de cliente.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef PMKID_CAPTURE_H
#define PMKID_CAPTURE_H

#include <Arduino.h>
#include <esp_wifi.h>

// PMKID capturado
typedef struct {
    uint8_t pmkid[16];
    uint8_t bssid[6];
    uint8_t clientMac[6];
    char ssid[33];
    bool valid;
} PMKIDResult;

class PMKIDCapture {
public:
    /**
     * @brief Inicia captura de PMKID
     * @param targetBSSID BSSID alvo (nullptr = todos)
     */
    static void start(const uint8_t* targetBSSID = nullptr);
    
    /**
     * @brief Para captura
     */
    static void stop();
    
    /**
     * @brief Verifica se está capturando
     */
    static bool isCapturing();
    
    /**
     * @brief Retorna quantidade de PMKIDs capturados
     */
    static int getCaptureCount();
    
    /**
     * @brief Retorna PMKID por índice
     */
    static PMKIDResult getCapture(int index);
    
    /**
     * @brief Exporta para formato hashcat (modo 22000)
     * @param filename Arquivo de saída
     */
    static bool exportHashcat(const char* filename);
    
    /**
     * @brief Limpa capturas
     */
    static void clear();

private:
    static bool _capturing;
    static uint8_t _targetBSSID[6];
    static bool _hasTarget;
    static PMKIDResult _captures[50];
    static int _captureCount;
    
    static void promiscuousCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static bool parseEAPOL(const uint8_t* data, size_t len, PMKIDResult* result);
    static bool extractPMKID(const uint8_t* eapolData, size_t len, uint8_t* pmkid);
};

#endif // PMKID_CAPTURE_H
