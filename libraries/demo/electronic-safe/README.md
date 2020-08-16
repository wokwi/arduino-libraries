This project implements an electronic safe, powered by an Arduino Uno.

The safe has three main components:
An [16x02 LCD Monitor](https://wokwi.com/playground/lcd1602)
and a [Membrane Keypad](https://wokwi.com/playground/keypad) for the
user interface, and a Servo motor that powers the lock mechanism.

The state of the safe (locked/unlocked), as well as the secret code
are stored in the Arduino's [EEPROM](https://wokwi.com/playground/eeprom), so
the code isn't wiped even when the power goes off.
