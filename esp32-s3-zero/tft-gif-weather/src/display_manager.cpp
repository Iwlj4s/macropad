#define LGFX_USE_V1 
#include "display_manager.h"

static lgfx::LGFX_Device* global_lcd = nullptr;

void DisplayManager::init(LGFX_SpaceCat& targetLcd) {
    lcd = &targetLcd;
    global_lcd = lcd;
    
    lcd->init();
    lcd->setRotation(1); 
    
    gif.begin(GIF_PALETTE_RGB565_BE); 
    
    current_gif = nullptr;
    current_size = 0;
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
        if (gif.openFLASH((uint8_t *)current_gif, (int)current_size, GIFDraw)) {
            Serial.println("GIF successfully opened!");
        } else {
            Serial.println("Opening GIF Error!");
        }
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
}

void DisplayManager::play() {
    if (current_gif != nullptr && current_size > 0) {
        int delay_ms = 0;
        
        if (!gif.playFrame(true, &delay_ms)) {
            gif.reset(); 
        }
        
        if (delay_ms > 0) {
            delay(delay_ms);
        }
    }
}
