#include "http_manager.h"
#include "HTTPClient.h"

bool HTTPManager::getJsonResponse(const String& fullURL, JsonDocument& doc) {
    Serial.println("--- HTTP REQUEST STARTS ---");
    HTTPClient http;

    if (!http.begin(fullURL)) {
        Serial.println("HTTP begin failed!");
        return false;
    }

    int httpResponseCode = http.GET();
    if (httpResponseCode != HTTP_CODE_OK) {
        Serial.print("HTTP Error code: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }

    Serial.println("Status: OK");
    String payload = http.getString();
    http.end(); 

    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print("FAILURE: Json parsing failed: ");
        Serial.println(error.c_str());
        return false;
    }

    return true;
}