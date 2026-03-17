# ============================================
# ALERT-LoRa Flask Backend
# Main server file - connects all parts
# CINEC Campus — BSc ETE Project 2026
# ============================================

from flask import Flask, render_template, jsonify
import threading
import time
from mock_data import (
    generate_mock_node_data,
    PacketTracker
)
from lqi_calculator import predict_trend
from config import UPDATE_INTERVAL

# ============================================
# INITIALIZE FLASK APP
# ============================================
app = Flask(__name__)
app.config['SECRET_KEY'] = 'alert-lora-2026'

# ============================================
# GLOBAL DATA STORE
# Holds latest data from both nodes
# ============================================
latest_data = {
    'node1'       : {},
    'node2'       : {},
    'stats'       : {
        'sent'      : 0,
        'received'  : 0,
        'lost'      : 0,
        'crc_errors': 0
    },
    'lqi1_history': [],
    'lqi2_history': []
}

# ============================================
# PACKET TRACKER INSTANCE
# ============================================
tracker = PacketTracker()

# ============================================
# BACKGROUND THREAD CONTROL
# Prevents multiple threads from starting
# ============================================
thread_started  = False
thread_lock     = threading.Lock()

# ============================================
# BACKGROUND DATA UPDATE FUNCTION
# Runs continuously in separate thread
# Generates mock data every 2 seconds
# ============================================
def update_data():
    """
    Background thread function.
    Generates mock data and updates global store.
    Runs every UPDATE_INTERVAL seconds.
    """
    global latest_data

    print('Data update thread started ✅')

    while True:
        try:
            # Generate mock data for both nodes
            node1 = generate_mock_node_data(
                        '01', 'normal')
            node2 = generate_mock_node_data(
                        '02', 'distance')

            # Update packet statistics
            stats = tracker.update(node1, node2)

            # Update LQI history for trend analysis
            latest_data['lqi1_history'].append(
                node1['lqi'])
            latest_data['lqi2_history'].append(
                node2['lqi'])

            # Keep only last 10 LQI values
            if len(latest_data['lqi1_history']) > 10:
                latest_data['lqi1_history'].pop(0)
            if len(latest_data['lqi2_history']) > 10:
                latest_data['lqi2_history'].pop(0)

            # Calculate trend and prediction
            trend1, pred1 = predict_trend(
                latest_data['lqi1_history'])
            trend2, pred2 = predict_trend(
                latest_data['lqi2_history'])

            # Add trend data to node data
            node1['trend']     = trend1
            node1['predicted'] = pred1
            node2['trend']     = trend2
            node2['predicted'] = pred2

            # Update global data store
            latest_data['node1'] = node1
            latest_data['node2'] = node2
            latest_data['stats'] = stats

            # Print status to terminal
            print(
                f"Node01 LQI:{node1['lqi']:.1f}"
                f" Mode:{node1['mode']}"
                f" Trend:{trend1}"
                f" | Node02 LQI:{node2['lqi']:.1f}"
                f" Mode:{node2['mode']}"
                f" Trend:{trend2}"
            )

        except Exception as e:
            # Log any errors
            print(f'Update error: {str(e)}')

        # Wait before next update
        time.sleep(UPDATE_INTERVAL)

# ============================================
# START BACKGROUND THREAD
# Called once when app starts
# ============================================
def start_background_thread():
    """
    Start the background data update thread.
    Uses lock to ensure only one thread runs.
    """
    global thread_started

    with thread_lock:
        if not thread_started:
            thread_started = True
            thread = threading.Thread(
                         target=update_data,
                         name='DataUpdateThread')
            thread.daemon = True
            thread.start()
            print('Background thread started ✅')

# ============================================
# FLASK ROUTES
# ============================================

@app.route('/')
def index():
    """
    Main dashboard page route.
    Renders index.html template.
    """
    return render_template('index.html')


@app.route('/api/data')
def get_data():
    """
    API endpoint for latest data.
    Returns complete data as JSON.
    Test: http://localhost:5001/api/data
    """
    return jsonify(latest_data)


@app.route('/api/live')
def live_data():
    """
    API endpoint for live dashboard updates.
    Called by JavaScript every 2 seconds.
    Test: http://localhost:5001/api/live
    """
    return jsonify({
        'node1' : latest_data['node1'],
        'node2' : latest_data['node2'],
        'stats' : latest_data['stats']
    })


@app.route('/api/node1')
def node1_data():
    """
    API endpoint for Node 01 data only.
    Test: http://localhost:5001/api/node1
    """
    return jsonify(latest_data['node1'])


@app.route('/api/node2')
def node2_data():
    """
    API endpoint for Node 02 data only.
    Test: http://localhost:5001/api/node2
    """
    return jsonify(latest_data['node2'])


@app.route('/api/stats')
def stats_data():
    """
    API endpoint for packet statistics.
    Test: http://localhost:5001/api/stats
    """
    return jsonify(latest_data['stats'])


@app.route('/api/test')
def test_data():
    """
    API test endpoint.
    Generates one sample data packet.
    Test: http://localhost:5001/api/test
    """
    sample = generate_mock_node_data(
                 '01', 'normal')
    return jsonify(sample)


# ============================================
# RUN FLASK SERVER
# ============================================
if __name__ == '__main__':

    # Print startup message
    print('==========================================')
    print('ALERT-LoRa Dashboard Starting...')
    print('Open browser: http://localhost:5001')
    print('==========================================')

    # Start background data thread
    start_background_thread()

    # Run Flask development server
    app.run(
        host        = '0.0.0.0',
        port        = 5001,
        debug       = False,
        use_reloader= False,
        threaded    = True
    )