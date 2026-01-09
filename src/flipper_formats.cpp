/**
 * @file flipper_formats.cpp
 * @brief Flipper Zero Compatible File Format Parser Implementation
 *
 * @author MorphNode Team
 * @date 2026-01-08
 */

#include "flipper_formats.h"
#include "core/aggressive_sd.h"
#include <SD.h>

// ============================================================================
// INTERNAL HELPERS
// ============================================================================

static bool parseKeyValue(const char *line, char *key, char *value) {
    const char *colonPos = strchr(line, ':');
    if (!colonPos) return false;

    size_t keyLen = colonPos - line;
    strncpy(key, line, keyLen);
    key[keyLen] = '\0';

    // Skip colon and whitespace
    const char *valStart = colonPos + 1;
    while (*valStart == ' ') valStart++;

    strcpy(value, valStart);
    // Remove trailing newline/carriage return
    size_t len = strlen(value);
    while (len > 0 && (value[len - 1] == '\n' || value[len - 1] == '\r')) { value[--len] = '\0'; }

    return true;
}

static uint32_t parseHexBytes(const char *hexStr) {
    uint32_t result = 0;
    char *endPtr;

    // Parse space-separated hex bytes (e.g., "04 00 00 00")
    const char *p = hexStr;
    int shift = 0;
    while (*p && shift < 32) {
        while (*p == ' ') p++;
        if (!*p) break;

        uint8_t byte = (uint8_t)strtol(p, &endPtr, 16);
        result |= ((uint32_t)byte << shift);
        shift += 8;
        p = endPtr;
    }

    return result;
}

// ============================================================================
// IR PARSING
// ============================================================================

bool flipper_parse_ir(const char *filename, FlipperIR *out) {
    if (!out || !filename) return false;

    memset(out, 0, sizeof(FlipperIR));
    out->frequency = 38000; // Default IR frequency

    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.printf("[FLIPPER] Failed to open IR file: %s\n", filename);
        return false;
    }

    char line[256];
    char key[64], value[192];

    while (file.available()) {
        size_t len = file.readBytesUntil('\n', line, sizeof(line) - 1);
        line[len] = '\0';

        if (!parseKeyValue(line, key, value)) continue;

        if (strcmp(key, "name") == 0) {
            strncpy(out->name, value, FLIPPER_MAX_NAME_LEN - 1);
        } else if (strcmp(key, "protocol") == 0) {
            strncpy(out->protocol, value, FLIPPER_MAX_PROTOCOL_LEN - 1);
        } else if (strcmp(key, "address") == 0) {
            out->address = parseHexBytes(value);
        } else if (strcmp(key, "command") == 0) {
            out->command = parseHexBytes(value);
        } else if (strcmp(key, "frequency") == 0) {
            out->frequency = atoi(value);
        } else if (strcmp(key, "type") == 0) {
            out->isRaw = (strcmp(value, "raw") == 0);
        } else if (strcmp(key, "data") == 0 && out->isRaw) {
            // Parse raw timing data
            const char *p = value;
            char *endPtr;
            while (*p && out->rawLen < FLIPPER_MAX_RAW_LEN) {
                while (*p == ' ') p++;
                if (!*p) break;
                out->rawData[out->rawLen++] = (uint16_t)strtol(p, &endPtr, 10);
                p = endPtr;
            }
        }
    }

    file.close();

    Serial.printf(
        "[FLIPPER] Parsed IR: %s (protocol=%s, addr=0x%X, cmd=0x%X)\n",
        out->name,
        out->protocol,
        out->address,
        out->command
    );

    return strlen(out->protocol) > 0;
}

// ============================================================================
// SUBGHZ PARSING
// ============================================================================

bool flipper_parse_subghz(const char *filename, FlipperSubGHz *out) {
    if (!out || !filename) return false;

    memset(out, 0, sizeof(FlipperSubGHz));

    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.printf("[FLIPPER] Failed to open SubGHz file: %s\n", filename);
        return false;
    }

    char line[512];
    char key[64], value[448];

    while (file.available()) {
        size_t len = file.readBytesUntil('\n', line, sizeof(line) - 1);
        line[len] = '\0';

        if (!parseKeyValue(line, key, value)) continue;

        if (strcmp(key, "Frequency") == 0) {
            out->frequency = atol(value);
        } else if (strcmp(key, "Preset") == 0) {
            strncpy(out->preset, value, FLIPPER_MAX_PROTOCOL_LEN - 1);
        } else if (strcmp(key, "Protocol") == 0) {
            strncpy(out->protocol, value, FLIPPER_MAX_PROTOCOL_LEN - 1);
        } else if (strcmp(key, "RAW_Data") == 0) {
            // Parse raw timing data (positive = high, negative = low)
            const char *p = value;
            char *endPtr;
            while (*p && out->rawLen < FLIPPER_MAX_RAW_LEN) {
                while (*p == ' ') p++;
                if (!*p) break;
                out->rawData[out->rawLen++] = (int16_t)strtol(p, &endPtr, 10);
                p = endPtr;
            }
        } else if (strcmp(key, "Key") == 0) {
            // Parsed protocol key
            out->code = parseHexBytes(value);
        } else if (strcmp(key, "Bit") == 0) {
            out->bits = atoi(value);
        }
    }

    file.close();

    // Extract name from filename
    const char *nameStart = strrchr(filename, '/');
    if (nameStart) nameStart++;
    else nameStart = filename;
    strncpy(out->name, nameStart, FLIPPER_MAX_NAME_LEN - 1);
    // Remove extension
    char *dot = strrchr(out->name, '.');
    if (dot) *dot = '\0';

    Serial.printf(
        "[FLIPPER] Parsed SubGHz: %s (freq=%lu Hz, protocol=%s)\n", out->name, out->frequency, out->protocol
    );

    return out->frequency > 0;
}

