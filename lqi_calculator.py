# ============================================
# ALERT-LoRa LQI Calculator
# Combines all 7 factors into one score
# This is the NOVELTY of the project!
# ============================================

from config import LQI_WEIGHTS

def normalize_rssi(rssi):
    # Normalize RSSI from -120 to -40 dBm → 0 to 100
    return min(100, max(0, ((rssi + 120) / 80) * 100))

def normalize_snr(snr):
    # Normalize SNR from -20 to +20 dB → 0 to 100
    return min(100, max(0, ((snr + 20) / 40) * 100))

def normalize_packet_size(size):
    # Smaller packet size is better
    # Normalize from 255 bytes → 0 to 100
    return min(100, max(0, (1 - size / 255) * 100))

def normalize_interval(interval):
    # Shorter interval in good conditions is better
    # Normalize from 1000ms to 10000ms → 0 to 100
    return min(100, max(0, (1 - (interval - 1000) / 9000) * 100))

def calculate_lqi(rssi, snr, pdr, packet_loss,
                  packet_size, crc_score, tx_interval):
    # ----------------------------------------
    # Step 1: Normalize all 7 factors to 0-100
    # ----------------------------------------
    rssi_norm     = normalize_rssi(rssi)
    snr_norm      = normalize_snr(snr)
    pdr_norm      = pdr
    loss_norm     = 100 - packet_loss
    size_norm     = normalize_packet_size(packet_size)
    crc_norm      = crc_score
    interval_norm = normalize_interval(tx_interval)

    # ----------------------------------------
    # Step 2: Apply weights to each factor
    # ----------------------------------------
    lqi = (
        rssi_norm     * LQI_WEIGHTS['rssi']     +
        snr_norm      * LQI_WEIGHTS['snr']      +
        pdr_norm      * LQI_WEIGHTS['pdr']      +
        loss_norm     * LQI_WEIGHTS['loss']     +
        size_norm     * LQI_WEIGHTS['size']     +
        crc_norm      * LQI_WEIGHTS['crc']      +
        interval_norm * LQI_WEIGHTS['interval']
    )

    # ----------------------------------------
    # Step 3: Return final LQI score (0-100)
    # ----------------------------------------
    return round(lqi, 2)

def get_lqi_status(lqi):
    # Classify LQI score into status
    if lqi >= 75:
        return 'GOOD'
    elif lqi >= 60:
        return 'FAIR'
    else:
        return 'POOR'