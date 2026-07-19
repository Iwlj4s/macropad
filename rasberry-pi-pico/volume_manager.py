import board
import analogio
import time
from kmk.keys import KC

class VolumeManager:
    def __init__(self):
        # Set up potentiometr on analog pin  GP26_A0
        self.pot = analogio.AnalogIn(board.GP26)
        self.last_pot_value = self.pot.value
        self.pot_deadzone = 2000  # Noise filter

    def loop(self, keyboard_instance):
        current_val = self.pot.value
        
        # If the grip is turned further than the noise filter
        if abs(current_val - self.last_pot_value) > self.pot_deadzone:
            # FAST WALK: Send the command 5 times in a row in one turn!
            if current_val > self.last_pot_value:
                for _ in range(5):
                    keyboard_instance.tap_key(KC.AUDIO_VOL_DOWN)	# VOLUME_DOWN
            else:
                for _ in range(5):
                    keyboard_instance.tap_key(KC.AUDIO_VOL_UP)	# 233 = VOLUME_UP
                    
            self.last_pot_value = current_val


