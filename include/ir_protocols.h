/**
 * @file ir_protocols.h
 * @brief Protocolos IR Multi-formato (NEC, RC5, Sony, Portão)
 * 
 * Usa IRremoteESP8266 para suporte a múltiplos protocolos IR.
 * 
 * @author Monster S3 Team
 * @date 2025-12-23
 */

#ifndef IR_PROTOCOLS_H
#define IR_PROTOCOLS_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

// Pinos IR
#define IR_SEND_PIN     17
#define IR_RECV_PIN     18

// Protocolos suportados
typedef enum {
    IR_PROTO_NEC,
    IR_PROTO_RC5,
    IR_PROTO_RC6,
    IR_PROTO_SONY,
    IR_PROTO_SAMSUNG,
    IR_PROTO_LG,
    IR_PROTO_PANASONIC,
    IR_PROTO_RAW
} IRProtocolType;

// Código IR capturado
typedef struct {
    IRProtocolType protocol;
    uint64_t code;
    uint16_t bits;
    uint16_t rawLength;
    uint16_t* rawData;
    bool valid;
} IRCode;

class IRProtocols {
public:
    /**
     * @brief Inicializa transmissor e receptor IR
     */
    static void init();
    
    /**
     * @brief Envia código IR
     * @param protocol Protocolo a usar
     * @param code Código a enviar
     * @param bits Número de bits
     */
    static void send(IRProtocolType protocol, uint64_t code, uint16_t bits = 0);
    
    /**
     * @brief Envia código raw
     * @param data Array de timings (us)
     * @param length Tamanho do array
     * @param frequency Frequência em kHz (38 padrão)
     */
    static void sendRaw(const uint16_t* data, uint16_t length, uint16_t frequency = 38);
    
    /**
     * @brief Captura próximo código IR
     * @return Código capturado
     */
    static IRCode capture();
    
    /**
     * @brief Verifica se há código disponível
     */
    static bool available();
    
    /**
     * @brief Retorna nome do protocolo
     */
    static const char* getProtocolName(IRProtocolType proto);
    
    // ============================================
    // Brute Force Multi-Protocolo
    // ============================================
    
    /**
     * @brief Inicia brute force em protocolo específico
     * @param protocol Protocolo alvo
     * @param startCode Código inicial
     * @param endCode Código final
     * @param delayMs Delay entre códigos (ms)
     */
    static void startBrute(IRProtocolType protocol, uint64_t startCode, uint64_t endCode, uint16_t delayMs = 50);
    
    /**
     * @brief Atualiza brute force (chamar no loop)
     * @return true se ainda em progresso
     */
    static bool updateBrute();
    
    /**
     * @brief Para brute force
     */
    static void stopBrute();
    
    /**
     * @brief Retorna progresso do brute (0-100%)
     */
    static uint8_t getBruteProgress();
    
    // ============================================
    // TV Power Codes (Multi-marca)
    // ============================================
    
    /**
     * @brief Envia código power de TV específica
     * @param brand 0=Samsung, 1=LG, 2=Sony, 3=Panasonic, 4=Philips
     */
    static void sendTVPower(uint8_t brand);
    
    /**
     * @brief TV-B-Gone: envia power para todas as marcas
     */
    static void tvBGone();
    
    // ============================================
    // Portão/Garage
    // ============================================
    
    /**
     * @brief Envia código de portão comum (433MHz style via IR)
     * @param code Código de 12-24 bits
     * @param bits Número de bits
     */
    static void sendGate(uint32_t code, uint8_t bits = 12);
    
    /**
     * @brief Brute force portão 12 bits
     */
    static void bruteGate12bit();

private:
    static IRsend _irsend;
    static IRrecv _irrecv;
    static decode_results _results;
    static bool _initialized;
    
    // Brute state
    static bool _bruteActive;
    static IRProtocolType _bruteProtocol;
    static uint64_t _bruteStart;
    static uint64_t _bruteEnd;
    static uint64_t _bruteCurrent;
    static uint16_t _bruteDelay;
    static uint32_t _bruteLastTime;
};

#endif // IR_PROTOCOLS_H
