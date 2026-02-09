# BimmerGarageSoftware
Smart software for smarter cars.


ESP32 CAN DASHBOARD WITH LVGL AND ESP-NOW

Real-time automotive dashboard system based on ESP32, capable of reading CAN bus data directly from the OBD2 port and displaying it wirelessly on a TFT display using LVGL.

The system is split into two ESP32 devices:

CAN Sender – reads CAN frames from the vehicle and sends parsed sensor data via ESP-NOW

Display Receiver – receives data wirelessly and displays it using LVGL (bars, labels, real-time updates)

FEATURES

Direct CAN Bus reading (no ELM327 latency)

Uses ESP32 TWAI (CAN) controller

Wireless communication via ESP-NOW

Real-time graphical dashboard using LVGL

Supported parameters:

Coolant temperature

Oil pressure

Oil temperature

Battery / charging level

Fast refresh rate suitable for live driving data

Designed for automotive 12V environments

SYSTEM ARCHITECTURE

ESP32 CAN SENDER --> ESP-NOW --> ESP32 DISPLAY RECEIVER
| |
| |
OBD2 CAN TFT DISPLAY
|
Vehicle CAN Bus

HARDWARE REQUIREMENTS

CAN SENDER:

ESP32 Dev Board (WROOM-32)

CAN Transceiver:

SN65HVD230 (recommended, 3.3V logic)

OBD2 connector

DC-DC Buck Converter (12V to 5V)

Vehicle with CAN bus (OBD2)

DISPLAY RECEIVER:

ESP32 Dev Board

TFT Display (ST7789 / 240x320)

LVGL compatible wiring

OBD2 CAN PINOUT

Pin 6 - CAN High
Pin 14 - CAN Low
Pin 4 - Ground
Pin 16 - +12V Battery

WARNING:
Never connect ESP32 directly to CAN lines without a CAN transceiver.

POWER SUPPLY

Vehicle 12V is converted using a DC-DC buck converter

Output 5V powers the ESP32 via VIN or USB

ESP32 internal logic runs at 3.3V

CAN transceiver powered from 3.3V or 5V depending on model

SOFTWARE STACK

Arduino Framework

ESP-NOW

ESP32 TWAI (CAN driver)

LVGL graphics library

LovyanGFX display driver

TESTING WITHOUT A VEHICLE

The system can be tested without a car by sending simulated sensor data via Serial Monitor.

Sender serial input format:
coolant,oil_pressure,oil_temperature,battery

Example:
90,4,105,80

DISPLAY USER INTERFACE

Horizontal bar indicators

Real-time value updates

Custom ranges per sensor

Optimized for in-car visibility

FUTURE IMPROVEMENTS

Vehicle-specific CAN ID decoding (BMW, Mercedes, VAG)

Touch screen support

Multiple dashboard pages

RPM, speed, boost pressure

SD card data logging

Over-the-air firmware updates

Persistent settings storage
