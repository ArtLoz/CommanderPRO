# CommanderPRO Firmware

Custom firmware for an **ATmega32U4** board that emulates a Corsair Commander PRO device over USB HID.

This project uses:
- [CorsairLightingProtocol](https://github.com/Legion2/CorsairLightingProtocol) (`dev` branch)
- [FastLED](https://github.com/FastLED/FastLED)
- PlatformIO + Arduino framework

## Features

- 2 PWM-controlled fan channels (pins `D9` and `D10`)
- Corsair Lighting Protocol HID integration
- WS2812B LED output (pin `D3`)
- Fan control modes supported by `SimpleFanController`:
  - fixed power
  - fixed RPM (mapped to PWM)
  - temperature curve
- EEPROM persistence for fan settings

## Hardware Assumptions

Current defaults in `src/main.cpp`:
- Fan 1 PWM pin: `9`
- Fan 2 PWM pin: `10`
- LED data pin: `3`
- Fan RPM range (both channels): `200` to `2500`
- LEDs per fan: `8`
- Total LEDs configured: `24`

The board definition is located at `boards/commanderpro.json` and targets:
- MCU: `ATmega32U4`
- USB VID/PID: `0x1B1C / 0x0C10`
- Upload protocol: `avr109`

## Build

```bash
pio run -e commanderpro
```

## Upload

```bash
pio run -e commanderpro -t upload
```

## Serial Monitor

```bash
pio device monitor -b 115200
```

## Configuration Notes

To customize the firmware, edit constants in `src/main.cpp`:
- fan pins and RPM limits
- LED pin and LED count
- USB serial string (`mySerialNumber`)

Fan settings are stored in EEPROM by `SimpleFanController` with a magic byte (`0xAB`) and restored on boot.

## Project Layout

- `src/main.cpp` - firmware entry point and wiring of controllers
- `src/PWMFan.*` - low-level PWM fan output logic (Timer1)
- `src/SimpleFanController.*` - fan mode/state management and EEPROM persistence
- `src/DummyTempController.h` - stub temperature provider
- `boards/commanderpro.json` - custom PlatformIO board definition
- `platformio.ini` - environment, dependencies, and build/upload settings

## License

MIT. See `LICENSE`.
