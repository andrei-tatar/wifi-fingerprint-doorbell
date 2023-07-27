#include "Arduino.h"
#include "camera.h"
#include "io.h"
#include "web.h"
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>

Camera camera;
Web web;
Io io;

// Replace with your network credentials
const char *ssid = "SSID";
const char *password = "PASS";

void setup() {
  setCpuFrequencyMhz(80);

  io.begin();
  camera.begin();
  WiFi.begin(ssid, password);

  web.onCaptureImage([](uint8_t **img, size_t *imgLen) {
       return camera.captureJpeg(img, imgLen);
     })
      .begin();

  MDNS.begin("wifi-doorbell");
  MDNS.enableArduino();
  MDNS.addService("http", "tcp", 80);
  ArduinoOTA.setMdnsEnabled(false).begin();

  io.ledConnecting();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  io.ledConnected();
}

void loop() {
  io.loop();
  camera.loop();
  ArduinoOTA.handle();
}