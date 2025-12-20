/**
 * Dashboard Statistics Module - LELE PENTEST DEVICE
 * Real-time charts and metrics using Chart.js
 */

const DashboardStats = {
    charts: {},
    statsHistory: { attacks: [], cpu: [], ram: [], time: [] },
    maxHistoryPoints: 50,

    init() {
        this.initCharts();
        this.startRealtimeUpdates();
        console.log('[STATS] Dashboard statistics initialized');
    },

    initCharts() {
        // Attack Timeline Chart
        const timelineCtx = document.getElementById('attackTimelineChart');
        if (timelineCtx) {
            this.charts.timeline = new Chart(timelineCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Attacks/min',
                        data: [],
                        borderColor: '#00f3ff',
                        backgroundColor: 'rgba(0,243,255,0.1)',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { display: false } },
                    scales: {
                        x: { display: false },
                        y: { beginAtZero: true, grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: '#888' } }
                    }
                }
            });
        }

        // Attack Types Doughnut
        const typesCtx = document.getElementById('attackTypesChart');
        if (typesCtx) {
            this.charts.types = new Chart(typesCtx, {
                type: 'doughnut',
                data: {
                    labels: ['WiFi', 'BLE', 'RF', 'NFC', 'IR'],
                    datasets: [{
                        data: [35, 25, 20, 12, 8],
                        backgroundColor: ['#00f3ff', '#ff00ff', '#ffff00', '#00ff41', '#ff003c'],
                        borderWidth: 0
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { position: 'bottom', labels: { color: '#888', padding: 10 } } },
                    cutout: '70%'
                }
            });
        }

        // Resource Gauge (CPU/RAM)
        const gaugeCtx = document.getElementById('resourceGaugeChart');
        if (gaugeCtx) {
            this.charts.gauge = new Chart(gaugeCtx, {
                type: 'bar',
                data: {
                    labels: ['CPU', 'RAM', 'PSRAM'],
                    datasets: [{
                        data: [45, 62, 30],
                        backgroundColor: ['#00f3ff', '#ff00ff', '#00ff41'],
                        borderRadius: 4
                    }]
                },
                options: {
                    indexAxis: 'y',
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: { legend: { display: false } },
                    scales: {
                        x: { max: 100, grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: '#888' } },
                        y: { grid: { display: false }, ticks: { color: '#00f3ff' } }
                    }
                }
            });
        }
    },

    updateChart(chartName, newData, newLabel) {
        const chart = this.charts[chartName];
        if (!chart) return;

        chart.data.labels.push(newLabel);
        chart.data.datasets[0].data.push(newData);

        if (chart.data.labels.length > this.maxHistoryPoints) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }
        chart.update('none');
    },

    updateGauge(cpu, ram, psram) {
        if (this.charts.gauge) {
            this.charts.gauge.data.datasets[0].data = [cpu, ram, psram];
            this.charts.gauge.update('none');
        }
    },

    updateAttackTypes(wifi, ble, rf, nfc, ir) {
        if (this.charts.types) {
            this.charts.types.data.datasets[0].data = [wifi, ble, rf, nfc, ir];
            this.charts.types.update('none');
        }
    },

    startRealtimeUpdates() {
        setInterval(() => {
            if (typeof app !== 'undefined' && app.isPreview) {
                // Simulate data in preview mode
                const now = new Date().toLocaleTimeString();
                this.updateChart('timeline', Math.floor(Math.random() * 50), now);
                this.updateGauge(
                    30 + Math.random() * 40,
                    40 + Math.random() * 30,
                    20 + Math.random() * 30
                );
            }
        }, 3000);
    },

    // Animated counter for stat cards
    animateCounter(elementId, targetValue, duration = 1000) {
        const el = document.getElementById(elementId);
        if (!el) return;
        
        const start = parseInt(el.textContent) || 0;
        const increment = (targetValue - start) / (duration / 16);
        let current = start;
        
        const timer = setInterval(() => {
            current += increment;
            if ((increment > 0 && current >= targetValue) || (increment < 0 && current <= targetValue)) {
                el.textContent = targetValue;
                clearInterval(timer);
            } else {
                el.textContent = Math.floor(current);
            }
        }, 16);
    },

    exportStats() {
        const data = {
            timestamp: new Date().toISOString(),
            history: this.statsHistory,
            charts: {
                attackTypes: this.charts.types?.data.datasets[0].data || [],
                resources: this.charts.gauge?.data.datasets[0].data || []
            }
        };
        
        const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = `lele_stats_${Date.now()}.json`;
        a.click();
        URL.revokeObjectURL(url);
    }
};

// Auto-init when DOM ready
document.addEventListener('DOMContentLoaded', () => {
    if (typeof Chart !== 'undefined') {
        Chart.defaults.color = '#888';
        Chart.defaults.font.family = "'Share Tech Mono', monospace";
        DashboardStats.init();
    }
});

// Export
window.DashboardStats = DashboardStats;
