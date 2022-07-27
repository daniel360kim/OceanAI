# Transmitter Board
## This directory contains the PCB, 3D models, and firmware for the transmitter board.

### Since RF and GPS do not function well underwater, the submarine will feature an optional transmitter module that is floating in the water. The transmitter board is a simple, inexpensive, and easy to use module that can be used to transmit data to the submarine. Powered by the ATSAMD21G18A, the board has several features:
### Power:
* 1S LiPo battery 
* Solar power to recharge LiPo battery
### Sensors:
* BMP388 pressure sensor
* 10K Thermistor for water temperature readings
### Communication:
* Communicated to the submarine [main_board](https://github.com/daniel360kim/OceanAI/tree/master/main_board) through an ethernet cable through I2C.
* I2C optimized for long range communication using the LTC4311 chip.
* The board has 2 SPI interfaces: One for a 2.4GHz RF module and one for a LoRa radio module.
* The board has a UART interface for GPS connectivity.
* The board features SWD interface for debugging. 

## What's done?
* Schematic is finished.

## What's to do?
* PCB layout
* Ordering and fabrication
* 3D models
