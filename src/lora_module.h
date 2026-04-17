// ============================================
// ALERT-LoRa Sensor Node
// Ra-02 SX1278 LoRa Module + HH:MM:SS Timestamp
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef LORA_MODULE_H
#define LORA_MODULE_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include "crc16.h"

// ============================================
// LINK DATA STRUCTURE
// ============================================
struct LoRaLinkData {
    int    rssi;
    float  snr;
    int    packetSize;
    bool   crcValid;
    bool   ackReceived;
    String mode;
};

// Global counters
int totalSent     = 0;

// ============================================
// INITIALIZE LORA
// ============================================
bool initLoRa() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(LORA_FREQUENCY)) {
        Serial.println("Ra-02 FAILED ❌");
        return false;
    }

    LoRa.setSpreadingFactor(LORA_SF_FAST);
    LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    LoRa.setCodingRate4(LORA_CODING_RATE);
    LoRa.setTxPower(LORA_TX_POWER);
    LoRa.enableCrc();

    Serial.println("Ra-02 initialized ✅");
    Serial.println("Freq: 433 MHz | SF7 FAST mode");
    return true;
}

// ============================================
// SET LORA MODE
// ============================================
void setLoRaMode(String mode) {
    if (mode == "FAST") {
        LoRa.setSpreadingFactor(LORA_SF_FAST);
        Serial.println("Mode: FAST (SF7)");
    } else {
        LoRa.setSpreadingFactor(LORA_SF_SAFE);
        Serial.println("Mode: SAFE (SF12)");
    }
}

// ============================================
// SEND TELEMETRY PACKET with HH:MM:SS Timestamp
// ============================================
bool sendTelemetryPacket(float temperature, float humidity, float current,
                         float vibration, String mode, int packetCount) {

    totalSent++;

    // Generate HH:MM:SS from millis()
    unsigned long totalSeconds = millis() / 1000;
    int hours   = (totalSeconds / 3600) % 24;
    int minutes = (totalSeconds / 60) % 60;
    int seconds = totalSeconds % 60;

    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);

    // Build packet exactly as requested
    String packet = "";
    packet += NODE_ID;
    packet += ",";
    packet += timeStr;                    // ← 10:45:23
    packet += ",";
    packet += String(temperature, 1);
    packet += ",";
    packet += String(humidity, 1);
    packet += ",";
    packet += String(current, 2);
    packet += ",";
    packet += String(vibration, 3);
    packet += ",";
    packet += (mode == "FAST") ? "0" : "1";
    packet += ",";
    packet += String(packetCount);

    // Calculate CRC-16
    uint8_t bytes[packet.length() + 1];
    packet.getBytes(bytes, packet.length() + 1);
    uint16_t crc = calculateCRC16(bytes, packet.length());

    // Append CRC
    packet += ",";
    packet += String(crc, HEX);

    // Transmit
    LoRa.beginPacket();
    LoRa.print(packet);
    bool result = LoRa.endPacket();

    Serial.println("--- TX Telemetry Packet ---");
    Serial.println(packet);
    Serial.print("CRC: 0x"); Serial.println(crc, HEX);
    Serial.println(result ? "Packet Sent Successfully ✅" : "Send Failed ❌");

    return result;
}

#endif