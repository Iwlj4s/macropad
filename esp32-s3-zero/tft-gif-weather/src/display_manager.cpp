#define LGFX_USE_V1 
#include "display_manager.h"
#include "weather_icons.h"
#include <LittleFS.h> 

// Singleton pointer to maintain a globally accessible reference for static callbacks
static DisplayManager* instance = nullptr;

// UI State variables for button click feedback overlays
static String currentButtonText = "";
static uint16_t currentButtonColor = TFT_WHITE;
static unsigned long buttonTextExpireTime = 0;


// Global raw file buffer allocated dynamically inside the high-speed external Octal PSRAM.
// It holds the entire extracted .gif binary data to eliminate filesystem seek latency during playback.
static uint8_t* global_file_buffer = nullptr;

void DisplayManager::init(LGFX_SpaceCat& targetLcd) {
    /**
     * Initializes the hardware display, configures LovyanGFX sprite configurations,
     * and mounts the LittleFS partition.
     */
    lcd = &targetLcd;
    instance = this; 
    
    lcd->init();
    lcd->setRotation(1); // Set landscape orientation
    
    // Allocate a 16-bit RGB565 canvas sprite buffer in internal SRAM for tear-free rendering
    canvas.setColorDepth(16);
    canvas.createSprite(320, 240);
    canvas.fillScreen(0x780F); // Fill with signature SpaceCat purple color

    // Baseline initialization for the AnimatedGIF decoder instance
    gif.begin(GIF_PALETTE_RGB565_BE); 
    
    current_gif = nullptr;
    current_size = 0;
    last_frame_time = 0;
    next_frame_delay = 0;

    // Dynamic mount execution for the LittleFS partition on the 8MB flash module
    if (!LittleFS.begin(true)) {
        Serial.println("[FS ERROR]: LittleFS Mount Failed!");
    } else {
        Serial.println("[FS SUCCESS]: LittleFS Mounted Successfully!");
    }
}

void DisplayManager::showReadyScreen() {
    /**
     * Displays an immediate splash/loading screen upon microcontroller boot phase.
    */
    canvas.fillScreen(0x780F); 
    canvas.setTextColor(TFT_WHITE, 0x780F); 
    canvas.setFont(&fonts::Font4); 
    canvas.setCursor(20, 80);
    canvas.print("ESP32 Modular Mod!"); 
    canvas.pushSprite(lcd, 0, 0);
}

void DisplayManager::updateButtonState(const char* StateText, uint16_t color) {
    /**
     * Updates the state overlay cache when an animation cycle is triggered via hardware buttons.
    */
    currentButtonText = String(StateText);
    currentButtonColor = color;
    buttonTextExpireTime = millis() + 2000; // Keep message visible for 2 seconds
}