// ============================================================================
// EXPORT FUNCTIONS
// ============================================================================

bool flipper_export_ir(const char *filename, const FlipperIR *data) {
    if (!data || !filename) return false;

    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.printf("[FLIPPER] Failed to create IR file: %s\n", filename);
        return false;
    }

    file.println("Filetype: IR signals file");
    file.println("Version: 1");
    file.println("#");
    file.printf("name: %s\n", data->name);

    if (data->isRaw) {
        file.println("type: raw");
        file.printf("frequency: %u\n", data->frequency);
        file.print("data:");
        for (size_t i = 0; i < data->rawLen; i++) { file.printf(" %u", data->rawData[i]); }
        file.println();
    } else {
        file.println("type: parsed");
        file.printf("protocol: %s\n", data->protocol);
        file.printf(
            "address: %02X %02X %02X %02X\n",
            (data->address) & 0xFF,
            (data->address >> 8) & 0xFF,
            (data->address >> 16) & 0xFF,
            (data->address >> 24) & 0xFF
        );
        file.printf(
            "command: %02X %02X %02X %02X\n",
            (data->command) & 0xFF,
            (data->command >> 8) & 0xFF,
            (data->command >> 16) & 0xFF,
            (data->command >> 24) & 0xFF
        );
    }

    file.close();
    Serial.printf("[FLIPPER] Exported IR to: %s\n", filename);
    return true;
}

bool flipper_export_subghz(const char *filename, const FlipperSubGHz *data) {
    if (!data || !filename) return false;

    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.printf("[FLIPPER] Failed to create SubGHz file: %s\n", filename);
        return false;
    }

    file.println("Filetype: Flipper SubGhz RAW File");
    file.println("Version: 1");
    file.printf("Frequency: %lu\n", data->frequency);
    file.printf("Preset: %s\n", strlen(data->preset) > 0 ? data->preset : "FuriHalSubGhzPresetOok650Async");
    file.printf("Protocol: %s\n", strlen(data->protocol) > 0 ? data->protocol : "RAW");

    if (data->rawLen > 0) {
        file.print("RAW_Data:");
        for (size_t i = 0; i < data->rawLen; i++) { file.printf(" %d", data->rawData[i]); }
        file.println();
    }

    file.close();
    Serial.printf("[FLIPPER] Exported SubGHz to: %s\n", filename);
    return true;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int flipper_protocol_to_enum(const char *protocolName) {
    if (strcmp(protocolName, "NEC") == 0) return 1;
    if (strcmp(protocolName, "NECext") == 0) return 1;
    if (strcmp(protocolName, "RC5") == 0) return 2;
    if (strcmp(protocolName, "RC6") == 0) return 3;
    if (strcmp(protocolName, "Sony") == 0) return 4;
    if (strcmp(protocolName, "Samsung32") == 0) return 5;
    if (strcmp(protocolName, "Princeton") == 0) return 10;
    if (strcmp(protocolName, "CAME") == 0) return 11;
    if (strcmp(protocolName, "NiceFLO") == 0) return 12;
    return 0; // Unknown
}

const char *flipper_enum_to_protocol(int proto) {
    switch (proto) {
        case 1: return "NEC";
        case 2: return "RC5";
        case 3: return "RC6";
        case 4: return "Sony";
        case 5: return "Samsung32";
        case 10: return "Princeton";
        case 11: return "CAME";
        case 12: return "NiceFLO";
        default: return "RAW";
    }
}

int flipper_list_ir_files(const char *directory, char files[][64], int maxFiles) {
    File dir = SD.open(directory);
    if (!dir || !dir.isDirectory()) return 0;

    int count = 0;
    File entry;
    while ((entry = dir.openNextFile()) && count < maxFiles) {
        if (!entry.isDirectory()) {
            const char *name = entry.name();
            size_t len = strlen(name);
            if (len > 3 && strcmp(name + len - 3, ".ir") == 0) {
                strncpy(files[count], name, 63);
                files[count][63] = '\0';
                count++;
            }
        }
        entry.close();
    }
    dir.close();
    return count;
}

int flipper_list_subghz_files(const char *directory, char files[][64], int maxFiles) {
    File dir = SD.open(directory);
    if (!dir || !dir.isDirectory()) return 0;

    int count = 0;
    File entry;
    while ((entry = dir.openNextFile()) && count < maxFiles) {
        if (!entry.isDirectory()) {
            const char *name = entry.name();
            size_t len = strlen(name);
            if (len > 4 && strcmp(name + len - 4, ".sub") == 0) {
                strncpy(files[count], name, 63);
                files[count][63] = '\0';
                count++;
            }
        }
        entry.close();
    }
    dir.close();
    return count;
}
