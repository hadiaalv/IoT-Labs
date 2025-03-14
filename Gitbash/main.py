# For Esp32 S3
# Just for Boot button
#Lab2: RGB blink
from machine import Pin
from time import sleep
from neopixel import NeoPixel


btn=Pin(0,Pin.IN,Pin.PULL_UP) # Pin 0 boot
# input--pull resistor (thats why pull up)

while True:

    sleep(.7)
    print(btn.value()) #press value=0, other=1


# For RGB blink now: single color
    


pin = Pin(48, Pin.OUT)   # set GPIO48  to output to drive NeoPixel
neo = NeoPixel(pin, 1)   # create NeoPixel driver on GPIO48 for 1 pixel
neo[0] = (200, 55, 0) # set the first pixel to white
time.sleep(.2)
neo.write()              # write data to all pixels


# FOR RGB blink: Multi-Colorsss

pin = Pin(48, Pin.OUT)   # set GPIO48  to output to drive NeoPixel
neo = NeoPixel(pin, 1)   # create NeoPixel driver on GPIO48 for 1 pixel


while True:
    for i in range(1, 253):
        neo[0] = (min(i, 255), min(i+1, 255), min(i+2, 255))  # Ensure values stay in range
        neo.write()  # Write data to all pixels
        print("done")
        time.sleep(0.2) 