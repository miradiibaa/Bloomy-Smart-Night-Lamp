#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const int LED_PIN     = 4; //ni buat nyalain lampu
const int BTN_PIN     = 19; // buat nerima input tombol
const int LDR_PIN     = 34; // ini buat ngedetek gelap

// buat konfigurasi PWM sama Timer
const int TIMER_DETIK    = 30;
const int PWM_FREQ       = 5000;
const int PWM_RESOLUTION = 8;

// ini wifi
const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ini firebase rest api nya
const char* FIREBASE_URL = "https://lampu-tidur-otomatis-default-rtdb.asia-southeast1.firebasedatabase.app";

bool          isOn       = false;
unsigned long startTime  = 0;
unsigned long lastUpload = 0;
unsigned long lastCheck  = 0;
const int     UPLOAD_MS  = 300;  // kirim ke Firebase tiap 300ms saat fade
const int     CHECK_MS   = 500;  // cek remoteOn tiap 500ms

// ini patch (kirim data) ke firebase
void firebasePatch(String path, String json) {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin(String(FIREBASE_URL) + path + ".json");
  http.addHeader("Content-Type", "application/json");
  int code = http.PATCH(json);
  if (code < 0) Serial.println("PATCH error: " + String(code));
  http.end();
}

// ini nerima data Get
String firebaseGet(String path) {
  if (WiFi.status() != WL_CONNECTED) return "null";
  HTTPClient http;
  http.begin(String(FIREBASE_URL) + path + ".json");
  int code = http.GET();
  String result = "null";
  if (code == 200) result = http.getString();
  http.end();
  return result;
}


void uploadStatus(bool lampuOn, int brightness) {
  int  ldrVal = analogRead(LDR_PIN);
  bool gelap  = ldrVal > 1660;

  // /lampu
  String jsonLampu = "{";
  jsonLampu += "\"status\":"     + String(lampuOn ? "true" : "false") + ",";
  jsonLampu += "\"brightness\":" + String(brightness) + ",";
  jsonLampu += "\"timestamp\":"  + String(millis()) + ",";
  jsonLampu += "\"remoteOn\":false";
  jsonLampu += "}";
  firebasePatch("/lampu", jsonLampu);

  // /sensor
  String jsonSensor = "{";
  jsonSensor += "\"ldr\":"          + String(ldrVal) + ",";
  jsonSensor += "\"kondisiGelap\":" + String(gelap ? "true" : "false");
  jsonSensor += "}";
  firebasePatch("/sensor", jsonSensor);
}

// ini remote dari dashboard
bool cekRemoteOn() {
  String val = firebaseGet("/lampu/remoteOn");
  val.trim();
  if (val == "true") {
    firebasePatch("/lampu", "{\"remoteOn\":false}");
    return true;
  }
  return false;
}

bool kondisiGelap() {
  int val = analogRead(LDR_PIN);
  Serial.print("Nilai LDR: ");
  Serial.println(val);
  return val > 1660;
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Bloomy Smart Night Lamp ===");

  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);
  pinMode(BTN_PIN, INPUT_PULLUP);
  analogSetAttenuation(ADC_11db);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi terhubung! IP: " + WiFi.localIP().toString());
    uploadStatus(false, 0);
    Serial.println("Firebase: status awal dikirim.");
  } else {
    Serial.println("\nGagal konek WiFi.");
  }

  Serial.println("Setup selesai. Tekan tombol untuk menyalakan lampu.");
}

void loop() {
  int  ldrVal = analogRead(LDR_PIN);
  bool gelap  = ldrVal > 1660;

  // tombol fisiknya
  if (digitalRead(BTN_PIN) == LOW && !isOn && gelap) {
    isOn      = true;
    startTime = millis();
    ledcWrite(LED_PIN, 255);
    Serial.println(">> Lampu MENYALA via tombol!");
    uploadStatus(true, 255);
  }

  // buat cek remote on dama update sensornya
  if (!isOn && millis() - lastCheck >= CHECK_MS) {
    lastCheck = millis();

    // Selalu kirim data sensor terbaru
    String jsonSensor = "{";
    jsonSensor += "\"ldr\":"          + String(ldrVal) + ",";
    jsonSensor += "\"kondisiGelap\":" + String(gelap ? "true" : "false");
    jsonSensor += "}";
    firebasePatch("/sensor", jsonSensor);

    // Cek apakah dashboard minta nyala
    if (gelap && cekRemoteOn()) {
      isOn      = true;
      startTime = millis();
      ledcWrite(LED_PIN, 255);
      Serial.println(">> Lampu MENYALA via Dashboard Remote!");
      uploadStatus(true, 255);
    }
  }

  // ini buat fadeoutnya
  if (isOn) {
    unsigned long elapsed = millis() - startTime;
    unsigned long total   = (unsigned long)TIMER_DETIK * 1000;

    if (elapsed >= total) {
      ledcWrite(LED_PIN, 0);
      isOn = false;
      Serial.println(">> Lampu MATI. Fade out selesai.");
      uploadStatus(false, 0);
    } else {
      int bri = 255 - (int)((elapsed * 255UL) / total);
      ledcWrite(LED_PIN, bri);

      if (millis() - lastUpload >= UPLOAD_MS) {
        lastUpload = millis();
        Serial.printf("Brightness: %d | Sisa: %.1f s | LDR: %d\n",
                      bri, (total - elapsed) / 1000.0, ldrVal);
        uploadStatus(true, bri);
      }
    }
  }

  delay(10);
}
