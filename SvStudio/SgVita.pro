#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T08:45:05
#
#-------------------------------------------------
CONFIG += c++11

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SgVita
TEMPLATE = app

INCLUDEPATH += ../SgCore

SOURCES += mainVita.cpp\
    ../SgCore/SgCoreItem.cpp \
    ../SgCore/SgView.cpp \
    SgSml/SgVitaWidget.cpp \
    Host/SvBuilder.cpp \
    Host/SvControllerLocal.cpp \
    Host/SvDebugLink.cpp \
    Host/SvMirror.cpp \
    Host/SvMirrorExtern.cpp \
    Host/SvMirrorInternet.cpp \
    Host/SvMirrorLocal.cpp \
    Host/SvMirrorRemote.cpp \
    Host/SvMirrorUsb.cpp \
    Host/SvProgramm.cpp \
    Vpu/SvController.cpp \
    Vpu/SvVpuStr.cpp \
    SgSml/SgImageProvider.cpp \
    SgSml/SgItem.cpp \
    SgSml/SgValue.cpp \
    SgSml/SgSignal.cpp \
    SgSml/SgRect.cpp \
    SgSml/SgCompiler2.cpp \
    SgSml/SgCompiler3.cpp \
    SgSml/SgOperandCondition.cpp \
    SgSml/SgPropertyInt.cpp \
    SgSml/SgPropertyString.cpp \
    SgSml/SgPropertyDouble.cpp \
    SgSml/SgPropertyVpu.cpp \
    SgSml/SgPropertyAlias.cpp \
    SgSml/SgaSignal.cpp \
    SgSml/SgImage.cpp \
    SgSml/SgText.cpp \
    SgSml/SgMirror.cpp \
    Host/SvMirrorThread.cpp \
    SgSml/SgMonitorInt.cpp \
    SgSml/SgImageSlider.cpp \
    SgSml/SgBorderImage.cpp \
    SgSml/SgCircle.cpp \
    SgSml/SgCompiler1.cpp \
    SgSml/SgColumn.cpp \
    SgSml/SgRow.cpp \
    SgSml/SgSettingDouble.cpp \
    SgSml/SgItemList.cpp \
    SgSml/SgSettingInt.cpp \
    SgSml/SgSettingString.cpp \
    SgSml/SgFileDir.cpp \
    SgSml/SgGuide.cpp \
    SgSml/SgPrevList.cpp \
    SgSml/SgProfilePreview.cpp \
    SgSml/SgOperandItem.cpp \
    SgSml/SgFunctor.cpp \
    SgSml/SgEditLine.cpp \
    SgSml/SgEditValue.cpp \
    Compiler/SvCompiler4A.cpp \
    Compiler/SvCompiler5.cpp \
    Compiler/SvCompiler6.cpp \
    Compiler/SvCompiler7.cpp \
    Compiler/SvCompiler8.cpp \
    Compiler/SvCompiler9.cpp \
    Compiler/SvCompilerA.cpp \
    Compiler/SvHelp.cpp \
    Compiler/SvStr32.cpp \
    Compiler/SvType.cpp \
    Compiler/SvValue.cpp \
    Compiler/SvVariable.cpp \
    Compiler/SvVpuCompiler.cpp \
    Compiler/SvCompiler4.cpp \
    Compiler/SvCompiler3.cpp \
    Compiler/SvCompiler2.cpp \
    Compiler/SvCompiler1.cpp \
    Compiler/SvClass.cpp

HEADERS  += ../SgCore/SgCore.h \
    ../SgCore/SgView.h \
    SgSml/SgVitaWidget.h \
    Host/SvBuilder.h \
    Host/SvControllerLocal.h \
    Host/SvDebugLink.h \
    Host/SvLinkInfo.h \
    Host/SvMirror.h \
    Host/SvMirrorExtern.h \
    Host/SvMirrorInternet.h \
    Host/SvMirrorLocal.h \
    Host/SvMirrorRemote.h \
    Host/SvMirrorUsb.h \
    Host/SvProgramm.h \
    Vpu/SvController.h \
    Vpu/SvVpuCore.h \
    Vpu/SvVpuStr.h \
    Vpu/SvVpuTypes.h \
    Vpu/vpuSys.h \
    SgSml/SgCompiler.h \
    SgSml/SgObjects.h \
    vitaConfig.h \
    Host/SvMirrorThread.h \
    Host/SvCompilerBase.h \
    Compiler/SvCompiler.h \
    Compiler/SvVpuCompiler.h
