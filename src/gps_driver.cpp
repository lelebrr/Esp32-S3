/**
 * @file gps_driver.cpp
 * @brief GPS Driver Implementation for GY-NEO6MV2
 */

#include "gps_driver.h"
#include "core/aggressive_sd.h"

// Static member initialization
TinyGPSPlus GPSDriver::_gps;
HardwareSerial GPSDriver::_serial(GPS_UART_NUM);
bool GPSDriver::_initialized = false;
uint32_t GPSDriver::_lastUpdate = 0;
uint32_t GPSDriver::_charsProcessed = 0;

bool GPSDriver::init() {
    Serial.println("[GPS] Initializing GY-NEO6MV2...");

    // Initialize UART2 with GPS pins
    _serial.begin(GPS_BAUD_RATE, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);

    _initialized = true;
    _lastUpdate = millis();

    Serial.printf("[GPS] UART2 configured: RX=%d, TX=%d @ %d baud\n", PIN_GPS_RX, PIN_GPS_TX, GPS_BAUD_RATE);

    return true;
}

void GPSDriver::update() {
    if (!_initialized) return;

    // Read all available characters from GPS
    while (_serial.available() > 0) {
        char c = _serial.read();
        _gps.encode(c);
        _charsProcessed++;
    }

    _lastUpdate = millis();
}

bool GPSDriver::isAvailable() {
    // Consider available if we've received any data recently
    return _initialized && (millis() - _lastUpdate < 5000) && _charsProcessed > 0;
}

bool GPSDriver::isValid() { return _gps.location.isValid() && _gps.location.age() < 3000; }

double GPSDriver::getLatitude() { return _gps.location.lat(); }

double GPSDriver::getLongitude() { return _gps.location.lng(); }

double GPSDriver::getAltitude() { return _gps.altitude.meters(); }

double GPSDriver::getSpeedKmh() { return _gps.speed.kmph(); }

double GPSDriver::getCourse() { return _gps.course.deg(); }

int GPSDriver::getSatellites() { return _gps.satellites.value(); }

double GPSDriver::getHDOP() { return _gps.hdop.hdop(); }

String GPSDriver::getCoordsString() {
    if (!isValid()) return "N/A";

    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f,%.6f", getLatitude(), getLongitude());
    return String(buf);
}

String GPSDriver::getDateString() {
    if (!_gps.date.isValid()) return "N/A";

    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", _gps.date.year(), _gps.date.month(), _gps.date.day());
    return String(buf);
}

String GPSDriver::getTimeString() {
    if (!_gps.time.isValid()) return "N/A";

    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", _gps.time.hour(), _gps.time.minute(), _gps.time.second());
    return String(buf);
}

double GPSDriver::distanceTo(double lat, double lon) {
    return TinyGPSPlus::distanceBetween(getLatitude(), getLongitude(), lat, lon);
}

double GPSDriver::bearingTo(double lat, double lon) {
    return TinyGPSPlus::courseTo(getLatitude(), getLongitude(), lat, lon);
}

bool GPSDriver::logPosition(const char *filename, const char *ssid, const char *bssid, int rssi) {
    if (!isValid()) return false;
    if (!AggressiveSD::isReady()) return false;

    // Format: timestamp,lat,lon,alt,sats,hdop,ssid,bssid,rssi
    char logLine[256];

    if (ssid != nullptr) {
        // Wardriving format (Kismet-compatible CSV)
        snprintf(
            logLine,
            sizeof(logLine),
            "%s %s,%.6f,%.6f,%.1f,%d,%.1f,%s,%s,%d\n",
            getDateString().c_str(),
            getTimeString().c_str(),
            getLatitude(),
            getLongitude(),
            getAltitude(),
            getSatellites(),
            getHDOP(),
            ssid,
            bssid ? bssid : "",
            rssi
        );
    } else {
        // Simple position log
        snprintf(
            logLine,
            sizeof(logLine),
            "%s %s,%.6f,%.6f,%.1f,%d,%.1f\n",
            getDateString().c_str(),
            getTimeString().c_str(),
            getLatitude(),
            getLongitude(),
            getAltitude(),
            getSatellites(),
            getHDOP()
        );
    }

    return AggressiveSD::appendFile(filename, logLine);
}

bool GPSDriver::exportToKML(const char *inputFile, const char *outputFile) {
    if (!AggressiveSD::isReady()) return false;

    // KML header
    String kml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    kml += "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
    kml += "<Document>\n";
    kml += "  <name>MorphNode Wardriving Log</name>\n";
    kml += "  <Style id=\"network\">\n";
    kml += "    <IconStyle><color>ff0000ff</color></IconStyle>\n";
    kml += "  </Style>\n";

    // Read input file and convert to placemarks
    String content = AggressiveSD::readFile(inputFile);

    // Parse lines and create placemarks
    int lineStart = 0;
    int placemarkCount = 0;

    while (lineStart < (int)content.length()) {
        int lineEnd = content.indexOf('\n', lineStart);
        if (lineEnd < 0) lineEnd = content.length();

        String line = content.substring(lineStart, lineEnd);
        lineStart = lineEnd + 1;

        // Parse CSV: date time,lat,lon,alt,sats,hdop[,ssid,bssid,rssi]
        int commaPos = line.indexOf(',');
        if (commaPos < 0) continue;

        String timestamp = line.substring(0, commaPos);
        String rest = line.substring(commaPos + 1);

        // Extract lat,lon
        commaPos = rest.indexOf(',');
        if (commaPos < 0) continue;
        String lat = rest.substring(0, commaPos);
        rest = rest.substring(commaPos + 1);

        commaPos = rest.indexOf(',');
        if (commaPos < 0) continue;
        String lon = rest.substring(0, commaPos);

        // Create placemark
        kml += "  <Placemark>\n";
        kml += "    <name>" + timestamp + "</name>\n";
        kml += "    <Point><coordinates>" + lon + "," + lat + ",0</coordinates></Point>\n";
        kml += "  </Placemark>\n";

        placemarkCount++;
    }

    kml += "</Document>\n</kml>";

    bool result = AggressiveSD::writeFile(outputFile, kml.c_str());

    if (result) { Serial.printf("[GPS] Exported %d points to KML\n", placemarkCount); }

    return result;
}

TinyGPSPlus &GPSDriver::getGPS() { return _gps; }
