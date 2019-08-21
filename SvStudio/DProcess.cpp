/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "DProcess.h"
#include "ui_DProcess.h"
#include "SvDebugThread.h"

#include <QMetaObject>
#include <QMessageBox>
#include <QDebug>

DProcess::DProcess(bool link, bool flash, bool runOrPause, QWidget *parent) :
  QDialog(parent),
  mLink(link),
  mFlash(flash),
  mRunOrPause(runOrPause),
  ui(new Ui::DProcess)
  {
  ui->setupUi(this);

  connect( this, &DProcess::compile, SvDebugThread::mClient, &SvMirror::compileFlashRun );
  connect( SvDebugThread::mClient, &SvMirror::processChanged, this, &DProcess::onProcessChanged );
  connect( ui->mCancel, &QPushButton::click, this, &DProcess::close );
  QTimer::singleShot( 30, this, &DProcess::onStart );
  }


//Конструктор, обеспечивающй отправку или прием проекта
DProcess::DProcess(bool sendOrReceiv, QWidget *parent) :
  QDialog(parent),
  mLink(false),
  mFlash(false),
  mRunOrPause(false),
  ui(new Ui::DProcess)
  {
  ui->setupUi(this);

  //В зависимости от флага прием-передача проводим связывание либо со слотом приема либо со слотом передачи
  if( sendOrReceiv )
    connect( this, &DProcess::transferProject, SvDebugThread::mClient, &SvMirror::sendProject );
  else
    connect( this, &DProcess::transferProject, SvDebugThread::mClient, &SvMirror::receivProject );
  connect( SvDebugThread::mClient, &SvMirror::processChanged, this, &DProcess::onProcessChanged );
  connect( ui->mCancel, &QPushButton::click, this, &DProcess::close );
  QTimer::singleShot( 30, this, &DProcess::onStart );
  }




DProcess::~DProcess()
  {
  delete ui;
  }



//При запуске процесса (вызывается когда диалог построен)
void DProcess::onStart()
  {
  //Отправить сигнал, чтобы вызвать компиляцию
  emit compile( mLink, mFlash, mRunOrPause );
  //Отправить сигнал, чтобы вызвать передачу проекта
  emit transferProject();
  }





//Установка диапазона
void DProcess::onProcessChanged( const QString status, bool processStatus, const QString error )
  {
  if( processStatus ) {
    //Процесс еще идет

    //Добавить строку состояния в список
    ui->mHistory->addItem( status );

    //Установить текущее состояние
    ui->mAction->setText( status );
    }
  else {
    //Процесс завершен
    if( !error.isEmpty() )
      QMessageBox::critical( this, tr("Error"), error );
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
