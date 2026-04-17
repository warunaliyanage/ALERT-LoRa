// ============================================
// ALERT-LoRa Sensor Node
// MPU6050 Accelerometer & Gyroscope
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "config.h"

// Create MPU6050 object
MPU6050 mpu;

// ============================================
// DATA STRUCTURE
// ============================================
struct MPU6050Data {
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float vibration;
    bool  valid;
};

// ============================================
// INITIALIZE MPU6050
// ============================================
bool initMPU6050() {
    Wire.begin(I2C_SDA, I2C_SCL);
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 FAILED ❌");
        return false;
    }

    Serial.println("MPU6050 initialized ✅");
    return true;
}

// ============================================
// READ MPU6050
// ============================================
MPU6050Data readMPU6050() {
    MPU6050Data data;
    int16_t ax, ay, az, gx, gy, gz;

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convert raw values to real units
    data.accelX = ax / 16384.0;
    data.accelY = ay / 16384.0;
    data.accelZ = az / 16384.0;
    data.gyroX  = gx / 131.0;
    data.gyroY  = gy / 131.0;
    data.gyroZ  = gz / 131.0;

    // Calculate vibration magnitude
    data.vibration = sqrt(
        data.accelX * data.accelX +
        data.accelY * data.accelY +
        data.accelZ * data.accelZ
    );

    data.valid = true;

    Serial.print("Vibration: ");
    Serial.print(data.vibration, 3);
    Serial.println(" g");

    return data;
}

#endif