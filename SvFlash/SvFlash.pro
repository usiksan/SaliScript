#-------------------------------------------------
#
# Project created by QtCreator 2019-03-23T12:07:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SvFlash
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ENABLE_USB10

unix: !macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

INCLUDEPATH += $$PWD/../libusb-1.0
INCLUDEPATH += ../SvNet
DEPENDPATH += $$PWD/../libusb-1.0

INCLUDEPATH += ../SvStudio

win32: LIBS += -L$$PWD/../libusb-1.0/ -llibusb-1.0.dll



# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        SvFlash.cpp \
    ../SvStudio/Host/SvMirror.cpp \
    ../SvStudio/Host/SvMirrorExtern.cpp \
    ../SvStudio/Host/SvMirrorThread.cpp \
    ../SvStudio/Host/SvMirrorUsb.cpp \
    ../SvStudio/Host/SvMirrorUsb10.cpp \
    ../SvStudio/Host/SvProgramm.cpp \
    ../SvStudio/Compiler/SvClass.cpp \
    ../SvStudio/Compiler/SvCompiler1.cpp \
    ../SvStudio/Compiler/SvCompiler2.cpp \
    ../SvStudio/Compiler/SvCompiler3.cpp \
    ../SvStudio/Compiler/SvCompiler4.cpp \
    ../SvStudio/Compiler/SvCompiler4A.cpp \
    ../SvStudio/Compiler/SvCompiler5.cpp \
    ../SvStudio/Compiler/SvCompiler6.cpp \
    ../SvStudio/Compiler/SvCompiler7.cpp \
    ../SvStudio/Compiler/SvCompiler8.cpp \
    ../SvStudio/Compiler/SvCompiler9.cpp \
    ../SvStudio/Compiler/SvCompilerA.cpp \
    ../SvStudio/Compiler/SvHelp.cpp \
    ../SvStudio/Compiler/SvType.cpp \
    ../SvStudio/Compiler/SvValue.cpp \
    ../SvStudio/Compiler/SvVariable.cpp \
    ../SvStudio/Compiler/SvVpuCompiler.cpp

HEADERS += \
        SvFlash.h \
    ../SvStudio/Host/SvMirror.h \
    ../SvStudio/Host/SvMirrorExtern.h \
    ../SvStudio/Host/SvMirrorThread.h \
    ../SvStudio/Host/SvMirrorUsb.h \
    ../SvStudio/Host/SvMirrorUsb10.h \
    ../SvStudio/Host/SvProgramm.h \
    ../SvStudio/Compiler/SvCompiler.h \
    ../SvStudio/Compiler/SvVpuCompiler.h

FORMS += \
        SvFlash.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
