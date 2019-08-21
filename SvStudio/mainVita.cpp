/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Приложение для использования стандартных sml
    При запуске sml берет из каталога sml/main.sml
    Выбор места для скрипта и способ запуска определяется свойством в main.sml
*/
#include "vitaConfig.h"
#include "SgSml/SgVitaWidget.h"
#include "Host/SvBuilder.h"
#include <QApplication>

int main(int argc, char *argv[])
  {
  QApplication a(argc, argv);
  SvBuilder();
  SgVitaWidget sw( QString("") );
  sw.setWindowTitle( QString("SaliLAB SgVita v%1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR) );
  sw.resize( 800, 480 );
  sw.show();
  sw.setSml( BASE_SML );

  return a.exec();
  }
