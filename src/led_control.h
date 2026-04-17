// ============================================
// ALERT-LoRa Sensor Node
// LED Control
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "config.h"     // ← MUST include config.h

void initLEDs() {
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE,  OUTPUT);
    pinMode(LED_RED,   OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE,  LOW);
    digitalWrite(LED_RED,   LOW);

    // Startup blink test
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE,  HIGH);
    digitalWrite(LED_RED,   HIGH);
    delay(500);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE,  LOW);
    digitalWrite(LED_RED,   LOW);

    Serial.println("LEDs initialized ✅");
}

void setGreenLED(bool state) { digitalWrite(LED_GREEN, state ? HIGH : LOW); }
void setBlueLED(bool state)  { digitalWrite(LED_BLUE,  state ? HIGH : LOW); }
void setRedLED(bool state)   { digitalWrite(LED_RED,   state ? HIGH : LOW); }

void blinkBlueLED() {
    digitalWrite(LED_BLUE, HIGH); delay(100); digitalWrite(LED_BLUE, LOW);
}

void blinkRedLED(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_RED, HIGH); delay(200);
        digitalWrite(LED_RED, LOW);  delay(200);
    }
}

void showFastMode() {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED,   LOW);
}

void showSafeMode() {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED,   HIGH);
}

#endif