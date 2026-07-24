#include "http_manager.h"
#include "HTTPClient.h"

bool HTTPManager::get_basic_response(JsonDocument& doc) {
    Serial.println("--- HTTP STARTS ---");
    HTTPClient http;

    String fullURL = "http://" + apiURL + cityName + "&appid=" + apiKey + "&units=metric";
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

WeatherData HTTPManager::get_weather() {
    WeatherData data;
    data.valid = false;

    JsonDocument doc;
    if (!get_basic_response(doc)) {
        return data;
    }

    if (doc.isNull() || !doc["main"].is<JsonObject>() || !doc["weather"].is<JsonArray>()) {
        return data;
    }

    snprintf(data.temp, sizeof(data.temp), "%.1fC", (float)doc["main"]["temp"]);
    snprintf(data.feels_like, sizeof(data.feels_like), "%.1fC", (float)doc["main"]["feels_like"]);
    snprintf(data.humidity, sizeof(data.humidity), "%d%%", (int)doc["main"]["humidity"]);
    snprintf(data.wind_speed, sizeof(data.wind_speed), "%.1fm/s", (float)doc["wind"]["speed"]);

    const char* statusStr = doc["weather"][0]["main"];
    if (statusStr != nullptr) {
        strlcpy(data.status, statusStr, sizeof(data.status));
    }

    data.valid = true;
    Serial.println("SUCCESS: weather parsed for main screen");
    
    return data;
}