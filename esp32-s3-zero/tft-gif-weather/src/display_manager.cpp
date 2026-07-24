#define LGFX_USE_V1 
#include "display_manager.h"

static lgfx::LGFX_Device* global_lcd = nullptr;

static String gTemp = "--.-C", gStatus = "Wait...", gHum = "Hum: --%", gWind = "WS: --m/s";

void DisplayManager::init(LGFX_SpaceCat& targetLcd) {
    lcd = &targetLcd;
    global_lcd = lcd;
    
    lcd->init();
    lcd->setRotation(1); 
    gif.begin(GIF_PALETTE_RGB565_BE); 
}

void DisplayManager::showReadyScreen() {
    lcd->fillScreen(0x780F); 
    lcd->setTextColor(TFT_WHITE, 0x780F); 
    lcd->setFont(&fonts::Font4); 
    lcd->setCursor(20, 80);
    lcd->print("ESP32 Modular Mod!"); 
}

void DisplayManager::updateButtonState(const char* StateText, uint16_t color) {
    lcd->fillRect(20, 210, 280, 25, 0x780F); 
    lcd->setCursor(20, 212); 
    lcd->setTextColor(color, 0x780F); 
    lcd->print(StateText); 
}

void DisplayManager::setAnimation(const uint8_t* gif_array, unsigned int gif_size) {
    current_gif = gif_array;
    current_size = gif_size;
    lcd->fillScreen(0x780F); 
    gif.close(); 
    
    if (current_gif != nullptr && current_size > 0) {
        gif.openFLASH((uint8_t *)current_gif, (int)current_size, GIFDraw);
    }
}

void DisplayManager::GIFDraw(GIFDRAW *pDraw) {
    if (global_lcd == nullptr) return;
    
    uint8_t *s = pDraw->pPixels;
    uint16_t *pPalette = pDraw->pPalette;
    int iX = pDraw->iX;
    int iY = pDraw->iY + pDraw->y;
    int iWidth = pDraw->iWidth;

    uint16_t line_buffer[320]; 

    if (pDraw->ucHasTransparency) {
        uint8_t ucTransparent = pDraw->ucTransparent;
        for (int i = 0; i < iWidth; i++) {
            if (s[i] != ucTransparent) {
                global_lcd->drawPixel(iX + i, iY, pPalette[s[i]]);
            }
        }
    } else {
        for (int i = 0; i < iWidth; i++) {
            line_buffer[i] = pPalette[s[i]];
        }
        global_lcd->pushImage(iX, iY, iWidth, 1, line_buffer);
    }

    if (pDraw->y == pDraw->iHeight - 1) {
        
        global_lcd->setTextColor(TFT_WHITE); 
        
        global_lcd->setFont(&fonts::Font4);
        global_lcd->setCursor(10, 10); 
        global_lcd->print(gTemp);

        global_lcd->setCursor(10, 40);  global_lcd->print(gStatus);
        global_lcd->setCursor(10, 70);  global_lcd->print(gHum);
        global_lcd->setCursor(10, 100); global_lcd->print(gWind);
    }
}

void DisplayManager::play() {
    if (current_gif != nullptr && current_size > 0) {
        if (millis() - last_frame_time >= next_frame_delay) {
            if (!gif.playFrame(true, &next_frame_delay)) {
                gif.reset(); 
            }
            last_frame_time = millis();
        }
    }
}

void DisplayManager::drawWeather(String temp, String status, String hum, String wind) {
    gTemp = temp;
    gStatus = status;
    gHum = hum;
    gWind = wind;
}
