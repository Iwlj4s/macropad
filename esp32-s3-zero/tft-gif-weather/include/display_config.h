#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX_SpaceCat : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789  _panel_instance;
    lgfx::Bus_SPI       _bus_instance;

    public:
        LGFX_SpaceCat() {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;     
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;    // 40Mgh
            cfg.pin_sclk = 13;            
            cfg.pin_mosi = 12;            
            cfg.pin_miso = -1;            
            cfg.pin_dc   = 10;            
            _bus_instance.config(cfg);

            auto p_cfg = _panel_instance.config();
            p_cfg.pin_cs           = 9;   
            p_cfg.pin_rst          = 11;  
            p_cfg.panel_width      = 240;
            p_cfg.panel_height     = 320;
            p_cfg.offset_x         = 0;
            p_cfg.offset_y         = 0;
            p_cfg.invert           = true; 
            _panel_instance.config(p_cfg);

            _panel_instance.setBus(&_bus_instance);
            setPanel(&_panel_instance);
        }
};
#endif
