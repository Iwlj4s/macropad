#include <Arduino.h>
#define LGFX_USE_V1
#include "time.h"
#include "button_manager.h"
#include "display_manager.h"
#include "animations_data.h"
#include "animations_manager.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include "weather_manager.h"

LGFX_SpaceCat lcd;
ButtonManager buttonHelper;
DisplayManager displayHelper;
WifiManager wifiHelper;
TimeManager timeHelper;
WeatherManager weatherHelper;
AnimationsManager animHelper;

const int BTN_NEXT_PIN = 5;
const int BTN_PREV_PIN = 6;

void setup() {
    delay(2000);
    Serial.begin(115200);

    Serial.println("=========================================");
    Serial.println("SETUP STARTED");
    Serial.println("=========================================\n");

    buttonHelper.init();

    Serial.printf("Total animations loaded: %d\n", animHelper.TOTAL_ANIMS);
    Serial.println("---------------------------\n");
    AnimationItem activeAnim = animHelper.get_currentAnimation();

    displayHelper.init(lcd);
    displayHelper.setAnimation(activeAnim.data, activeAnim.size);

    delay(1000);
    wifiHelper.connect();
    wifiHelper.getInfo();
    
    if (wifiHelper.isConnected()) {
        timeHelper.initNTP();
    }
}

void loop() {
    displayHelper.play(timeHelper.getFormatedTime()); 

    buttonHelper.checkButtons(animHelper, displayHelper);
    
    weatherHelper.updateWeather(wifiHelper, displayHelper);
    
}
