/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "DProcess.h"
#include "ui_DProcess.h"
#include "WMain.h"

#include <QMetaObject>
#include <QMessageBox>
#include <QDebug>


DProcess::DProcess(QWidget *parent) :
  QDialog(parent),
  mCompleted(false),
  ui(new Ui::DProcess)
  {
  ui->setupUi(this);

  connect( svMirrorManager->mirror(), &SvMirror::transferProcess, this, &DProcess::onTransferProcess );
  connect( ui->mCancel, &QPushButton::click, this, &DProcess::close );
  }




DProcess::~DProcess()
  {
  delete ui;
  }






//Отображение процесса
void DProcess::onTransferProcess(bool complete, const QString msg)
  {
  mCompleted = complete;
  if( !complete ) {
    //Процесс еще идет

    //Добавить строку состояния в список
    ui->mHistory->addItem( msg );

    //Установить текущее состояние
    ui->mAction->setText( msg );
    }
  else {
    //Процесс завершен
    if( !msg.isEmpty() )
      QMessageBox::critical( this, tr("Error"), msg );
    done(0);
    }
  }







void DProcess::changeEvent(QEvent *e)
  {
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
    }
  }
