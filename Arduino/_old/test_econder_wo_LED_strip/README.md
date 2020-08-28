# README

## GENERAL

* main scale: 0 - 1023;
* content scale: 0 - 31 (all the points, interesting and not);
* activable contents: they are 8 in total at positions 14,16,17,19,22,26,27,29;
* activation time (the load time which will pass before the content will be put on play
	after the carriage has selected it): 2s;
* content duration (the duration of the video): 37s (except two of them which are 34s long);

## STANDBY (USE ME) status

should be a status where we have:
* a light breathing effect for the carriage (SIDELOBE larger);
* eventually ligth dots on the LED strip in order for the phisical interface to
	represent interesting point to be examined;

## TODO

* test the Animator_Sine _reset phase_ function.
* find a way to put light points on the timeline for the interesting points to appear;
* find a way to make the sidelobes "streatchable";
