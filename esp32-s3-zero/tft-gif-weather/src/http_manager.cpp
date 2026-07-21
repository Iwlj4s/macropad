#include "http_manager.h"

String HTTPManager::get_temp(){
    Serial.println("--- HTTP STARTS ---");
    String fullURL = "http://" + apiURL + cityName + "&appid=" + apiKey + "&units=metric";
    http.begin(fullURL);
    int httpResponseCode = http.GET();

    String resultText = "--.- C";

    if (httpResponseCode == HTTP_CODE_OK) {
        Serial.println("Status: OK");
        String payload = http.getString();

        JsonDocument doc;
        
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            float temperature = doc["main"]["temp"];

            resultText = String(temperature, 1) + "C";
            Serial.print("SUCCESS: tempreture parsing");
            Serial.println(resultText);

        }
        else {
            Serial.println("FAILURE: JSON parsing failed: ");
            Serial.println(error.c_str());
        }
    }
    else{
        Serial.print("HTTP Error code: ");
        Serial.println(httpResponseCode);
    }

    http.end();

    return resultText; 

}