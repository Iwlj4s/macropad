# File: clock_manager.py
import time
import board
import adafruit_ds3231

class ClockManager:
    def __init__(self, i2c_bus):
        # Инициализируем модуль времени напрямую через прокинутый bitbang-интерфейс
        try:
            self.rtc = adafruit_ds3231.DS3231(i2c_bus)
        except Exception as e:
            print("DS3231 Init Error inside manager:", e)
            self.rtc = None

    def get_time_string(self):
        if self.rtc:
            try:
                t = self.rtc.datetime
                return f"{t.tm_hour:02d}:{t.tm_min:02d}"
            except Exception:
                return "00:00"
        return "NO CLK"

    def get_date_string(self):
        if self.rtc:
            try:
                t = self.rtc.datetime
                return f"{t.tm_mday:02d}.{t.tm_mon:02d}.{t.tm_year}"
            except Exception:
                return "00.00.0000"
        return "NO DATE"
