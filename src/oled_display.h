// ============================================
// ALERT-LoRa Sensor Node
// OLED Display 0.96" I2C
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// Create display object
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// ============================================
// INITIALIZE OLED
// ============================================
bool initOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED FAILED ❌");
        return false;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("ALERT-LoRa");
    display.println("Node: " + String(NODE_ID));
    display.println("CINEC Campus");
    display.println("Starting...");
    display.display();
    delay(2000);

    Serial.println("OLED initialized ✅");
    return true;
}

// ============================================
// DISPLAY SENSOR DATA
// ============================================
void displaySensorData(float temp, float hum, float curr, float vib) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("-- SENSOR DATA --");
    display.setCursor(0, 12);
    display.print("Temp: "); display.print(temp, 1); display.println(" C");
    display.setCursor(0, 22);
    display.print("Hum:  "); display.print(hum, 1); display.println(" %");
    display.setCursor(0, 32);
    display.print("Curr: "); display.print(curr, 2); display.println(" A");
    display.setCursor(0, 42);
    display.print("Vib:  "); display.print(vib, 3); display.println(" g");
    display.display();
}

// ============================================
// DISPLAY LINK QUALITY
// ============================================
void displayLinkQuality(int rssi, float snr, float lqi, float pdr, String mode) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("-- LINK QUALITY --");
    display.setCursor(0, 12);
    display.print("RSSI: "); display.print(rssi); display.println(" dBm");
    display.setCursor(0, 22);
    display.print("SNR:  "); display.print(snr, 1); display.println(" dB");
    display.setCursor(0, 32);
    display.print("LQI:  "); display.print(lqi, 1);
    display.setCursor(0, 42);
    display.print("PDR:  "); display.print(pdr, 1); display.println(" %");
    display.setCursor(0, 52);
    display.print("Mode: "); display.println(mode);
    display.display();
}

#endif