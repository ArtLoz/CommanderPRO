#include "PWMFan.h"

static bool timerConfigured = false;

static void configureTimer1() {
  if (timerConfigured) return;
  timerConfigured = true;
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  ICR1 = 320;
}

PWMFan::PWMFan(uint8_t pwmPin, uint16_t minRPM, uint16_t maxRPM)
    : pwmPin(pwmPin), minRPM(minRPM), maxRPM(maxRPM) {
}

void PWMFan::begin() {
  pinMode(pwmPin, OUTPUT);
  configureTimer1();
  switch (pwmPin) {
    case 9:  TCCR1A |= _BV(COM1A1); OCR1A = 0; break;
    case 10: TCCR1A |= _BV(COM1B1); OCR1B = 0; break;
  }
}

void PWMFan::setPower(uint8_t percentage) {
  uint16_t duty;
  if (percentage == 0) {
    duty = 0;
  } else if (percentage == 255) {
    duty = 320;
  } else {
    duty = (uint16_t)percentage * 320 / 255;
     if (duty < 64) duty = 64;
  }
  switch (pwmPin) {
    case 9:  OCR1A = duty; break;
    case 10: OCR1B = duty; break;
  }
}

uint8_t PWMFan::calculatePowerFromSpeed(uint16_t rpm) {
  if (maxRPM <= minRPM) return 0;
  if (rpm <= minRPM) return 0;
  if (rpm >= maxRPM) return 255;
  return (uint8_t)(((uint32_t)(rpm - minRPM) * 255) / (maxRPM - minRPM));
}

uint16_t PWMFan::calculateSpeedFromPower(uint8_t power) {
  return minRPM + (uint16_t)(((uint32_t)power * (maxRPM - minRPM)) / 255);
}
