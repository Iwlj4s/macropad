#define LGFX_USE_V1 
#include "display_manager.h"
#include "weather_icons.h"

static DisplayManager* instance = nullptr;

static String currentButtonText = "";
static uint16_t currentButtonColor = TFT_WHITE;
static unsigned long buttonTextExpireTime = 0;

void DisplayManager::init(LGFX_SpaceCat& targetLcd) {
    lcd = &targetLcd;
    instance = this; 
    
    lcd->init();
    lcd->setRotation(1); 
    
    canvas.setColorDepth(16);
    canvas.createSprite(320, 240);
    canvas.fillScreen(0x780F);

    gif.begin(GIF_PALETTE_RGB565_BE); 
    
    current_gif = nullptr;
    current_size = 0;
    last_frame_time = 0;
    next_frame_delay = 0;
}

void DisplayManager::showReadyScreen() {
    canvas.fillScreen(0x780F); 
    canvas.setTextColor(TFT_WHITE, 0x780F); 
    canvas.setFont(&fonts::Font4); 
    canvas.setCursor(20, 80);
    canvas.print("ESP32 Modular Mod!"); 
    canvas.pushSprite(lcd, 0, 0);
}

void DisplayManager::updateButtonState(const char* StateText, uint16_t color) {
    currentButtonText = String(StateText);
    currentButtonColor = color;
    buttonTextExpireTime = millis() + 2000; 
}

void DisplayManager::setAnimation(const uint8_t* gif_array, unsigned int gif_size) {
    current_gif = gif_array;
    current_size = gif_size;
    canvas.fillScreen(0x780F); 
    gif.close(); 
    
    if (current_gif != nullptr && current_size > 0) {
        if (gif.openFLASH((uint8_t *)current_gif, (int)current_size, GIFDraw)) {
            Serial.println("GIF successfully opened!");
        }
    }
}

void DisplayManager::GIFDraw(GIFDRAW *pDraw) {
    if (instance == nullptr) return;
    
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
                instance->canvas.drawPixel(iX + i, iY, pPalette[s[i]]);
            }
        }
    } else {
        for (int i = 0; i < iWidth; i++) {
            line_buffer[i] = pPalette[s[i]];
        }
        instance->canvas.pushImage(iX, iY, iWidth, 1, line_buffer);
    }
}

void DisplayManager::play() {
    if (current_gif != nullptr && current_size > 0) {
        if (millis() - last_frame_time >= (unsigned long)next_frame_delay) {
            
            if (!gif.playFrame(true, &next_frame_delay)) {
                gif.reset(); 
            }
            last_frame_time = millis();

            if (hasWeather) {
                canvas.setTextColor(TFT_WHITE); 
                // Good fonts:
                // 1. &fonts::FreeMonoBold9pt7b
                // 2. &fonts::FreeMonoBoldOblique9pt7b
                canvas.setFont(&fonts::FreeMonoBoldOblique9pt7b); 

                // --- 1. Temp ---
                canvas.drawBitmap(10, 12, icon_thermometer, 16, 16, TFT_WHITE);
                canvas.setTextSize(1.2); 
                canvas.setCursor(32, 10);
                canvas.print(cachedTemp);

                canvas.setTextSize(1.0); 

                // --- 2. Weather Status (sunny, cloudy, e.t.c) ---
                const uint8_t* statusIcon = icon_status_cloud; 
                if (cachedStatus.equalsIgnoreCase("Clear")) {
                    statusIcon = icon_status_sun;
                } else if (cachedStatus.equalsIgnoreCase("Rain") || 
                           cachedStatus.equalsIgnoreCase("Drizzle") || 
                           cachedStatus.equalsIgnoreCase("Thunderstorm")) {
                    statusIcon = icon_status_rain;
                }
                
                canvas.drawBitmap(10, 42, statusIcon, 16, 16, TFT_WHITE);
                canvas.setCursor(32, 40);
                canvas.print(cachedStatus);

                // --- 3. Wind Speed ---
                canvas.drawBitmap(10, 72, icon_wind, 16, 16, TFT_WHITE);
                canvas.setCursor(32, 70);
                canvas.print(cachedWind);

                // --- 4. Humidity ---
                canvas.drawBitmap(10, 102, icon_drop, 16, 16, TFT_WHITE);
                canvas.setCursor(32, 100);
                canvas.print(cachedHum);
            }

            // 3.Button Status
            if (millis() < buttonTextExpireTime && currentButtonText != "") {
                canvas.setTextColor(currentButtonColor);
                canvas.setFont(&fonts::Font4);
                canvas.setTextSize(1.0); 
                canvas.setCursor(20, 210);
                canvas.print(currentButtonText);
            }

            canvas.pushSprite(lcd, 0, 0); 
        }
    }
}



void DisplayManager::drawWeather(String temp, String status, String hum, String wind) {
    cachedTemp = temp;
    cachedStatus = status;
    cachedHum = hum;
    cachedWind = wind;
    hasWeather = true;
    Serial.println("-> Weather strings updated in display cache!");
}
