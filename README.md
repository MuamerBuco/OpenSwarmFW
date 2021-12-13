# OpenSwarm Firmware v1.0

OpenSwarm Firmware for the ESP32 development board


# Still Under Development....

<img src="https://github.com/MuamerBuco/ThetaSwarmFW/blob/master/images/IMG_6138_00Cut.png" alt="robotImageInternals1" width="800" height="600" align="center">


## Current Features

- Per wheel motor drive commands
- UDP communication with master
- LED control for WS2812 LED Ring, preprogrammed effects and custom low-level commands
- 2DOF Bucket commands
- Regular battery checks

## Coming Soon

- Two-way communication
- Automatic signal lighting
- Low-battery notifications

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

