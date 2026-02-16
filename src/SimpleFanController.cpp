#include "SimpleFanController.h"
#include <EEPROM.h>

#define EEPROM_MAGIC 0xAB

SimpleFanController::SimpleFanController(
    TemperatureController* tempCtrl, uint16_t updateRate, uint16_t eepromAddr)
    : tempCtrl(tempCtrl), updateRate(updateRate), eepromAddr(eepromAddr) {
  memset(extTemp, 0, sizeof(extTemp));
  load();
}

void SimpleFanController::addFan(uint8_t index, PWMFan* fan) {
  if (index < FAN_NUM) {
    fans[index] = fan;
    fanData[index].detectionType = FanDetectionType::FourPin;
  }
}

static uint16_t interpolate(const FanCurve& c, uint16_t temp) {
  if (temp <= c.temperatures[0]) return c.rpms[0];
  if (temp >= c.temperatures[FAN_CURVE_POINTS_NUM - 1])
    return c.rpms[FAN_CURVE_POINTS_NUM - 1];
  for (uint8_t i = 0; i < FAN_CURVE_POINTS_NUM - 1; i++) {
    if (temp >= c.temperatures[i] && temp < c.temperatures[i + 1]) {
      uint16_t t0 = c.temperatures[i], t1 = c.temperatures[i + 1];
      uint16_t r0 = c.rpms[i], r1 = c.rpms[i + 1];
      if (t1 == t0) return r0;
      return r0 + (uint16_t)(((uint32_t)(temp - t0) * (r1 - r0)) / (t1 - t0));
    }
  }
  return c.rpms[FAN_CURVE_POINTS_NUM - 1];
}

bool SimpleFanController::updateFans() {
  unsigned long now = millis();
  if (now - lastUpdate < updateRate) return false;
  lastUpdate = now;
  for (uint8_t i = 0; i < FAN_NUM; i++) {
    if (!fans[i]) continue;
    uint8_t pwr = 0;
    switch (fanData[i].mode) {
      case FAN_MODE_FIXED_POWER:
        pwr = fanData[i].power;
        break;
      case FAN_MODE_FIXED_RPM:
        pwr = fans[i]->calculatePowerFromSpeed(fanData[i].speed);
        break;
      case FAN_MODE_CURVE: {
        uint16_t temp = 0;
        uint8_t grp = fanData[i].tempGroup;
        if (grp < TEMPERATURE_NUM)
          temp = tempCtrl->getTemperature(grp);
        else
          temp = extTemp[i];
        pwr = fans[i]->calculatePowerFromSpeed(
            interpolate(fanData[i].fanCurve, temp));
        break;
      }
    }
    fans[i]->setPower(pwr);
    fanData[i].power = pwr;
  }
  if (needSave) { save(); needSave = false; }
  return true;
}

uint16_t SimpleFanController::getFanSpeed(uint8_t fan) {
  if (fan >= FAN_NUM || !fans[fan]) return 0;
  return fans[fan]->calculateSpeedFromPower(fanData[fan].power);
}
void SimpleFanController::setFanSpeed(uint8_t fan, uint16_t speed) {
  if (fan >= FAN_NUM) return;
  fanData[fan].mode = FAN_MODE_FIXED_RPM;
  fanData[fan].speed = speed;
  needSave = true;
}
uint8_t SimpleFanController::getFanPower(uint8_t fan) {
  if (fan >= FAN_NUM) return 0;
  return fanData[fan].power;
}
void SimpleFanController::setFanPower(uint8_t fan, uint8_t percentage) {
  if (fan >= FAN_NUM) return;
  fanData[fan].mode = FAN_MODE_FIXED_POWER;
  fanData[fan].power = percentage;
  needSave = true;
}
void SimpleFanController::setFanCurve(uint8_t fan, uint8_t group, FanCurve& curve) {
  if (fan >= FAN_NUM) return;
  fanData[fan].mode = FAN_MODE_CURVE;
  fanData[fan].tempGroup = group;
  fanData[fan].fanCurve = curve;
  needSave = true;
}
void SimpleFanController::setFanExternalTemperature(uint8_t fan, uint16_t temp) {
  if (fan >= FAN_NUM) return;
  extTemp[fan] = temp;
}
void SimpleFanController::setFanForce3PinMode(bool flag) { force3Pin = flag; }
FanDetectionType SimpleFanController::getFanDetectionType(uint8_t fan) {
  if (fan >= FAN_NUM) return FanDetectionType::Disconnected;
  return fanData[fan].detectionType;
}
void SimpleFanController::setFanDetectionType(uint8_t fan, FanDetectionType type) {
  if (fan >= FAN_NUM) return;
  fanData[fan].detectionType = type;
  needSave = true;
}

bool SimpleFanController::load() {
  uint16_t addr = eepromAddr;
  if (EEPROM.read(addr) != EEPROM_MAGIC) return false;
  addr++;
  for (uint8_t i = 0; i < FAN_NUM; i++) {
    EEPROM.get(addr, fanData[i]);
    addr += sizeof(FanData);
  }
  return true;
}
bool SimpleFanController::save() {
  uint16_t addr = eepromAddr;
  EEPROM.update(addr, EEPROM_MAGIC);
  addr++;
  for (uint8_t i = 0; i < FAN_NUM; i++) {
    EEPROM.put(addr, fanData[i]);
    addr += sizeof(FanData);
  }
  return true;
}
