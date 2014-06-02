pomodoro-timer
==============

Minimalistic app for time management purposes like Pomodoro technique or preventing eyestrain.

Controls
========

Controlled via tray icon (popup menu).
Each state is depicted by corresponding tray icon picture.

* Start/interrupt: starts new interval or interrupts a running one.
* Settings: shows/hides settings window.
* Quit - quits the program.

Single click on tray icon does the same as 'Start/interrupt' menu item.

Settings
========

* Toggle autostart - if on, new interval is started when break is ended.
* Pomodoro length - length of the main time interval.
* Break length - length of the break interval.
* Start break command - command to execute when break interval is started (like play a sound or show notification pop-up).
* End break command - command to execute when break interval is ended.

Default settings is 25 minutes for work and 5 minutes for break.

Installation
============

Dependecies:

* Qt4 framework.
* [libchton](https://github.com/umi0451/libchthon)

Run from project directory:

	qmake && make

Then run `pomodoro` app or copy it to whatever place you want it to be.

