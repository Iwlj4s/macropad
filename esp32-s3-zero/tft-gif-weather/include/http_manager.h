#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include <ArduinoJson.h>
#include <secrets.h>

class HTTPManager {
    private:
        const char *apiKey = WEATHER_API_KEY;
    public:
        bool getJsonResponse(const String& fullURL, JsonDocument& doc);

};

#endif