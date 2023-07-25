/*********
  Rui Santos
  Complete project details at
https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/

  IMPORTANT!!!
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button
to put your board in flashing mode

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "Arduino.h"
#include "DFRobot_ID809_I2C.h"
#include "camera.h"
#include "esp_http_server.h"
#include <WiFi.h>

DFRobot_ID809_I2C fingerprint;
Camera camera;

// Replace with your network credentials
const char *ssid = "SSID";
const char *password = "PASS";

httpd_handle_t stream_httpd = NULL;

static esp_err_t stream_handler(httpd_req_t *req) {
  esp_err_t res = httpd_resp_set_type(req, "image/jpeg");
  if (res != ESP_OK) {
    return res;
  }

  size_t imgLen = 0;
  uint8_t *img = NULL;
  if (camera.captureJpeg(&img, &imgLen) == ESP_OK) {
    res = httpd_resp_send(req, (char *)img, imgLen);
    free(img);
  } else {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  return res;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {.uri = "/",
                           .method = HTTP_GET,
                           .handler = stream_handler,
                           .user_ctx = NULL};

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}

void setup() {
  Serial.begin(115200);
  fingerprint.begin();
  camera.begin();

  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eBreathing,
                        DFRobot_ID809::eLEDColor_t::eLEDMagenta, 1);
    delay(1000);
  }

  fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eFastBlink,
                      DFRobot_ID809::eLEDColor_t::eLEDGreen, 3);

  startCameraServer();
}

void loop() {
  camera.loop();

  static uint8_t lastFingerState = 0;
  uint8_t fingerState = fingerprint.detectFinger();
  if (fingerState != lastFingerState) {
    Serial.printf("finger detected: %d\n", fingerState);
    fingerprint.ctrlLED(fingerState ? DFRobot_ID809::eLEDMode_t::eFadeIn
                                    : DFRobot_ID809::eLEDMode_t::eFadeOut,
                        DFRobot_ID809::eLEDColor_t::eLEDYellow, 0);
    lastFingerState = fingerState;
  }

  delay(100);
}