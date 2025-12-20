/**
 * @file web_dashboard.h
 * @brief Web Interface and WebSocket Controller
 * 
 * Manages the asynchronous web server, WebSocket communication for
 * the dashboard, and JSON command handling.
 * 
 * @author Monster S3 Team
 * @date 2025
 */

#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

// Initialize web dashboard (starts hidden AP)
void setup_web_dashboard();

// Main loop for WebSocket processing
void web_dashboard_loop();

// Check if web server is running
bool web_is_running();

// Add log entry
void web_log(const char* msg);

#endif // WEB_DASHBOARD_H

