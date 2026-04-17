// ============================================
// ALERT-LoRa Sensor Node – FULL VERSION
// No Gateway Required (Simulated Link)
// 7-Factor LQI + Predictive Mode Switching
// CINEC Campus — BSc ETE 2026
// ============================================

#include <Arduino.h>
#include "config.h"
#include "led_control.h"
#include "dht22_sensor.h"
#include "mpu6050_sensor.h"
#include "acs712_sensor.h"
#include "oled_display.h"
#include "lora_module.h"
#include "crc16.h"

// Global variables
String  txMode        = "FAST";
int     txInterval    = TX_INTERVAL_FAST;
float   lqiHistory[LQI_HISTORY_SIZE];
int     lqiCount      = 0;
int     packetCount   = 0;
bool    showSensors   = true;

float calculateLQI(int rssi, float snr, float pdr, float packetLoss,
                   int packetSize, float crcScore, int txIntervalMs) {
    float rssiNorm = constrain(((rssi + 120.0) / 80.0) * 100.0, 0.0, 100.0);
    float snrNorm  = constrain(((snr + 20.0) / 40.0) * 100.0, 0.0, 100.0);
    float pdrNorm  = constrain(pdr, 0.0, 100.0);
    float lossNorm = constrain(100.0 - packetLoss, 0.0, 100.0);
    float sizeNorm = constrain((1.0 - packetSize / 255.0) * 100.0, 0.0, 100.0);
    float crcNorm  = constrain(crcScore, 0.0, 100.0);
    float txiNorm  = (txIntervalMs <= TX_INTERVAL_FAST) ? 90.0 : 40.0;

    return (rssiNorm * W_RSSI) + (snrNorm * W_SNR) + (pdrNorm * W_PDR) +
           (lossNorm * W_LOSS) + (sizeNorm * W_SIZE) + (crcNorm * W_CRC) +
           (txiNorm * W_INTERVAL);
}

void storeLQI(float lqi) {
    for (int i = LQI_HISTORY_SIZE - 1; i > 0; i--) {
        lqiHistory[i] = lqiHistory[i - 1];
    }
    lqiHistory[0] = lqi;
    if (lqiCount < LQI_HISTORY_SIZE) lqiCount++;
}

String predictTrend() {
    if (lqiCount < 3) return "stable";
    float v1 = lqiHistory[2], v2 = lqiHistory[1], v3 = lqiHistory[0];
    if (v1 > v2 && v2 > v3) return "degrading";
    if (v1 < v2 && v2 < v3) return "improving";
    return "stable";
}

String decideMode(float lqi, String trend) {
    if (lqi >= 75.0 && trend != "degrading") {
        return "FAST";
    } else if (lqi < 50.0 || trend == "degrading") {
        return "SAFE";
    } else {
        return "FAST";
    }
}

void printLQIDetails(int rssi, float snr, float pdr, float packetLoss,
                     int packetSize, float crcScore, float lqi, String trend) {
    Serial.print("RSSI: "); Serial.print(rssi); Serial.println(" dBm");
    Serial.print("SNR: "); Serial.print(snr); Serial.println(" dB");
    Serial.print("PDR: "); Serial.print(pdr); Serial.println("%");
    Serial.print("Packet Loss: "); Serial.print(packetLoss); Serial.println("%");
    Serial.print("Packet Size: "); Serial.print(packetSize); Serial.println(" bytes");
    Serial.print("CRC Score: "); Serial.print(crcScore); Serial.println("%");
    Serial.print("LQI Score: "); Serial.print(lqi); Serial.println();
    Serial.print("Trend: "); Serial.println(trend);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n==========================================");
    Serial.println("       ALERT-LoRa Sensor Node");
    Serial.println("  Adaptive Link Enhanced Reliable");
    Serial.println("       Telemetry System");
    Serial.println("------------------------------------------");
    Serial.print("Node ID:   "); Serial.println(NODE_ID);
    Serial.println("Company:   CINEC Campus");
    Serial.println("Project:   BSc ETE 2026");
    Serial.println("==========================================\n");

    for (int i = 0; i < LQI_HISTORY_SIZE; i++) lqiHistory[i] = 0.0;

    initLEDs();
    initDHT22();
    if (!initMPU6050()) blinkRedLED(3);
    initACS712();
    if (!initOLED()) blinkRedLED(2);
    if (!initLoRa()) {
        Serial.println("LoRa CRITICAL ERROR!");
        while (true) { blinkRedLED(5); delay(1000); }
    }

    setGreenLED(true);
    Serial.println("\n==========================================");
    Serial.println("ALL COMPONENTS INITIALIZED SUCCESSFULLY ✅");
    Serial.println("Starting Full Sensor Node Telemetry...");
    Serial.println("==========================================\n");
    delay(1500);
}

