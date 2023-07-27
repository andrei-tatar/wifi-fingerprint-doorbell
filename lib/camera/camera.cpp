#include "camera.h"
#include "esp_camera.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include <Arduino.h>
#include <string.h>

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM 15
#define XCLK_GPIO_NUM 27
#define SIOD_GPIO_NUM 25
#define SIOC_GPIO_NUM 23

#define Y9_GPIO_NUM 19
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 39
#define Y5_GPIO_NUM 5
#define Y4_GPIO_NUM 34
#define Y3_GPIO_NUM 35
#define Y2_GPIO_NUM 32
#define VSYNC_GPIO_NUM 22
#define HREF_GPIO_NUM 26
#define PCLK_GPIO_NUM 21

Camera::Camera() { _lock = xSemaphoreCreateMutex(); }

Camera::~Camera() { vSemaphoreDelete(_lock); }

esp_err_t Camera::begin() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // Camera init
  return esp_camera_init(&config);
}

esp_err_t Camera::internalCaptureJpeg() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    return ESP_FAIL;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    bool jpeg_converted = frame2jpg(fb, 80, &_img, &_imgLen);
    esp_camera_fb_return(fb);
    esp_camera_deinit();
    return jpeg_converted ? ESP_OK : ESP_FAIL;
  }

  _imgLen = fb->len;
  _img =
      (uint8_t *)heap_caps_malloc(fb->len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!*_img) {
    return ESP_FAIL;
  }
  memcpy(_img, fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return ESP_OK;
}

esp_err_t Camera::captureJpeg(uint8_t **out, size_t *out_len) {
  esp_err_t ret = ESP_FAIL;
  if (xSemaphoreTake(_lock, portMAX_DELAY)) {
    if (_img) {
      *out_len = _imgLen;
      *out = (uint8_t *)heap_caps_malloc(_imgLen,
                                         MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
      memcpy(*out, _img, _imgLen);
      ret = ESP_OK;
    }

    xSemaphoreGive(_lock);
  }
  return ret;
}

void Camera::loop() {
  uint32_t now = millis();
  if (now + 500 > _lastCapture) {
    if (xSemaphoreTake(_lock, portMAX_DELAY)) {
      if (_img) {
        free(_img);
        _img = NULL;
      }
      internalCaptureJpeg();
      _lastCapture = now;
      xSemaphoreGive(_lock);
    }
  }
}
