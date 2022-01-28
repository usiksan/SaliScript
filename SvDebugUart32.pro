#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T21:58:19
#
#-------------------------------------------------

QT       += core gui network serialport

CONFIG += c++17


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = SvDebug
TEMPLATE = app




RESOURCES += \
    SvDebugUart32/SvRes.qrc


SOURCES += \
  SvDebugUart32/SvSymbol.cpp \
  SvDebugUart32/SvUtils.cpp \
  SvDebugUart32/WCommand.cpp \
  SvDebugUart32/WDPortSettings.cpp \
  SvDebugUart32/WDebugTable.cpp \
  SvDebugUart32/WMain.cpp \
  SvDebugUart32/WOscillograph.cpp \
  SvDebugUart32/bug.cpp \
  SvDebugUart32/main.cpp \
  SvHost/SvDir.cpp \
  SvHost/SvTextStreamIn.cpp \
  SvHost/SvTextStreamOut.cpp


DISTFILES +=

HEADERS += \
  SvDebugUart32/SvConfig.h \
  SvDebugUart32/SvDebugUart32.h \
  SvDebugUart32/SvSymbol.h \
  SvDebugUart32/SvUtils.h \
  SvDebugUart32/WCommand.h \
  SvDebugUart32/WDPortSettings.h \
  SvDebugUart32/WDebugTable.h \
  SvDebugUart32/WMain.h \
  SvDebugUart32/WOscillograph.h \
  SvHost/7bit.h \
  SvHost/SvDir.h \
  SvHost/SvTextStreamIn.h \
  SvHost/SvTextStreamOut.h

FORMS += \
  SvDebugUart32/WDPortSettings.ui
