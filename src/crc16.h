// ============================================
// ALERT-LoRa Sensor Node
// CRC-16 Error Detection (Polynomial: x^16 + x^12 + x^5 + 1)
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef CRC16_H
#define CRC16_H

#include <Arduino.h>

// ============================================
// CALCULATE CRC-16
// ============================================
uint16_t calculateCRC16(uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;   // Initial value

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// ============================================
// VALIDATE CRC-16 (Optional - useful for gateway later)
// ============================================
bool validateCRC16(uint8_t* data, size_t length, uint16_t receivedCRC) {
    uint16_t calculated = calculateCRC16(data, length);
    return (calculated == receivedCRC);
}

#endif