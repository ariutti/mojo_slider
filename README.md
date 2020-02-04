# README

Arduino sketches to create a slider using different kind of sensors:

* SpectraSymbol linear potentiometer;
* TOF
* etc...

This is still a WIP

## Intersting things:

* Lidar Lite v3 [lidar](https://www.robot-italy.com/it/lidar-lite-v3.html) for Arduino (5 cm to 40 meters - 1cm accuracy);
* hack a laser distance measurements [repo](https://github.com/erniejunior/UT390B_Arduino_Library) and [post](http://blog.qartis.com/arduino-laser-distance-meter/)
* Seed Studio [lidar](https://www.seeedstudio.com/TFmini-S-LiDAR-module-Short-Range-ToF-LIDAR-Range-Finder-p-4425.html). YouTube [video](https://www.youtube.com/watch?v=VhbFbxyOI1k) distance sensors comparision (**logic level converter** needed) - 0.3 to 12 m (0.5 cm accuracy)
* [TF mini plus](https://www.seeedstudio.com/TFmini-Plus-LIDAR-Range-Finder-based-on-ToF-p-3222.html) (range: 0.1m~12m@90% reflectivity, Accuracy: ±5cm@（0.1-6m)±1%@（6m-12m, Distance resolution: 5mm)

## Working with lasers

How to power laser without using AA batteries:

## Voltage regulator (step down) to 1.5V

Regolatore LDO, 1,5 V, TO-220, 5A, Fissa, 3 Pin, Positivo

take a look also at 

https://www.eleccircuit.com/usb-battery-replacement-by-lm317/
https://www.eleccircuit.com/lm317-voltage-regulators/