/*
  Ultrasonic Sensor + OLED Display
  ---------------------------------
  Ultrasonic Sensor:
    TRIG --> A1 (pin 15)
    ECHO --> A2 (pin 16)

  OLED (SSD1306, 128x64, I2C):
    SDA  --> A4
    SCL  --> A5

  Libraries required (install via Library Manager):
    - Adafruit SSD1306
    - Adafruit GFX Library
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ── OLED config ──────────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── Ultrasonic sensor pins ────────────────────────────────────
#define TRIG_PIN A1
#define ECHO_PIN A2

#define MAX_DISTANCE_CM  400
#define MIN_DISTANCE_CM    2

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 not found! Check wiring/address."));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println(F("Ultrasonic Sensor"));
  display.setCursor(35, 38);
  display.println(F("Initializing..."));
  display.display();
  delay(1500);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) return -1;

  float distanceCm = (duration * 0.0343f) / 2.0f;

  if (distanceCm < MIN_DISTANCE_CM || distanceCm > MAX_DISTANCE_CM)
    return -1;

  return distanceCm;
}

void loop() {
  float cm = measureDistance();
  float inches = (cm > 0) ? cm / 2.54f : -1;

  if (cm > 0) {
    Serial.print(F("Distance: "));
    Serial.print(cm, 1);
    Serial.print(F(" cm  |  "));
    Serial.print(inches, 1);
    Serial.println(F(" in"));
  } else {
    Serial.println(F("Distance: Out of range"));
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println(F("-- DISTANCE --"));
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  if (cm > 0) {
    display.setTextSize(3);
    String cmStr = String(cm, 1);
    int16_t x = (SCREEN_WIDTH - cmStr.length() * 18) / 2;
    display.setCursor(max((int16_t)0, x), 16);
    display.print(cmStr);

    display.setTextSize(2);
    display.setCursor(98, 20);
    display.println(F("cm"));

    display.setTextSize(1);
    display.setCursor(30, 50);
    display.print(F("( "));
    display.print(inches, 1);
    display.print(F(" inches )"));
  } else {
    display.setTextSize(2);
    display.setCursor(10, 22);
    display.println(F("OUT OF"));
    display.setCursor(18, 42);
    display.println(F("RANGE"));
  }

  display.display();
  delay(200);
}
