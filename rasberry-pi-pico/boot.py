# File: boot.py
import board
import digitalio
import storage

# Use a free pin of the screen button that is not twitched by the matrix
button = digitalio.DigitalInOut(board.GP16)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

# The disk will only hide if the button is NOT held down at startup.
if button.value:
    storage.disable_usb_drive()
