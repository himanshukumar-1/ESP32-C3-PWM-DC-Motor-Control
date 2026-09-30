#include <SoftwareSerial.h>

SoftwareSerial link(12, 13);   // RX = D12 (from ESP32 GPIO 10), TX unused

const byte ledPins[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

#define RED_LED_INDEX  9
#define RED_THRESHOLD  95

unsigned long lastData = 0;

void showBar(int duty) {
  // LEDs 1-9: one per 10 %
  int n = duty / 10;

  for (int i = 0; i < 9; i++) {
    digitalWrite(ledPins[i], i < n ? HIGH : LOW);
  }

  // LED 10 (red): on above 95 %
  digitalWrite(ledPins[RED_LED_INDEX],
               duty > RED_THRESHOLD ? HIGH : LOW);
}

void setup() {
  link.begin(9600);

  for (int i = 0; i < 10; i++)
    pinMode(ledPins[i], OUTPUT);

  // Start-up test
  for (int i = 0; i < 10; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(60);
  }

  delay(300);
  showBar(0);
}

void loop() {
  while (link.available()) {
    int d = link.read();

    if (d >= 0 && d <= 100) {
      showBar(d);
      lastData = millis();
    }
  }

  // If the ESP32 goes silent for 2 s, blank the bar
  if (millis() - lastData > 2000)
    showBar(0);
}
