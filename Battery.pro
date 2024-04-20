TEMPLATE	= app
LANGUAGE	= C++

QT += serialport xml printsupport

CONFIG  += qt warn_on release

HEADERS += src/eventLoop.h \
           src/batteryPack.h \
           src/loadHardware.h
           
SOURCES += src/main.cpp \
           src/eventLoop.cpp \
           src/batteryPack.cpp \
           src/loadHardware.cpp           

# Load hardware libraries           
HEADERS += src/Tenma7213210Load.h
SOURCES += src/Tenma7213210Load.cpp


HEADERS += src/PiDischarger.h \
           lib/ABElectronics_CPP_Libraries/ADCPi/ABE_ADCPi.h \
           lib/ABElectronics_CPP_Libraries/IOPi/ABE_IoPi.h
           
SOURCES += src/PiDischarger.cpp \
           lib/ABElectronics_CPP_Libraries/ADCPi/ABE_ADCPi.cpp \
           lib/ABElectronics_CPP_Libraries/IOPi/ABE_IoPi.cpp
           
ARCH  = $$system("uname -m")
OPSYS = $$system("uname")
DESTDIR = bin-$$OPSYS-$$ARCH/

CONFIG *= c++11

contains(ARCH, armv7l){
  DEFINES += RaspberryPI
  CONFIG += RaspberryPI
}

UI_DIR = .$$OPSYS-$$ARCH/ui
MOC_DIR = .$$OPSYS-$$ARCH/moc
OBJECTS_DIR = .$$OPSYS-$$ARCH/obj

message("Operating system: $$OPSYS")
message("    Architecture: $$ARCH")
message("      Qt version: $$[QT_VERSION]")
message(" ")
