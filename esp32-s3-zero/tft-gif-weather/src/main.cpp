#include <Arduino.h>
#define LGFX_USE_V1
#include "display_manager.h"
#include "animations.h"

LGFX_SpaceCat lcd;
DisplayManager displayHelper;

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
    displayHelper.setAnimation(cat_in_boat_anim, cat_in_boat_size);
}

void loop() {
    displayHelper.play();
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