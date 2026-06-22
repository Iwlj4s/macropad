# File: volume_manager.py
import board
import analogio
import usb_hid
from adafruit_hid.consumer_control import ConsumerControl

class VolumeManager:
    def __init__(self):
        # Set up potentiometr on analog pin  GP26_A0
        self.pot = analogio.AnalogIn(board.GP26)
        self.last_pot_value = self.pot.value
        self.pot_deadzone = 2000  # Noise filter
        
        # Volume Controller Windows
        self.cc = ConsumerControl(usb_hid.devices)

    def loop(self):
        current_val = self.pot.value
        
        # If the grip is turned further than the noise filter
        if abs(current_val - self.last_pot_value) > self.pot_deadzone:
            # FAST WALK: Send the command 5 times in a row in one turn!
            if current_val > self.last_pot_value:
                for _ in range(5):
                    self.cc.send(234)  # 234 = VOLUME_DOWN
            else:
                for _ in range(5):
                    self.cc.send(233)  # 233 = VOLUME_UP
                    
            self.last_pot_value = current_val


