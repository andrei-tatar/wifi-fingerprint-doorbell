#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "esp_err.h"
#include <stddef.h>

class Camera {
private:
  uint32_t _nextCapture;

public:
  Camera();

  esp_err_t begin();
  esp_err_t captureJpeg(uint8_t **out, size_t *out_len);
  void loop();
};

#endif