void DisplayManager::setAnimation(const uint8_t* gif_array, unsigned int gif_size) {
    /**
     * Prepares and streams requested animations. Handles memory segmentation between 
     * internal Flash-resident micro-GIFs (Boat) and massive PSRAM-loaded file system animations (Fire).
    */
    current_gif = gif_array;
    current_size = gif_size;
    canvas.fillScreen(0x780F); 
    gif.close(); // Clean terminate any active decoders running
    
    // Crucial memory protection: free heap block before requesting a new allocation
    if (global_file_buffer != nullptr) {
        delete[] global_file_buffer;
        global_file_buffer = nullptr;
    }

    if (current_gif != nullptr && current_size > 0) {
        /**
         * HEAP CONFIGURATION CHECK:
         * If the current requested size exceeds 500KB, it's processed as a high-fidelity 
         * background animation stored natively in LittleFS storage.
         */
        if (current_size > 500000) { 
            // Crucial: Pillow-generated GIFs utilize Little Endian palettes.
            gif.begin(GIF_PALETTE_RGB565_LE); 

            File gifFile = LittleFS.open("/fire.gif", "r");
            if (gifFile) {
                size_t fileSize = gifFile.size();
                
                // Allocate contiguous heap space in external Octal PSRAM using non-throwing fallback allocator
                global_file_buffer = new (std::nothrow) uint8_t[fileSize];
                
                if (global_file_buffer != nullptr) {
                    // Block read operation to blast data from the partition straight into PSRAM arrays
                    gifFile.read(global_file_buffer, fileSize);
                    gifFile.close();

                    // Open the dynamic memory array. Does not lock or trigger LittleFS file system crashes.
                    if (gif.open(global_file_buffer, fileSize, GIFDraw)) {
                        Serial.println("[GIFFS SUCCESS]: Fire.gif loaded to RAM and running!");
                    } else {
                        Serial.printf("[GIFFS ERROR]: AnimatedGIF parse failed! Code: %d\n", gif.getLastError());
                        delete[] global_file_buffer;
                        global_file_buffer = nullptr;
                    }
                } else {
                    Serial.println("[MEMORY ERROR]: RAM allocation for fire.gif failed!");
                    gifFile.close();
                }
            } else {
                Serial.println("[FS ERROR]: fire.gif not found in LittleFS storage!");
            }
        } else {
            /**
             * STANDARD FLASH MICRO-GIF BACKUP PATHWAY (e.g. Cat in Boat)
             * Uses regular openFLASH stream. Utilizes Big Endian formatting scheme natively.
             */
            gif.begin(GIF_PALETTE_RGB565_BE); 
            if (gif.openFLASH((uint8_t *)current_gif, (int)current_size, GIFDraw)) {
                Serial.println("Boat GIF successfully opened from Flash!");
            }
        }
    }
}

void DisplayManager::GIFDraw(GIFDRAW *pDraw) {
    /**
     * GIF RENDER CORE FUNCTION (CRITICAL CODEPATH)
     * Executed row-by-row by the bitbank2 decoder engine.
     * Decodes pixel lines and handles delta frame transparency optimizations without readPixel thrashing.
    */
    if (instance == nullptr) return;
    
    uint8_t *s = pDraw->pPixels;         // Pointer to array of pixel color indices for the current row
    uint16_t *pPalette = pDraw->pPalette; // Pointer to decoded 16-bit color table maps
    
    int iX = pDraw->iX;
    int iY = pDraw->iY + pDraw->y;        // Destination absolute screen Y location coordinate
    int iWidth = pDraw->iWidth;

    // Temporary inline static row array allocation used to bundle processed pixels into one DMA-push slice
    uint16_t line_buffer[320]; 

    // Endianness identification conditional based on active frame memory layout constraints
    bool swapBytes = (instance->current_size > 500000); 

    if (pDraw->ucHasTransparency) {
        uint8_t ucTransparent = pDraw->ucTransparent; // Identified index mapping out hidden layer points
        
        /**
         * Delta-compressed GIFs contain massive sequences of transparent gaps on rows.
         * Instead of wasting CPU cycles polling canvas background pixels or pushing single pixel points,
         * we trace non-transparent continuous spans ("chunks"), bundle them, and blast them in a single DMA push.
         */
        int chunkStart = 0;
        int chunkLen = 0;

        for (int i = 0; i < iWidth; i++) {
            if (s[i] == ucTransparent) {
                // Transparency hit: flush any non-transparent chunk parsed so far onto the layout canvas
                if (chunkLen > 0) {
                    instance->canvas.pushImage(iX + chunkStart, iY, chunkLen, 1, line_buffer);
                    chunkLen = 0; // Reset length for the next chunk
                }
            } else {
                uint16_t color = pPalette[s[i]];
                
                // Hardware-accelerated byte-swapping (LE to BE configuration alignment) 
                // performed natively via ESP32-S3 hardware instructions. Crucial for rich orange coat color.
                if (swapBytes) color = __builtin_bswap16(color); 
                
                if (chunkLen == 0) {
                    chunkStart = i; // Lock the start coordinate offset index of this color group stretch
                }
                line_buffer[chunkLen] = color;
                chunkLen++;
            }
        }
        // Final catch check: flush remaining processed colors stranded inside row tail spans
        if (chunkLen > 0) {
            instance->canvas.pushImage(iX + chunkStart, iY, chunkLen, 1, line_buffer);
        }
    } else {
        /**
         * FULL OVERWRITE FLAT ROW RENDERING CONFIGURATION (No transparency markers on row)
         * Standard loop iteration executing raw array copies without compression checks.
         */
        for (int i = 0; i < iWidth; i++) {
            uint16_t color = pPalette[s[i]];
            if (swapBytes) color = __builtin_bswap16(color);
            line_buffer[i] = color;
        }
        instance->canvas.pushImage(iX, iY, iWidth, 1, line_buffer);
    }
}

