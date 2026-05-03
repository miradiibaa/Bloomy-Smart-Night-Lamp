#include <Arduino.h>

// ── Definisi Pin ────────────────────────────────────────────────
const int LED_PIN     = 4;   // Pin LED (PWM)
const int BTN_PIN     = 19;  // Pin tombol push button
const int LDR_PIN     = 34;  // Pin sensor LDR (ADC)

// ── Konfigurasi Timer & PWM ──────────────────────────────────────
const int TIMER_DETIK    = 4;     // Durasi fade out (detik)
const int PWM_FREQ       = 5000;  // Frekuensi PWM (Hz)
const int PWM_RESOLUTION = 8;     // Resolusi PWM 8-bit (0-255)

// ── Variabel State ───────────────────────────────────────────────
bool isOn = false;              // Status lampu
unsigned long startTime = 0;    // Waktu mulai lampu menyala

void setup() {
  Serial.begin(115200);
  Serial.println("=== Lampu Tidur Otomatis ===");
  Serial.println("Sistem siap...");

  // Inisialisasi PWM untuk LED
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);

  // Tombol pakai INPUT_PULLUP (LOW = ditekan)
  pinMode(BTN_PIN, INPUT_PULLUP);

  // Atenuasi ADC 11db agar bisa baca tegangan 0-3.3V
  analogSetAttenuation(ADC_11db);

  Serial.println("Setup selesai. Tekan tombol untuk menyalakan lampu.");
}

// Fungsi cek kondisi gelap dari sensor LDR
bool kondisiGelap() {
  int nilaiLDR = analogRead(LDR_PIN);
  Serial.print("Nilai LDR: ");
  Serial.println(nilaiLDR);
  // Nilai > 1660 berarti cahaya redup / gelap
  return nilaiLDR > 1660;
}

void loop() {
  // Cek tombol ditekan + lampu belum nyala + kondisi gelap
  if (digitalRead(BTN_PIN) == LOW && !isOn && kondisiGelap()) {
    isOn      = true;
    startTime = millis();
    ledcWrite(LED_PIN, 255); // Nyalakan LED maksimal
    Serial.println(">> Lampu MENYALA! Fade out dimulai...");
  }

  // Jika lampu sedang menyala, jalankan fade out
  if (isOn) {
    unsigned long elapsed = millis() - startTime;
    unsigned long total   = (unsigned long)TIMER_DETIK * 1000;

    if (elapsed >= total) {
      // Waktu habis, matikan lampu
      ledcWrite(LED_PIN, 0);
      isOn = false;
      Serial.println(">> Lampu MATI. Fade out selesai.");
    } else {
      // Hitung brightness menurun secara linear
      int bri = 255 - (int)((elapsed * 255) / total);
      ledcWrite(LED_PIN, bri);
      Serial.print("Brightness: ");
      Serial.print(bri);
      Serial.print(" | Sisa waktu: ");
      Serial.print((total - elapsed) / 1000.0, 1);
      Serial.println(" detik");
    }
  }

  delay(10);
}