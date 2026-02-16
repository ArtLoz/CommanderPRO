#include <Arduino.h>
#include <CorsairLightingProtocol.h>
#include <FastLED.h>
#include "PWMFan.h"
#include "SimpleFanController.h"
#include "DummyTempController.h"
#include <EEPROM.h>

// Настройки кулеров
#define FAN1_PWM_PIN   9
#define FAN1_MIN_RPM   200
#define FAN1_MAX_RPM   2500

#define FAN2_PWM_PIN   10
#define FAN2_MIN_RPM   200
#define FAN2_MAX_RPM   2500

// Подсветка
#define LED_PIN        3
#define LEDS_PER_FAN   8
#define TOTAL_LEDS     (LEDS_PER_FAN * 3)

CRGB leds[TOTAL_LEDS];

// Серийный номер (смени при нескольких устройствах)
const char mySerialNumber[] PROGMEM = "A0B1C2D3E4F5";

// Кулеры
PWMFan fan1(FAN1_PWM_PIN, FAN1_MIN_RPM, FAN1_MAX_RPM);
PWMFan fan2(FAN2_PWM_PIN, FAN2_MIN_RPM, FAN2_MAX_RPM);

// Контроллеры
DummyTempController tempController;
SimpleFanController fanController(&tempController, 500, 0);

// v0.15.0 API
CorsairLightingFirmwareStorageEEPROM firmwareStorage;
CorsairLightingFirmware firmware(CORSAIR_COMMANDER_PRO, &firmwareStorage);

FastLEDControllerStorageEEPROM ledStorage;
FastLEDController ledController(&tempController, &ledStorage);

CorsairLightingProtocolController cLP(
    &ledController, &tempController, &fanController, &firmware);

CorsairLightingProtocolHID cHID(&cLP, mySerialNumber);

void setup() {
  DeviceID deviceId = {0xDE, 0xAD, 0xBE, 0xEF};
  firmware.setDeviceID(deviceId);
  fan1.begin();
  fan2.begin();
  fanController.addFan(0, &fan1);
  fanController.addFan(1, &fan2);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
  ledController.addLEDs(0, leds, TOTAL_LEDS);
}

void loop() {
  cHID.update();
  if (ledController.updateLEDs()) {
    FastLED.show();
  }
  fanController.updateFans();
}
