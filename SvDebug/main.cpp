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

#include "SvHost/7bit.h"

unsigned addr = 0;
int val  = 0;
short sval = 0;
char cval = 0;

int main(int argc, char *argv[])
  {
  char d[20];
  w32to7( 0xc765432f, 0xffffffff, d );
  //w32to7( 0xffffffff, 0, d );
  addr = parse7to32( d, &val );

  w16to7( 0xc765432f, 0xffff, d );
  addr = parse7to16( d, &sval );

  w8to7( 0xc765432f, 0xff, d );
  addr = parse7to8( d, &cval );

  //Установки для хранения настроек
  QCoreApplication::setOrganizationName("SaliLAB");
  QCoreApplication::setOrganizationDomain("http://salilab.com/");
  QCoreApplication::setApplicationName("SvDebug");

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
