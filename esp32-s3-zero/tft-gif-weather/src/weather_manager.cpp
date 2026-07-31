#include "weather_manager.h"
#include "wifi_manager.h"
#include "display_manager.h"


WeatherData WeatherManager::getWeather() {
    WeatherData data;

    data.valid = false;

    // Create full URL for get all weather
    String fullURL = "http://" + apiURL + cityName + "&appid=" + apiKey + "&units=metric";

    JsonDocument doc;


    // If network down return empty struct
    if (!network.getJsonResponse(fullURL, doc)) {
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

void WeatherManager::updateWeather(WifiManager& wifiHelper, DisplayManager& displayHelper) {
    if (wifiHelper.isConnected()) {
            if (lastWeatherUpdate == 0 || (millis() - lastWeatherUpdate > 3600000)) {
                WeatherData wData = getWeather();
                if (wData.valid) {

                    displayHelper.setWeather(
                        String(wData.temp), 
                        String(wData.status), 
                        String(wData.humidity), 
                        String(wData.wind_speed)
                    );
                    lastWeatherUpdate = millis();
                }
            }
        }
}