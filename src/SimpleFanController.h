#pragma once
#include "Arduino.h"
#include <FanController.h>
#include "PWMFan.h"
#include <TemperatureController.h>

#define FAN_MODE_FIXED_POWER 0
#define FAN_MODE_FIXED_RPM   1
#define FAN_MODE_CURVE       2

struct FanData {
  uint8_t mode = FAN_MODE_FIXED_POWER;
  uint8_t power = 0;
  uint16_t speed = 0;
  FanDetectionType detectionType = FanDetectionType::Disconnected;
  uint8_t tempGroup;
  FanCurve fanCurve;
};

class SimpleFanController : public FanController {
public:
  SimpleFanController(TemperatureController* tempCtrl,
                      uint16_t updateRate, uint16_t eepromAddr);
  void addFan(uint8_t index, PWMFan* fan);
  virtual bool updateFans();
    uint8_t lastMode = 0;
protected:
  virtual uint16_t getFanSpeed(uint8_t fan) override;
  virtual void setFanSpeed(uint8_t fan, uint16_t speed) override;
  virtual uint8_t getFanPower(uint8_t fan) override;
  virtual void setFanPower(uint8_t fan, uint8_t percentage) override;
  virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& curve) override;
  virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) override;
  virtual void setFanForce3PinMode(bool flag) override;
  virtual FanDetectionType getFanDetectionType(uint8_t fan) override;
  virtual void setFanDetectionType(uint8_t fan, FanDetectionType type) override;
  bool load();
  bool save();
  TemperatureController* const tempCtrl;
  PWMFan* fans[FAN_NUM] = {nullptr};
  bool force3Pin = false;
  FanData fanData[FAN_NUM];
  uint16_t extTemp[FAN_NUM];
  uint16_t updateRate;
  uint16_t eepromAddr;
  bool needSave = false;
  unsigned long lastUpdate = 0;
};
