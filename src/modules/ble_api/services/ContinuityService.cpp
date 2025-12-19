#include "ContinuityService.hpp"
#include <NimBLEDevice.h>

ContinuityService::ContinuityService() : LeleBLEService() {}

ContinuityService::~ContinuityService() {}

class ContinuityCallbacks : public NimBLECharacteristicCallbacks {
    ContinuityService *service;

    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        // Inject values based on the characteristic written
        if (pCharacteristic == service->title_char) {
            service->setTitle("WhatsApp");
        } else if (pCharacteristic == service->message_char) {
            service->setMessage("Nova mensagem: seu Pix chegou");
        } else if (pCharacteristic == service->deeplink_char) {
            // For Android, use intent URI
            service->setDeeplink("intent:#Intent;component=com.whatsapp/.Main;S.data=whatsapp://send?text=clique+aqui+bit.ly/fake;end");
        }
    }

public:
    explicit ContinuityCallbacks(ContinuityService *srv) : service(srv) {}
};

void ContinuityService::setup(NimBLEServer *pServer) {
    // Create service with a UUID, perhaps use the Generic Access service or a custom one
    // For spoofing, maybe use a vendor service
    pService = pServer->createService("1800"); // Generic Access

    // Create characteristics with the specified UUIDs
    title_char = pService->createCharacteristic(
        "0079", // 0x79
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    message_char = pService->createCharacteristic(
        "007A", // 0x7A
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    deeplink_char = pService->createCharacteristic(
        "007B", // 0x7B
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    callbacks = new ContinuityCallbacks(this);
    title_char->setCallbacks(callbacks);
    message_char->setCallbacks(callbacks);
    deeplink_char->setCallbacks(callbacks);

    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void ContinuityService::end() {
    delete callbacks;
}

void ContinuityService::setTitle(const String &title) {
    title_char->setValue(title);
}

void ContinuityService::setMessage(const String &message) {
    message_char->setValue(message);
}

void ContinuityService::setDeeplink(const String &deeplink) {
    deeplink_char->setValue(deeplink);
}

// UberService implementation
UberService::UberService() : LeleBLEService() {}
UberService::~UberService() {}

void UberService::setup(NimBLEServer *pServer) {
    pService = pServer->createService("FF01");
    request_char = pService->createCharacteristic(
        "2A06",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void UberService::end() {}

String UberService::getRideRequest() {
    return String(request_char->getValue().c_str());
}

// NetflixService implementation
NetflixService::NetflixService() : LeleBLEService() {}
NetflixService::~NetflixService() {}

void NetflixService::setup(NimBLEServer *pServer) {
    pService = pServer->createService("7905F431-B5CE-4E99-A40F-4B1E122D00D0"); // ANCS UUID
    auth_char = pService->createCharacteristic(
        "9FBF120D-6301-42D9-8C58-25E699A21DBD",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void NetflixService::end() {}

void NetflixService::injectAuthLink(const String &link) {
    auth_char->setValue(link);
}

// iFoodService implementation
iFoodService::iFoodService() : LeleBLEService() {}
iFoodService::~iFoodService() {}

void iFoodService::setup(NimBLEServer *pServer) {
    pService = pServer->createService("ABCD");
    cart_char = pService->createCharacteristic(
        "ABCD",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void iFoodService::end() {}

void iFoodService::confirmOrder(const String &qrCode) {
    cart_char->setValue(qrCode);
}

// SpotifyService implementation
SpotifyService::SpotifyService() : LeleBLEService() {}
SpotifyService::~SpotifyService() {}

void SpotifyService::setup(NimBLEServer *pServer) {
    pService = pServer->createService("19B9");
    playback_char = pService->createCharacteristic(
        "19B9",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void SpotifyService::end() {}

String SpotifyService::getPlaybackToken() {
    return String(playback_char->getValue().c_str());
}

// InstagramService implementation
InstagramService::InstagramService() : LeleBLEService() {}
InstagramService::~InstagramService() {}

void InstagramService::setup(NimBLEServer *pServer) {
    pService = pServer->createService("FACEB00C-0000-1000-8000-00805F9B34FB"); // Custom
    message_char = pService->createCharacteristic(
        "FACEB00C-0000-1000-8000-00805F9B34FB",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void InstagramService::end() {}

String InstagramService::getDirectMessages() {
    return String(message_char->getValue().c_str());
}
