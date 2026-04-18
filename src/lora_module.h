// ============================================
// ALERT-LoRa Sensor Node
// Ra-02 SX1278 LoRa Module + Real ACK Support
// HH:MM:SS Timestamp + CRC-16
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

// ============================================
// GLOBAL COUNTERS (Declared here, defined in main.cpp)
// ============================================
extern int totalSent;
extern int totalReceived;
extern String txMode;     // Current transmission mode

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
// SET LORA MODE (FAST / SAFE)
// ============================================
void setLoRaMode(String mode) {
    txMode = mode;
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

    // Build packet
    String packet = "";
    packet += NODE_ID;
    packet += ",";
    packet += timeStr;
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

// ============================================
// WAIT FOR REAL ACK FROM GATEWAY
// ============================================
LoRaLinkData waitForACK(int timeoutMs) {
    LoRaLinkData link;
    link.rssi = -120;
    link.snr = -20;
    link.packetSize = 0;
    link.crcValid = false;
    link.ackReceived = false;
    link.mode = txMode;

    unsigned long start = millis();

    while (millis() - start < timeoutMs) {
        int pktSize = LoRa.parsePacket();
        if (pktSize > 0) {
            totalReceived++;

            String response = "";
            while (LoRa.available()) {
                response += (char)LoRa.read();
            }

            link.rssi = LoRa.packetRssi();
            link.snr  = LoRa.packetSnr();
            link.packetSize = pktSize;
            link.crcValid = true;
            link.ackReceived = true;

            if (response.startsWith("ACK")) {
                int first = response.indexOf(',');
                int second = response.indexOf(',', first + 1);
                int third = response.indexOf(',', second + 1);

                if (third > 0) {
                    String modeStr = response.substring(second + 1, third);
                    link.mode = modeStr;
                }

                Serial.println("Real ACK received from Gateway ✅");
                Serial.print("RSSI from Gateway: "); Serial.print(link.rssi); Serial.println(" dBm");
                Serial.print("SNR from Gateway:  "); Serial.print(link.snr, 1); Serial.println(" dB");
                Serial.print("Mode Command: "); Serial.println(link.mode);
            } 
            else if (response.startsWith("NACK")) {
                Serial.println("NACK received - Link problem");
                link.rssi = -110;
            }

            return link;
        }
    }

    Serial.println("ACK Timeout - No response from Gateway ❌");
    link.rssi = -115;
    link.snr = -12;
    return link;
}

#endif