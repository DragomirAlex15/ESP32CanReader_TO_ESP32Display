#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "driver/twai.h"   // driver CAN (TWAI)

uint8_t receiverMAC[] = { 0xA8, 0x42, 0xE3, 0xA8, 0xBA, 0x00 };

typedef struct {
    int incarcare;
    int temp_apa;
    int temp_ulei;
    int pres_ulei;
    int lambda;
    int intake;
    int rpm;
    int maf;
    int map;
} SensorData;

SensorData tx;

/******************** CAN INIT ********************/
void initCAN() {
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  twai_driver_install(&g_config, &t_config, &f_config);
  twai_start();
}

/******************** ESP-NOW INIT ********************/
void initESPNOW() {
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receiverMAC, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

/******************** SETUP ********************/
void setup() {
  Serial.begin(115200);

  initCAN();
  initESPNOW();

  Serial.println("Sender CAN → ESP-NOW pornit.");
}

/******************** LOOP ********************/
void loop() {

  twai_message_t msg;

  if (twai_receive(&msg, pdMS_TO_TICKS(10)) == ESP_OK) {

    // Exemplu: extragi valori în funcție de ID-ul CAN
    switch (msg.identifier) {

      case 0x280://rpm
        tx.rpm = ((msg.data[3] << 8) | msg.data[4]) / 4;
        break;


      case 0x288://coolant
        tx.temp_apa = msg.data[0] - 48;
        break;


      case 0x3E0://ulei temp
        tx.temp_ulei = msg.data[2] - 48;
        break;


      case 0x0CFF3456:   // OIL PRESSURE
        tx.pres_ulei = msg.data[0];
        break;

      case 0x3E8:   // MAF
        tx.intake = msg.data[1] - 48;
        tx.lambda = ((msg.data[2] << 8) | msg.data[3]) / 128.0;
        tx.map = (msg.data[4] << 8) | msg.data[5]; 
        tx.maf = ((msg.data[6] << 8) | msg.data[7]) / 100.0;
        break;

      //case 0x3E8:   // MAP
      //  tx.map = (msg.data[4] << 8) | msg.data[5];
      //break;

      //case 0x3E8:   // LAMBDA
      //tx.lambda = ((msg.data[2] << 8) | msg.data[3]) / 128.0;
      //break;

      //case 0x3E8:   // INTAKE TEMP
      //tx.intake = msg.data[1] - 48;
      //break;

      //case 0x0CFF3333:   // BATTERY %
      // tx.incarcare = msg.data[0];
      //break;
    }

    // Trimite structura completă
    esp_now_send(receiverMAC, (uint8_t*)&tx, sizeof(tx));
  }
}
