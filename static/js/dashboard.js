/* ============================================
   ALERT-LoRa Dashboard JavaScript
   Handles live data updates and charts
   CINEC Campus — BSc ETE Project 2026
   ============================================ */


/* ============================================
   CLOCK — Updates every second
   ============================================ */
function updateClock() {
    var now = new Date();
    var h   = String(now.getHours()).padStart(2, '0');
    var m   = String(now.getMinutes()).padStart(2, '0');
    var s   = String(now.getSeconds()).padStart(2, '0');
    document.getElementById('clock')
            .textContent = h + ':' + m + ':' + s;
}
/* Run clock every 1 second */
setInterval(updateClock, 1000);
updateClock();


/* ============================================
   CHART DATA ARRAYS
   Store history for rolling graphs
   ============================================ */
var chartLabels  = [];
var lqi1History  = [];
var lqi2History  = [];
var rssi1History = [];
var rssi2History = [];
var pdr1History  = [];
var pdr2History  = [];

/* Maximum data points to show on charts */
var MAX_POINTS = 25;


/* ============================================
   CHART FACTORY FUNCTION
   Creates a Chart.js line chart
   ============================================ */
function createLineChart(canvasId, color1, color2,
                          yMin, yMax) {
    /* Get canvas context */
    var ctx = document.getElementById(canvasId)
                      .getContext('2d');

    return new Chart(ctx, {
        type : 'line',
        data : {
            labels   : chartLabels,
            datasets : [
                {
                    /* Node 01 dataset */
                    label           : 'Node 01',
                    data            : [],
                    borderColor     : color1,
                    backgroundColor : color1 + '18',
                    borderWidth     : 2,
                    pointRadius     : 0,
                    tension         : 0.4,
                    fill            : true
                },
                {
                    /* Node 02 dataset */
                    label           : 'Node 02',
                    data            : [],
                    borderColor     : color2,
                    backgroundColor : color2 + '18',
                    borderWidth     : 2,
                    pointRadius     : 0,
                    tension         : 0.4,
                    fill            : true
                }
            ]
        },
        options : {
            responsive          : true,
            maintainAspectRatio : false,
            /* Hide default legend */
            plugins : {
                legend : { display : false }
            },
            scales : {
                /* X axis — time labels */
                x : {
                    ticks : {
                        color         : '#3a5a7a',
                        font          : { size : 9 },
                        maxTicksLimit : 6,
                        maxRotation   : 0
                    },
                    grid : {
                        color : 'rgba(0, 180, 255, 0.05)'
                    }
                },
                /* Y axis — value range */
                y : {
                    min   : yMin,
                    max   : yMax,
                    ticks : {
                        color : '#3a5a7a',
                        font  : { size : 9 }
                    },
                    grid : {
                        color : 'rgba(0, 180, 255, 0.05)'
                    }
                }
            },
            /* Fast animation */
            animation : { duration : 400 }
        }
    });
}


/* ============================================
   CREATE ALL THREE CHARTS
   LQI, RSSI, and PDR charts
   ============================================ */

/* LQI trend chart — main novelty chart */
var lqiChart = createLineChart(
    'lqiChart',
    '#00e676',   /* Node 01 — green */
    '#ffc107',   /* Node 02 — amber */
    40, 100      /* Y axis range */
);
/* Assign correct data arrays */
lqiChart.data.datasets[0].data = lqi1History;
lqiChart.data.datasets[1].data = lqi2History;

/* RSSI over time chart */
var rssiChart = createLineChart(
    'rssiChart',
    '#00b4ff',   /* Node 01 — blue */
    '#ff5252',   /* Node 02 — red */
    -120, -40    /* Y axis range */
);
rssiChart.data.datasets[0].data = rssi1History;
rssiChart.data.datasets[1].data = rssi2History;

/* PDR over time chart */
var pdrChart = createLineChart(
    'pdrChart',
    '#00e676',   /* Node 01 — green */
    '#ffc107',   /* Node 02 — amber */
    60, 100      /* Y axis range */
);
pdrChart.data.datasets[0].data = pdr1History;
pdrChart.data.datasets[1].data = pdr2History;


/* ============================================
   SET LQI NUMBER AND COLOR
   Updates big LQI display with correct color
   ============================================ */
function setLQI(elementId, value) {
    var el = document.getElementById(elementId);
    el.textContent = value.toFixed(1);

    /* Set color based on LQI value */
    if (value >= 75) {
        el.className = 'lqi-number lqi-good';  /* Green */
    } else if (value >= 60) {
        el.className = 'lqi-number lqi-ok';    /* Amber */
    } else {
        el.className = 'lqi-number lqi-bad';   /* Red */
    }
}


/* ============================================
   SET TREND ARROW
   Shows up/down/flat indicator
   ============================================ */
function setTrend(elementId, trend) {
    var el = document.getElementById(elementId);

    if (trend === 'improving') {
        el.textContent = '▲';
        el.className   = 'trend-up';
    } else if (trend === 'degrading') {
        el.textContent = '▼';
        el.className   = 'trend-down';
    } else {
        el.textContent = '—';
        el.className   = 'trend-flat';
    }
}


