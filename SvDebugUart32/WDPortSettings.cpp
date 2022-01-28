/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Диалоговое окно настроек порта
*/
#include "SvConfig.h"
#include "WDPortSettings.h"
#include "ui_WDPortSettings.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>

WDPortSettings::WDPortSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::WDPortSettings)
  {
  ui->setupUi(this);

  //Получить список доступных портов
  QDir dir("/dev/");
  QFileInfoList portList = dir.entryInfoList( {"ttyUSB*"} );
  for( const QFileInfo &info : portList ) {
    //Добавить очередной порт к визуальному списку
    ui->mPortName->addItem( info.filePath() );
    }

  //Добавить скорости
  ui->mBaudRate->addItems( {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" } );

  //Достать исходные значения параметров
  QSettings s;
  ui->mPortName->setEditText( s.value( CFG_DEBUG_PORT, QString("/dev/ttyUSB0") ).toString() );
  ui->mBaudRate->setEditText( s.value( CFG_DEBUG_BAUDRATE, QString("115200") ).toString() );
  }



WDPortSettings::~WDPortSettings()
  {
  delete ui;
  }





void WDPortSettings::accept()
  {
  //Записать значения параметров
  QSettings s;
  s.setValue( CFG_DEBUG_PORT, ui->mPortName->currentText() );
  s.setValue( CFG_DEBUG_BAUDRATE, ui->mBaudRate->currentText() );
  QDialog::accept();
  }
