// ============================================
// ALERT-LoRa Sensor Node
// Configuration File
// CINEC Campus — BSc ETE 2026
// ============================================

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// NODE IDENTIFICATION
// ============================================
#define NODE_ID             "01"

// ============================================
// LED PIN DEFINITIONS
// ============================================
#define LED_GREEN           32    // Power indicator
#define LED_BLUE            25    // TX indicator
#define LED_RED             13    // Error / SAFE mode indicator

// ============================================
// DHT22 SENSOR PIN
// ============================================
#define DHT_PIN             4
#define DHT_TYPE            DHT22

// ============================================
// MPU6050 I2C PINS
// ============================================
#define I2C_SDA             21
#define I2C_SCL             22
#define MPU_INT_PIN         27

// ============================================
// ACS712 CURRENT SENSOR
// ============================================
#define ACS712_PIN          34
#define ACS712_SENSITIVITY  0.066   // 66mV/A for 30A version
#define ACS712_ZERO_VOLTAGE 1.488

// ============================================
// OLED DISPLAY SETTINGS
// ============================================
#define OLED_ADDRESS        0x3C
#define OLED_WIDTH          128
#define OLED_HEIGHT         64

// ============================================
// Ra-02 SX1278 LoRa MODULE PINS
// ============================================
#define LORA_NSS            5
#define LORA_RST            14
#define LORA_DIO0           26
#define LORA_SCK            18
#define LORA_MISO           19
#define LORA_MOSI           23
#define LORA_FREQUENCY      433E6
#define LORA_SF_FAST        7
#define LORA_SF_SAFE        12
#define LORA_TX_POWER       17
#define LORA_BANDWIDTH      125E3
#define LORA_CODING_RATE    5

// ============================================
// TRANSMISSION & LQI SETTINGS
// ============================================
#define TX_INTERVAL_FAST    2000
#define TX_INTERVAL_SAFE    8000
#define ACK_TIMEOUT         3000

#define LQI_FAST_THRESHOLD  75.0
#define LQI_SAFE_THRESHOLD  70.0

#define W_RSSI              0.25
#define W_SNR               0.20
#define W_PDR               0.20
#define W_LOSS              0.15
#define W_SIZE              0.08
#define W_CRC               0.07
#define W_INTERVAL          0.05

#define LQI_HISTORY_SIZE    5

#endif