/* ============================================
   SET MODE BADGE
   Updates FAST/SAFE badge on node card
   ============================================ */
function setMode(elementId, mode) {
    var el = document.getElementById(elementId);
    el.textContent = mode + ' MODE';
    el.className   = (mode === 'FAST')
                     ? 'mode-fast'
                     : 'mode-safe';
}


/* ============================================
   UPDATE 7 FACTOR BARS
   Updates all 7 factor progress bars
   ============================================ */
function updateFactorBars(factors) {
    /* Map factor keys to bar element IDs */
    var factorMap = {
        'rssi'     : 'rssi',
        'snr'      : 'snr',
        'pdr'      : 'pdr',
        'loss'     : 'loss',
        'size'     : 'size',
        'crc'      : 'crc',
        'interval' : 'txi'
    };

    /* Update each bar width and value */
    for (var key in factorMap) {
        var barId = factorMap[key];
        var val   = factors[key] || 0;
        var pct   = Math.min(100, Math.max(0, val));

        document.getElementById('bar-' + barId)
                .style.width = pct.toFixed(0) + '%';
        document.getElementById('val-' + barId)
                .textContent = pct.toFixed(0);
    }
}


/* ============================================
   ADD LOG TABLE ROW
   Inserts new row at top of data log table
   ============================================ */
function addLogRow(nodeData, nodeId) {
    var body = document.getElementById('log-body');
    var row  = document.createElement('tr');

    /* Mode tag with color */
    var modeTag = nodeData.mode === 'FAST'
        ? '<span class="tag-fast">FAST</span>'
        : '<span class="tag-safe">SAFE</span>';

    /* Build table row */
    row.innerHTML =
        '<td>' + nodeData.timestamp + '</td>' +
        '<td>' + nodeId             + '</td>' +
        '<td>' + nodeData.rssi      + '</td>' +
        '<td>' + nodeData.snr       + '</td>' +
        '<td>' + nodeData.lqi       + '</td>' +
        '<td>' + modeTag            + '</td>';

    /* Insert at top of table */
    body.insertBefore(row, body.firstChild);

    /* Keep only last 8 rows */
    if (body.rows.length > 8) {
        body.deleteRow(body.rows.length - 1);
    }
}


/* ============================================
   CREATE ALERT ITEM HTML
   Builds colored alert notification
   ============================================ */
function makeAlertHTML(type, message) {
    /* Color map for dot colors */
    var dotColors = {
        'ok'   : 'green',
        'warn' : 'amber',
        'error': 'red'
    };

    var dotColor = dotColors[type] || 'green';

    return '<div class="alert-item alert-' + type + '">' +
        '<span class="dot ' + dotColor + '" ' +
        'style="animation:none;opacity:1;flex-shrink:0;">' +
        '</span>' +
        '<span>' + message + '</span>' +
        '</div>';
}


/* ============================================
   UPDATE SYSTEM ALERTS PANEL
   Generates alerts based on current data
   ============================================ */
function updateAlerts(node1, node2) {
    var html = '';

    /* Node 01 LQI alert */
    if (node1.lqi >= 75) {
        html += makeAlertHTML('ok',
            'Node 01 — LQI Good: ' +
            node1.lqi.toFixed(1) +
            ' (' + node1.mode + ' mode)');
    } else if (node1.lqi >= 60) {
        html += makeAlertHTML('warn',
            'Node 01 — LQI Fair: ' +
            node1.lqi.toFixed(1) +
            ' — monitoring link quality');
    } else {
        html += makeAlertHTML('error',
            'Node 01 — LQI Poor: ' +
            node1.lqi.toFixed(1) +
            ' — switched to SAFE mode!');
    }

    /* Node 02 LQI alert */
    if (node2.lqi >= 75) {
        html += makeAlertHTML('ok',
            'Node 02 — LQI Good: ' +
            node2.lqi.toFixed(1) +
            ' (' + node2.mode + ' mode)');
    } else if (node2.lqi >= 60) {
        html += makeAlertHTML('warn',
            'Node 02 — LQI Fair: ' +
            node2.lqi.toFixed(1) +
            ' — monitoring link quality');
    } else {
        html += makeAlertHTML('error',
            'Node 02 — LQI Poor: ' +
            node2.lqi.toFixed(1) +
            ' — SAFE mode active');
    }

    /* RSSI warning for Node 01 */
    if (node1.rssi < -100) {
        html += makeAlertHTML('warn',
            'Node 01 — Weak RSSI: ' +
            node1.rssi + ' dBm');
    } else {
        html += makeAlertHTML('ok',
            'Node 01 — RSSI OK: ' +
            node1.rssi + ' dBm');
    }

    /* RSSI warning for Node 02 */
    if (node2.rssi < -100) {
        html += makeAlertHTML('warn',
            'Node 02 — Weak RSSI: ' +
            node2.rssi + ' dBm');
    } else {
        html += makeAlertHTML('ok',
            'Node 02 — RSSI OK: ' +
            node2.rssi + ' dBm');
    }

    /* Update alerts panel */
    document.getElementById('alerts-body')
            .innerHTML = html;
}


