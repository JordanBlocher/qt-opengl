######################################################################
# Automatically generated by qmake (2.01a) Wed Aug 28 22:18:36 2013
######################################################################

# add opengl functionality
QT += opengl

TEMPLATE = app
TARGET = ../bin/assignment01
DEPENDPATH += .
INCLUDEPATH += ../../common/include
LIBS += -L../../common/lib -lqglcommon -lGLEW
QMAKE_CXXFLAGS += -std=c++0x

libqglcommon.target = ../../common/lib/libqglcommon.a
libqglcommon.commands = cd ../../common && make
QMAKE_EXTRA_TARGETS += libqglcommon
PRE_TARGETDEPS += ../../common/lib/libqglcommon.a

CONFIG += debug

# Input
SOURCES += ../src/*.cpp
HEADERS += ../src/*.hpp

