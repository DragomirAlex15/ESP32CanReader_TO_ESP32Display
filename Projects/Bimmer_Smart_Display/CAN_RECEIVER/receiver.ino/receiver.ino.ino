/******************** INCLUDES ********************/
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <lvgl.h>
#include <LovyanGFX.hpp>
#include <lv_conf.h>

/******************** ESP-NOW DATA ********************/
typedef struct {
  int apa;
  int ulei_p;
  int ulei_t;
  int incarcare;
} SensorData;

volatile bool dataNoua = false;
SensorData rxData;

/******************** DISPLAY ********************/
#define TFT_W 320
#define TFT_H 240

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel;
  lgfx::Bus_Parallel8 _bus;

public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.freq_write = 25000000;
      cfg.pin_wr = 4;
      cfg.pin_rd = 2;
      cfg.pin_rs = 16;
      cfg.pin_d0 = 15;
      cfg.pin_d1 = 13;
      cfg.pin_d2 = 12;
      cfg.pin_d3 = 14;
      cfg.pin_d4 = 27;
      cfg.pin_d5 = 25;
      cfg.pin_d6 = 33;
      cfg.pin_d7 = 32;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    {
      auto cfg = _panel.config();
      cfg.pin_cs = 17;
      cfg.pin_rst = -1;
      cfg.panel_width = 240;
      cfg.panel_height = 320;

      cfg.rgb_order = true;



      _panel.config(cfg);
    }
    setPanel(&_panel);
  }
};

static LGFX tft;

/******************** LVGL ********************/
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[TFT_W * 40];
static lv_color_t buf2[TFT_W * 40];

