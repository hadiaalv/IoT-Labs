import network
import BlynkLib
from machine import Pin, PWM
import time

# WiFi Credentials
WIFI_SSID = "Hadia"
WIFI_PASS = "877hadia"

# Blynk Authentication Token
BLYNK_AUTH = "9hOM66bT0OVi8EPyghJ8upwG3rTxNx1l"

# Connect to WiFi
def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(WIFI_SSID, WIFI_PASS)
    
    while not wlan.isconnected():
        print("Connecting to WiFi...")
        time.sleep(1)
    
    print("Connected:", wlan.ifconfig())

connect_wifi()

# Initialize Blynk
blynk = BlynkLib.Blynk(BLYNK_AUTH, server="blynk.cloud", port=443)



# RGB LED PWM Pins
red = PWM(Pin(15), freq=1000)
green = PWM(Pin(2), freq=1000)
blue = PWM(Pin(4), freq=1000)

# Function to set RGB color
def set_rgb(r, g, b):
    red.duty(r)
    green.duty(g)
    blue.duty(b)

# Blynk Virtual Pin Handlers
@blynk.on("V0")  # Red
def v0_write(value):
    red.duty(int(value[0]))

@blynk.on("V1")  # Green
def v1_write(value):
    green.duty(int(value[0]))

@blynk.on("V2")  # Blue
def v2_write(value):
    blue.duty(int(value[0]))

# Run Blynk
while True:
    blynk.run()
