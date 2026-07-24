#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "display_config.h"
#include <AnimatedGIF.h> 
#include "http_manager.h"

class DisplayManager {
    private:
        LGFX_SpaceCat* lcd; 
        LGFX_Sprite weatherCanvas; 
        AnimatedGIF gif; 

        const uint8_t* current_gif;
        unsigned int current_size;

        unsigned long last_frame_time;
        int next_frame_delay;

        String cachedTemp = "--.-C";
        String cachedStatus = "Wait...";
        String cachedHum = "Hum: --%";
        String cachedWind = "WS: --m/s";

    public:
        void init(LGFX_SpaceCat& targetLcd); 
        void showReadyScreen();
        void updateButtonState(const char* StateText, uint16_t color);

        void setAnimation(const uint8_t* gif_array, unsigned int gif_size);
        void play();
        
        void drawWeather(String temp, String status, String hum, String wind);
        static void GIFDraw(GIFDRAW *pDraw);
};

#endif
