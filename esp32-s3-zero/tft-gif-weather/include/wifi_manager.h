#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <secrets.h>

class WifiManager {
    private:
        const char *ssid = WIFI_SSID;
        const char *password = WIFI_PASS;

    public:
        void connect();
        void getInfo();

        // Getter for fast check connection in main loop
        bool isConnected();
};

#endif