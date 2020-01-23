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
  SvCompiler/SvCompiler.h \
  SvCompiler/SvVpuCompiler.h \
  SvHost/SvProgramm.h

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
  SvCompilerMain.cpp \
  SvHost/SvProgramm.cpp

FORMS +=

