#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi Credentials
const char* ssid = "Shaham";
const char* password = "abcd1234";

// Firebase Configuration
const String FIREBASE_HOST = "lab11-firebase-8fb44-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "XOxhg0FrcFjtWyN5hXPJZeuLonlOlvpNWskloXVZ";
const String FIREBASE_PATH = "/lab11_data.json";

// DHT Sensor
#define DHTPIN 4
#define DHTTYPE DHT11

// Timing
const unsigned long SEND_INTERVAL = 10000;
const unsigned long SENSOR_DELAY = 2000;

// Global Objects
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastSendTime = 0;
unsigned long lastReadTime = 0;

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 5 * 3600, 60000);  // UTC+5 for Pakistan

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32-S3 DHT11 Firebase Monitor");

  initDHT();
  connectWiFi();

  timeClient.begin();
  timeClient.update();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  timeClient.update();  // keep NTP time updated

  if (millis() - lastReadTime >= SENSOR_DELAY) {
    float temp, hum;
    if (readDHT(&temp, &hum)) {
      if (millis() - lastSendTime >= SEND_INTERVAL) {
        sendToFirebase(temp, hum);
        lastSendTime = millis();
      }
    }
    lastReadTime = millis();
  }
}

// Sensor Functions
void initDHT() {
  dht.begin();
  Serial.println("DHT sensor initialized");
  delay(500);
}

bool readDHT(float* temp, float* humidity) {
  *temp = dht.readTemperature();
  *humidity = dht.readHumidity();

  if (isnan(*temp) || isnan(*humidity)) {
    Serial.println("DHT read failed! Retrying...");
    digitalWrite(DHTPIN, LOW);
    pinMode(DHTPIN, INPUT);
    delay(100);
    initDHT();
    return false;
  }

  Serial.printf("DHT Read: %.1f°C, %.1f%%\n", *temp, *humidity);
  return true;
}

// WiFi Functions
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.disconnect(true);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 15) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed!");
  }
}

// Firebase Upload Function
void sendToFirebase(float temp, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send - WiFi disconnected");
    return;
  }

  HTTPClient http;
  String url = "https://" + FIREBASE_HOST + FIREBASE_PATH + "?auth=" + FIREBASE_AUTH;

  // Get epoch time and convert to structured time
  time_t rawTime = timeClient.getEpochTime();
  struct tm* timeinfo = localtime(&rawTime);

  // Format time as 12-hour without leading zero (e.g., 3:05)
  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  String hour12 = String((hour % 12 == 0) ? 12 : hour % 12);
  if (minute < 10) {
    hour12 += ":0" + String(minute);
  } else {
    hour12 += ":" + String(minute);
  }

  // Format date as DD-MM-YYYY
  String dateStr = String(timeinfo->tm_mday) + "-" +
                   String(timeinfo->tm_mon + 1) + "-" +
                   String(timeinfo->tm_year + 1900);

  // Build JSON payload
  String jsonPayload = "{\"temperature\":" + String(temp) +
                       ",\"humidity\":" + String(humidity) +
                       ",\"time\":\"" + hour12 +
                       "\",\"date\":\"" + dateStr + "\"}";

  Serial.println("Sending to Firebase...");
  Serial.println(jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonPayload);

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Firebase update successful");
  } else {
    Serial.printf("Firebase error: %d\n", httpCode);
  }
  
  http.end();
}
