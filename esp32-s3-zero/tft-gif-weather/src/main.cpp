#include <Arduino.h>
#define LGFX_USE_V1
#include "display_manager.h"
#include "animations.h"
#include "wifi_manager.h"
#include "http_manager.h"

LGFX_SpaceCat lcd;
DisplayManager displayHelper;
WifiManager wifiHelper;
HTTPManager weatherHelper;

struct AnimationItem {
    const uint8_t* data;
    unsigned int size;
    const char* name;
};

AnimationItem animList[] = {
    { cat_in_boat_anim, cat_in_boat_size, "Cat in Boat" }
    // { cat_sleeping_anim, cat_sleeping_size, "Sleeping Cat" }, 
    // { cat_running_anim, cat_running_size, "Running Cat" }
};
const int TOTAL_ANIMS = sizeof(animList) / sizeof(animList[0]);
int currentAnimIndex = 0;

const int BTN_NEXT_PIN = 5;
const int BTN_PREV_PIN = 6;

void setup() {
    delay(2000);
    Serial.begin(115200);

    Serial.println("=========================================");
    Serial.println("SETUP STARTED");
    Serial.println("=========================================");
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);

    displayHelper.init(lcd);
    displayHelper.setAnimation(animList[currentAnimIndex].data, animList[currentAnimIndex].size);

    delay(1000);
    wifiHelper.connect();
    wifiHelper.getInfo();
}

void loop() {
    displayHelper.play(); 
    static WeatherData stableWeatherData; 
    static bool hasFirstWeather = false;
    
    static unsigned long last_weather_update = 0;
    
    if (wifiHelper.isConnected()) {
        if (last_weather_update == 0 || (millis() - last_weather_update > 3600000)) {
            WeatherData wData = weatherHelper.get_weather();
            if (wData.valid) {

                displayHelper.drawWeather(
                    String(wData.temp), 
                    String(wData.status), 
                    String(wData.humidity), 
                    String(wData.wind_speed)
                );
                last_weather_update = millis();
            }
        }
    }

    static unsigned long last_button_time = 0;
    
    if (millis() - last_button_time > 250) {
        
        if (digitalRead(BTN_PREV_PIN) == LOW) {
            currentAnimIndex--;
            if (currentAnimIndex < 0) {
                currentAnimIndex = TOTAL_ANIMS - 1; 
            }
            
            displayHelper.setAnimation(animList[currentAnimIndex].data, animList[currentAnimIndex].size);
            displayHelper.updateButtonState(animList[currentAnimIndex].name, TFT_GREEN);
            
            Serial.printf("Switch to PREV anim: %s\n", animList[currentAnimIndex].name);
            last_button_time = millis();
        }

        if (digitalRead(BTN_NEXT_PIN) == LOW) {
            currentAnimIndex++;
            if (currentAnimIndex >= TOTAL_ANIMS) {
                currentAnimIndex = 0; 
            }
            
            displayHelper.setAnimation(animList[currentAnimIndex].data, animList[currentAnimIndex].size);
            displayHelper.updateButtonState(animList[currentAnimIndex].name, TFT_WHITE);
            
            Serial.printf("Switch to NEXT anim: %s\n", animList[currentAnimIndex].name);
            last_button_time = millis();
        }
    }
}
