#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "display_config.h"
#include <AnimatedGIF.h> 

class DisplayManager {
private:
    LGFX_SpaceCat* lcd; 
    AnimatedGIF gif; 

    const uint8_t* current_gif;
    unsigned int current_size;

public:
    void init(LGFX_SpaceCat& targetLcd); 
    void showReadyScreen();
    void updateButtonState(const char* StateText, uint16_t color);

    void setAnimation(const uint8_t* gif_array, unsigned int gif_size);
    void play();

    static void GIFDraw(GIFDRAW *pDraw);
};

#endif
