#include "Arduino.h"

// Pin tanımlamaları
const int signalPin = 18; // Frekans ölçmek için kullanılan GPIO pini
volatile unsigned long pulseCount = 0; // Gelen darbeleri saymak için değişken
volatile bool measureReady = false;    // Ölçümün tamamlandığını işaretlemek için değişken

// Timer konfigürasyonu için değişkenler
hw_timer_t *timer = NULL;

// Timer interrupt fonksiyonu
void IRAM_ATTR onTimer() {
  measureReady = true;  // Timer süresi dolduğunda ölçüm hazır
}

// Pulse sayacı fonksiyonu
void IRAM_ATTR countPulse() {
  pulseCount++;  // Her darbe algılandığında sayaç artırılır
}

void setup() {
  Serial.begin(9600); // Baud rate artırıldı

  // Sinyal pini girişi olarak ayarlanır ve interrupt tanımlanır
  pinMode(signalPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(signalPin), countPulse, RISING);

  // Timer ayarları
  timer = timerBegin(0, 80, true); // 80 prescaler ile 1 µs çözünürlük
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); // 1 saniye (1,000,000 µs)
  timerAlarmEnable(timer);               // Timer alarmı etkinleştir
}

void loop() {
  if (measureReady) {
    noInterrupts();          // Interruptları devre dışı bırak
    unsigned long pulses = pulseCount; // Darbe sayısını al
    pulseCount = 0;          // Sayaç sıfırla
    measureReady = false;    // Ölçüm hazır işaretini temizle
    interrupts();            // Interruptları yeniden etkinleştir

    // Frekans hesaplama
    uint32_t frequency = pulses; // 1 saniyelik ölçümde frekans doğrudan pulses olur

    // Sonuçları seriyal monitöre yazdır
    Serial.print("Frekans: ");
    Serial.print(frequency);
    Serial.println(" Hz");
  }
}
