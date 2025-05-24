#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <time.h>

// WiFi Credentials
const char* ssid = "NTU FSD";
const char* password = "";

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

void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32-S3 DHT11 Firebase Monitor");

  initDHT();
  connectWiFi();

  // Configure time (UTC+5 for Pakistan)
  configTime(5 * 3600, 0, "pool.ntp.org");
  setenv("TZ", "PKT-5", 1);  // Set TZ for Asia/Karachi
  tzset();  // Apply timezone

  delay(2000); // Let time sync
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

  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  char timeStr[6];
  char dateStr[11];

  // Format: HH:MM (12-hour) and DD-MM-YYYY
  strftime(timeStr, sizeof(timeStr), "%I:%M", timeinfo);
  strftime(dateStr, sizeof(dateStr), "%d-%m-%Y", timeinfo);

  // Build JSON payload
  String jsonPayload = "{\"temperature\":" + String(temp) +
                       ",\"humidity\":" + String(humidity) +
                       ",\"time\":\"" + String(timeStr) +
                       "\",\"date\":\"" + String(dateStr) + "\"}";

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
