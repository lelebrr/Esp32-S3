#pragma once

#include <NimBLEDevice.h>
#include <globals.h>

void setupContinuitySpoof();
void startContinuityAdvertising(const char *deviceName);
void stopContinuitySpoof();
void rotateMAC();
void enterDeepSleep();
void wakeOnBLEScan();
void runContinuitySpoof();
void runLowbattBLE();
void runAirTagSpoof();
void runAudioBleed();
void runBLENetworkDriverSpoof();
void runUberBLE();
void runNetflixBLE();
void runiFoodBLE();
void runSpotifyBLE();
void runInstagramBLE();
void runBLEDeauthAttack();
