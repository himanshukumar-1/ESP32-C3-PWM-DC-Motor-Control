#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pins (ESP32-C3 SuperMini) ----------
#define ENA_PIN     5
#define IN1_PIN     6
#define IN2_PIN     7
#define POT_PIN     0
#define SDA_PIN     20
#define SCL_PIN     21
#define LINK_TX_PIN 10

// ---------- Settings ----------
#define PWM_FREQ  1000
#define PWM_RES   8
#define POT_MAX   4095
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int  dutyPct   = 0;
bool forward   = true;
bool brakeMode = true;
bool armed     = false;
bool oledOK    = false;

void applyMotor() {
  int d = armed ? dutyPct * 255 / 100 : 0;

  if (brakeMode) {
    ledcWrite(ENA_PIN, 255);
    ledcWrite(IN1_PIN, forward ? d : 0);
    ledcWrite(IN2_PIN, forward ? 0 : d);
  } else {
    ledcWrite(ENA_PIN, d);
    ledcWrite(IN1_PIN, forward ? 255 : 0);
    ledcWrite(IN2_PIN, forward ? 0 : 255);
  }
}

int readPotPct() {
  static float filtered = 0;
  long sum = 0;

  for (int i = 0; i < 32; i++) {
    sum += analogRead(POT_PIN);
    delayMicroseconds(200);
  }

  filtered = 0.85 * filtered + 0.15 * (sum / 32);

  int pct = map((int)filtered, 0, POT_MAX, 0, 100);

  if (pct < 3) pct = 0;

  return constrain(pct, 0, 100);
}

void drawScreen() {
  if (!oledOK) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("PWM Motor Control");

  if (!armed) {
    display.setCursor(0, 24);
    display.print("Turn knob to 0");
    display.setCursor(0, 36);
    display.print("to start");
  } else {
    display.setTextSize(3);
    display.setCursor(0, 14);
    display.printf("%3d%%", dutyPct);

    display.setTextSize(1);
    display.setCursor(0, 42);
    display.printf("%s   %s",
                   forward ? "FWD" : "REV",
                   brakeMode ? "BRAKE" : "COAST");

    display.drawRect(0, 54, 128, 10, SSD1306_WHITE);
    display.fillRect(2, 56, dutyPct * 124 / 100, 6, SSD1306_WHITE);
  }

  display.display();
}

void checkOled() {
  Wire.beginTransmission(OLED_ADDR);

  if (Wire.endTransmission() != 0) {
    oledOK = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    Wire.setClock(100000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  ledcAttach(ENA_PIN, PWM_FREQ, PWM_RES);
  ledcAttach(IN1_PIN, PWM_FREQ, PWM_RES);
  ledcAttach(IN2_PIN, PWM_FREQ, PWM_RES);

  applyMotor();

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  if (!oledOK)
    Serial.println("OLED not found - running without display");

  Serial1.begin(9600, SERIAL_8N1, -1, LINK_TX_PIN);

  Serial.println("Knob = speed. Serial: r = reverse, m = switch mode");
}

void loop() {
  // 1. Knob -> duty
  int p = readPotPct();

  if (!armed && p == 0) {
    armed = true;
    Serial.println("Armed - motor enabled");
  }

  if (abs(p - dutyPct) >= 2 ||
      (p == 0 && dutyPct != 0) ||
      (p == 100 && dutyPct != 100)) {

    dutyPct = p;
    applyMotor();

    if (armed)
      Serial.printf("Duty: %d %%\n", dutyPct);
  }

  // 2. Serial commands
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();

    if (s == "r") {
      int saved = dutyPct;

      dutyPct = 0;
      applyMotor();
      delay(300);

      forward = !forward;

      dutyPct = saved;
      applyMotor();

      Serial.println(forward ? "Direction: FWD" : "Direction: REV");

    } else if (s == "m") {
      brakeMode = !brakeMode;
      applyMotor();

      Serial.println(brakeMode ? "Mode: BRAKE" : "Mode: COAST");
    }
  }

  // 3. Screen refresh + health check
  static unsigned long lastDraw = 0, lastCheck = 0;

  if (millis() - lastDraw > 200) {
    lastDraw = millis();
    drawScreen();
  }

  if (millis() - lastCheck > 2000) {
    lastCheck = millis();
    checkOled();
  }

  // 4. Send duty to Nano LED bar 5 times per second
  static unsigned long lastSend = 0;

  if (millis() - lastSend > 200) {
    lastSend = millis();
    Serial1.write((uint8_t)(armed ? dutyPct : 0));
  }
}
