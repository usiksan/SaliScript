#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T21:58:19
#
#-------------------------------------------------

QT       += core gui network

CONFIG += c++17

  DEFINES += ENABLE_USB10


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = SvStudio
TEMPLATE = app




RESOURCES += \
    SvStudio/SvRes.qrc

FORMS += \
    SvStudio/DPrjProp.ui \
    SvStudio/DProcess.ui \
    SvStudio/DTextEditorSettings.ui

unix: !macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

INCLUDEPATH += $$PWD/../libusb-1.0
INCLUDEPATH += ../SvNet
DEPENDPATH += $$PWD/../libusb-1.0

win32: LIBS += -L$$PWD/../libusb-1.0/ -llibusb-1.0.dll

HEADERS += \
  SvCompiler/SvCompiler.h \
  SvCompiler/SvVpuCompiler.h \
  SvHost/SvDir.h \
  SvHost/SvMirror.h \
  SvHost/SvMirrorCom.h \
  SvHost/SvMirrorExtern.h \
  SvHost/SvMirrorLocal.h \
  SvHost/SvMirrorManager.h \
  SvHost/SvMirrorRemote.h \
  SvHost/SvMirrorUsb10.h \
  SvHost/SvNetClientMirror.h \
  SvHost/SvNetHandlerFile.h \
  SvHost/SvProgramm.h \
  SvHost/SvVMachineLocal.h \
  SvNet/SvNetBridgeMaster.h \
  SvNet/SvNetBridgeSlave.h \
  SvNet/SvNetChannel.h \
  SvNet/SvNetChannelFile.h \
  SvNet/SvNetLocalMaster.h \
  SvNet/SvNetLocalSlave.h \
  SvNet/SvNetPacketInfo.h \
  SvNet/SvNetServer.h \
  SvNet/SvNetService.h \
  SvNet/SvNetServiceFileMaster.h \
  SvNet/SvNetServiceFileSlave.h \
  SvStudio/DNetProcess.h \
  SvStudio/DNetProcessFileDirRequest.h \
  SvStudio/DNetProcessFileOperation.h \
  SvStudio/DNetProcessFileRequest.h \
  SvStudio/DNetProcessFileSend.h \
  SvStudio/DPrjProp.h \
  SvStudio/DProcess.h \
  SvStudio/DTextEditorSettings.h \
  SvStudio/Highlighter.h \
  SvStudio/IngDebugCalculator.h \
  SvStudio/SvConfig.h \
  SvStudio/SvPeriodicParser.h \
  SvStudio/SvProject.h \
  SvStudio/SvUtils.h \
  SvStudio/WBrowser.h \
  SvStudio/WCChartLegend.h \
  SvStudio/WCModeBoard.h \
  SvStudio/WCModeEditor.h \
  SvStudio/WCModeHelp.h \
  SvStudio/WCModeIntro.h \
  SvStudio/WCommand.h \
  SvStudio/WDebugTable.h \
  SvStudio/WMain.h \
  SvStudio/WNetFileList.h \
  SvStudio/WNetFileManager.h \
  SvStudio/WNetViewer.h \
  SvStudio/WOscillograph.h \
  SvStudio/WTextEditor.h \
  SvStudio/WTextSearchPanel.h \
  SvVMachine/Sv6Plc.h \
  SvVMachine/Sv6Sys.h \
  SvVMachine/SvVMachine.h \
  SvVMachine/SvVmByteCode.h \
  SvVMachine/SvVmCodeHeader.h \
  SvVMachine/SvVmInterface.h \
  SvVMachine/SvVmTypes.h \
  SvVMachine/SvVmUtils.h \
  SvVMachine/SvVmVpu.h \
  SvVMachine/SvVmVpuState.h

