#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include <ArduinoJson.h>
#include <secrets.h>

struct WeatherData {
    char temp[12]       = "--.-C";
    char feels_like[12] = "--.-C";
    char humidity[8]    = "--%";
    char wind_speed[16] = "-.-m/s";
    char status[24]     = "Unknown";
    bool valid = false;
};

class HTTPManager {
    private:
        const char *apiKey = WEATHER_API_KEY;
        String cityName = "Cherepovets";

        String apiURL = "api.openweathermap.org/data/2.5/weather?q=";    
    public:
        bool get_basic_response(JsonDocument& doc);
        WeatherData get_weather();

};

#endif