print("Starting of NeoPixel flashing")

from machine import Pin
from neopixel import NeoPixel
import time

btn = Pin(0, Pin.IN, Pin.PULL_UP)  # ESP32 S3 built-in boot button
pin = Pin(48, Pin.OUT)             # Set GPIO 48 for NeoPixel
neo = NeoPixel(pin, 1)             # Create NeoPixel driver for 1 pixel

press_count = 0  # Counter for button presses
colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]  # List of colors to cycle through
color_index = 0  # Index for color selection

while True:
    if btn.value() == 0:  # Button is pressed
        press_count += 1
        print(f"Button pressed {press_count} times")

        if press_count >= 5:  # After 5 presses, change color
            color_index = (color_index + 1) % len(colors)  # Cycle through colors
            neo[0] = colors[color_index]
            neo.write()
            print(f"Color changed to: {colors[color_index]}")
            press_count = 0  # Reset the counter

        time.sleep(0.3)  # Debounce delay to prevent multiple counts per press