static void my_disp_flush(lv_disp_drv_t *disp,
                          const lv_area_t *area,
                          lv_color_t *color_p) {

  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

/******************** UI OBJECTS ********************/
lv_obj_t *lblApa, *lblUleiT, *lblUleiP, *lblInc;

/******************** GLOBAL STYLES (FIX CRITICAL) ********************/
lv_style_t style_title;
lv_style_t style_value;

/******************** ESP-NOW CALLBACK ********************/
void OnDataRecv(const esp_now_recv_info *info,
                const uint8_t *incomingData,
                int len) {

  if (len == sizeof(SensorData)) {
    memcpy(&rxData, incomingData, sizeof(SensorData));
    dataNoua = true;
  }
}

/******************** UI CREATE ********************/
void create_dashboard() {

  lv_obj_t* scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);   // fundal negru pur
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  // --- PORTOCALIU OEM BMW ---
  lv_color_t orange_title = lv_color_hex(0xFF0000);   // titluri
  lv_color_t orange_value = lv_color_hex(0xFF0000);   // valori

  // --- STYLE TITLU ---
  lv_style_init(&style_title);
  lv_style_set_text_color(&style_title, orange_title);
  lv_style_set_text_font(&style_title, &lv_font_montserrat_16);
  lv_style_set_text_letter_space(&style_value, 1); 
  lv_style_set_bg_opa(&style_value, LV_OPA_TRANSP); 
  lv_style_set_border_width(&style_value, 0);
  

  // --- STYLE VALOARE ---
  lv_style_init(&style_value);
  lv_style_set_text_color(&style_value, orange_value);
  lv_style_set_text_font(&style_value, &lv_font_montserrat_26);
  lv_style_set_text_letter_space(&style_value, 1); 
  lv_style_set_bg_opa(&style_value, LV_OPA_TRANSP); 
  lv_style_set_border_width(&style_value, 0);

  // --- GRID EXACT CA ÎN DISPLAY ---
  int col1 = 10; 
  int col2 = 115;
  int col3 = 220;
  int row1 = 10;
  int row2 = 100;
  int row3 = 180;

  // ------------------ ROW 1 ------------------

  lv_obj_t* t1 = lv_label_create(scr);
  lv_label_set_text(t1, "BATTERY");
  lv_obj_add_style(t1, &style_title, 0);
  lv_obj_set_pos(t1, col1, row1);

  lv_obj_t* t2 = lv_label_create(scr);
  lv_label_set_text(t2, "COOLANT");
  lv_obj_add_style(t2, &style_title, 0);
  lv_obj_set_pos(t2, col2, row1);

  lv_obj_t* t3 = lv_label_create(scr);
  lv_label_set_text(t3, "RADIATO");
  lv_obj_add_style(t3, &style_title, 0);
  lv_obj_set_pos(t3, col3, row1);

  // ------------------ ROW 2 ------------------

  lv_obj_t* t4 = lv_label_create(scr);
  lv_label_set_text(t4, "LAMBDA");
  lv_obj_add_style(t4, &style_title, 0);
  lv_obj_set_pos(t4, col1, row2);

  lv_obj_t* t5 = lv_label_create(scr);
  lv_label_set_text(t5, "INTAKE");
  lv_obj_add_style(t5, &style_title, 0);
  lv_obj_set_pos(t5, col2, row2);

  lv_obj_t* t6 = lv_label_create(scr);
  lv_label_set_text(t6, "OIL TEMP");
  lv_obj_add_style(t6, &style_title, 0);
  lv_obj_set_pos(t6, col3, row2);

  // ------------------ ROW 3 ------------------

  lv_obj_t* t7 = lv_label_create(scr);
  lv_label_set_text(t7, "RON ROA");
  lv_obj_add_style(t7, &style_title, 0);
  lv_obj_set_pos(t7, col1, row3);

  lv_obj_t* t8 = lv_label_create(scr);
  lv_label_set_text(t8, "TPS");
  lv_obj_add_style(t8, &style_title, 0);
  lv_obj_set_pos(t8, col2, row3);

  lv_obj_t* t9 = lv_label_create(scr);
  lv_label_set_text(t9, "ENGINE L");
  lv_obj_add_style(t9, &style_title, 0);
  lv_obj_set_pos(t9, col3, row3);

// ------------------ VALORI ------------------

// ROW 1
lv_obj_t* valBattery = lv_label_create(scr);
lv_label_set_text(valBattery, "--.--");
lv_obj_add_style(valBattery, &style_value, 0);
lv_obj_set_pos(valBattery, col1, row1 + 25);

lblApa = lv_label_create(scr);  // COOLANT
lv_label_set_text(lblApa, "--.--");
lv_obj_add_style(lblApa, &style_value, 0);
lv_obj_set_pos(lblApa, col2, row1 + 25);

lblUleiP = lv_label_create(scr); // RADIATOR
lv_label_set_text(lblUleiP, "--.--");
lv_obj_add_style(lblUleiP, &style_value, 0);
lv_obj_set_pos(lblUleiP, col3, row1 + 25);

// ROW 2
lv_obj_t* valLambda = lv_label_create(scr);
lv_label_set_text(valLambda, "--.--");
lv_obj_add_style(valLambda, &style_value, 0);
lv_obj_set_pos(valLambda, col1, row2 + 25);

lv_obj_t* valIntake = lv_label_create(scr);
lv_label_set_text(valIntake, "--.--");
lv_obj_add_style(valIntake, &style_value, 0);
lv_obj_set_pos(valIntake, col2, row2 + 25);

lblUleiT = lv_label_create(scr); // OIL TEMP
lv_label_set_text(lblUleiT, "--.--");
lv_obj_add_style(lblUleiT, &style_value, 0);
lv_obj_set_pos(lblUleiT, col3, row2 + 25);

// ROW 3
lv_obj_t* valRon = lv_label_create(scr);
lv_label_set_text(valRon, "--.--");
lv_obj_add_style(valRon, &style_value, 0);
lv_obj_set_pos(valRon, col1, row3 + 25);

lv_obj_t* valTps = lv_label_create(scr);
lv_label_set_text(valTps, "--.--");
lv_obj_add_style(valTps, &style_value, 0);
lv_obj_set_pos(valTps, col2, row3 + 25);

lv_obj_t* valEngineL = lv_label_create(scr);
lv_label_set_text(valEngineL, "--.--");
lv_obj_add_style(valEngineL, &style_value, 0);
lv_obj_set_pos(valEngineL, col3, row3 + 25);

}



/******************** SETUP ********************/
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);

  lv_init();
  tft.init();
  tft.setRotation(1);
  

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_W * 40);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_W;
  disp_drv.ver_res = TFT_H;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  

  create_dashboard();
}

/******************** LOOP ********************/
void loop() {

  if (dataNoua) {
    dataNoua = false;

    char buf[16];

    sprintf(buf, "%d C", rxData.apa);
    lv_label_set_text(lblApa, buf);

    sprintf(buf, "%d C", rxData.ulei_t);
    lv_label_set_text(lblUleiT, buf);

    sprintf(buf, "%d bar", rxData.ulei_p);
    lv_label_set_text(lblUleiP, buf);

    sprintf(buf, "%d %%", rxData.incarcare);
    lv_label_set_text(lblInc, buf);
  }

  lv_timer_handler();
  delay(5);
}
