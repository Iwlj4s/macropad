#ifndef WEATHER_MANAGER_H
#define WEATHER_MANAGER_H

#include <Arduino.h>
#include <secrets.h>
#include "http_manager.h"

class WifiManager;
class DisplayManager;

struct WeatherData {
    char temp[12]       = "--.-C";
    char feels_like[12] = "--.-C";
    char humidity[8]    = "--%";
    char wind_speed[16] = "-.-m/s";
    char status[24]     = "Unknown";
    bool valid = false;
};

class WeatherManager {
    private:
        const char *apiKey = WEATHER_API_KEY;
        String cityName = "Cherepovets";
        // Basic api URL
        String apiURL = "api.openweathermap.org/data/2.5/weather?q=";    

        unsigned long lastWeatherUpdate = 0;


        // Create local network manager
        HTTPManager network;
    
    public:
        WeatherData getWeather();
        void updateWeather(WifiManager& wifiHelper, DisplayManager& displayHelper);
};

#endif