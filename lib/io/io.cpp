#include "io.h"

#define PIN_IRQ 37

void Io::begin() {
  pinMode(PIN_IRQ, INPUT);
  fingerprint.begin();
}

void Io::loop() {
  if (digitalRead(PIN_IRQ) || lastFingerState) {
    uint8_t fingerState = fingerprint.detectFinger();
    if (fingerState != lastFingerState) {
      lastFingerState = fingerState;

      fingerprint.ctrlLED(fingerState ? DFRobot_ID809::eLEDMode_t::eFadeIn
                                      : DFRobot_ID809::eLEDMode_t::eFadeOut,
                          DFRobot_ID809::eLEDColor_t::eLEDYellow, 0);

      if (fingerState) {
        uint8_t res = fingerprint.collectionFingerprint(1);
        if (res != ERR_ID809) {
          uint8_t id = fingerprint.search();

          fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eFastBlink,
                              DFRobot_ID809::eLEDColor_t::eLEDGreen, 3);
        } else {
          fingerprint.ctrlLED(DFRobot_ID809::eLEDMode_t::eFastBlink,
                              DFRobot_ID809::eLEDColor_t::eLEDRed, 3);
        }
      }
    }
  }
}