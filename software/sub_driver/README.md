# Sub Driver Software

Software to control the submarine driver board.

## Build
The project's directories are built with PlatformIO. To build the project, open the project in PlatformIO and click the build button.

## Dependencies Modifications
* Must delete 
    ```cpp
    #define swap 
    ```

    From Arducam dependencies to resolve conflicts with the standard library.
* Must delete utility folder from Buzzer library and remove the include from the source header file.


