#include "ui_screen.hpp"

LGFX Display;

static int32_t touch_x = 0;
static int32_t touch_y = 0;

// Basit layout ölçüleri
static const int16_t STATUS_BAR_HEIGHT = 24;
static const int16_t INPUT_BAR_HEIGHT  = 40;

// UTF-8 Türkçe karakterleri ASCII'ye indirger (kare yerine okunabilir metin).
static String sanitize_for_display(const String& in) {
  String out;
  const char* p = in.c_str();

  while (*p) {
    uint8_t c = (uint8_t)*p;

    if (c < 0x80) {
      // Düz ASCII karakter
      out += (char)c;
      p++;
    } else {
      // UTF-8 2 byte dizileri (Türkçe karakterler)
      uint8_t c2 = (uint8_t)p[1];

      // ı (U+0131)  -> i
      if (c == 0xC4 && c2 == 0xB1) { out += 'i'; }
      // İ (U+0130)  -> I
      else if (c == 0xC4 && c2 == 0xB0) { out += 'I'; }
      // ğ (U+011F)  -> g
      else if (c == 0xC4 && c2 == 0x9F) { out += 'g'; }
      // Ğ (U+011E)  -> G
      else if (c == 0xC4 && c2 == 0x9E) { out += 'G'; }
      // ş (U+015F)  -> s
      else if (c == 0xC5 && c2 == 0x9F) { out += 's'; }
      // Ş (U+015E)  -> S
      else if (c == 0xC5 && c2 == 0x9E) { out += 'S'; }
      // ö (U+00F6)  -> o
      else if (c == 0xC3 && c2 == 0xB6) { out += 'o'; }
      // Ö (U+00D6)  -> O
      else if (c == 0xC3 && c2 == 0x96) { out += 'O'; }
      // ü (U+00FC)  -> u
      else if (c == 0xC3 && c2 == 0xBC) { out += 'u'; }
      // Ü (U+00DC)  -> U
      else if (c == 0xC3 && c2 == 0x9C) { out += 'U'; }
      // ç (U+00E7)  -> c
      else if (c == 0xC3 && c2 == 0xA7) { out += 'c'; }
      // Ç (U+00C7)  -> C
      else if (c == 0xC3 && c2 == 0x87) { out += 'C'; }
      else {
        // Tanımadığımız UTF-8 karakterlerini şimdilik '?' yap
        out += '?';
      }

      // 2 byte ilerle
      p += 2;
    }
  }

  return out;
}

void ui_init() {
  Display.init();

  // Landscape
  if (Display.width() < Display.height()) {
    Display.setRotation(Display.getRotation() ^ 1);
  }

  Display.setBrightness(255);
  Display.fillScreen(TFT_BLACK);

  // Boot ekranında biraz büyük font
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
  Display.fillRect(0, 0, w, STATUS_BAR_HEIGHT, TFT_BLUE);
  Display.setTextDatum(textdatum_t::middle_left);
  Display.setTextColor(TFT_WHITE, TFT_BLUE);
  Display.setTextSize(1);

  Display.drawString("DeskGPT", 4, STATUS_BAR_HEIGHT / 2);

  // Sağ tarafta WiFi/IP bilgisi
  Display.setTextDatum(textdatum_t::middle_right);
  Display.drawString(statusLine, w - 4, STATUS_BAR_HEIGHT / 2);

  // ORTA CHAT ALANI (şimdilik boş, chat fonksiyonları dolduracak)
  int16_t chat_y = STATUS_BAR_HEIGHT;
  int16_t chat_h = h - STATUS_BAR_HEIGHT - INPUT_BAR_HEIGHT;
  Display.fillRect(0, chat_y, w, chat_h, TFT_BLACK);

  // ALTA INPUT BAR
  int16_t input_y = h - INPUT_BAR_HEIGHT;
  Display.fillRect(0, input_y, w, INPUT_BAR_HEIGHT, TFT_DARKGREY);
  Display.setTextDatum(textdatum_t::middle_left);
  Display.setTextColor(TFT_WHITE, TFT_DARKGREY);
  Display.setTextSize(1);
  Display.drawString(" [input area]  (keyboard coming soon)", 4, input_y + INPUT_BAR_HEIGHT / 2);

  // Chat için default ayarlar
  Display.setTextDatum(textdatum_t::top_left);
  Display.setTextColor(TFT_WHITE, TFT_BLACK);
  Display.setTextSize(2);   // Chat metinleri biraz daha büyük
}

// Chat alanını tamamen temizler
void ui_chat_clear() {
  int16_t w = Display.width();
  int16_t h = Display.height();

  int16_t chat_y = STATUS_BAR_HEIGHT;
  int16_t chat_h = h - STATUS_BAR_HEIGHT - INPUT_BAR_HEIGHT;
  Display.fillRect(0, chat_y, w, chat_h, TFT_BLACK);
}

// Kullanıcı mesajını (mikrofon / Me) çizer: [M] prefix
// Kullanıcı mesajını çizer: ">>" prefix (sen)
void ui_draw_user_message(const String& text) {
  int16_t w = Display.width();
  int16_t h = Display.height();

  int16_t chat_y = STATUS_BAR_HEIGHT;
  int16_t margin = 8;   // biraz daha geniş boşluk

  String clean = sanitize_for_display(text);

  Display.setTextDatum(textdatum_t::top_left);
  Display.setTextColor(TFT_WHITE, TFT_BLACK);  // Kullanıcı için sade beyaz
  Display.setTextSize(2);                      // Büyük font

  int16_t x = margin;
  int16_t y = chat_y + margin;                 // Chat alanının en üstüne yakın

  Display.setCursor(x, y);
  Display.print(">> ");
  Display.print(clean);
}

// Asistan cevabını [D] prefix ile typewriter gibi yazar
// Asistan cevabını "GPT:" prefix ile typewriter gibi yazar (ben)
void ui_draw_assistant_message_stream(const String& text) {
  int16_t w = Display.width();
  int16_t h = Display.height();

  int16_t chat_y = STATUS_BAR_HEIGHT;
  int16_t margin = 8;

  String clean = sanitize_for_display(text);

  Display.setTextDatum(textdatum_t::top_left);
  Display.setTextColor(TFT_GREEN, TFT_BLACK);  // Matrix yeşili vibe
  Display.setTextSize(2);

  // Kullanıcı satırını yukarı çiziyoruz; onun biraz ALTINA yazalım
  // Font size 2 için ~18–20 px satır yüksekliği, biz 32 verelim rahat olsun
  int16_t x = margin;
  int16_t y = chat_y + margin + 32;   // user satırından güvenli mesafe

  Display.setCursor(x, y);

  // "GPT:" prefix
  Display.print("GPT: ");

  // Typewriter effect: karakter karakter yaz
  const char* p = clean.c_str();
  while (*p) {
    Display.print(*p);
    p++;
    delay(25);   // 25 ms per char -> akıcı animasyon
  }
}

// Şimdilik touch: her yerde çizim yapsın, sonra chat/input'a göre kısıtlarız
void ui_handle_touch() {
  if (Display.getTouch(&touch_x, &touch_y)) {
    Display.fillRect(touch_x - 2, touch_y - 2, 5, 5, TFT_GREEN);
  }
}