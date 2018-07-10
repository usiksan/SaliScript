/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Точка входа в программу.

    Создание фабрики компиляторов, добавление компиляторов
*/
#include "SvConfig.h"
#include "WMain.h"
#include "SvProject.h"
#include "SvDebugThread.h"
#include "Compiler/SvVpuCompiler.h"
#include "Host/SvNetClientMirror.h"
#include <QApplication>
#include <QSettings>


int main(int argc, char *argv[])
  {
  //Установки для хранения настроек
  QCoreApplication::setOrganizationName("SaliLAB");
  QCoreApplication::setOrganizationDomain("http://salilab.com/");
  QCoreApplication::setApplicationName("SvStudio");



  //Создание приложения
  QApplication a(argc, argv);

  //Создать рабочий проект
  svProject = new SvProject();

  //Создать отладчик
  new SvDebugThread();
#ifdef Q_OS_LINUX
  SvDebugThread::mThread->mUseNewLibusb  = true;
#endif
  for (auto i=0; i<argc; ++i){

    if (!strcmp(argv[i], "-libusb10")){
      SvDebugThread::mThread->mUseNewLibusb = true;
      }
    if (!strcmp(argv[i], "-libusb01")){
      SvDebugThread::mThread->mUseNewLibusb = false;
      }
    }



  QSettings s;

  svNetClientMirror = new SvNetClientMirror( nullptr,
                                             0, //s.value( QStringLiteral(CFG_BRIDGE_PASSW) ).toInt(),
                                             0, //s.value( QStringLiteral(CFG_BRIDGE_ID) ).toInt(),
                                             s.value( QStringLiteral(CFG_BRIDGE_IP), QString(DEFAULT_INTERNET_IP) ).toString(),
                                             s.value( QStringLiteral(CFG_BRIDGE_PORT), DEFAULT_REMOTE_PORT ).toInt() );

  //Создание главного окна приложения
  WMain w;
  if( s.value(QString(CFG_WMAIN_MAX)).toBool() )
    w.showMaximized();
  else
    w.show();

  //w.restorePositions();

  //Запустить отладчик на исполнение
  SvDebugThread::mThread->start();

  int res = a.exec();

  //Остановить работу сети
  SvDebugThread::mThread->quit();
  SvDebugThread::mThread->wait();

  //Остановить работу удаленного управления
  svNetClientMirror->stopClient();

  //Сохранить настройки удаленного управления
  s.setValue( QStringLiteral(CFG_BRIDGE_PASSW), svNetClientMirror->getPassw() );
  s.setValue( QStringLiteral(CFG_BRIDGE_ID),    svNetClientMirror->getId() );
  s.setValue( QStringLiteral(CFG_BRIDGE_IP),    svNetClientMirror->getIp() );
  s.setValue( QStringLiteral(CFG_BRIDGE_PORT),  svNetClientMirror->getPort() );

  delete svNetClientMirror;

  return res;
  }
