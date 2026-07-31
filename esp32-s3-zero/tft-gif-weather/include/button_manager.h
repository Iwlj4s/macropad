#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "animations_manager.h"
#include "display_manager.h"

class ButtonManager {
    private:
        const int BTN_NEXT_PIN = 5;
        const int BTN_PREV_PIN = 6;

        unsigned long lastButtonTime = 0;
    
    public:
        void init();
        void checkButtons(AnimationsManager& animManager, DisplayManager& displayHelper);
};

#endif