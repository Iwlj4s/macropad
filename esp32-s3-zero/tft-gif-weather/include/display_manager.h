#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "display_config.h"

class DisplayManager {
private:
    LGFX_SpaceCat* lcd; 
public:
    void init(LGFX_SpaceCat& targetLcd); 
    void showReadyScreen();
    void updateButtonState(const char* StateText, uint16_t color);
};

#endif
