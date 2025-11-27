#include "ui_screen.hpp"

LGFX Display;

static int32_t touch_x = 0;
static int32_t touch_y = 0;

// Basit layout ölçüleri
static const int16_t STATUS_BAR_HEIGHT = 24;
static const int16_t INPUT_BAR_HEIGHT  = 40;

void ui_init() {
  Display.init();

  // Landscape
  if (Display.width() < Display.height()) {
    Display.setRotation(Display.getRotation() ^ 1);
  }

  Display.setBrightness(255);
  Display.fillScreen(TFT_BLACK);

  Display.setTextDatum(textdatum_t::middle_center);
  Display.setTextSize(2);
  Display.setTextColor(TFT_WHITE, TFT_BLACK);
}

void ui_draw_boot() {
  Display.fillScreen(TFT_BLACK);

  int16_t cx = Display.width()  / 2;
  int16_t cy = Display.height() / 2;

  Display.drawString("DeskGPT", cx, cy - 20);
  Display.setTextSize(1);
  Display.drawString("Booting...", cx, cy + 10);
}

// Ana layout: üstte status bar, ortada chat alanı, altta input alanı
void ui_draw_main_layout(const String& statusLine) {
  int16_t w = Display.width();
  int16_t h = Display.height();

  // Arka plan
  Display.fillScreen(TFT_BLACK);

  // ÜST STATUS BAR
  Display.fillRect(0, 0, w, STATUS_BAR_HEIGHT, TFT_DARKGREY);
  Display.setTextDatum(textdatum_t::middle_left);
  Display.setTextColor(TFT_WHITE, TFT_DARKGREY);
  Display.setTextSize(1);

  Display.drawString("DeskGPT", 4, STATUS_BAR_HEIGHT / 2);

  // Sağ tarafta WiFi/IP bilgisi
  Display.setTextDatum(textdatum_t::middle_right);
  Display.drawString(statusLine, w - 4, STATUS_BAR_HEIGHT / 2);

  // ORTA CHAT ALANI
  int16_t chat_y = STATUS_BAR_HEIGHT;
  int16_t chat_h = h - STATUS_BAR_HEIGHT - INPUT_BAR_HEIGHT;
  Display.fillRect(0, chat_y, w, chat_h, TFT_BLACK);

  // Placeholder bir balon çizelim
  Display.setTextDatum(textdatum_t::top_left);
  Display.setTextColor(TFT_WHITE, TFT_BLACK);
  Display.setTextSize(1);
  Display.drawString(">> Merhaba dostum, ben DeskGPT :)", 6, chat_y + 6);

  // ALTA INPUT BAR
  int16_t input_y = h - INPUT_BAR_HEIGHT;
  Display.fillRect(0, input_y, w, INPUT_BAR_HEIGHT, TFT_DARKGREY);
  Display.setTextDatum(textdatum_t::middle_left);
  Display.setTextColor(TFT_WHITE, TFT_DARKGREY);
  Display.drawString(" [input]  (ileride klavye vs.)", 4, input_y + INPUT_BAR_HEIGHT / 2);

  // Touch testi için text ayarını tekrar resetleyelim
  Display.setTextDatum(textdatum_t::top_left);
  Display.setTextColor(TFT_WHITE, TFT_BLACK);
  Display.setTextSize(1);
}

// Şimdilik touch: her yerde çizim yapsın, sonra chat/input'a göre kısıtlarız
void ui_handle_touch() {
  if (Display.getTouch(&touch_x, &touch_y)) {
    Display.fillRect(touch_x - 2, touch_y - 2, 5, 5, TFT_GREEN);
  }
}