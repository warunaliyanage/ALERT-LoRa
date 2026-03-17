# ============================================
# ALERT-LoRa LQI Calculator
# Combines all 7 factors into one LQI score
# This is the NOVELTY of the project!
# CINEC Campus — BSc ETE Project 2026
# ============================================

from config import (
    LQI_WEIGHTS,
    RSSI_MIN, RSSI_MAX,
    SNR_MIN,  SNR_MAX,
    MAX_PACKET_SIZE,
    FAST_INTERVAL, SAFE_INTERVAL,
    FAST_THRESHOLD, SAFE_THRESHOLD
)


# ============================================
# NORMALIZATION FUNCTIONS
# Convert raw values to 0-100 scale
# ============================================

def normalize_rssi(rssi):
    """
    Normalize RSSI from dBm to 0-100 scale.
    -40 dBm (strong) = 100
    -120 dBm (weak)  = 0
    """
    normalized = ((rssi - RSSI_MIN) /
                  (RSSI_MAX - RSSI_MIN)) * 100
    return min(100, max(0, normalized))


def normalize_snr(snr):
    """
    Normalize SNR from dB to 0-100 scale.
    +20 dB (excellent) = 100
    -20 dB (poor)      = 0
    """
    normalized = ((snr - SNR_MIN) /
                  (SNR_MAX - SNR_MIN)) * 100
    return min(100, max(0, normalized))


def normalize_packet_size(size):
    """
    Normalize packet size to 0-100 scale.
    Smaller packet = higher score (better reliability)
    0 bytes   = 100
    255 bytes = 0
    """
    normalized = (1 - size / MAX_PACKET_SIZE) * 100
    return min(100, max(0, normalized))


def normalize_tx_interval(interval):
    """
    Normalize transmission interval to 0-100 scale.
    Short interval (2000ms) = 90 score
    Long interval  (8000ms) = 40 score
    """
    if interval <= FAST_INTERVAL:
        return 90
    else:
        return 40


# ============================================
# MAIN LQI CALCULATION FUNCTION
# Combines all 7 normalized factors
# ============================================

def calculate_lqi(rssi, snr, pdr, packet_loss,
                  packet_size, crc_score, tx_interval):
    """
    Calculate Link Quality Index (LQI) from 7 factors.

    Parameters:
        rssi        : Received Signal Strength (dBm)
        snr         : Signal-to-Noise Ratio (dB)
        pdr         : Packet Delivery Ratio (%)
        packet_loss : Packet Loss Rate (%)
        packet_size : Packet Size (bytes)
        crc_score   : CRC Validation Score (%)
        tx_interval : Transmission Interval (ms)

    Returns:
        lqi : Link Quality Index score (0-100)
    """

    # --- Step 1: Normalize all 7 factors to 0-100 ---
    rssi_norm     = normalize_rssi(rssi)
    snr_norm      = normalize_snr(snr)
    pdr_norm      = float(pdr)
    loss_norm     = 100 - float(packet_loss)
    size_norm     = normalize_packet_size(packet_size)
    crc_norm      = float(crc_score)
    interval_norm = normalize_tx_interval(tx_interval)

    # --- Step 2: Apply weights to each factor ---
    lqi = (
        rssi_norm     * LQI_WEIGHTS['rssi']     +
        snr_norm      * LQI_WEIGHTS['snr']      +
        pdr_norm      * LQI_WEIGHTS['pdr']      +
        loss_norm     * LQI_WEIGHTS['loss']     +
        size_norm     * LQI_WEIGHTS['size']     +
        crc_norm      * LQI_WEIGHTS['crc']      +
        interval_norm * LQI_WEIGHTS['interval']
    )

    # --- Step 3: Return rounded LQI score ---
    return round(lqi, 2)


# ============================================
# LQI STATUS CLASSIFIER
# ============================================

def get_lqi_status(lqi):
    """
    Classify LQI score into human readable status.

    Returns:
        'GOOD' : LQI >= 75 (FAST mode)
        'FAIR' : LQI >= 60 (borderline)
        'POOR' : LQI < 60  (SAFE mode)
    """
    if lqi >= FAST_THRESHOLD:
        return 'GOOD'
    elif lqi >= SAFE_THRESHOLD:
        return 'FAIR'
    else:
        return 'POOR'


# ============================================
# MODE DECISION FUNCTION
# ============================================

def decide_mode(lqi):
    """
    Decide transmission mode based on LQI score.

    Returns:
        'FAST' : Good link quality
        'SAFE' : Poor link quality
    """
    if lqi >= FAST_THRESHOLD:
        return 'FAST'
    else:
        return 'SAFE'


# ============================================
# TREND PREDICTOR
# Analyzes LQI history to predict next value
# ============================================

def predict_trend(lqi_history):
    """
    Predict LQI trend from history window.

    Parameters:
        lqi_history : List of recent LQI values

    Returns:
        trend     : 'degrading', 'improving', 'stable'
        predicted : Predicted next LQI value
    """

    # Need at least 3 values to analyze trend
    if len(lqi_history) < 3:
        return 'stable', lqi_history[-1] if lqi_history else 0

    # Get last 3 values for trend analysis
    recent = lqi_history[-3:]

    # Check if consistently going down
    if recent[0] > recent[1] > recent[2]:
        rate      = recent[0] - recent[2]
        predicted = max(0, recent[2] - rate / 2)
        return 'degrading', round(predicted, 2)

    # Check if consistently going up
    elif recent[0] < recent[1] < recent[2]:
        rate      = recent[2] - recent[0]
        predicted = min(100, recent[2] + rate / 2)
        return 'improving', round(predicted, 2)

    # Stable - no clear trend
    else:
        predicted = sum(recent) / len(recent)
        return 'stable', round(predicted, 2)


# ============================================
# GET NORMALIZED FACTOR VALUES
# Used for factor breakdown bars in dashboard
# ============================================

def get_normalized_factors(rssi, snr, pdr,
                            packet_loss, packet_size,
                            crc_score, tx_interval):
    """
    Returns all 7 normalized factor values.
    Used for displaying breakdown bars.
    """
    return {
        'rssi'     : round(normalize_rssi(rssi), 1),
        'snr'      : round(normalize_snr(snr), 1),
        'pdr'      : round(float(pdr), 1),
        'loss'     : round(100 - float(packet_loss), 1),
        'size'     : round(normalize_packet_size(
                               packet_size), 1),
        'crc'      : round(float(crc_score), 1),
        'interval' : round(normalize_tx_interval(
                               tx_interval), 1)
    }