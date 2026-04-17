#ifndef DHT22_SENSOR_H
#define DHT22_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"     // ← Added

DHT dht(DHT_PIN, DHT_TYPE);

struct DHT22Data {
    float temperature;
    float humidity;
    bool  valid;
};

void initDHT22() {
    dht.begin();
    delay(2000);
    Serial.println("DHT22 initialized ✅");
}

DHT22Data readDHT22() {
    DHT22Data data;
    data.temperature = dht.readTemperature();
    data.humidity    = dht.readHumidity();

    if (isnan(data.temperature) || isnan(data.humidity)) {
        data.valid = false;
        data.temperature = 0.0;
        data.humidity = 0.0;
        Serial.println("DHT22 read FAILED ❌");
    } else {
        data.valid = true;
        Serial.print("Temp: "); Serial.print(data.temperature); Serial.println(" °C");
        Serial.print("Hum:  "); Serial.print(data.humidity); Serial.println(" %");
    }
    return data;
}

#endif