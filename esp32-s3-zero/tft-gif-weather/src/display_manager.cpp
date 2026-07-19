#include "display_manager.h"

void DisplayManager::init(LGFX_SpaceCat& targetLcd) {
    lcd = &targetLcd;
    lcd->init();
    lcd->setRotation(1);
}

void DisplayManager::showReadyScreen() {
    lcd->fillScreen(0x780F); 
    lcd->setTextColor(TFT_WHITE, 0x780F); 
    lcd->setFont(&fonts::Font4); 
    lcd->setCursor(20, 80);
    lcd->print("ESP32 Modular Mod!"); 
}

void DisplayManager::updateButtonState(const char* StateText, uint16_t color) {
    lcd->fillRect(20, 150, 280, 40, 0x780F); 
    lcd->setCursor(20, 150); 
    lcd->setTextColor(color, 0x780F); 
    lcd->print(StateText); 
}
