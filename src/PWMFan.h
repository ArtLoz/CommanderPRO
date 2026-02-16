#pragma once
#include "Arduino.h"

class PWMFan {
public:
  PWMFan(uint8_t pwmPin, uint16_t minRPM, uint16_t maxRPM);
  void begin();
  virtual void setPower(uint8_t percentage);
  virtual uint8_t calculatePowerFromSpeed(uint16_t rpm);
  virtual uint16_t calculateSpeedFromPower(uint8_t power);
protected:
  const uint8_t pwmPin;
  const uint16_t minRPM;
  const uint16_t maxRPM;
};
