# README

Arduino sketches to create a slider using different kind of sensors:

* SpectraSymbol linear potentiometer;
* TOF
* etc...

This is still a WIP

## Note on the new implementation (w/ optical encoder and two optical forks)

This new implementation uses a 100 steps optical encoder (which are subdivided in 4 steps inside the Arduino code) in tandem w/ 2 Panasonic PM45 optical switches to make resets.

Knowing that we are making a total of 100 steps per encoder shaft full rotation (400 raw step into the Arduino code) and the carriage is moving 186 mm of linear distance in a full encoder rotation we know that:

* A single encoder step (4 raw steps) represents a carriage linear movement of 1.86 mm;

We also know that (from empirical experimens):

* using a maximum force we can move the carriage from the extreme left and right corners of the slider in 0.3 s (300ms);
* and that there's a distance of 1952 steps for the full lenght of the slider (wich corresponds to 907,6 mm of run - see calculation below)

	400(steps):186(mm)=1952(steps):x(mm)
	
	x = 1952 * 1.86 / 400 = 907,6 mm (a full meter almost)

It means that Arduino is requested to crunch 1952 isp (interrupt service routine) in 0,3(s).
Making a quick proportion:

	1952(isps):0.3(s)=x(isps):1(s)
	
	x = 1952 * 1 / 0.3 = 6506 (isps per second approx) --> 6.5 KHz

which means we have a step every 150µs (see calculation below):

	1 / 6506 = 0,00015 (s) = 0,15 (ms) = 150 µs

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