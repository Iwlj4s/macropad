#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

static lgfx::Panel_ST7789 _panel;
static lgfx::Bus_SPI      _bus;
lgfx::LGFX_Device lcd;

const int BTN_NEXT_PIN = 5;
const int BTN_PREV_PIN = 6;

void setup() {
    delay(2000); 
    Serial.begin(115200);
    Serial.println("=========================================");
    Serial.println("STARTING");
    Serial.println("=========================================");

    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_PREV_PIN, INPUT_PULLUP);

    auto cfg = _bus.config();
    cfg.spi_host = SPI2_HOST;     
    cfg.spi_mode = 0;
    cfg.freq_write = 60000000;    
    cfg.pin_sclk = 13;            // CLK -> IO13
    cfg.pin_mosi = 12;            // MOSI -> IO12
    cfg.pin_miso = -1;            
    cfg.pin_dc   = 10;            // DC -> IO10
    _bus.config(cfg);
    _panel.setBus(&_bus);

    auto p_cfg = _panel.config();
    p_cfg.pin_cs           = 9;   // CS -> IO9
    p_cfg.pin_rst          = 11;  // RESET -> IO11
    p_cfg.panel_width      = 240;
    p_cfg.panel_height     = 320;
    p_cfg.offset_x         = 0;
    p_cfg.offset_y         = 0;
    p_cfg.invert           = true; 
    _panel.config(p_cfg);
    lcd.setPanel(&_panel);

    lcd.init();
    lcd.setRotation(1);
    lcd.fillScreen(0x780F); 
    
    lcd.setTextColor(TFT_WHITE, 0x780F);
    lcd.setFont(&fonts::Font4);
    lcd.setCursor(20, 80);
    lcd.print("Pico-ESP SpaceCat OK!");
    
    lcd.setCursor(20, 110);
    lcd.print("Waiting for keys...");
    Serial.println("Graphik working");
}

void loop() {
    static unsigned long last_print = 0;
    if (millis() - last_print > 3000) {
        Serial.println("All working waiting buttons...");
        last_print = millis();
    }

    if (digitalRead(BTN_NEXT_PIN) == LOW){
        Serial.println("Log: Button NEXT pressed");
        lcd.fillRect(20, 150, 280, 40, 0x780F); 
        lcd.setCursor(20, 150);
        lcd.setTextColor(TFT_WHITE, 0x780F);
        lcd.print("NEXT: ACTIVE");
        delay(250); 
    }

    if (digitalRead(BTN_PREV_PIN) == LOW) {
        Serial.println("Log: Button PREV pressed");
        lcd.fillRect(20, 150, 280, 40, 0x780F);
        lcd.setCursor(20, 150);
        lcd.setTextColor(TFT_YELLOW, 0x780F);
        lcd.print("PREV: ACTIVE");
        delay(250);
    }
}
