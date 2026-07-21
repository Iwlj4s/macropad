#include "wifi_manager.h"

void WifiManager::connect() {
    Serial.println("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println(".");
    }
    Serial.println("---------------------------\n");

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("--- SUCCESS Wifi connected ---");
    }
    else {
        Serial.println("--- FAILURE Cant connect to this ssid ---");
    }
}

void WifiManager::getInfo() {
    Serial.println("\n---Checking connection ---");

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Status: CONNECTED");

        Serial.println("IP Adress: ");
        Serial.println(WiFi.localIP());

        Serial.println("Signal Strength (RSSI): ");
        Serial.println(WiFi.RSSI());
        Serial.println("dBm");
    }
    else {
        Serial.println("Status: DISCONNECTED");
        Serial.print("Reason Code: ");
        Serial.println(WiFi.status()); 
    }
    Serial.println("---------------------------\n");

}

bool WifiManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}