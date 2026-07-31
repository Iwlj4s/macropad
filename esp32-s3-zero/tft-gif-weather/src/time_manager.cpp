#include "time_manager.h"

void TimeManager::initNTP() {
    configTime(0, 0, ntpServer);
    setenv("TZ", timeZone, 1);
    tzset();
    Serial.println("[TIME]: SUCCESS NTP Time Sync Initialized");
    Serial.println("---------------------------\n");
}

String TimeManager::getFormatedTime() {
    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {
        return "--:--";
    }

    char timeStringBuff[16];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeInfo);

    return String(timeStringBuff);
}