SOURCES += \
  SvCompiler/SvClass.cpp \
  SvCompiler/SvCompilerConstExpression.cpp \
  SvCompiler/SvCompilerConstValue.cpp \
  SvCompiler/SvCompilerErrors.cpp \
  SvCompiler/SvCompilerExpression.cpp \
  SvCompiler/SvCompilerInit.cpp \
  SvCompiler/SvCompilerInputStream.cpp \
  SvCompiler/SvCompilerListing.cpp \
  SvCompiler/SvCompilerPreprocessor.cpp \
  SvCompiler/SvCompilerScaner.cpp \
  SvCompiler/SvCompilerSyntax.cpp \
  SvCompiler/SvFunction.cpp \
  SvCompiler/SvFunctionList.cpp \
  SvCompiler/SvFunctionType.cpp \
  SvCompiler/SvHelp.cpp \
  SvCompiler/SvOperator.cpp \
  SvCompiler/SvOperatorBlock.cpp \
  SvCompiler/SvOperatorReturn.cpp \
  SvCompiler/SvSource.cpp \
  SvCompiler/SvSourceFile.cpp \
  SvCompiler/SvType.cpp \
  SvCompiler/SvTypeList.cpp \
  SvCompiler/SvValue.cpp \
  SvCompiler/SvValueBinaryLong.cpp \
  SvCompiler/SvValueCall.cpp \
  SvCompiler/SvValueList.cpp \
  SvCompiler/SvValueMemberVariable.cpp \
  SvCompiler/SvValueVariable.cpp \
  SvCompiler/SvVariable.cpp \
  SvCompiler/SvVariableList.cpp \
  SvCompiler/SvVpuCompiler1.cpp \
  SvCompiler/SvVpuCompiler2.cpp \
  SvHost/SvDir.cpp \
  SvHost/SvMirror.cpp \
  SvHost/SvMirrorCom.cpp \
  SvHost/SvMirrorExtern.cpp \
  SvHost/SvMirrorLocal.cpp \
  SvHost/SvMirrorManager.cpp \
  SvHost/SvMirrorRemote.cpp \
  SvHost/SvMirrorUsb10.cpp \
  SvHost/SvNetClientMirror.cpp \
  SvHost/SvProgramm.cpp \
  SvHost/SvVMachineLocal.cpp \
  SvNet/SvNetBridgeMaster.cpp \
  SvNet/SvNetBridgeSlave.cpp \
  SvNet/SvNetChannel.cpp \
  SvNet/SvNetLocalMaster.cpp \
  SvNet/SvNetLocalSlave.cpp \
  SvNet/SvNetPacketInfo.cpp \
  SvNet/SvNetServer.cpp \
  SvNet/SvNetService.cpp \
  SvNet/SvNetServiceFileMaster.cpp \
  SvNet/SvNetServiceFileSlave.cpp \
  SvStudio/DNetProcess.cpp \
  SvStudio/DNetProcessFileDirRequest.cpp \
  SvStudio/DNetProcessFileOperation.cpp \
  SvStudio/DNetProcessFileRequest.cpp \
  SvStudio/DNetProcessFileSend.cpp \
  SvStudio/DPrjProp.cpp \
  SvStudio/DProcess.cpp \
  SvStudio/DTextEditorSettings.cpp \
  SvStudio/Highlighter.cpp \
  SvStudio/IngDebugCalculator.cpp \
  SvStudio/SvPeriodicParser.cpp \
  SvStudio/SvProject.cpp \
  SvStudio/SvUtils.cpp \
  SvStudio/WBrowser.cpp \
  SvStudio/WCModeBoard.cpp \
  SvStudio/WCModeEditor.cpp \
  SvStudio/WCModeHelp.cpp \
  SvStudio/WCModeIntro.cpp \
  SvStudio/WCommand.cpp \
  SvStudio/WDebugTable.cpp \
  SvStudio/WMain.cpp \
  SvStudio/WNetFileList.cpp \
  SvStudio/WNetFileManager.cpp \
  SvStudio/WNetViewer.cpp \
  SvStudio/WOscillograph.cpp \
  SvStudio/WTextEditor.cpp \
  SvStudio/WTextSearchPanel.cpp \
  SvStudio/WVisualEditor.cpp \
  SvStudio/bug.cpp \
  SvStudio/main.cpp \
  SvVMachine/SvVMachine.cpp


DISTFILES +=
