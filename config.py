# ============================================
# ALERT-LoRa Configuration File
# All settings and LQI weights stored here
# ============================================

# --- LQI Weight Configuration ---
# These weights are your research contribution!
# Total must equal 1.0 (100%)
LQI_WEIGHTS = {
    'rssi'     : 0.25,   # 25% weight
    'snr'      : 0.20,   # 20% weight
    'pdr'      : 0.20,   # 20% weight
    'loss'     : 0.15,   # 15% weight
    'size'     : 0.08,   # 8%  weight
    'crc'      : 0.07,   # 7%  weight
    'interval' : 0.05    # 5%  weight
}

# --- Mode Switching Thresholds ---
FAST_THRESHOLD = 75   # Above this = FAST mode
SAFE_THRESHOLD = 70   # Below this = SAFE mode

# --- Trend Analysis Window ---
TREND_WINDOW = 5      # Number of LQI values to analyze

# --- Flask Server Settings ---
HOST  = '0.0.0.0'
PORT  = 5001
DEBUG = False

# --- Update Interval ---
UPDATE_INTERVAL = 2   # Seconds between updates