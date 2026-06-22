# File: code.py
from kb import Macropad
from display_manager import DisplayManager
from volume_manager import VolumeManager 

# Init keyboard, display and volume
keyboard = Macropad()
display_helper = DisplayManager()
volume_helper = VolumeManager()

# Background cycle of polling all systems simultaneously
def before_matrix_scan():
    display_helper.loop()  # Querying the screen buttons
    volume_helper.loop()   # Querying the sound potentiometer

keyboard.before_matrix_scan = before_matrix_scan

if __name__ == '__main__':
    keyboard.go()
