#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stddef.h>

class Camera {
private:
  uint32_t _lastCapture;
  SemaphoreHandle_t _lock;
  uint8_t *_img;
  size_t _imgLen;
  esp_err_t internalCaptureJpeg();

public:
  Camera();
  ~Camera();

  esp_err_t begin();
  esp_err_t captureJpeg(uint8_t **out, size_t *out_len);
  void loop();
};

#endif