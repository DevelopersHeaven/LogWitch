logWitch - GUI logging tool for analyzing remote logging sessions
=================================================================

This graphgical logging tool was written to support developer for their daily
business and was heavily used by myself. Many logging frameworks support remote
socket logging, but there was no really goot logging viewer for a high traffic
logging while performing trace logging action. The tool was written to support
1 million and up loglines with a possibility to search within them for error
analysis and bug hunting.

Usage
=====
docker build github.com/Kiwi-MethodPark/LogWitch -t logwitch

docker run -e DISPLAY -v $XAUTHORITY:/root/.Xauthority --net=host --privileged logwitch

If you don't want to run the program in privileged mode use
docker run -p 9998:9998 -e DISPLAY -v $XAUTHORITY:/root/.Xauthority -v /tmp/.X11-unix:/tmp/.X11-unix logwitch

* For open GL support (to solve `libGL error: MESA-LOADER: failed to retrieve device information` warning) add parameter -v /dev/dri:/dev/dri


Features
========

* Remote logging using the following frameworks (but it is extensible)
    * log4cplus
    * python3 (logging.handlers.SocketHandler)
* Reading of logfiles (for the moment limited to a fixed pattern
* Writing out logfiles in own format (still text file, readable)
* Reading of logifles from own format and showing as saved
* Filtering the logs by specific criteria for e.g. hierarchy based
* Searching within logs with multple acitve expressions
* Highlighting log lines for easier analyzation with colors.
    * Fully customizeable, foreground, background, icons, ...
    * Use expressions (find, regex) with and, or, not to match certain fields or
      texts 
* High performance while doing actions with many log lines
* Calculations with timestamps for meassurements (diff between line or set a line 
  as reference)

Screenshot
==========

A current screenshot of the Tool:

![Screenshot](https://raw.githubusercontent.com/wiki/DevelopersHeaven/LogWitch/images/Screenshot_2019-04-22_15-00-01.png)

Installation
============

Legacy build
------------
* First clone the repository
* Install prerequesites needed (depends on the features you activate)
* Build it (see INSTALL)


Copyright and license
=====================

Copyright (C) 2019, Sven Steckmann et al.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 or later as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
