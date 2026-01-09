#ifndef RF_CORE_H
#define RF_CORE_H

#include <Arduino.h>
#include <stdint.h>

// ============================================================================
// CC1101 RF CORE - Complete Attack Suite
// Frequencies, Modulations, Attacks, Protocols
// ============================================================================

// ============================
// FREQUENCY DEFINITIONS
// ============================
#define FREQ_315      315.00f
#define FREQ_433      433.92f
#define FREQ_868      868.35f
#define FREQ_915      915.00f

// Common Sub-bands for scanning
#define FREQ_GARAGE_US    315.00f
#define FREQ_GARAGE_EU    433.92f
#define FREQ_CAR_US       315.00f
#define FREQ_CAR_EU       433.92f
#define FREQ_IOT_EU       868.00f
#define FREQ_IOT_US       915.00f

// ============================
// MODULATION TYPES (SmartRC-CC1101-Driver-Lib)
// ============================
#define MOD_2FSK      0
#define MOD_GFSK      1
#define MOD_ASK_OOK   2
#define MOD_4FSK      3
#define MOD_MSK       4

// ============================
// POWER LEVELS (dBm)
// ============================
#define POWER_MIN     -30
#define POWER_LOW     -10
#define POWER_MID     0
#define POWER_HIGH    7
#define POWER_MAX     10

// ============================
// RX BUFFER SIZES
// ============================
#define RX_BUFFER_SIZE      256
#define REPLAY_BUFFER_SIZE  512
#define PROTOCOL_MAX_BITS   64

// ============================
// ATTACK TYPES
// ============================
typedef enum {
    RF_ATTACK_NONE = 0,
    RF_ATTACK_JAMMER_CONTINUOUS,
    RF_ATTACK_JAMMER_BURST,
    RF_ATTACK_JAMMER_SMART,
    RF_ATTACK_REPLAY_SIMPLE,
    RF_ATTACK_REPLAY_GHOST,
    RF_ATTACK_SCAN_FREQUENCY,
    RF_ATTACK_SCAN_PROTOCOL,
    RF_ATTACK_SPECTRUM_ANALYZER,
    RF_ATTACK_BRUTE_FORCE,
    RF_ATTACK_DEBRUIJN
} RFAttackType;

// ============================
// PROTOCOL DEFINITIONS
// ============================
typedef enum {
    PROTO_RAW = 0,
    PROTO_PRINCETON,
    PROTO_NICE_FLO,
    PROTO_CAME,
    PROTO_LINEAR,
    PROTO_HOLTEK,
    PROTO_PT2262,
    PROTO_KEELOQ,
    PROTO_UNKNOWN
} RFProtocol;

// ============================
// CAPTURED SIGNAL STRUCTURE
// ============================
typedef struct {
    float frequency;
    int modulation;
    RFProtocol protocol;
    uint32_t code;
    uint8_t bitLength;
    uint8_t rawData[REPLAY_BUFFER_SIZE];
    uint16_t rawLength;
    int8_t rssi;
    uint32_t timestamp;
    bool valid;
} CapturedSignal;

// ============================
// SPECTRUM DATA
// ============================
typedef struct {
    float startFreq;
    float endFreq;
    float stepSize;
    int8_t rssiValues[128];
    uint8_t numSamples;
} SpectrumData;

// ============================================================================
// RF CORE CLASS
// ============================================================================
class RFCore {
public:
    // ============================
    // INITIALIZATION
    // ============================
    static bool init();
    static void stop();
    static bool isInitialized();
    static bool checkHardware();
    
    // ============================
    // CONFIGURATION
    // ============================
    static void setFrequency(float freq);
    static float getFrequency();
    static void setTxPower(int dbm);
    static void setModulation(int mod);
    static void setDataRate(float kbps);
    static void setBandwidth(float khz);
    static void setDeviation(float khz);
    static void setSyncWord(uint16_t sync);
    
