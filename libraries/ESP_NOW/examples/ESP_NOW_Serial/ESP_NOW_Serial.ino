/*
    ESP-NOW Serial Example - Unicast transmission
    Lucas Saavedra Vaz - 2024
    Send data between two ESP32s using the ESP-NOW protocol in one-to-one (unicast) configuration.
    Note that different MAC addresses are used for different interfaces.
    The devices can be in different modes (AP or Station) and still communicate using ESP-NOW.
    The only requirement is that the devices are on the same Wi-Fi channel.
    Set the peer MAC address according to the device that will receive the data.

    Example setup:
    - Device 1: AP mode with MAC address F6:12:FA:42:B6:E8
                Peer MAC address set to the Station MAC address of Device 2 (F4:12:FA:40:64:4C)
    - Device 2: Station mode with MAC address F4:12:FA:40:64:4C
                Peer MAC address set to the AP MAC address of Device 1 (F6:12:FA:42:B6:E8)

    The device running this sketch will also receive and print data from any device that has its MAC address set as the peer MAC address.
    To properly visualize the data being sent, set the line ending in the Serial Monitor to "Both NL & CR".
*/

#include "ESP32_NOW_Serial.h"
#include "MacAddress.h"
#include "WiFi.h"

#include "esp_wifi.h"

// 0: AP mode, 1: Station mode
#define ESPNOW_WIFI_MODE_STATION 1

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 1

#if ESPNOW_WIFI_MODE_STATION // ESP-NOW using WiFi Station mode
    #define ESPNOW_WIFI_IF   WIFI_IF_STA
    #define GET_IF_MAC       WiFi.macAddress

    // Set the MAC address of the device that will receive the data
    // For example: F6:12:FA:42:B6:E8
    const MacAddress peer_mac({0xF6, 0x12, 0xFA, 0x42, 0xB6, 0xE8});
#else // ESP-NOW using WiFi AP mode
    #define ESPNOW_WIFI_IF   WIFI_IF_AP
    #define GET_IF_MAC       WiFi.softAPmacAddress

    // Set the MAC address of the device that will receive the data
    // For example: F4:12:FA:40:64:4C
    const MacAddress peer_mac({0xF4, 0x12, 0xFA, 0x40, 0x64, 0x4C});
#endif

ESP_NOW_Serial_Class NowSerial(peer_mac, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IF);

void setup() {
    Serial.begin(115200);

    Serial.print("WiFi Mode: ");

    #if ESPNOW_WIFI_MODE_STATION
    Serial.println("STA");
    WiFi.mode(WIFI_STA);
    // ToDo: Set the channel using WiFi.setChannel() when using Station mode
    esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
    #else
    Serial.println("AP");
    WiFi.softAP(WiFi.getHostname(), NULL, ESPNOW_WIFI_CHANNEL, 1);
    #endif

    Serial.print("Channel: ");
    Serial.println(ESPNOW_WIFI_CHANNEL);

    Serial.print("MAC Address: ");
    Serial.println(GET_IF_MAC());

    // Start the ESP-NOW communication
    Serial.println("ESP-NOW communication starting...");
    NowSerial.begin(115200);
    Serial.println("You can now send data to the peer device using the Serial Monitor.\n");
}

void loop() {
    while (NowSerial.available())
    {
        Serial.write(NowSerial.read());
    }

    while (Serial.available() && NowSerial.availableForWrite())
    {
        if (NowSerial.write(Serial.read()) <= 0)
        {
            Serial.println("Failed to send data");
            break;
        }
    }

    delay(1);
}