/* ============================================
   UPDATE CHARTS WITH NEW DATA
   Adds new points and removes old ones
   ============================================ */
function updateCharts(node1, node2, timeLabel) {

    /* Add new data points */
    chartLabels.push(timeLabel);
    lqi1History.push(node1.lqi);
    lqi2History.push(node2.lqi);
    rssi1History.push(node1.rssi);
    rssi2History.push(node2.rssi);
    pdr1History.push(node1.pdr);
    pdr2History.push(node2.pdr);

    /* Remove oldest point if over limit */
    if (chartLabels.length > MAX_POINTS) {
        chartLabels.shift();
        lqi1History.shift();
        lqi2History.shift();
        rssi1History.shift();
        rssi2History.shift();
        pdr1History.shift();
        pdr2History.shift();
    }

    /* Update all three charts */
    lqiChart.update();
    rssiChart.update();
    pdrChart.update();
}


/* ============================================
   MAIN DASHBOARD UPDATE FUNCTION
   Called every time new data arrives
   Updates ALL dashboard elements
   ============================================ */
function updateDashboard(data) {

    /* Extract data from response */
    var n1 = data.node1;
    var n2 = data.node2;
    var st = data.stats;

    /* Check if data is valid */
    if (!n1 || !n2 || !n1.lqi) return;

    /* Get current time for chart label */
    var now = new Date();
    var timeLabel =
        now.getHours() + ':' +
        String(now.getMinutes()).padStart(2, '0') + ':' +
        String(now.getSeconds()).padStart(2, '0');

    /* ---- Update Node 01 sensor values ---- */
    document.getElementById('temp1').innerHTML =
        n1.temperature +
        ' <span class="sensor-unit">°C</span>';
    document.getElementById('hum1').innerHTML =
        n1.humidity +
        ' <span class="sensor-unit">%</span>';
    document.getElementById('cur1').innerHTML =
        n1.current +
        ' <span class="sensor-unit">A</span>';
    document.getElementById('vib1').innerHTML =
        n1.vibration +
        ' <span class="sensor-unit">g</span>';

    /* ---- Update Node 02 sensor values ---- */
    document.getElementById('temp2').innerHTML =
        n2.temperature +
        ' <span class="sensor-unit">°C</span>';
    document.getElementById('hum2').innerHTML =
        n2.humidity +
        ' <span class="sensor-unit">%</span>';
    document.getElementById('cur2').innerHTML =
        n2.current +
        ' <span class="sensor-unit">A</span>';
    document.getElementById('vib2').innerHTML =
        n2.vibration +
        ' <span class="sensor-unit">g</span>';

    /* ---- Update LQI scores ---- */
    setLQI('lqi1', n1.lqi);
    setLQI('lqi2', n2.lqi);

    /* ---- Update trend arrows ---- */
    setTrend('trend1', n1.trend || 'stable');
    setTrend('trend2', n2.trend || 'stable');

    /* ---- Update predicted values ---- */
    document.getElementById('pred1')
            .textContent = (n1.predicted || n1.lqi)
                           .toFixed(1);
    document.getElementById('pred2')
            .textContent = (n2.predicted || n2.lqi)
                           .toFixed(1);

    /* ---- Update mode badges ---- */
    setMode('mode-n1', n1.mode);
    setMode('mode-n2', n2.mode);

    /* ---- Update 7 factor bars ---- */
    if (n1.factors) {
        updateFactorBars(n1.factors);
    }

    /* ---- Update packet statistics ---- */
    document.getElementById('stat-sent')
            .textContent = st.sent;
    document.getElementById('stat-recv')
            .textContent = st.received;
    document.getElementById('stat-lost')
            .textContent = st.lost;
    document.getElementById('stat-crc')
            .textContent = st.crc_errors;

    /* ---- Update charts ---- */
    updateCharts(n1, n2, timeLabel);

    /* ---- Add row to data log ---- */
    addLogRow(n1, '01');

    /* ---- Update system alerts ---- */
    updateAlerts(n1, n2);
}


/* ============================================
   FETCH DATA FROM API
   Simple HTTP polling — no Socket.IO needed
   Calls /api/live endpoint every 2 seconds
   ============================================ */
function fetchLiveData() {
    fetch('/api/live')
        .then(function(response) {
            /* Check response is OK */
            if (!response.ok) {
                throw new Error(
                    'HTTP error: ' + response.status);
            }
            return response.json();
        })
        .then(function(data) {
            /* Update dashboard with new data */
            updateDashboard(data);
        })
        .catch(function(error) {
            /* Log any fetch errors */
            console.error('Fetch error:', error);
        });
}

/* ============================================
   START LIVE DATA POLLING
   Fetch immediately then every 2 seconds
   ============================================ */

/* Fetch data immediately on page load */
fetchLiveData();

/* Then fetch every 2 seconds */
setInterval(fetchLiveData, 2000);

/* Log startup message */
console.log('ALERT-LoRa Dashboard initialized ✅');
console.log('Polling /api/live every 2 seconds...');