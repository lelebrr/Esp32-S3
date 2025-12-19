#ifndef AI_ATTACK_LEARNER_H
#define AI_ATTACK_LEARNER_H

#include <Arduino.h>
#include <Preferences.h>
#include <SD.h>
#include <FS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// TensorFlow Lite is optional (requires HAS_TFLITE flag)
#include <Crypto.h>
#include <SHA256.h>

#ifdef HAS_TFLITE
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#endif

// Attack types enumeration
enum AttackType {
    ATTACK_DEAUTH = 0,
    ATTACK_FAULT_INJECT = 1,
    ATTACK_REPLAY = 2,
    ATTACK_EVIL_TWIN = 3,
    ATTACK_NFC_SPOOF = 4,
    ATTACK_IR_CLONE = 5,
    ATTACK_CC1101_REPLAY = 6,
    ATTACK_NRF24_SNIFF = 7,
    ATTACK_NONE = 8
};

// Context features for AI decision
struct ContextFeatures {
    float gps_lat;
    float gps_lon;
    float temperature;
    int gesture_type;
    float battery_voltage;
    bool nfc_available;
    bool subghz_available;
    bool ble_available;
    bool wifi_available;
    int network_type; // 0=WPA2, 1=WEP, 2=Open, 3=WPS
};

// Top attacks cache structure for PSRAM
struct TopAttackCache {
    AttackType attack_type;
    float success_rate;
    uint32_t usage_count;
    uint32_t last_used;
};

// Hardware fingerprint for CVE filtering
struct HardwareFingerprint {
    bool has_ble;
    bool has_nfc;
    bool has_wifi;
    bool has_subghz;
    bool has_ir;
    bool has_gps;
    String esp32_variant;
    String firmware_version;
};

// Q-Learning parameters
#define NUM_STATES 64    // Discretized state space
#define NUM_ACTIONS 8   // Number of attack types
#define LEARNING_RATE 0.1f
#define DISCOUNT_FACTOR 0.9f
#define EPSILON 0.1f

// Advanced AI parameters
#define TOP_ATTACKS_COUNT TOP_ATTACKS_CACHE_SIZE
#define MAX_BACKUP_FILES 5
#define API_TIMEOUT_MS 10000
#define STEALTH_CHECK_INTERVAL_MS 5000

class AIAttackLearner {
public:
    AIAttackLearner();
    ~AIAttackLearner();

    // Core AI functions
    bool init();
    AttackType predictAttack(const ContextFeatures& context);
    void updateQTable(AttackType action, float reward);

    // API update functions
    bool updateAttacksFromAPI();
    bool downloadExploitDB();
    bool downloadNIST();
    bool growBruteForceList();

    // Persistence
    bool saveModel();
    bool loadModel();
    void backupModel();

    // Manual feedback
    void provideFeedback(bool success);

    // Stealth mode
    void enterStealthMode();
    void exitStealthMode();

    // Context gathering
    ContextFeatures gatherContext();

    // Advanced AI features
    AttackType predictAttackCached(const ContextFeatures& context);
    void updateTopAttacksCache(AttackType attack, bool success);
    bool isStealthModeRequired();
    void checkAndEnterStealthMode();
    HardwareFingerprint getHardwareFingerprint();
    bool filterCVEForHardware(const String& cveId);
    bool validateAndCompileExploit(const JsonObject& exploit);
    void createAutomaticBackup();
    bool loadFromBackup();
    void wakeOnGesture();

    // Voice wake and gesture control
    bool initVoiceWake();
    void enableVoiceWake();
    void disableVoiceWake();
    bool wasVoiceActivated();

private:
    // Q-Learning table
    float q_table_[NUM_STATES][NUM_ACTIONS];

    // TensorFlow Lite components for advanced ML
#ifdef HAS_TFLITE
    const tflite::Model* model_;
    tflite::MicroInterpreter* interpreter_;
    TfLiteTensor* input_tensor_;
    TfLiteTensor* output_tensor_;
    uint8_t* tensor_arena_;
#endif

    // Top attacks cache in PSRAM
    TopAttackCache* top_attacks_cache_;
    bool cache_initialized_;

    // Hardware fingerprint
    HardwareFingerprint hw_fingerprint_;

    // Preferences for persistence
    Preferences prefs_;

    // HTTP client for API calls
    HTTPClient http_;

    // Crypto for checksums
    SHA256 sha256_;

    // Internal state
    bool initialized_;
    bool stealth_mode_;
    AttackType last_action_;
    unsigned long last_update_;
    unsigned long last_stealth_check_;
    unsigned long last_backup_;

    // Voice/Gesture wake state
    bool voice_wake_enabled_;
    bool gesture_wake_active_;
    unsigned long last_gesture_time_;

    // Helper functions
    int discretizeState(const ContextFeatures& context);
    float getMaxQValue(int state);
    AttackType epsilonGreedy(int state);
    bool validateExploit(const JsonObject& exploit);
    bool compileExploit(const String& code);
    String getHardwareFingerprintOld();
    void updateStealthStatus();
    void updateCacheStatistics();
    bool performSecureUpdate(const String& url, const String& filename);

    // Brute force helper functions
    void extractBruteKeywords(const String& payload);
    void extractBrazilianCodes(const String& payload);
    void parseHtmlForPins(const String& html);
    void loadBruteForceCache();
    void generateVariants();
    void updateBruteForceQTable();
    void saveBruteForceList(const String& filename, const std::vector<String>& codes);
    std::vector<String> loadBruteForceList(const String& filename);
};

extern AIAttackLearner aiLearner;

#endif // AI_ATTACK_LEARNER_H
`n    // Voice-integrated attack execution`n    void execute_attack_with_voice(int attack_type);
