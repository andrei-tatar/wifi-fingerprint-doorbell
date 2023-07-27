#ifndef _WEB_H_
#define _WEB_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

typedef std::function<DynamicJsonDocument *()> ReadConfigHandler;
typedef std::function<void(String config)> SetConfigHandler;
typedef std::function<void(JsonVariant doc)> AppendStatusHandler;
typedef std::function<esp_err_t(uint8_t **img, size_t *imgLen)>
    CaptureJpegHandler;

class Web {
private:
  AsyncWebServer _server;
  ReadConfigHandler _readConfig;
  SetConfigHandler _setConfig;
  AppendStatusHandler _appendStatus;
  CaptureJpegHandler _captureImage;
  String _type;
  Ticker _rebootTicker;

  void getConfig(AsyncWebServerRequest *req);
  void getStatus(AsyncWebServerRequest *req);
  void getImage(AsyncWebServerRequest *req);
  void updateConfig(AsyncWebServerRequest *req, uint8_t *data, size_t len,
                    size_t index, size_t total);
  void reboot(AsyncWebServerRequest *req);
  void handleNotFound(AsyncWebServerRequest *req);

public:
  Web();

  Web &onReadConfig(ReadConfigHandler readConfig);
  Web &onSetConfig(SetConfigHandler setConfig);
  Web &onAppendStatus(AppendStatusHandler appendStatus);
  Web &onCaptureImage(CaptureJpegHandler captureImage);

  void begin();
};

#endif