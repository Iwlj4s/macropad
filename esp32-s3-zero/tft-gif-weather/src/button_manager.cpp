#include "button_manager.h"

void ButtonManager::init() {
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);
    Serial.println("[BUTTONS]: Hardware pins initialized.");
    Serial.println("---------------------------\n");
}

void ButtonManager::checkButtons(AnimationsManager& animManager, DisplayManager& displayHelper) {
    if (millis() - lastButtonTime > 250) {
        
        if (digitalRead(BTN_PREV_PIN) == LOW) {
            animManager.nextAnimation();
            AnimationItem activeAnim = animManager.get_currentAnimation();

            displayHelper.setAnimation(activeAnim.data, activeAnim.size);
            displayHelper.updateButtonState(activeAnim.name, TFT_GREEN);
            
            Serial.printf("Switch to PREV anim: %s\n", activeAnim.name);
            lastButtonTime = millis();
        }

        if (digitalRead(BTN_NEXT_PIN) == LOW) {
            animManager.prevAnimation();

            AnimationItem activeAnim = animManager.get_currentAnimation();
            displayHelper.setAnimation(activeAnim.data, activeAnim.size);
            displayHelper.updateButtonState(activeAnim.name, TFT_WHITE);
            
            Serial.printf("Switch to NEXT anim: %s\n", activeAnim.name);
            lastButtonTime = millis();
        }
    }
}