void loop() {
    packetCount++;

    Serial.println("\n==========================================");
    Serial.print("CYCLE #"); Serial.print(packetCount);
    Serial.print("  |  Mode: "); Serial.print(txMode);
    Serial.print("  |  Interval: "); Serial.print(txInterval / 1000); Serial.println("s");
    Serial.println("==========================================");

    // Step 1: Read Sensors
    Serial.println("--- Step 1: Reading All Sensors ---");
    DHT22Data   dhtData = readDHT22();
    MPU6050Data mpuData = readMPU6050();
    ACS712Data  acsData = readACS712();

    // Step 2: Send Telemetry Packet
    Serial.println("--- Step 2: Building & Sending Telemetry Packet ---");
    blinkBlueLED();
    bool sent = sendTelemetryPacket(
        dhtData.temperature, dhtData.humidity,
        acsData.current, mpuData.vibration,
        txMode, packetCount);

    // Step 3: Simulated Link Quality (No Gateway Yet)
    Serial.println("--- Step 3: Simulated Link Quality ---");
    LoRaLinkData link;
    link.rssi = -78;           // Good simulated signal
    link.snr  = 9.5;
    link.packetSize = 48;
    link.crcValid = true;
    link.ackReceived = true;
    link.mode = txMode;

    float pdr = 98.5;
    float packetLoss = 1.5;
    float crcScore = 100.0;

    // Step 4: Calculate 7-Factor LQI
    Serial.println("--- Step 4: 7-Factor LQI Calculation ---");
    float lqi = calculateLQI(link.rssi, link.snr, pdr, packetLoss,
                             link.packetSize, crcScore, txInterval);

    storeLQI(lqi);
    String trend = predictTrend();

    printLQIDetails(link.rssi, link.snr, pdr, packetLoss,
                    link.packetSize, crcScore, lqi, trend);

    // Step 5: Decide Mode (Proactive Switching)
    Serial.println("--- Step 5: Mode Decision ---");
    String newMode = decideMode(lqi, trend);   // You can use the decideMode function from earlier full code
    if (newMode != txMode) {
        txMode = newMode;
        txInterval = (txMode == "FAST") ? TX_INTERVAL_FAST : TX_INTERVAL_SAFE;
        setLoRaMode(txMode);
        Serial.print(">>> Switched to "); Serial.println(txMode);
    }

    // Update LEDs
    if (txMode == "FAST") showFastMode(); else showSafeMode();

    // Update OLED (Alternate screens)
    if (showSensors) {
        displaySensorData(dhtData.temperature, dhtData.humidity,
                          acsData.current, mpuData.vibration);
    } else {
        displayLinkQuality(link.rssi, link.snr, lqi, pdr, txMode);
    }
    showSensors = !showSensors;

    // Summary
    Serial.println("\n=== SENSOR NODE SUMMARY ===");
    Serial.print("Telemetry Packet Built → ");
    Serial.print(NODE_ID); Serial.print(",");
    Serial.print(millis()); Serial.print(",");
    Serial.print(dhtData.temperature,1); Serial.print(",");
    Serial.print(dhtData.humidity,1); Serial.print(",");
    Serial.print(acsData.current,2); Serial.print(",");
    Serial.print(mpuData.vibration,3); Serial.print(",");
    Serial.print((txMode=="FAST") ? "0" : "1"); Serial.print(",");
    Serial.print(packetCount); Serial.println(",[CRC]");
    Serial.print("LQI Score: "); Serial.println(lqi, 2);
    Serial.print("Trend:     "); Serial.println(trend);
    Serial.print("Current Mode: "); Serial.println(txMode);
    Serial.println("===========================\n");

    delay(txInterval);
}