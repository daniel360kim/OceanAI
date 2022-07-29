# PCBs
## Contains all the files for the printed circuit boards.
![1](/Resources/project_images/pcb/render.gif)
### main_board
* The PCB on the submarine that controls the submarine.
* This is the main board that controls navigation, outputs, loggins etc.

### transmitter_board
* This is an optional module that is floating in the water.
* It is connnected through a cable to the main board.
* It is above water so it can access GPS, RF, and other radio frequencies.

### sensor_board
* This board is at the front of the submarine. 
* It reads the underwater sensors and sends the data to the main board through I2C.
* It is not self powered - must have a clean 3.3V power supply.
* Programmed through SWD.