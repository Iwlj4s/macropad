import adafruit_ds3231
import time  
class ClockManager:
    def __init__(self, i2c_bus):
        # Connect to clocks with I2C
        try:
            self.rtc = adafruit_ds3231.DS3231(i2c_bus)
            #self.rtc.datetime = time.struct_time((2026, 6, 21, 18, 37, 0, 6, -1, -1))
        except Exception as e:
            print("Connection Error DS3231:", e)
            self.rtc = None

    def get_time_string(self):
        if self.rtc:
            try:
                t = self.rtc.datetime
                # Format to hours:minutes
                return f"{t.tm_hour:02d}:{t.tm_min:02d}"
            except Exception:
                return "RTC Error"
        return "NO CLOCKS"
    
    def get_date_string(self):
        if self.rtc:
            try:
                t = self.rtc.datetime
                return f"{t.tm_mday:02d}.{t.tm_mon:02d}.{t.tm_year}"
            except Exception:
                return "ERROR"
        
        return "NO DATE"

