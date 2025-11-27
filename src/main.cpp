#include <Arduino.h>
#include <WiFi.h>

// ----------- WiFi & OpenAI config -----------
#include "ui/ui_screen.hpp"
#include "net/net_openai.hpp"
#include "config_local.hpp"  

// ----------- Yardımcı fonksiyonlar -----------

static void connect_wifi_blocking()
{
  Serial.print("[WiFi] Connecting to ");
  Serial.println(CFG_WIFI_SSID);

  WiFi.disconnect(true, true);
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.begin(CFG_WIFI_SSID, CFG_WIFI_PASS);

  uint32_t start      = millis();
  wl_status_t lastSta = WL_IDLE_STATUS;

  while (true) {
    wl_status_t s = WiFi.status();
    if (s != lastSta) {
      Serial.print("[WiFi] Status changed: ");
      Serial.println((int)s);
      lastSta = s;
    }

    if (s == WL_CONNECTED) {
      Serial.println();
      Serial.println("[WiFi] CONNECTED!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      break;
    }

    if (millis() - start > 20000) {  // 20 sn timeout
      Serial.println();
      Serial.println("[WiFi] TIMEOUT, could not connect.");
      break;
    }

    delay(500);
    Serial.print(".");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("[DeskGPT] Booting...");

  // Ekran & temel UI
  ui_init();
  ui_draw_boot();

  // WiFi bağlantısı
  connect_wifi_blocking();

  // Status bar için metin
  String statusLine;
  if (WiFi.status() == WL_CONNECTED) {
    statusLine = "WiFi OK  " + WiFi.localIP().toString();
  } else {
    statusLine = "WiFi FAIL";
  }

  // Ana layout: üstte status bar, ortada chat alanı, altta input bar
  ui_draw_main_layout(statusLine);

  Serial.println("[DeskGPT] UI ready.");

  // --- OpenAI test (şimdilik sadece Serial) ---
  if (WiFi.status() == WL_CONNECTED && strlen(CFG_OPENAI_API_KEY) > 0) {
    String reply;
    bool ok = net::openai_chat(CFG_OPENAI_API_KEY,
                               "Kanka bu bir test mesajidir, bana kisaca cevap ver.",
                               reply);
    if (ok) {
      Serial.println("[DeskGPT] Assistant reply:");
      Serial.println(reply);
    } else {
      Serial.println("[DeskGPT] OpenAI chat call FAILED.");
    }
  } else {
    Serial.println("[DeskGPT] Skipping OpenAI test (no WiFi or API key).");
  }
}

void loop()
{
  ui_handle_touch();
}