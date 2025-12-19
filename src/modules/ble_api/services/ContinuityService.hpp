#pragma once

#include "LeleBLEService.hpp"
#include <NimBLEDevice.h>

class ContinuityCallbacks;

class ContinuityService : public LeleBLEService {
    friend class ContinuityCallbacks;
    NimBLECharacteristic *title_char = nullptr;
    NimBLECharacteristic *message_char = nullptr;
    NimBLECharacteristic *deeplink_char = nullptr;
    ContinuityCallbacks *callbacks = nullptr;
public:
    ContinuityService();
    ~ContinuityService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    void setTitle(const String &title);
    void setMessage(const String &message);
    void setDeeplink(const String &deeplink);
};

// App-specific BLE services for pentest
class UberService : public LeleBLEService {
    NimBLECharacteristic *request_char = nullptr;
public:
    UberService();
    ~UberService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    String getRideRequest();
};

class NetflixService : public LeleBLEService {
    NimBLECharacteristic *auth_char = nullptr;
public:
    NetflixService();
    ~NetflixService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    void injectAuthLink(const String &link);
};

class iFoodService : public LeleBLEService {
    NimBLECharacteristic *cart_char = nullptr;
public:
    iFoodService();
    ~iFoodService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    void confirmOrder(const String &qrCode);
};

class SpotifyService : public LeleBLEService {
    NimBLECharacteristic *playback_char = nullptr;
public:
    SpotifyService();
    ~SpotifyService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    String getPlaybackToken();
};

class InstagramService : public LeleBLEService {
    NimBLECharacteristic *message_char = nullptr;
public:
    InstagramService();
    ~InstagramService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    String getDirectMessages();
};
