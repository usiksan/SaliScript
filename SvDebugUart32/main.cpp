/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Точка входа в программу.
*/
#include "SvConfig.h"
#include "WMain.h"
#include <QApplication>
#include <QSettings>


int main(int argc, char *argv[])
  {

  //Установки для хранения настроек
  QCoreApplication::setOrganizationName("SaliLAB");
  QCoreApplication::setOrganizationDomain("http://salilab.com/");
  QCoreApplication::setApplicationName("SvDebugUart32");

  //Создание приложения
  QApplication a(argc, argv);

  QSettings s;

  //Создание главного окна приложения
  WMain w;
  if( s.value(QString(CFG_WMAIN_MAX)).toBool() )
    w.showMaximized();
  else
    w.show();


  int res = a.exec();

  return res;
  }
