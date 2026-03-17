# ============================================
# ALERT-LoRa Configuration File
# All project settings stored here
# CINEC Campus — BSc ETE Project 2026
# ============================================

# --- LQI Weight Configuration ---
# These weights are your research contribution!
# All weights must add up to 1.0 (100%)
LQI_WEIGHTS = {
    'rssi'     : 0.25,   # 25% — Signal strength
    'snr'      : 0.20,   # 20% — Signal quality
    'pdr'      : 0.20,   # 20% — Delivery ratio
    'loss'     : 0.15,   # 15% — Packet loss
    'size'     : 0.08,   # 8%  — Packet size
    'crc'      : 0.07,   # 7%  — CRC validation
    'interval' : 0.05    # 5%  — TX interval
}

# --- RSSI Normalization Range ---
RSSI_MIN = -120   # Worst RSSI value (dBm)
RSSI_MAX = -40    # Best RSSI value (dBm)

# --- SNR Normalization Range ---
SNR_MIN = -20     # Worst SNR value (dB)
SNR_MAX = 20      # Best SNR value (dB)

# --- Mode Switching Thresholds ---
FAST_THRESHOLD = 75   # LQI above this = FAST mode
SAFE_THRESHOLD = 70   # LQI below this = SAFE mode

# --- Trend Analysis Window ---
# Number of LQI values used for prediction
TREND_WINDOW = 5

# --- Flask Server Settings ---
HOST  = '0.0.0.0'    # Listen on all interfaces
PORT  = 5001          # Server port number
DEBUG = False         # Debug mode off

# --- Update Interval ---
UPDATE_INTERVAL = 2   # Seconds between data updates

# --- Packet Settings ---
MAX_PACKET_SIZE = 255  # Maximum LoRa packet size bytes

# --- Transmission Intervals ---
FAST_INTERVAL = 2000   # 2 seconds in FAST mode (ms)
SAFE_INTERVAL = 8000   # 8 seconds in SAFE mode (ms)