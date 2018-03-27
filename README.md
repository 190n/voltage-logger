voltage-logger
==============

Logs voltage information from a battery (connected to A0) onto an SD card (via Adafruit's Micro-SD breakout board).

Requires SD card library.

Voltages are written to /VOLTAGES.TXT on the SD card. Each line is a reading, in mV. Readings are taken 100ms apart. Right now it stops after 1000 readings.

An LED on digital pin 9 is used for status updates. When it turns on and stays on, it is done (it has taken 1000 readings). One blink at startup = error initializing SD card. Two blinks at startup = error opening /VOLTAGES.TXT.
