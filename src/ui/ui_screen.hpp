#pragma once

// LovyanGFX V1 API kullanacağız
#ifndef LGFX_USE_V1
  #define LGFX_USE_V1
#endif

#include <Arduino.h>
#include <LovyanGFX.hpp>

// SC01 Plus (WT32-S3-WROVER-N16R2) için panel + touch tanımı
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796   _panel_instance;   // ST7796UI LCD
  lgfx::Bus_Parallel8  _bus_instance;     // 8080 8-bit paralel
  lgfx::Light_PWM      _light_instance;   // Backlight için PWM
  lgfx::Touch_FT5x06   _touch_instance;   // FT6336U kapasitif touch

public:
  LGFX(void) {
    { // --- BUS (8080 8-bit) konfig ---
      auto cfg = _bus_instance.config();

      cfg.freq_write = 40000000;  // 40 MHz yazma hızı

      cfg.pin_wr = 47;   // WR
      cfg.pin_rd = -1;   // RD kullanılmıyor
      cfg.pin_rs = 0;    // D/C (RS)

      // 8-bit data hattı (kart pinout'una göre)
      cfg.pin_d0 = 9;
      cfg.pin_d1 = 46;
      cfg.pin_d2 = 3;
      cfg.pin_d3 = 8;
      cfg.pin_d4 = 18;
      cfg.pin_d5 = 17;
      cfg.pin_d6 = 16;
      cfg.pin_d7 = 15;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    { // --- PANEL konfig ---
      auto cfg = _panel_instance.config();

      cfg.pin_cs   = -1;          // CS sabit bağlı
      cfg.pin_rst  = 4;           // RESET pini
      cfg.pin_busy = -1;          // Busy yok

      cfg.panel_width  = 320;
      cfg.panel_height = 480;

      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;

      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;

      cfg.readable   = false;     // LCD'den geri okuma kullanmıyoruz
      cfg.invert     = true;      // Renk invert flag (bu panelde genelde true)
      cfg.rgb_order  = false;     // RGB sırası normal
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;     // Bus paylaşımı yok

      _panel_instance.config(cfg);
    }

    { // --- BACKLIGHT (PWM) ---
      auto cfg = _light_instance.config();

      cfg.pin_bl      = 45;   // Backlight pini
      cfg.invert      = false;
      cfg.freq        = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    { // --- TOUCH FT6336U (FT5x06 driver) ---
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;
      cfg.x_max = 319;
      cfg.y_min = 0;
      cfg.y_max = 479;

      cfg.pin_int       = 7;     // Touch interrupt pini (INT)
      cfg.bus_shared    = true;  // I2C bus paylaşımlı
      cfg.offset_rotation = 0;

      cfg.i2c_port = 1;
      cfg.i2c_addr = 0x38;       // FT6336U default adres
      cfg.pin_sda  = 6;
      cfg.pin_scl  = 5;
      cfg.freq     = 400000;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

// Global display objesi
extern LGFX Display;

// Basit UI fonksiyonları
void ui_init();
void ui_draw_boot();
void ui_handle_touch();

// Global display objesi
extern LGFX Display;

// Basit UI fonksiyonları
void ui_init();
void ui_draw_boot();

// Yeni eklediğimiz fonksiyonlar:
void ui_draw_main_layout(const String& statusLine);
void ui_handle_touch();