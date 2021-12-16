# ThetaSwarm Firmware v1.0

ThetaSwarm Firmware for the ESP32 development board


# Still Under Development....

<img src="https://github.com/MuamerBuco/ThetaSwarmFW/blob/master/images/IMG_6138_00Cut.png" alt="robotImageInternals1" width="800" height="420" align="center">


## Current Features

- Safety auto-shutdown
- UDP communication with master
- Automatic signal lighting
- LED control for WS2812 LED Ring, preprogrammed effects and custom low-level commands
- 2DOF Bucket control

## Coming Soon

- Two-way communication
- Low-battery signal

## Communication Format

UDP datagrams expected in 12 byte format, with the first byte used for command parsing.  

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

