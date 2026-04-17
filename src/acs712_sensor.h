// ============================================
// ALERT-LoRa Sensor Node
// ACS712 30A Current Sensor
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef ACS712_SENSOR_H
#define ACS712_SENSOR_H

#include <Arduino.h>
#include "config.h"

// ============================================
// DATA STRUCTURE
// ============================================
struct ACS712Data {
    float voltage;
    float current;
    bool  valid;
};

// ============================================
// INITIALIZE ACS712
// ============================================
void initACS712() {
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    Serial.println("ACS712 initialized ✅");
}

// ============================================
// READ ACS712
// ============================================
ACS712Data readACS712() {
    ACS712Data data;

    // Take multiple samples for accuracy
    float sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += analogRead(ACS712_PIN);
        delay(1);
    }

    float avgADC = sum / 100.0;

    // Convert ADC to voltage
    data.voltage = (avgADC / 4095.0) * 3.3;

    // Calculate current (with zero offset)
    data.current = (data.voltage - ACS712_ZERO_VOLTAGE) / ACS712_SENSITIVITY;

    data.valid = true;

    Serial.print("Current: ");
    Serial.print(data.current, 3);
    Serial.println(" A");

    return data;
}

#endif