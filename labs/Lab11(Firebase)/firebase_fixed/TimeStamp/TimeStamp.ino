#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <time.h>

// WiFi Credentials
const char* ssid = "Shaham";
const char* password = "abcd1234";

// Firebase Configuration
const String FIREBASE_HOST = "lab11-firebase-8fb44-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "XOxhg0FrcFjtWyN5hXPJZeuLonlOlvpNWskloXVZ";
const String FIREBASE_PATH = "/lab11_data.json";

// DHT Sensor
#define DHTPIN 4       // GPIO4 (change if needed)
#define DHTTYPE DHT11  // DHT11 or DHT22

// Timing
const unsigned long SEND_INTERVAL = 10000;  // 10 seconds
const unsigned long SENSOR_DELAY = 2000;    // 2 seconds between reads

// Global Objects
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastSendTime = 0;
unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32-S3 DHT11 Firebase Monitor");

  initDHT();
  connectWiFi();
  initTime();  // Initialize NTP time
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

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
    Serial.print("...");
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

// Time/NTP Functions
void initTime() {
  configTime(5 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // UTC+5 for Pakistan
  Serial.print("Waiting for time sync");
  time_t now = time(nullptr);
  int retries = 0;
  while (now < 8 * 3600 * 2 && retries < 10) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    retries++;
  }
  Serial.println();

  if (now >= 8 * 3600 * 2) {
    Serial.println("Time synchronized.");
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.printf("Current PKT time: %s", asctime(&timeinfo));
  } else {
    Serial.println("Failed to synchronize time");
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

  // Get current timestamp
  time_t now = time(nullptr);

  // Format the time in 12-hour format (3:51 format)
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char timeStr[6];  // To hold formatted time string "h:mm"
  strftime(timeStr, sizeof(timeStr), "%l:%M", &timeinfo); // %l for 12-hour format without leading zero, %M for minutes

  // Build JSON payload
  String jsonPayload = "{\"temperature\":" + String(temp) +
                       ",\"humidity\":" + String(humidity) +
                       ",\"timestamp\":\"" + String(timeStr) + "\"}"; // Use formatted time

  Serial.println("Sending to Firebase...");
  Serial.println(jsonPayload);

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonPayload);

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Firebase update successful");
  } else {
    Serial.printf("Firebase error: %d\n", httpCode);
    if (httpCode == -1) {
      Serial.println("Check your Firebase URL and authentication");
    }
  }

  http.end();
}
