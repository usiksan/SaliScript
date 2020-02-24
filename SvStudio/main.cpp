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
#include "SvHost/SvMirrorManager.h"
#include "SvHost/SvMirrorLocal.h"
#include "SvHost/SvMirrorRemote.h"
#include "SvCompiler/SvVpuCompiler.h"
#include "SvNet/SvNetServer.h"
#include "SvNet/SvNetHandlerMirror.h"

#include <QApplication>
#include <QSettings>

SvMirrorManager *svMirrorManager;

int main(int argc, char *argv[])
  {
  //Установки для хранения настроек
  QCoreApplication::setOrganizationName("SaliLAB");
  QCoreApplication::setOrganizationDomain("http://salilab.com/");
  QCoreApplication::setApplicationName("SvStudio");

  //Создание приложения
  QApplication a(argc, argv);

  qRegisterMetaType<SvMirrorPtr>("SvMirrorPtr");

  //Создать рабочий проект
  svProject = new SvProject();

  //Создать менеджер зеркал
  svMirrorManager = new SvMirrorManager();
  svMirrorManager->addMirrorFabric( SMT_LOCAL, [] () { return new SvMirrorLocal( new SvVMachineLocal() ); } );
  svMirrorManager->addMirrorFabric( SMT_REMOTE, [] () { return new SvMirrorRemote(); } );

  QSettings s;

  //Сервер для доступа к зеркалу по локальной сети
  //SvNetServer *localServer = new SvNetServer( 1971 );

  //Обработчик сетевого управления
  //SvNetHandlerMirror *handlerMirror = new SvNetHandlerMirror();

  //Соединения
  //localServer->connect( localServer, &SvNetServer::addNetChannel, handlerMirror, &SvNetHandlerMirror::addNetChannel );
  //handlerMirror->connect( svMirrorManager, &SvMirrorManager::mirrorChanged, handlerMirror, &SvNetHandlerMirror::mirrorChanged );

  //Создание главного окна приложения
  WMain w;
  if( s.value(QString(CFG_WMAIN_MAX)).toBool() )
    w.showMaximized();
  else
    w.show();


  //Запустить отладчик на исполнение
//  svMirrorManager->start();

  int res = a.exec();

  //Остановить работу сети
  svMirrorManager->deleteLater();
  //svMirrorManager->stop();

  //Остановить работу удаленного управления
  //localServer->deleteLater();

//  svNetClientMirror->stopClient();

  //Сохранить настройки удаленного управления
//  s.setValue( QStringLiteral(CFG_BRIDGE_PASSW), svNetClientMirror->getPassw() );
//  s.setValue( QStringLiteral(CFG_BRIDGE_ID),    svNetClientMirror->getId() );
//  s.setValue( QStringLiteral(CFG_BRIDGE_IP),    svNetClientMirror->getIp() );
//  s.setValue( QStringLiteral(CFG_BRIDGE_PORT),  svNetClientMirror->getPort() );

//  delete svNetClientMirror;

  return res;
  }
