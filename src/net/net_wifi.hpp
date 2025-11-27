#pragma once
#include <Arduino.h>

namespace net {

  // Belirtilen SSID/PASS ile WiFi'ye bağlanır. true = başarı, false = timeout.
  bool wifi_connect(const char* ssid, const char* password, uint32_t timeout_ms = 15000);

  // Bağlandıysa "192.168.x.x" döner, bağlı değilse boş String.
  String wifi_ip();

} // namespace net