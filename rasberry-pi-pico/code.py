# File: code.py
from kb import Macropad

# Инициализируем клавиатуру и менеджер экрана
keyboard = Macropad()

print("Движок KMK подгрузил файл kb.py!")

print("УСПЕХ! Полный макропад запущен в модульном режиме!")

# Запуск бесконечного моторчика KMK
if __name__ == '__main__':
    keyboard.go()
