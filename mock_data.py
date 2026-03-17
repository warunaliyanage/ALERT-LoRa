# ============================================
# ALERT-LoRa Mock Data Generator
# Generates realistic fake sensor data
# Used for testing dashboard without hardware
# Replace with real ESP32 data later
# CINEC Campus — BSc ETE Project 2026
# ============================================

import random
import time
from lqi_calculator import (
    calculate_lqi,
    get_lqi_status,
    decide_mode,
    get_normalized_factors
)
from config import FAST_INTERVAL, SAFE_INTERVAL


# ============================================
# SCENARIO CONFIGURATIONS
# Different signal conditions for testing
# ============================================

SCENARIOS = {

    # Normal line-of-sight condition
    # Good signal, high PDR
    'normal': {
        'rssi_range'  : (-65, -80),
        'snr_range'   : (7, 12),
        'pdr_range'   : (95, 100),
        'description' : 'Line-of-Sight (LOS)'
    },

    # Obstacle condition
    # Reduced signal due to walls/barriers
    'obstacle': {
        'rssi_range'  : (-90, -108),
        'snr_range'   : (-2, 5),
        'pdr_range'   : (70, 88),
        'description' : 'Obstacle (NLOS)'
    },

    # Distance variation condition
    # Signal weakens with distance
    'distance': {
        'rssi_range'  : (-95, -115),
        'snr_range'   : (-5, 3),
        'pdr_range'   : (65, 82),
        'description' : 'Distance Variation'
    },

    # Mixed condition
    # Randomly varies between good and poor
    'mixed': {
        'rssi_range'  : (-70, -110),
        'snr_range'   : (-3, 10),
        'pdr_range'   : (75, 100),
        'description' : 'Mixed Conditions'
    }
}


# ============================================
# MAIN MOCK DATA GENERATOR
# ============================================

def generate_mock_node_data(node_id,
                             scenario='normal'):
    """
    Generate complete mock telemetry data
    for one sensor node.

    Parameters:
        node_id  : Node identifier ('01' or '02')
        scenario : Signal condition scenario

    Returns:
        Dictionary with all sensor and link data
    """

    # Get scenario configuration
    config = SCENARIOS.get(scenario,
                            SCENARIOS['normal'])

    # --- Generate link quality values ---
    rssi = round(random.uniform(
        config['rssi_range'][0],
        config['rssi_range'][1]
    ), 1)

    snr = round(random.uniform(
        config['snr_range'][0],
        config['snr_range'][1]
    ), 1)

    pdr = round(random.uniform(
        config['pdr_range'][0],
        config['pdr_range'][1]
    ), 1)

    # --- Calculate derived values ---
    packet_loss = round(100 - pdr, 1)
    packet_size = random.randint(12, 20)

    # CRC score based on signal quality
    if pdr > 90:
        crc_score = round(random.uniform(97, 100), 1)
    elif pdr > 80:
        crc_score = round(random.uniform(88, 97), 1)
    else:
        crc_score = round(random.uniform(75, 88), 1)

    # --- Calculate LQI score (7 factors) ---
    # Determine interval based on current LQI
    # First calculate quick LQI to decide interval
    quick_lqi = calculate_lqi(
        rssi, snr, pdr, packet_loss,
        packet_size, crc_score, FAST_INTERVAL
    )

    # Set interval based on quick LQI estimate
    tx_interval = (FAST_INTERVAL
                   if quick_lqi >= 70
                   else SAFE_INTERVAL)

    # Final LQI calculation with correct interval
    lqi = calculate_lqi(
        rssi, snr, pdr, packet_loss,
        packet_size, crc_score, tx_interval
    )

    # --- Determine mode ---
    mode = decide_mode(lqi)

    # --- Get normalized factors for dashboard ---
    factors = get_normalized_factors(
        rssi, snr, pdr, packet_loss,
        packet_size, crc_score, tx_interval
    )

    # --- Generate sensor readings ---
    # Slightly different ranges per node
    if node_id == '01':
        temperature = round(
            random.uniform(26.0, 29.5), 1)
        humidity    = round(
            random.uniform(60.0, 70.0), 1)
        current     = round(
            random.uniform(1.8, 3.2), 2)
        vibration   = round(
            random.uniform(0.05, 0.25), 3)
    else:
        temperature = round(
            random.uniform(27.0, 31.0), 1)
        humidity    = round(
            random.uniform(58.0, 68.0), 1)
        current     = round(
            random.uniform(1.5, 2.8), 2)
        vibration   = round(
            random.uniform(0.03, 0.20), 3)

    # --- Build complete data dictionary ---
    return {
        # Node identification
        'node_id'       : node_id,
        'timestamp'     : time.strftime('%H:%M:%S'),
        'scenario'      : config['description'],

        # Link quality factors (7 factors)
        'rssi'          : rssi,
        'snr'           : snr,
        'pdr'           : pdr,
        'packet_loss'   : packet_loss,
        'packet_size'   : packet_size,
        'crc_score'     : crc_score,
        'tx_interval'   : tx_interval,

        # LQI score (novelty!)
        'lqi'           : lqi,
        'lqi_status'    : get_lqi_status(lqi),
        'mode'          : mode,

        # Normalized factor values for bars
        'factors'       : factors,

        # Physical sensor readings
        'temperature'   : temperature,
        'humidity'      : humidity,
        'current'       : current,
        'vibration'     : vibration
    }


# ============================================
# PACKET STATISTICS TRACKER
# ============================================

class PacketTracker:
    """
    Tracks packet statistics for both nodes.
    Calculates sent, received, lost, CRC errors.
    """

    def __init__(self):
        # Initialize all counters to zero
        self.sent       = 0
        self.received   = 0
        self.lost       = 0
        self.crc_errors = 0

    def update(self, node1_data, node2_data):
        """
        Update packet statistics based on
        latest node data.
        """
        # Increment sent count for both nodes
        self.sent += 2

        # Count received based on PDR values
        n1_recv = 1 if node1_data['pdr'] > 85 else 0
        n2_recv = 1 if node2_data['pdr'] > 85 else 0
        self.received += n1_recv + n2_recv

        # Calculate lost packets
        self.lost = self.sent - self.received

        # Count CRC errors
        if node1_data['crc_score'] < 95:
            self.crc_errors += 1
        if node2_data['crc_score'] < 95:
            self.crc_errors += 1

        return {
            'sent'      : self.sent,
            'received'  : self.received,
            'lost'      : max(0, self.lost),
            'crc_errors': self.crc_errors
        }