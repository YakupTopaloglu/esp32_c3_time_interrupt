#include "Arduino.h"

// Pin tanımlamaları
const int signalPin = 18; // Frekans ölçmek için kullanılan GPIO pini
volatile uint32_t pulseCount = 0; // 32-bit sayaç (daha büyük frekansları ölçmek için)
volatile bool measureReady = false; // Ölçüm tamamlandığında işaretlenir

// Timer değişkenleri
hw_timer_t *timer = NULL;

// Hızlı GPIO okuma makrosu
#define READ_PIN ((GPIO.in >> signalPin) & 0x1)

// Timer interrupt fonksiyonu (100ms ölçüm süresi)
void IRAM_ATTR onTimer() {
  measureReady = true;
}

// Pulse sayacı fonksiyonu (Maksimum hız için optimize edildi)
void IRAM_ATTR countPulse() {
  pulseCount++;  
}

void setup() {
  Serial.begin(9600); // Daha hızlı veri aktarımı için 115200 baud

  // GPIO pini giriş olarak ayarlandı ve kesme (interrupt) tanımlandı
  pinMode(signalPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(signalPin), countPulse, RISING);

  // Timer ayarları (100ms ölçüm süresi)
  timer = timerBegin(0, 80, true); // 80 prescaler = 1µs çözünürlük
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); // 100ms zamanlayıcı (100.000 µs)
  timerAlarmEnable(timer);
}

void loop() {
  if (measureReady) {
    measureReady = false;

    // Kesme kapatılıyor (veri tutarsızlığını önlemek için)
    noInterrupts();
    uint32_t pulses = pulseCount; // Ölçülen darbe sayısını al
    pulseCount = 0; // Sayaç sıfırla
    interrupts(); // Kesme tekrar etkinleştir

    // Frekans hesaplama (100ms ölçüm yaptığımız için 10 ile çarpıyoruz)
    uint32_t frequency = pulses * 1;

    Serial.print("Frekans: ");
    Serial.print(frequency);
    Serial.println(" Hz");
  }
}
