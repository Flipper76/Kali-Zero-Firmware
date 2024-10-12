[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/ginkage)
[![paypal](https://www.paypalobjects.com/en_GB/i/btn/btn_donate_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=LF9S5WAF6E4VA)

# BMI Air Mouse

## Brief

> "You can turn anything into an air mouse if you're brave enough"

 — Piper, a.k.a. Pez

Naturally, the quote above applies to [Flipper](https://flipperzero.one/) as well.

## What?

The app allows you to turn your Flipper into a USB or Bluetooth air mouse (you do need an extra module, see the Hardware section below)...

Using it is really simple:
 * Connect the Flipper via a USB cable and pick `USB`, or pick `Bluetooth` and pair it with your PC;
 * Hold the Flipper in your hand with the buttons pointing towards the screen;
 * Wave your Flipper like you don't care to move the cursor;
 * Up button for Left mouse click;
 * Down button for Right mouse click;
 * Center button for Middle mouse click;
 * Left and Right buttons for scrolling;
 * Use calibration menu option if you notice significant drift (place your Flipper onto a level surface, make sure it doesn't move, run this option, wait 2 seconds, done).

See early prototype [in action](https://www.youtube.com/watch?v=DdxAmmsYfMA).

## How?

 * Clone this repository with `git clone --recurse-submodules` to include the driver implementations.
 * Build the project using `ufbt`

## Hardware

The custom module is using Bosch BMI160 accelerometer/gyroscope chip connected via I2C.

Note: in fact, some other IMU chips are also supported.
It's detected via the first found I2C Address.

|   Chip   | Expected I2C Address |
|:--------:|:--------------------:|
|  BMI160  |         0x69         |
| LSM6DS3  |         0x6A         |
| LSM6DSO  |         0x6B         |

Take a look into the [schematic](https://github.com/ginkage/FlippAirMouse/tree/main/schematic) folder for Gerber, BOM and CPL files, so you can order directly from JLCPCB.

Original idea:

![What I thought it would look like](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/schematic.png)

Expectation:

![What EDA though it would look like](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/render.png)

Reality:

![What it looks like](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/flipper.jpg)

## Software

The code is based on the original Bosch [driver](https://github.com/BoschSensortec/BMI160_driver/) and an orientation tracking implementation from the Google [Cardboard](https://github.com/googlevr/cardboard/tree/master/sdk/sensors) project

## License

TL;DR: Use the code however you want, give credit where it's due, no warranty of any kind is provided.
