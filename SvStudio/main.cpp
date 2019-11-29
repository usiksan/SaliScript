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
#include "SvCompiler/SvVpuCompiler.h"
#include "Host/SvNetClientMirror.h"
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

  //Создать рабочий проект
  svProject = new SvProject();

  //Создать менеджер зеркал
  svMirrorManager = new SvMirrorManager();

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
  svMirrorManager->start();

  int res = a.exec();

  //Остановить работу сети
  svMirrorManager->stop();

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
