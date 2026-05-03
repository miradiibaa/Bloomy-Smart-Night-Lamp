# Bloomy Smart Night Light 🌸
### Automatic Night Light with ESP32, Firebase & Web Dashboard

## Deskripsi Proyek Bloomy SNL 🌸

Bloomy Smart Night Light adalah sistem lampu pintar berbasis ESP32 yang menyala secara otomatis ketika kondisi ruangan gelap dan mati perlahan (*fade out*) setelah beberapa detik. Sistem ini dilengkapi dengan dashboard web yang terhubung ke Firebase Realtime Database untuk monitoring dan kontrol jarak jauh.

## Fitur Utama Bloomy SNL 🌸

- Deteksi Cahaya Otomatis — Sensor LDR mendeteksi kondisi gelap/terang secara real-time
- Fade Out Effect — LED meredup perlahan selama 4 detik menggunakan PWM
- Dashboard Web — Antarmuka cantik untuk monitoring & kontrol jarak jauh
- Firebase Integration — Data tersinkronisasi real-time ke cloud
- Remote Control — Nyalakan lampu dari mana saja lewat browser
- Live Monitoring — Pantau nilai LDR, brightness, dan status lampu secara langsung

## Komponen Hardware

1. Satu perangkat ESP32 DevKit V1 — Mikrokontroler Utama
2. Satu Perangkat Sensor LDR (Photoresistor) — Mendeteksi intensitas cahaya
3. Satu Perangkat LED Ungu — Lampu indikator
4. Satu Perangkat Resistor 220Ω — Pembatas Arus LED
5. Satu Perangkat Push Button — Tombol Manual

## Konfigurasi Pin
----------------------------------------------------------------
| Pin ESP32 |           Komponen         |      Keterangan     |
----------------------------------------------------------------
| D4        | LED (+)                    | Output PWM          |
| D19       | Push Button                | Input dengan PULLUP |
| D34       | LDR (AO)                   | Input ADC analog    |
| 3V3       | LDR (VCC)                  | Tegangan sensor     |
| GND       | LED (-) via R, Button, LDR | Ground              |
----------------------------------------------------------------

## Cara Kerja Bloomy SNL 🌸

Sensor LDR baca cahaya
        ↓
  Nilai > 1660? (Gelap)
        ↓ Ya
  Tombol ditekan / Remote dashboard
        ↓
  LED menyala PWM = 255
        ↓
  Fade out 4 detik (255 → 0)
        ↓
  LED mati
        ↓
  Data dikirim ke Firebase
        ↓
  Dashboard update real-time

## Teknologi yang Digunakan Bloomy SNL 🌸

- **ESP32 Arduino Core** — Framework pemrograman mikrokontroler
- **PWM (Pulse Width Modulation)** — Kontrol kecerahan LED
- **ADC (Analog to Digital Converter)** — Pembacaan sensor LDR
- **Firebase Realtime Database** — Penyimpanan & sinkronisasi data cloud
- **HTML / CSS / JavaScript** — Dashboard web antarmuka pengguna
- **Firebase JS SDK v10** — Koneksi Firebase dari browser

## Struktur File Bloomy SNL 🌸

```
lampu-tidur-otomatis/
├── sketch.ino        # Kode program ESP32
├── diagram.json      # Konfigurasi rangkaian Wokwi
├── libraries.txt     # Library yang digunakan
├── dashboard.html    # Dashboard web
└── README.md         # Dokumentasi proyek
```

## Cara Menjalankan Bloomy SNL 🌸

### Simulasi (Wokwi)
1. Buka [wokwi.com](https://wokwi.com)
2. Buat project baru → ESP32
3. Copy isi `sketch.ino` ke editor
4. Copy isi `diagram.json` ke tab diagram
5. Klik ▶ **Play** untuk mulai simulasi

### Dashboard Web
1. Buka file `dashboard.html` menggunakan **Live Server** di VSCode
2. Klik tombol **"Connect to Firebase"**
3. Geser slider LDR ke nilai > 1660 (kondisi gelap)
4. Klik **"Tap to Bloom"** untuk menyalakan lampu

### Firebase Setup
1. Buat project di [Firebase Console](https://console.firebase.google.com)
2. Aktifkan **Realtime Database**
3. Set Rules menjadi public (untuk development):
```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

## Struktur Firebase Database

```
lampu-tidur-otomatis
└── lampu
    ├── isOn: false        # Status lampu (true/false)
    ├── brightness: 0      # Nilai kecerahan (0-255)
    ├── ldrValue: 1200     # Nilai sensor LDR (0-4095)
    ├── kondisiGelap: false # Kondisi ruangan (true = gelap)
    └── remoteOn: false    # Trigger dari dashboard
```

## Library Bloomy SNL 🌸
------------------------------------------------------------------------------------------
|               Library             |      Sumber    |               Fungsi              |
------------------------------------------------------------------------------------------
| Arduino.h                         | Built-in ESP32 | Fungsi dasar Arduino              |
| ledcAttach / ledcWrite            | Built-in ESP32 | Kontrol PWM LED                   |
| analogRead / analogSetAttenuation | Built-in ESP32 | Baca sensor LDR                   |
| Firebase ESP32 Client             | Mobizt         | Koneksi Firebase (hardware fisik) |
------------------------------------------------------------------------------------------

## Dibuat oleh

**Amira azza Nuuradiba**  


## Lisensi

Project ini dibuat untuk keperluan akademik.
