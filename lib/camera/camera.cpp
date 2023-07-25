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

Camera::Camera(/* args */) {}

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

static void *_malloc(size_t size) {
  void *res = malloc(size);
  if (res) {
    return res;
  }

  // check if SPIRAM is enabled and is allocatable
#if (CONFIG_SPIRAM_SUPPORT &&                                                  \
     (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC))
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#endif
  return NULL;
}

esp_err_t Camera::captureJpeg(uint8_t **out, size_t *out_len) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    return ESP_FAIL;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    bool jpeg_converted = frame2jpg(fb, 80, out, out_len);
    esp_camera_fb_return(fb);
    return jpeg_converted ? ESP_OK : ESP_FAIL;
  }

  *out_len = fb->len;
  *out = (uint8_t *)_malloc(fb->len);
  if (!*out) {
    return ESP_FAIL;
  }
  memcpy(*out, fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return ESP_OK;
}

void Camera::loop() {
  uint32_t now = millis();
  if (now > _nextCapture) {
    _nextCapture = now + 500;

    camera_fb_t *fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
  }
}