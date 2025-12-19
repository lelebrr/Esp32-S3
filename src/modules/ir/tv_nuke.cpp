#include "tv_nuke.h"
#include "core/display.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Pino IR padrão
#ifndef IR_TX_PIN
#define IR_TX_PIN 9
#endif

static IRsend irsend(IR_TX_PIN);
static bool _is_active = false;
static uint32_t _codes_sent = 0;
static int _current_index = 0;
static unsigned long _last_update = 0;
static const char* _current_brand = "";

// Mega lista de códigos de desligar TVs
// Formato: {protocolo, endereço, comando, bits}
struct TVCode {
    const char* brand;
    uint8_t protocol;  // 0=NEC, 1=Samsung, 2=Sony, 3=RC5, 4=RC6, 5=LG
    uint32_t code;
    uint8_t bits;
};

// Protocolos
#define PROTO_NEC     0
#define PROTO_SAMSUNG 1
#define PROTO_SONY    2
#define PROTO_RC5     3
#define PROTO_RC6     4
#define PROTO_LG      5

static const TVCode TV_CODES[] = {
    // Samsung
    {"Samsung", PROTO_SAMSUNG, 0xE0E040BF, 32},
    {"Samsung", PROTO_SAMSUNG, 0xE0E019E6, 32},
    {"Samsung", PROTO_NEC, 0x20DF10EF, 32},

    // LG
    {"LG", PROTO_LG, 0x20DF10EF, 32},
    {"LG", PROTO_NEC, 0x20DF10EF, 32},
    {"LG", PROTO_NEC, 0x20DF08F7, 32},

    // Sony
    {"Sony", PROTO_SONY, 0xA90, 12},
    {"Sony", PROTO_SONY, 0x290, 12},
    {"Sony", PROTO_SONY, 0x490, 15},
    {"Sony", PROTO_SONY, 0x0A90, 15},

    // Philips
    {"Philips", PROTO_RC5, 0x100C, 14},
    {"Philips", PROTO_RC6, 0x100C, 20},

    // Philco (BR)
    {"Philco", PROTO_NEC, 0x40BF00FF, 32},
    {"Philco", PROTO_NEC, 0x40BF807F, 32},

    // AOC
    {"AOC", PROTO_NEC, 0x00FE48B7, 32},
    {"AOC", PROTO_NEC, 0x807F807F, 32},

    // Panasonic
    {"Panasonic", PROTO_NEC, 0x400401FE, 32},

    // TCL
    {"TCL", PROTO_NEC, 0x02FD48B7, 32},
    {"TCL", PROTO_NEC, 0x32CD02FD, 32},

    // Semp Toshiba
    {"Semp", PROTO_NEC, 0x02FD48B7, 32},
    {"Toshiba", PROTO_NEC, 0x02FD48B7, 32},

    // Hisense
    {"Hisense", PROTO_NEC, 0x40BF12ED, 32},

    // Sharp
    {"Sharp", PROTO_NEC, 0x40BF12ED, 32},

    // Sanyo
    {"Sanyo", PROTO_NEC, 0x1CE348B7, 32},

    // JVC
    {"JVC", PROTO_NEC, 0xC5E8, 16},

    // RCA
    {"RCA", PROTO_NEC, 0x0FF00FF0, 32},

    // Vizio
    {"Vizio", PROTO_NEC, 0x20DF10EF, 32},

    // Element
    {"Element", PROTO_NEC, 0x08F748B7, 32},

    // Haier
    {"Haier", PROTO_NEC, 0x9966817E, 32},

    // Roku TV
    {"Roku", PROTO_NEC, 0x57E3E817, 32},

    // Fire TV
    {"Fire TV", PROTO_NEC, 0x00FF906F, 32},

    // Magnavox
    {"Magnavox", PROTO_NEC, 0x00FF00FF, 32},

    // Insignia
    {"Insignia", PROTO_NEC, 0x807F40BF, 32},

    // Emerson
    {"Emerson", PROTO_NEC, 0x00FF00FF, 32},

    // Sylvania
    {"Sylvania", PROTO_NEC, 0x00FF00FF, 32},

    // Westinghouse
    {"Westinghouse", PROTO_NEC, 0x807F00FF, 32},

    // Generic NEC
    {"Generic", PROTO_NEC, 0x00FF00FF, 32},
    {"Generic", PROTO_NEC, 0x807F00FF, 32},
    {"Generic", PROTO_NEC, 0x40BF00FF, 32},
    {"Generic", PROTO_NEC, 0x20DF10EF, 32},
    {"Generic", PROTO_NEC, 0x02FD48B7, 32},
};

#define TV_CODES_COUNT (sizeof(TV_CODES) / sizeof(TV_CODES[0]))

void tv_nuke_init() {
    irsend.begin();
    _is_active = false;
    _codes_sent = 0;
    _current_index = 0;
}

bool tv_nuke_start() {
    if (_is_active) return false;

    _is_active = true;
    _codes_sent = 0;
    _current_index = 0;

    displayInfo("TV NUKE ativo!\nEnviando codigos...", false);
    return true;
}

void tv_nuke_stop() {
    _is_active = false;

    String msg = "TV Nuke parado\n" + String(_codes_sent) + " codigos";
    displaySuccess(msg, true);
}

int tv_nuke_update() {
    if (!_is_active) return 0;

    unsigned long now = millis();
    if (now - _last_update < 100) return 0;  // 10 códigos/seg
    _last_update = now;

    if (_current_index >= TV_CODES_COUNT) {
        _current_index = 0;  // Loop infinito
    }

    const TVCode& code = TV_CODES[_current_index];
    _current_brand = code.brand;

    // Envia código baseado no protocolo
    switch (code.protocol) {
        case PROTO_NEC:
            irsend.sendNEC(code.code, code.bits);
            break;
        case PROTO_SAMSUNG:
            irsend.sendSAMSUNG(code.code, code.bits);
            break;
        case PROTO_SONY:
            irsend.sendSony(code.code, code.bits);
            break;
        case PROTO_RC5:
            irsend.sendRC5(code.code, code.bits);
            break;
        case PROTO_RC6:
            irsend.sendRC6(code.code, code.bits);
            break;
        case PROTO_LG:
            irsend.sendLG(code.code, code.bits);
            break;
    }

    _codes_sent++;
    _current_index++;

    // Atualiza display
    static unsigned long last_display = 0;
    if (now - last_display > 300) {
        last_display = now;
        tft.fillRect(0, tftHeight - 40, tftWidth, 40, leleConfig.bgColor);
        tft.setCursor(10, tftHeight - 35);
        tft.printf("Marca: %s", _current_brand);
        tft.setCursor(10, tftHeight - 15);
        tft.printf("Codigos: %d / %d", _current_index, TV_CODES_COUNT);
    }

    return 1;
}

bool tv_nuke_is_active() { return _is_active; }
uint32_t tv_nuke_get_codes_sent() { return _codes_sent; }
const char* tv_nuke_get_current_brand() { return _current_brand; }
