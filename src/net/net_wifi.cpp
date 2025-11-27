#include "net_wifi.hpp"
#include <WiFi.h>

namespace net {

  bool wifi_connect(const char* ssid, const char* password, uint32_t timeout_ms) {
    Serial.println("[WiFi] Connecting...");
    Serial.print("       SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeout_ms) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("[WiFi] Connected. IP: ");
      Serial.println(WiFi.localIP());
      return true;
    } else {
      Serial.println("[WiFi] Connection FAILED (timeout).");
      return false;
    }
  }

  String wifi_ip() {
    if (WiFi.status() == WL_CONNECTED) {
      return WiFi.localIP().toString();
    }
    return String();
  }

} // namespace net