import network
import time
import dht
import BlynkLib
from machine import Pin

# WiFi credentials
WIFI_SSID = "Hadia"
WIFI_PASS = "8777hadia"

# Blynk Authentication Token
BLYNK_AUTH = "dmz-Ce5kVlVDqP6AVgPU9JaaOUYldNHs"

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
blynk = BlynkLib.Blynk(BLYNK_AUTH)

# Setup DHT Sensor
dht_sensor = dht.DHT11(Pin(4))  # Use DHT22 if needed

# Function to send data to Blynk
def send_sensor_data():
    try:
        dht_sensor.measure()
        temp = dht_sensor.temperature()
        hum = dht_sensor.humidity()
        
        print(f"Temperature: {temp}°C, Humidity: {hum}%")
        
        # Send to Blynk (Virtual Pins V0 & V1)
        blynk.virtual_write(0, temp)
        blynk.virtual_write(1, hum)
        
    except Exception as e:
        print("Sensor Error:", e)

# Timer to send data every 5 seconds
while True:
    blynk.run()
    send_sensor_data()
    time.sleep(5)
