# Transmitter Board
![1](/Resources\project_images\sensor_board\render1.png)
## This directory contains the PCB designs for the sensor board.
### The front of the submarine contains several sensors underwater: a pressure transducer and a thermistor. The pressure transducer is used to measure the pressure of the water. The thermistor is used to measure the temperature of the water. The sensor board reads these sensors and sends the data to the main board through I2C. 
### Power:
* 3.3V (no voltage regulator onboard - must be clean 3.3V)
### Sensors:
* Analog input for external pressure transducer
* 10K Thermistor for water temperature readings
### Communication:
* Communicated to the submarine [main_board](https://github.com/daniel360kim/OceanAI/tree/master/main_board) through I2C.
* The board features SWD interface for debugging. 

## What's done?
* Schematic is finished.
* PCB layout is finished
* 3D models and fabrication outputs are generated
## What's to do?
* Ordering and fabrication