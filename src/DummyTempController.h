#pragma once
#include "Arduino.h"
#include <TemperatureController.h>

// Заглушка — все датчики температуры "не подключены"
class DummyTempController : public TemperatureController {
protected:
  bool isTemperatureSensorConnected(uint8_t sensor) override { return false; }
  uint16_t getTemperatureValue(uint8_t sensor) override { return 3000; }
  uint16_t getVoltageRail12V() override { return 12000; }
  uint16_t getVoltageRail5V() override { return 5000; }
  uint16_t getVoltageRail3V3() override { return 3300; }
};
