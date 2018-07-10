#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T21:58:19
#
#-------------------------------------------------

QT       += core gui network

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = SvStudio
TEMPLATE = app


SOURCES += main.cpp\
    Vpu/SvController.cpp \
    Host/SvControllerLocal.cpp \
    Host/SvMirror.cpp \
    Host/SvMirrorLocal.cpp \
    SvProject.cpp \
    SvDebugThread.cpp \
    Host/SvProgramm.cpp \
    WBrowser.cpp \
    WCModeBoard.cpp \
    WCModeEditor.cpp \
    WCModeHelp.cpp \
    WCModeIntro.cpp \
    WCommand.cpp \
    WMain.cpp \
    WTextEditor.cpp \
    WVisualEditor.cpp \
    WVisualLibrary.cpp \
    Highlighter.cpp \
    SvUtils.cpp \
    SvPeriodicParser.cpp \
    DsLibCategory.cpp \
    DPrjProp.cpp \
    DProcess.cpp \
    Host/SvMirrorUsb.cpp \
    Host/SvMirrorExtern.cpp \
    Host/SvMirrorRemote.cpp \
    Compiler/SvClass.cpp \
    Compiler/SvCompiler1.cpp \
    Compiler/SvCompiler2.cpp \
    Compiler/SvCompiler3.cpp \
    Compiler/SvCompiler4.cpp \
    Compiler/SvCompiler4A.cpp \
    Compiler/SvCompiler5.cpp \
    Compiler/SvCompiler6.cpp \
    Compiler/SvCompiler7.cpp \
    Compiler/SvCompiler8.cpp \
    Compiler/SvCompiler9.cpp \
    Compiler/SvCompilerA.cpp \
    Compiler/SvHelp.cpp \
    Compiler/SvType.cpp \
    Compiler/SvValue.cpp \
    Compiler/SvVariable.cpp \
    Compiler/SvVpuCompiler.cpp \
    Host/SvMirrorThread.cpp \
    WDebugTable.cpp \
    IngDebugCalculator.cpp \
    WTextSearchPanel.cpp \
    DTextEditorSettings.cpp \
    Host/SvMirrorUsb10.cpp \
    ../SvNet/SvDir.cpp \
    ../SvNet/SvNetClient.cpp \
    ../SvNet/SvNetClientFile.cpp \
    ../SvNet/SvNetPacket.cpp \
    ../SvNet/SvNetPacketBlock.cpp \
    ../SvNet/WNetFileList.cpp \
    ../SvNet/WNetFileManager.cpp \
    ../SvNet/WNetViewer.cpp \
    Host/SvNetClientMirror.cpp \
    ../SvNet/DNetProcess.cpp \
    bug.cpp \
    Host/SvStorageFile.cpp \
    Host/SvStorage.cpp \
    Host/SvStorageField.cpp \
    Host/SvStorageMemory.cpp \
    Host/SvStorageViewer.cpp \
    WOscillograph.cpp

HEADERS  += \
    Vpu/SvController.h \
    Vpu/SvVpuTypes.h \
    Host/SvControllerLocal.h \
    Host/SvMirror.h \
    Host/SvMirrorLocal.h \
    Host/SvProgramm.h \
    SvProject.h \
    SvDebugThread.h \
    WBrowser.h \
    WCModeBoard.h \
    WCModeEditor.h \
    WCModeHelp.h \
    WCModeIntro.h \
    WCommand.h \
    WMain.h \
    WTextEditor.h \
    WVisualEditor.h \
    WVisualLibrary.h \
    Highlighter.h \
    SvUtils.h \
    SvPeriodicParser.h \
    DsLibCategory.h \
    DPrjProp.h \
    DProcess.h \
    SvConfig.h \
    Host/SvMirrorUsb.h \
    Host/SvMirrorExtern.h \
    Host/SvMirrorRemote.h \
    Vpu/SvVpuCore.h \
    Compiler/SvCompiler.h \
    Compiler/SvVpuCompiler.h \
    Host/SvMirrorThread.h \
    Vpu/SvVpuUtils.h \
    Vpu/SvVpuUsbInterface.h \
    Vpu/vpu5Sys.h \
    WDebugTable.h \
    IngDebugCalculator.h \
    WTextSearchPanel.h \
    DTextEditorSettings.h \
    Host/SvMirrorUsb10.h \
    ../SvNet/SvDir.h \
    ../SvNet/SvNetClient.h \
    ../SvNet/SvNetClientFile.h \
    ../SvNet/SvNetFile.h \
    ../SvNet/SvNetPacket.h \
    ../SvNet/SvNetPacketBlock.h \
    ../SvNet/WNetFileList.h \
    ../SvNet/WNetFileManager.h \
    ../SvNet/WNetViewer.h \
    Host/SvNetClientMirror.h \
    ../SvNet/DNetProcess.h \
    Host/SvCompilerBase.h \
    Host/SvStorageFile.h \
    Host/SvStorage.h \
    Host/SvStorageField.h \
    Host/SvStorageMemory.h \
    Host/SvStorageViewer.h \
    WOscillograph.h


RESOURCES += \
    SvRes.qrc

FORMS += \
    DPrjProp.ui \
    DProcess.ui \
    DTextEditorSettings.ui

unix: !macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

INCLUDEPATH += $$PWD/libusb-1.0
INCLUDEPATH += ../SvNet
DEPENDPATH += $$PWD/libusb-1.0