void DisplayManager::drawSystemTime(const String& timeStr) {
    /**
     * Draws system clock text onto the layout canvas layer
    */
    canvas.setTextColor(TFT_WHITE);
    canvas.setFont(&fonts::FreeMonoBold12pt7b); 
    canvas.setTextSize(1.0);
    
    canvas.setCursor(240, 10); 
    canvas.print(timeStr); 
}

void DisplayManager::drawWeather() {
    /**
     * Draws weather data points and layout shapes on top of the layout frame
    */
    canvas.setTextColor(TFT_WHITE); 
    canvas.setFont(&fonts::FreeMonoBoldOblique9pt7b); 

    // --- 1. Temperature Block ---
    canvas.drawBitmap(10, 12, icon_thermometer, 16, 16, TFT_WHITE);
    canvas.setTextSize(1.2); 
    canvas.setCursor(32, 10);
    canvas.print(cachedTemp);

    canvas.setTextSize(1.0); 

    // --- 2. Weather Status Icon Matrix Mapping ---
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

    // --- 3. Wind Speed Block ---
    canvas.drawBitmap(10, 72, icon_wind, 16, 16, TFT_WHITE);
    canvas.setCursor(32, 70);
    canvas.print(cachedWind);

    // --- 4. Humidity Block ---
    canvas.drawBitmap(10, 102, icon_drop, 16, 16, TFT_WHITE);
    canvas.setCursor(32, 100);
    canvas.print(cachedHum);
}

void DisplayManager::play(const String& timeStr) {
    /**
     * Execution loop handler responsible for calling frames decoding ticks, 
     * blitting layouts overlays and pushing completed frame to physical screen array matrix.
    */
    if (current_gif != nullptr && current_size > 0) {
        
        // Fail-safe protection: block execution loop if background state misses array links
        if (current_size > 500000 && global_file_buffer == nullptr) {
            if (hasWeather) drawWeather();
            drawSystemTime(timeStr);
            canvas.pushSprite(lcd, 0, 0);
            return; 
        }

        if (millis() - last_frame_time >= (unsigned long)next_frame_delay) {
            // Trigger decoder step slice tick
            if (!gif.playFrame(true, &next_frame_delay)) {
                gif.reset(); 
            }
            last_frame_time = millis();

            /**
             * HARDWARE OVERCLOCK FRAME-RATE OVERRIDE:
             * Pillow-generated files have embedded static delay flags (100ms standard).
             * For a snappier feel, we forcefully drop background animation delay down to 30ms 
             * (~30 FPS simulation rate) exclusively for heavy files while executing frames.
             */
            if (current_size > 500000) {
                next_frame_delay = 30; 
            }

            // Blit weather layout layers
            if (hasWeather) {
                drawWeather();
            }

            // Blit time display layout layer
            drawSystemTime(timeStr);

            // Blit button change overlay layers
            if (millis() < buttonTextExpireTime && currentButtonText != "") {
                canvas.setTextColor(currentButtonColor);
                canvas.setFont(&fonts::Font4);
                canvas.setTextSize(1.0); 
                canvas.setCursor(20, 210);
                canvas.print(currentButtonText);
            }

            // Final output task: push completed composition to hardware LCD panels
            canvas.pushSprite(lcd, 0, 0); 
        }
    }
}

void DisplayManager::setWeather(String temp, String status, String hum, String wind) {
    /**
     * Weather buffer state updater
    */
    cachedTemp = temp;
    cachedStatus = status;
    cachedHum = hum;
    cachedWind = wind;
    hasWeather = true;
    Serial.println("-> Weather strings updated in display cache!");
}
