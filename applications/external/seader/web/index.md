---
layout: home
---
<script src="https://cdn.jsdelivr.net/npm/anchor-js/anchor.min.js"></script>

A [Flipper Zero](https://flipperzero.one/) application (aka "fap") to interface with a SAM from the Flipper Zero over UART.  Latest release on the [App Catalog](https://lab.flipper.net/apps/seader).

# FAQ

See "Probably Frequently Asked Questions:" on Red Team Tools [NARD SAM expansion board product page](https://www.redteamtools.com/nard-sam-expansion-board-for-flipper-zero-with-hid-seos-iclass-sam/)

# Download

## Release

Download release versions via the [Flipper App Catalog](https://lab.flipper.net/apps/seader) in the mobile companion apps.

## Beta/Dev

Download builds based off of git at [flipc](https://flipc.org/bettse/seader?branch=main)

# Bugs

File issues in [GitHub](https://github.com/bettse/seader/issues).

# Hardware

## Option 1: NARD Flipper add-on

Buy it assembled at [Red Team Tools](https://www.redteamtools.com/nard-sam-expansion-board-for-flipper-zero-with-hid-seos-iclass-sam/), with or without SAM.

Or build it yourself from the files in the [NARD repo](https://github.com/killergeek/nard).

Optionally 3d print a [case designed by Antiklesys](https://www.printables.com/model/576735-flipper-zero-samnard-protecting-cover).

## Option 2: Smart Card 2 Click

Put SAM ([USA](https://www.cdw.com/product/hp-sim-for-hid-iclass-for-hip2-reader-security-sim/4854794) [EU](https://www.rfideas-shop.com/en/kt-sim-se-sim-card-hid-iclass-and-seos-for-sphip-r.html) [CA](https://www.pc-canada.com/item/hp-sim-for-hid-iclass-se-and-hid-iclass-seos-for-hip2-reader/y7c07a)) into **[adapter](https://a.co/d/1E9Zk1h)** (because of chip on top) and plug into **[reader](https://www.mikroe.com/smart-card-2-click)** (alt: [digikey](https://www.digikey.com/en/products/detail/mikroelektronika/MIKROE-5492/20840872) with cheaper US shipping). Connect reader to Flipper Zero (See `Connections` below).

Optionally 3d print a [case designed by sean](https://www.printables.com/model/543149-case-for-flipper-zero-devboard-smart2click-samsim)

### Connections

| Smart Card 2 Click | Flipper     |
| ------------------ | ----------- |
| 5v                 | 1           |
| GND                | 8 / 11 / 18 |
| TX                 | 16          |
| RX                 | 15          |

# Development

See [repo readme](https://github.com/bettse/seader#seader).

<script>
   anchors.add();
</script>

----


[Buy Me A Coffee](http://buymeacoffee.com/bettse)

