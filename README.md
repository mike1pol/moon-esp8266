# Moon

## Requirments

### Hardware
- Wemos D1 mini / NodeMCU
- WS2812 led strip
- Button TTP223

### Software
- [Platform.io](https://platformio.org/)
- MQTT server

## Schema
![SCHEME](https://github.com/mike1pol/moon-esp8266/blob/master/schemes/scheme1.png)

## Install
- Select you board in `platformio.ini` in `default_envs`
- Install deps `pio lib install`
- Update config file `src/config.h`
- Flash by USB
- Wait WiFi network `MoonLamp`
- Connect to `MoonLamp` (can be changed in `config.h` - `AP_NAME`) WiFi (password: 12345678, can be changed in `config.h` - `AP_PASS`)
- Setup you home WiFi network

## MQTT
- State topic - `MoonLamp/MoonLamp_{CHIP_ID}/state`
- Command topic - `MoonLamp/MoonLamp_{CHIP_ID}/cmd`

### Commands
- Power On `P_ON`
- Power Off `P_OFF`
- Change brightness `BRI{1-255}` example `BRI100`
- Change moon color `MODE{0-5}` example `MODE4`
- Next moon color `NEXT`

## OTA update
- Triple click on button
- Wait until the moon starts blinking red
- Triple click on button
- Wait until the moon turns red
- Upload firmware by ip address and password from `AP_PASS`