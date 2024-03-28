/*
    ESP-NOW Broadcast Master
    Lucas Saavedra Vaz - 2024

    This sketch demonstrates how to broadcast messages to all devices within the ESP-NOW network.
    This example is intended to be used with the ESP-NOW Broadcast Slave example.

    The master device will broadcast a message every 5 seconds to all devices within the network.
    This will be done using by registering a peer object with the broadcast address.

    The slave devices will receive the broadcasted messages and print them to the Serial Monitor.
*/

#include "ESP32_NOW.h"
#include "WiFi.h"

#include <esp_mac.h> // For the MAC2STR and MACSTR macros

/* Definitions */

#define ESPNOW_WIFI_CHANNEL 6

/* Classes */

// Create a new class that inherits from the ESP_NOW_Peer class is required to implement the _onReceive and _onSent methods.

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
    ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk);
    ~ESP_NOW_Broadcast_Peer();

    bool begin();
    bool send_message(const uint8_t *data, size_t len);

    // ESP_NOW_Peer interfaces
    void _onReceive(const uint8_t *data, size_t len, bool broadcast);
    void _onSent(bool success);
};

/* Methods */

// Constructor of the class using the broadcast address
ESP_NOW_Broadcast_Peer::ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
  : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

// Destructor of the class
ESP_NOW_Broadcast_Peer::~ESP_NOW_Broadcast_Peer() {
    remove();
}

// Function to properly initialize the ESP-NOW and register the broadcast peer
bool ESP_NOW_Broadcast_Peer::begin() {
    if (!ESP_NOW.begin() || !add()) {
        log_e("Failed to initialize ESP-NOW or register the broadcast peer");
        return false;
    }
    return true;
}

// Function to send a message to all devices within the network
bool ESP_NOW_Broadcast_Peer::send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
        log_e("Failed to broadcast message");
        return false;
    }
    return true;
}

void ESP_NOW_Broadcast_Peer::_onReceive(const uint8_t *data, size_t len, bool broadcast) {
    // The broadcast peer will never receive any data. Rather, it will only send data.
    // Data broadcasted will be received by the actual object of the peer that made the broadcast.
    // It is still required to be implemented because it is a pure virtual method.
}

void ESP_NOW_Broadcast_Peer::_onSent(bool success) {
    // As broadcast messages does not require any acknowledgment, this method will never be called.
    // It is still required to be implemented because it is a pure virtual method.
}

/* Global Variables */

uint32_t msg_count = 0;

// Create a boradcast peer object
ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

/* Main */

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    Serial.println("ESP-NOW Example - Broadcast Master");
    Serial.println("Wi-Fi parameters:");
    Serial.println("  Mode: STA");
    Serial.println("  MAC Address: " + WiFi.macAddress());
    Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

    // Initialize the Wi-Fi module
    WiFi.mode(WIFI_STA);
    WiFi.setChannel(ESPNOW_WIFI_CHANNEL);

    // Register the broadcast peer
    if (!broadcast_peer.begin()) {
        Serial.println("Failed to initialize broadcast peer");
        Serial.println("Reebooting in 5 seconds...");
        delay(5000);
        ESP.restart();
    }

    Serial.println("Setup complete. Broadcasting messages every 5 seconds.");
}

void loop() {
    // Broadcast a message to all devices within the network
    char data[32];
    snprintf(data, sizeof(data), "Hello, World! #%lu", msg_count++);

    Serial.printf("Broadcasting message: %s\n", data);

    if (!broadcast_peer.send_message((uint8_t *)data, sizeof(data))) {
        Serial.println("Failed to broadcast message");
    }

    delay(5000);
}
