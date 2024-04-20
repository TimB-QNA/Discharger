# Discharger
Battery discharger using programmable load or custom hardware (PI)

The battery discharger can be used on a Tenma programmable load, or on special hardware on the Raspberry Pi.

Further details on hardware and connection will be available in the user manual (when I get round to writing it).

The code is dependant on th QT modules:
  * serialport
  * xml
  * printsupport
  
On debian, serialport can be installed by running apt-get install libqt5serialport5-dev.

To build the code, run the following commands from the root of the repository directory.

qmake
colormake

This compiles the code into a "bin" directory. The directory name is composed of the operating system and the system architecture. Therefore on a desktop Linux system, this directory will be called something like "bin-Linux-x86_64".

The code can be run from the command line by executing:

bin-Linux-x86_64/Battery