    // ============================
    // JAMMER ATTACKS
    // ============================
    static void startJammerContinuous(float freq);
    static void startJammerBurst(float freq, uint16_t burstMs, uint16_t pauseMs);
    static void startJammerSmart(float freq);  // Detect & jam only active transmissions
    static void stopJammer();
    static bool isJamming();
    static void updateJammer();  // Call in loop
    
    // ============================
    // CAPTURE / RECEIVE
    // ============================
    static bool startReceive(float freq);
    static void stopReceive();
    static bool hasSignal();
    static CapturedSignal getLastCapture();
    static int8_t getRSSI();
    static bool isReceiving();
    
    // ============================
    // REPLAY / TRANSMIT
    // ============================
    static bool transmitRaw(uint8_t* data, uint16_t len);
    static bool transmitCode(uint32_t code, uint8_t bits, RFProtocol proto);
    static bool replayLast();
    static void startGhostReplay(uint16_t minDelayMs, uint16_t maxDelayMs, uint8_t repeats);
    static void stopGhostReplay();
    static bool isGhostActive();
    
    // ============================
    // SCANNER / SPECTRUM
    // ============================
    static void startFrequencyScan(float startFreq, float endFreq, float step);
    static void stopFrequencyScan();
    static SpectrumData getSpectrumData();
    static float findStrongestFrequency();
    static void updateScanner();
    
    // ============================
    // PROTOCOL DETECTION
    // ============================
    static RFProtocol detectProtocol(uint8_t* data, uint16_t len);
    static bool decodeProtocol(CapturedSignal* signal);
    static const char* getProtocolName(RFProtocol proto);
    
    // ============================
    // BRUTE FORCE ATTACKS
    // ============================
    static void startBruteForce(float freq, uint8_t bits, RFProtocol proto);
    static void startDeBruijn(float freq, uint8_t bits);
    static void stopBruteForce();
    static bool isBruteForcing();
    static uint32_t getBruteProgress();
    static uint32_t getBruteTotal();
    static void updateBruteForce();
    
    // ============================
    // STATUS & DIAGNOSTICS
    // ============================
    static bool selfTest();
    static uint8_t getVersion();
    static int8_t getTemperature();
    static void printStatus();
    
    // ============================
    // SAVE / LOAD (SD Card)
    // ============================
    static bool saveSignal(const char* filename, CapturedSignal* sig);
    static bool loadSignal(const char* filename, CapturedSignal* sig);
    static bool saveFlipperFormat(const char* filename, CapturedSignal* sig);
    static bool loadFlipperFormat(const char* filename, CapturedSignal* sig);

private:
    static bool _initialized;
    static bool _jammerActive;
    static bool _receiverActive;
    static bool _ghostActive;
    static bool _bruteActive;
    static bool _scannerActive;
    
    static float _currentFreq;
    static int _currentPower;
    static int _currentMod;
    
    static RFAttackType _currentAttack;
    static CapturedSignal _lastCapture;
    static SpectrumData _spectrumData;
    
    // Jammer state
    static uint16_t _burstDuration;
    static uint16_t _burstPause;
    static uint32_t _lastBurstTime;
    static bool _burstState;
    
    // Ghost replay state
    static uint16_t _ghostMinDelay;
    static uint16_t _ghostMaxDelay;
    static uint8_t _ghostRepeats;
    static uint8_t _ghostCurrentRepeat;
    static uint32_t _ghostNextTime;
    
    // Brute force state
    static uint32_t _bruteCurrentCode;
    static uint32_t _bruteTotalCodes;
    static uint8_t _bruteBits;
    static RFProtocol _bruteProtocol;
    
    // Scanner state
    static float _scanStart;
    static float _scanEnd;
    static float _scanStep;
    static float _scanCurrent;
    static uint8_t _scanIndex;
    
    // Internal helpers
    static void sendNoise(uint16_t bytes);
    static void encodeProtocol(uint32_t code, uint8_t bits, RFProtocol proto, uint8_t* out, uint16_t* outLen);
};

#endif // RF_CORE_H
