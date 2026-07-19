import board
from kmk.kmk_keyboard import KMKKeyboard
from kmk.scanners import DiodeOrientation
from kmk.keys import KC

from kmk.extensions.media_keys import MediaKeys

class Macropad(KMKKeyboard):
    def __init__(self):
        super().__init__()
        
        # Set up matrix pins
        self.row_pins = (board.GP0, board.GP1)
        self.col_pins = (board.GP2, board.GP3, board.GP4)

        # Activate keys support
        self.extensions.append(MediaKeys())

        self.keymap = [
            [
                # ROW 1 (Upper): 
                # key_number 0 -> F13
                # key_number 1 -> Instant Screenshot
                # key_number 2 -> Control + V
                KC.F13, KC.LGUI(KC.PRINT_SCREEN), KC.LCTL(KC.V),
                
                # ROW 2(Lower): 
                # key_number 3 -> Windows Explorer
                # key_number 4 -> Window Screenshots (Win+Shift+S)
                # key_number 5 -> Task Manager
                KC.LGUI(KC.E), KC.LGUI(KC.LSFT(KC.S)), KC.LCTL(KC.LSFT(KC.ESC))
            ]
        ]

        self.diode_orientation = DiodeOrientation.COL2ROW
