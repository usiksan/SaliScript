QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_svtextstream.cpp \
  ../SvHost/SvTextStreamIn.cpp \
  ../SvHost/SvTextStreamOut.cpp

HEADERS += \
  ../SvHost/SvTextStreamIn.h \
  ../SvHost/SvTextStreamOut.h
