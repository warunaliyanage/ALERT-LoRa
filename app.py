# ============================================
# ALERT-LoRa Flask Backend
# Main server file
# ============================================

from flask import Flask, render_template, jsonify
from flask_socketio import SocketIO, emit
import threading
import time
from mock_data import generate_mock_node_data
from config import HOST, PORT, DEBUG, UPDATE_INTERVAL

# Initialize Flask app
app = Flask(__name__)
app.config['SECRET_KEY'] = 'alert-lora-secret'
socketio = SocketIO(app, cors_allowed_origins="*",
                    async_mode='threading')

# Store latest data
latest_data = {
    'node1': {},
    'node2': {},
    'stats': {
        'sent'      : 0,
        'received'  : 0,
        'lost'      : 0,
        'crc_errors': 0
    }
}

# Main page route
@app.route('/')
def index():
    return render_template('index.html')

# API route
@app.route('/api/data')
def get_data():
    return jsonify(latest_data)

# Background data update thread
def update_data():
    pkt_sent = 0
    pkt_recv = 0

    while True:
        # Generate mock data
        node1 = generate_mock_node_data('01', 'normal')
        node2 = generate_mock_node_data('02', 'distance')

        # Update packet stats
        pkt_sent += 2
        pkt_recv += 2 if (
            node1['pdr'] > 90 and
            node2['pdr'] > 90
        ) else 1

        # Update data store
        latest_data['node1'] = node1
        latest_data['node2'] = node2
        latest_data['stats'] = {
            'sent'      : pkt_sent,
            'received'  : pkt_recv,
            'lost'      : pkt_sent - pkt_recv,
            'crc_errors': pkt_sent - pkt_recv
        }

        # Send to browser
        socketio.emit('update', latest_data)
        time.sleep(UPDATE_INTERVAL)

# When client connects
@socketio.on('connect')
def handle_connect():
    print('Browser connected!')
    thread = threading.Thread(target=update_data)
    thread.daemon = True
    thread.start()

# Run server
if __name__ == '__main__':
    print('==========================================')
    print('ALERT-LoRa Dashboard Starting...')
    print('Open browser: http://localhost:5001')
    print('==========================================')
    socketio.run(
        app,
        host                  = '0.0.0.0',
        port                  = 5001,
        debug                 = False,
        use_reloader          = False,
        allow_unsafe_werkzeug = True
    )