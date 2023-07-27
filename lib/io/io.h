#ifndef _IO_H_
#define _IO_H_

#include "DFRobot_ID809_I2C.h"

class Io {
private:
  DFRobot_ID809_I2C fingerprint;
  uint8_t lastFingerState;

public:
  void begin();

  void ledConnecting() {
    fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eBreathing,
                        DFRobot_ID809::eLEDColor_t::eLEDMagenta, 0);
  }

  void ledConnected() {
    fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eFastBlink,
                        DFRobot_ID809::eLEDColor_t::eLEDGreen, 3);
  }

  void loop();
};

#endif