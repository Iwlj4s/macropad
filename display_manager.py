import board
import digitalio
import displayio
import busio

import time
from adafruit_displayio_ssd1306 import SSD1306
import terminalio
from adafruit_display_text import label
from clock_manager import ClockManager

class DisplayManager:
    def __init__(self):
        # 1. Customizing buttons for physical pins(GP14 и GP15)
        self.btn_next = digitalio.DigitalInOut(board.GP14)
        self.btn_next.direction = digitalio.Direction.INPUT
        self.btn_next.pull = digitalio.Pull.UP

        self.btn_prev = digitalio.DigitalInOut(board.GP15)
        self.btn_prev.direction = digitalio.Direction.INPUT
        self.btn_prev.pull = digitalio.Pull.UP

        self.current_screen = 3
        self.last_screen = -1

        # 2. Initializing the hardware bus (GP16=SDA, GP17=SCL)
        displayio.release_displays()
        
        self.i2c = busio.I2C(board.GP17, board.GP16)  
        
        self.display_bus = displayio.I2CDisplay(self.i2c, device_address=0x3C)
        self.display = SSD1306(self.display_bus, width=128, height=64)
        
        self.clock = ClockManager(self.i2c)
        self.last_clock_update = 0

        # Creating graphic layers
        self.main_group = displayio.Group()
        self.display.root_group = self.main_group
        
        # Central Big Clocks 
        self.time_label = label.Label(terminalio.FONT, text="00:00", color=0xFFFFFF, scale=3, x=20, y=24)
        
        # Date At The Bottom Of Clocks
        self.date_label = label.Label(terminalio.FONT, text="00.00.0000", color=0xFFFFFF, scale=1, x=34, y=48)

    def update_screen_structure(self):
        while len(self.main_group) > 0:
            self.main_group.pop()

        if self.current_screen == 0:
            text_area = label.Label(terminalio.FONT, text="SCREEN 1: READY", color=0xFFFFFF, x=15, y=32)
            self.main_group.append(text_area)
        elif self.current_screen == 1:
            text_area = label.Label(terminalio.FONT, text="SCREEN 2: READY", color=0xFFFFFF, x=15, y=32)
            self.main_group.append(text_area)
        elif self.current_screen == 2:
            text_area = label.Label(terminalio.FONT, text="SCREEN 3: READY", color=0xFFFFFF, x=15, y=32)
            self.main_group.append(text_area)
        elif self.current_screen == 3:
            # Display both the clock and the date line below it at the same time
            self.main_group.append(self.time_label)
            self.main_group.append(self.date_label)
            self.time_label.text = self.clock.get_time_string()
            self.date_label.text = self.clock.get_date_string()

    def check_buttons(self):
        total_screens = 4
        if not self.btn_next.value:
            self.current_screen = (self.current_screen + 1) % total_screens
            time.sleep(0.2)
        elif not self.btn_prev.value:
            self.current_screen = (self.current_screen - 1) % total_screens
            time.sleep(0.2)

    def refresh_clock_text(self):
        if self.current_screen == 3:
            current_time = time.monotonic()
            if current_time - self.last_clock_update > 1.0:
                self.time_label.text = self.clock.get_time_string()
                self.date_label.text = self.clock.get_date_string()  # Updating date in the background
                self.last_clock_update = current_time

    def loop(self):
        self.check_buttons()
        self.refresh_clock_text()
        
        if self.current_screen != self.last_screen:
            self.update_screen_structure()
            self.last_screen = self.current_screen
