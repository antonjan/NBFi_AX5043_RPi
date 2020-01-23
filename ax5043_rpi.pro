QT -= core
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -Ofast -funroll-loops -mfloat-abi=hard -mfpu=vfp

LIBS += -L=/lib/arm-linux-gnueabihf -L=/usr/lib/arm-linux-gnueabihf -L=/usr/lib -ldl

CONFIG(debug, debug|release){
    DESTDIR = ../debug/bin
}else{
    DESTDIR = ../release/bin
}

target.path = /home/pi/ax5043_rpi
INSTALLS += target

include(ax5043_rpi.pri)
