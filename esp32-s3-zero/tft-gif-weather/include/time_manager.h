#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include "time.h"

class TimeManager {
    private:
        const char* ntpServer = "pool.ntp.org";
        const char* timeZone = "MSK-3";

    public:
        void initNTP();

        String getFormatedTime();
};

#endif