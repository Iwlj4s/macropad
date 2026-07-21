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

LGFX_Sprite weatherSprite(&lcd); 
String weatherText = "--.- C";

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

    // Display init and set anim
    displayHelper.init(lcd);
    displayHelper.setAnimation(cat_in_boat_anim, cat_in_boat_size);

    delay(1000);
    // Connect to the wifi
    wifiHelper.connect();
    wifiHelper.getInfo();

    weatherSprite.createSprite(120, 30);

}

void loop() {
    displayHelper.play(); 
    
    static unsigned long last_weather_update = 0;

    if (wifiHelper.isConnected()) {
        if (last_weather_update == 0 || (millis() - last_weather_update > 3600000)){
            weatherText = weatherHelper.get_temp();
            Serial.println("Temp: " + weatherText);

            weatherSprite.fillScreen(0); 
            weatherSprite.setCursor(0, 0);
            weatherSprite.setTextColor(TFT_YELLOW); 
            weatherSprite.setFont(&fonts::Font4);
            weatherSprite.print(weatherText);

            last_weather_update = millis();
        } 
    } 

    weatherSprite.pushSprite(20, 20, 0x0000);

    if (digitalRead(BTN_NEXT_PIN) == LOW){
        displayHelper.updateButtonState(" NEXT - ACTIVE", TFT_WHITE);
        Serial.println("NEXT - ACTIVE");
        delay(250);
    }

    if (digitalRead(BTN_PREV_PIN) == LOW){
        displayHelper.updateButtonState(" PREV - ACTIVE", TFT_WHITE);
        Serial.println("PREV - ACTIVE");
        delay(250);
    }
}