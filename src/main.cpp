// ============================================
// ALERT-LoRa Sensor Node – FINAL VERSION
// Timestamp: HH:MM:SS | Full Sensors + LQI + Adaptive Mode
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

// Globals
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
    for (int i = LQI_HISTORY_SIZE - 1; i > 0; i--) lqiHistory[i] = lqiHistory[i-1];
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
    if (trend == "degrading" && lqi < LQI_FAST_THRESHOLD) return "SAFE";
    if (trend == "improving" && lqi > LQI_SAFE_THRESHOLD) return "FAST";
    return txMode;
}

void printLQIDetails(int rssi, float snr, float pdr, float loss, int size, float crc, float lqi, String trend) {
    Serial.println("=== 7-FACTOR LQI ===");
    Serial.print("RSSI: "); Serial.println(rssi);
    Serial.print("SNR:  "); Serial.println(snr,1);
    Serial.print("PDR:  "); Serial.println(pdr,1);
    Serial.print("Loss: "); Serial.println(loss,1);
    Serial.print("Size: "); Serial.println(size);
    Serial.print("CRC:  "); Serial.println(crc,1);
    Serial.print("Intv: "); Serial.println(txInterval);
    Serial.print("LQI:  "); Serial.println(lqi,2);
    Serial.print("Trend:"); Serial.println(trend);
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
    Serial.println("ALL COMPONENTS READY ✅");
    Serial.println("Packet Format: NODE,TIME,TEMP,HUM,CUR,VIB,MODE,COUNT,CRC");
    delay(1500);
}

void loop() {
    packetCount++;

    Serial.println("\n=== CYCLE #" + String(packetCount) + " | Mode: " + txMode + " | Interval: " + String(txInterval/1000) + "s ===");

    DHT22Data   dhtData = readDHT22();
    MPU6050Data mpuData = readMPU6050();
    ACS712Data  acsData = readACS712();

    blinkBlueLED();
    bool sent = sendTelemetryPacket(dhtData.temperature, dhtData.humidity,
                                    acsData.current, mpuData.vibration, txMode, packetCount);

    // Simulated ACK for testing (replace with real when gateway ready)
    LoRaLinkData link;
    link.rssi = -78; link.snr = 9.5; link.packetSize = 48;
    link.crcValid = true; link.ackReceived = true; link.mode = txMode;

    float pdr = 98.5; float loss = 1.5; float crcScore = 100.0;

    float lqi = calculateLQI(link.rssi, link.snr, pdr, loss, link.packetSize, crcScore, txInterval);
    storeLQI(lqi);
    String trend = predictTrend();

    printLQIDetails(link.rssi, link.snr, pdr, loss, link.packetSize, crcScore, lqi, trend);

    String newMode = decideMode(lqi, trend);
    if (newMode != txMode) {
        txMode = newMode;
        txInterval = (txMode == "FAST") ? TX_INTERVAL_FAST : TX_INTERVAL_SAFE;
        setLoRaMode(txMode);
        Serial.println(">>> Mode switched to " + txMode);
    }

    if (txMode == "FAST") showFastMode(); else showSafeMode();

    if (showSensors) {
        displaySensorData(dhtData.temperature, dhtData.humidity, acsData.current, mpuData.vibration);
    } else {
        displayLinkQuality(link.rssi, link.snr, lqi, pdr, txMode);
    }
    showSensors = !showSensors;

    delay(txInterval);
}