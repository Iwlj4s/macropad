#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <secrets.h>

class HTTPManager {
    private:
        HTTPClient http;

        const char *apiKey = WEATHER_API_KEY;
        String cityName = "Cherepovets";

        String apiURL = "api.openweathermap.org/data/2.5/weather?q=";    
    public:
        String get_temp();

};

#endif