#include <Arduino.h>
#include "driver/twai.h"   // CAN (TWAI) driver ESP32

/******************** CAN INIT ********************/
void initCAN() {
  // GPIO 5 = TX, GPIO 4 = RX
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("CAN driver instalat.");
  } else {
    Serial.println("Eroare instalare CAN!");
  }

  if (twai_start() == ESP_OK) {
    Serial.println("CAN pornit.");
  } else {
    Serial.println("Eroare pornire CAN!");
  }
}

/******************** SETUP ********************/
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Pornire citire CAN...");
  initCAN();
}

/******************** LOOP ********************/
void loop() {

  twai_message_t msg;

  // Așteaptă mesaj CAN
  if (twai_receive(&msg, pdMS_TO_TICKS(50)) == ESP_OK) {

    Serial.println("----- FRAME CAN -----");

    // ID
    Serial.print("ID: 0x");
    Serial.println(msg.identifier, HEX);

    // DLC
    Serial.print("Lungime: ");
    Serial.println(msg.data_length_code);

    // Date brute HEX
    Serial.print("Data: ");
    for (int i = 0; i < msg.data_length_code; i++) {
      Serial.printf("%02X ", msg.data[i]);
    }
    Serial.println();

    // Dacă vrei și decimal
    Serial.print("Data (dec): ");
    for (int i = 0; i < msg.data_length_code; i++) {
      Serial.print(msg.data[i]);
      Serial.print(" ");
    }
    Serial.println();
    Serial.println("---------------------");
  }
}
