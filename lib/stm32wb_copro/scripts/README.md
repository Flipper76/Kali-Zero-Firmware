# STM32WB Coprocessor Project extraction scripts

Scripts for extracting a subset of the STM32CubeWB package used to build Flipper Zero firmware.


## How to update

This repo's scritps must be called from a directory where the output files should be.

Run `python3 path/to/update.py -v v1.15.0` to update to specific version of STM32CubeWB package. See `python3 update.py -h` for more options.


## Configuration

Update script is configured using a JSON file. It is located in "config.json". It has 2 sections:

 - `patches`: list of file patches to apply to STM32CubeWB package;
 - `paths`: object with keys that are paths to folders in this repository and values that are corresponding paths to folders in STM32CubeWB package.