# ============================================
# ALERT-LoRa Mock Data Generator
# Generates fake sensor data for testing
# Replace with real data later
# ============================================

import random
import time
from lqi_calculator import calculate_lqi

def generate_mock_node_data(node_id, scenario='normal'):
    # ----------------------------------------
    # Generate mock sensor readings
    # scenario: normal / obstacle / distance
    # ----------------------------------------

    if scenario == 'normal':
        # Good signal conditions
        rssi        = random.uniform(-70, -80)
        snr         = random.uniform(7, 12)
        pdr         = random.uniform(95, 100)

    elif scenario == 'obstacle':
        # Signal blocked by obstacle
        rssi        = random.uniform(-95, -110)
        snr         = random.uniform(-2, 4)
        pdr         = random.uniform(70, 85)

    elif scenario == 'distance':
        # Far distance signal
        rssi        = random.uniform(-100, -115)
        snr         = random.uniform(-5, 2)
        pdr         = random.uniform(65, 80)

    # Common values for all scenarios
    packet_loss   = 100 - pdr
    packet_size   = random.randint(12, 20)
    crc_score     = random.uniform(95, 100)
    tx_interval   = 2000 if pdr > 90 else 8000

    # Calculate LQI using 7 factors
    lqi = calculate_lqi(
        rssi, snr, pdr, packet_loss,
        packet_size, crc_score, tx_interval
    )

    # Generate sensor readings
    temperature   = round(random.uniform(26, 30), 1)
    humidity      = round(random.uniform(60, 70), 1)
    current       = round(random.uniform(1.5, 3.0), 2)
    vibration     = round(random.uniform(0.05, 0.20), 3)

    # Determine mode based on LQI
    mode = 'FAST' if lqi >= 70 else 'SAFE'

    return {
        'node_id'     : node_id,
        'timestamp'   : time.strftime('%H:%M:%S'),
        'rssi'        : round(rssi, 1),
        'snr'         : round(snr, 1),
        'pdr'         : round(pdr, 1),
        'packet_loss' : round(packet_loss, 1),
        'packet_size' : packet_size,
        'crc_score'   : round(crc_score, 1),
        'tx_interval' : tx_interval,
        'lqi'         : lqi,
        'mode'        : mode,
        'temperature' : temperature,
        'humidity'    : humidity,
        'current'     : current,
        'vibration'   : vibration
    }