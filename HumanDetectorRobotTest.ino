#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#define TRIG_PIN 13 // Pin output digital untuk trigger sensor ultrasonik
#define ECHO_PIN 10 // Pin input digital untuk echo sensor ultrasonik
#define SERVO_PIN 11 // Pin output digital untuk servo motor
#define IN1 6 // Pin untuk motor kiri
#define IN2 7 // Pin untuk motor kiri
#define IN3 9 // Pin untuk motor kanan
#define IN4 8 // Pin untuk motor kanan

Servo servo; // Deklarasi objek servo

long duration; // Variabel untuk menyimpan durasi pulsa echo
int distance; // Variabel untuk menyimpan jarak ke objek
bool musuhDideteksi = false; // Flag untuk menandakan apakah musuh terdeteksi
int objekPos = -1; // Posisi objek yang terdeteksi

// Pin motor (harus dideklarasikan)
const int motorKiriPin = 3; // Pin PWM untuk motor kiri (sesuaikan dengan pin yang digunakan)
const int motorKananPin = 5; // Pin PWM untuk motor kanan (sesuaikan dengan pin yang digunakan)

// Variabel untuk mengontrol posisi servo
static int pos = 0; // Posisi awal servo
static bool direction = true; // Arah awal ke kanan

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  pinMode(TRIG_PIN, OUTPUT); // Set pin trigger sebagai output
  pinMode(ECHO_PIN, INPUT); // Set pin echo sebagai input
  pinMode(SERVO_PIN, OUTPUT); // Set pin servo motor sebagai output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  servo.attach(SERVO_PIN); // Hubungkan servo ke pin yang ditentukan
}

void loop() {
  // Lakukan scanning dengan servo jika objek tidak terdeteksi
  if (!musuhDideteksi) {
    servoMotorKontrol();
  }

  // Kirim pulsa trigger 10 mikrodetik
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Baca durasi pulsa echo
  duration = pulseIn(ECHO_PIN, HIGH);

  // Hitung jarak ke objek
  distance = duration / 58.2;

  // Deteksi musuh jika jaraknya kurang dari atau sama dengan 10 cm
  musuhDideteksi = distance <= 10; 

  if (musuhDideteksi) {
    objekPos = servo.read();
    if (objekPos >= 0 && objekPos <= 60) {
      belokKiri();
    } else if (objekPos >= 121 && objekPos <= 180) {
      belokKanan();
    } else if (objekPos >= 61 && objekPos <= 120) {
      majuMaksimum();
    }
  } else {
    objekPos = -1;
    berhenti(); // Hentikan motor ketika tidak mendeteksi objek
  }
  Serial.println(objekPos);
  delay(50); // Tunggu 50 milidetik
}

// Fungsi untuk mengontrol servo motor
void servoMotorKontrol() {
  if (!musuhDideteksi) {
    if (direction) {
      for (pos = 0; pos <= 180; pos += 2) {
        servo.write(pos);  // Arahkan servo ke posisi 'pos'
        delay(15);         // Tunggu 50 milidetik
        // Kirim pulsa trigger 10 mikrodetik
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        // Baca durasi pulsa echo
        duration = pulseIn(ECHO_PIN, HIGH);
        // Hitung jarak ke objek
        distance = duration / 58.2;
        // Deteksi musuh jika jaraknya kurang dari atau sama dengan 10 cm
        musuhDideteksi = distance <= 10;
        if (musuhDideteksi) {
          break; // Hentikan pergerakan servo jika musuh terdeteksi
        }
      }
      if (pos > 180) {
        pos = 180; // Pastikan pos tidak melebihi 180
        direction = false; // Ubah arah ke kiri
      }
    } else {
      for (pos = 180; pos >= 0; pos -= 2) {
        servo.write(pos);  // Arahkan servo ke posisi 'pos'
        delay(15);         // Tunggu 50 milidetik
        // Kirim pulsa trigger 10 mikrodetik
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        // Baca durasi pulsa echo
        duration = pulseIn(ECHO_PIN, HIGH);
        // Hitung jarak ke objek
        distance = duration / 58.2;
        // Deteksi musuh jika jaraknya kurang dari atau sama dengan 10 cm
        musuhDideteksi = distance <= 10;
        if (musuhDideteksi) {
          break; // Hentikan pergerakan servo jika musuh terdeteksi
        }
      }
      if (pos < 0) {
        pos = 0; // Pastikan pos tidak kurang dari 0
        direction = true; // Ubah arah ke kanan
      }
    }
  }
}

// Fungsi untuk maju dengan kecepatan maksimum
void majuMaksimum() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(motorKiriPin, 255); // Maju dengan kecepatan maksimum
  analogWrite(motorKananPin, 255);
}

// Fungsi untuk belok kiri
void belokKanan() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(motorKiriPin, 127); // Kecepatan sedang
  analogWrite(motorKananPin, 127); // Kecepatan sedang
}

// Fungsi untuk belok kanan
void belokKiri() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(motorKiriPin, 127); // Kecepatan sedang
  analogWrite(motorKananPin, 127); // Kecepatan sedang
}

// Fungsi untuk menghentikan motor
void berhenti() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(motorKiriPin, 0); // Berhenti
  analogWrite(motorKananPin, 0); // Berhenti
}