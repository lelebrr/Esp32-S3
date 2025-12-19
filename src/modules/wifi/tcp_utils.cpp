/**
 * @file tcp_utils.cpp
 * @brief Utilitários TCP com suporte a leitura em background
 * @note Implementa task FreeRTOS para leitura assíncrona de dados TCP
 */
#include "modules/wifi/tcp_utils.h"
#include "core/wifi/wifi_common.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Buffer circular para dados recebidos em background
static QueueHandle_t tcpRxQueue = NULL;
static const int TCP_RX_QUEUE_SIZE = 256;
static TaskHandle_t tcpBgTaskHandle = NULL;
static volatile bool tcpBgRunning = false;
static WiFiClient* tcpBgClient = nullptr;

// Task de leitura em background
void tcpBackgroundReadTask(void* param) {
    WiFiClient* client = (WiFiClient*)param;
    
    while (tcpBgRunning && client->connected()) {
        if (client->available()) {
            char c = client->read();
            if (tcpRxQueue != NULL) {
                xQueueSend(tcpRxQueue, &c, 0); // Non-blocking
            }
        }
        vTaskDelay(1); // Yield para outras tasks
    }
    
    tcpBgRunning = false;
    vTaskDelete(NULL);
}

// Inicia leitura em background para um cliente
bool startTcpBackgroundRead(WiFiClient* client) {
    if (tcpBgRunning) return false;
    
    if (tcpRxQueue == NULL) {
        tcpRxQueue = xQueueCreate(TCP_RX_QUEUE_SIZE, sizeof(char));
    } else {
        xQueueReset(tcpRxQueue);
    }
    
    tcpBgClient = client;
    tcpBgRunning = true;
    
    xTaskCreate(tcpBackgroundReadTask, "TCP_BG_Read", 2048, client, 1, &tcpBgTaskHandle);
    return true;
}

// Para leitura em background
void stopTcpBackgroundRead() {
    tcpBgRunning = false;
    if (tcpBgTaskHandle != NULL) {
        vTaskDelay(10);
        tcpBgTaskHandle = NULL;
    }
}

// Lê caractere do buffer de background
int readTcpBackground() {
    char c;
    if (tcpRxQueue != NULL && xQueueReceive(tcpRxQueue, &c, 0) == pdTRUE) {
        return c;
    }
    return -1;
}

// Dados disponíveis no buffer de background
int availableTcpBackground() {
    if (tcpRxQueue == NULL) return 0;
    return uxQueueMessagesWaiting(tcpRxQueue);
}

bool inputMode;

void listenTcpPort() {
    if (!wifiConnected) wifiConnectMenu();

    WiFiClient tcpClient;
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    String portNumber = keyboard("", 5, "TCP port to listen");
    if (portNumber.length() == 0) {
        displayError("No port number given, exiting");
        return;
    }
    int portNumberInt = atoi(portNumber.c_str());
    if (portNumberInt == 0) {
        displayError("Invalid port number, exiting");
        return;
    }

    WiFiServer server(portNumberInt);
    server.begin();

    tft.println("Listening...");
    tft.print(WiFi.localIP().toString().c_str());
    tft.println(":" + portNumber);

    for (;;) {
        WiFiClient client = server.accept(); // Wait for a client to connect

        if (client) {
            Serial.println("Client connected");
            tft.println("Client connected");

            while (client.connected()) {
                if (inputMode) {
                    String keyString = keyboard("", 16, "send input data, q=quit");
                    if (keyString == "q") {
                        displayError("Exiting Listener");
                        client.stop();
                        server.stop();
                        return;
                    }
                    inputMode = false;
                    tft.fillScreen(TFT_BLACK);
                    tft.setCursor(0, 0);
                    if (keyString.length() > 0) {
                        client.print(keyString); // Send the entire string to the client
                        Serial.print(keyString);
                    }
                } else {
                    if (client.available()) {
                        char incomingChar = client.read(); // Read one byte at time from the client
                        tft.print(incomingChar);
                        Serial.print(incomingChar);
                    }
                    if (check(SelPress)) { inputMode = true; }
                }
            }
            client.stop();
            Serial.println("Client disconnected");
            displayError("Client disconnected");
        }
        if (check(EscPress)) {
            displayError("Exiting Listener");
            server.stop();
            break;
        }
    }
}

void clientTCP() {
    if (!wifiConnected) wifiConnectMenu();

    String serverIP = keyboard("", 15, "Enter server IP");
    String portString = keyboard("", 5, "Enter server Port");
    int portNumber = atoi(portString.c_str());

    if (serverIP.length() == 0 || portNumber == 0) {
        displayError("Invalid IP or Port");
        return;
    }

    WiFiClient client;
    if (!client.connect(serverIP.c_str(), portNumber)) {
        displayError("Connection failed");
        return;
    }

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Connected to:");
    tft.println(serverIP + ":" + portString);
    Serial.println("Connected to server");

    while (client.connected()) {
        if (inputMode) {
            String keyString = keyboard("", 16, "send input data");
            inputMode = false;
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0);
            if (keyString.length() > 0) {
                client.print(keyString);
                Serial.print(keyString);
            }
        } else {
            if (client.available()) {
                char incomingChar = client.read();
                tft.print(incomingChar);
                Serial.print(incomingChar);
            }
            if (check(SelPress)) { inputMode = true; }
        }
        if (check(EscPress)) {
            displayError("Exiting Client");
            client.stop();
            break;
        }
    }

    displayError("Connection closed.");
    Serial.println("Connection closed.");
    client.stop();
}
