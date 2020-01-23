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
    SvDebug/SvRes.qrc


SOURCES += \
  SvDebug/SvSymbol.cpp \
  SvDebug/SvUtils.cpp \
  SvDebug/WCommand.cpp \
  SvDebug/WDPortSettings.cpp \
  SvDebug/WDebugTable.cpp \
  SvDebug/WMain.cpp \
  SvDebug/WOscillograph.cpp \
  SvDebug/bug.cpp \
  SvDebug/main.cpp \
  SvHost/7bit.cpp \
  SvHost/SvDir.cpp


DISTFILES +=

HEADERS += \
  SvDebug/SvConfig.h \
  SvDebug/SvSymbol.h \
  SvDebug/SvUtils.h \
  SvDebug/WCommand.h \
  SvDebug/WDPortSettings.h \
  SvDebug/WDebugTable.h \
  SvDebug/WMain.h \
  SvDebug/WOscillograph.h \
  SvHost/7bit.h \
  SvHost/SvDir.h

FORMS += \
  SvDebug/WDPortSettings.ui
