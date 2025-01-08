# ESPHome-Ring-Clock
ESPHome configuration to create an animated clock using one or two Neopixel LED rings.

This is also my project to learn about esphome.

# Features

* [x] Using RTC, no loading time
* [x] Works with a second (inner) ring for the hour hand
* [x] Three animation types:
    * [x] Classic Analog: the analog clock with ticks and a wide hour hand
    * [x] 5min Arc at Hand: shows a dim arc for the 5min interval for each hand
    * [x] Arc from Top to Hand: shows a dim arc from top to the hour/minute hands
* [x] Settings in web\_server or HASS
    * [x] Select animation type
    * [x] Select color palette
    * [x] Select colors and effects (not persistent)
    * [x] Enable/Disable fade out (not persistent)
    * [x] Enable/Disable second hand (not persistent)

## How to use

* If you're not using HASS, follow guide to [Install ESPHome Manually](https://esphome.io/guides/installing_esphome)
* Create a new ESPHome configuration YAML file using [esphome_ring_clock.yaml](./esphome_ring_clock.yaml) as a template
* Customize the basic device configuration (Leds, Board, WiFi, pins, etc.) to your liking
* Customize the fields in the `substitutions` part at the top
* Customize the !secrets substitutions to match your secrets.yaml file
* Customize color palettes
* Copy [ring_clock.h](./ring_clock.h).
* Deploy and enjoy!

## Hardware

For instantaneous start with correct time, connect an RTC board. The template uses ds1307.

If you use two LED rings, the first 60 LEDs are used for minute and second hands (and ticks), the second (inner) ring for the hour hand. This works best if the number of LEDs on the second ring is a multiple of 12.

My hardware:
* Wemos D1 mini v4
* ds1307 attached to wemos i2c connector
* RX pin to Din of 60 LED ring, Dout to Din of 48 LED ring.
* usb-c powered from a 12W USB charger
This is mounted in a picture frame (IKEA sannahed 25x25cm).
LED rings are sandwiched between 4mm 25x25cm mounting board and 4mm 25x25cm Acrylcolor Gray (Gutta).
The mounting board has holes for LED ring connections, all other electronics are mounted to the back of that board.

## Credits

Largely inspired by [/markusressel/ESPHome-Analog-Clock](https://github.com/markusressel/ESPHome-Analog-Clock), thanks markusressel!

# Contributing

GitHub is for social coding: if you want to write code, I encourage contributions through pull requests from forks
of this repository. Create GitHub tickets for bugs and new features and comment on the ones that you are interested in.

# License

CC0 see [LICENSE](./LICENSE)
