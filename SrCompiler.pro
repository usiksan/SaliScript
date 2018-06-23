#
#  Проект     "Скриптовый язык reduced c++ (rc++) v6"
#  Подпроект  "Пико-компилятор"
#  Автор
#    Alexander Sibilev
#  Интернет
#    www.rc.saliLab.ru - домашний сайт проекта
#    www.saliLab.ru
#    www.saliLab.com
#
#  Описание
#    Пико компилятор скриптового языка rc++ и генератор байт-кода
#
#

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    SrCompiler/SrCompiler.h \
    SrCompiler/SrVpuCompiler.h \
    SrHost/SrProgramm.h

SOURCES += \
    SrCompiler/SrClass.cpp \
    SrCompiler/SrCompiler1.cpp \
    SrCompiler/SrCompiler2.cpp \
    SrCompiler/SrCompiler3.cpp \
    SrCompiler/SrCompiler4.cpp \
    SrCompiler/SrCompiler5.cpp \
    SrCompiler/SrCompiler6.cpp \
    SrCompiler/SrCompiler7.cpp \
    SrCompiler/SrCompiler8.cpp \
    SrCompiler/SrCompiler9.cpp \
    SrCompiler/SrCompilerA.cpp \
    SrCompiler/SrCompilerB.cpp \
    SrCompiler/SrFunction.cpp \
    SrCompiler/SrFunctionList.cpp \
    SrCompiler/SrFunctionType.cpp \
    SrCompiler/SrHelp.cpp \
    SrCompiler/SrOperator.cpp \
    SrCompiler/SrOperatorBlock.cpp \
    SrCompiler/SrOperatorReturn.cpp \
    SrCompiler/SrType.cpp \
    SrCompiler/SrTypeList.cpp \
    SrCompiler/SrValue.cpp \
    SrCompiler/SrValueBinaryLong.cpp \
    SrCompiler/SrValueCall.cpp \
    SrCompiler/SrValueList.cpp \
    SrCompiler/SrValueMemberVariable.cpp \
    SrCompiler/SrValueVariable.cpp \
    SrCompiler/SrVariable.cpp \
    SrCompiler/SrVariableList.cpp \
    SrCompiler/SrVpuCompiler1.cpp \
    SrCompiler/SrVpuCompiler2.cpp \
    SrHost/SrProgramm.cpp \
    SrCompiler/SrSource.cpp \
    SrCompiler/SrSourceFile.cpp \
    SrCompiler/SrCompilerMain.cpp

