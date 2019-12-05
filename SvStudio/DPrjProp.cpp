/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "DPrjProp.h"
#include "ui_DPrjProp.h"
#include "SvProject.h"
#include "SvHost/SvMirror.h"

#include <QFileDialog>
#include <QFileInfo>

DPrjProp::DPrjProp(const QString &okTitle, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DPrjProp)
  {
  ui->setupUi(this);

  //Установить свойства в соответствии с проектом
  ui->mDebugSimulator->setChecked( svProject->mDebugType == SMT_LOCAL );
  ui->mDebugUsb->setChecked( svProject->mDebugType == SMT_USB );
  ui->mDebugRemote->setChecked( svProject->mDebugType == SMT_REMOTE );

  ui->mMainScript->setText( svProject->mMainScript );
  ui->mMainScript->setToolTip( tr("This file is start point to compile script files") );

  ui->mIp->setText( svProject->mRemoteIp );
  ui->mIp->setToolTip( tr("IP address of remote computer in net") );

  ui->mPort->setText( QString::number(svProject->mRemotePort) );
  ui->mPort->setToolTip( tr("Port of connection on remote computer in net") );

  ui->mId->setText( svProject->mGlobalId );
  ui->mId->setToolTip( tr("Global id of remote computer. On connections via internet.") );

  ui->mPassw->setText( svProject->mPassw );
  ui->mPassw->setToolTip( tr("Password of remote computer. On connections via internet.") );

  ui->mVid->setText( QString::number( svProject->mVid, 16 ) );
  ui->mVid->setToolTip( tr("VID value. On USB connection.") );

  ui->mPid->setText( QString::number( svProject->mPid, 16 ) );
  ui->mPid->setToolTip( tr("PID value. On USB connection.") );

  ui->mOk->setText( okTitle );

  //Связать выбор типа устройства с предустановленными свойствами
  connect( ui->mDebugRemote, &QRadioButton::clicked, this, &DPrjProp::selectedRemote );

  connect( ui->mCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect( ui->mOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect( ui->mSelectScript, SIGNAL(clicked()), this, SLOT(selectScript()) );
  }



DPrjProp::~DPrjProp()
  {
  delete ui;
  }


//Выбрать файл для главного скрипта
void DPrjProp::selectScript()
  {
  QString title = QFileDialog::getOpenFileName( this, tr("Main script file"), svProject->mProjectPath + ui->mMainScript->text(), "C, C++, script files (*.c *.cpp *" EXTENSION_SCRIPT ")");
  if( !title.isEmpty() )
    ui->mMainScript->setText( title );

  }





//Выбран интерфейс по локальной сети
void DPrjProp::selectedRemote()
  {
  ui->mIp->setText( DEFAULT_INTERNET_IP );
  ui->mPort->setText( QString::number( DEFAULT_REMOTE_PORT ) );
  }




void DPrjProp::accept()
  {
  //Установить проект в соответствии с установленными значениями
  if( ui->mDebugSimulator->isChecked() )
    svProject->mDebugType = SMT_LOCAL;
  else if( ui->mDebugUsb->isChecked() )
    svProject->mDebugType = SMT_USB;
  else if( ui->mDebugRemote->isChecked() )
    svProject->mDebugType = SMT_REMOTE;

  svProject->mMainScript = ui->mMainScript->text();

  svProject->mRemoteIp = ui->mIp->text();

  svProject->mRemotePort = ui->mPort->text().toInt();

  svProject->mGlobalId = ui->mId->text();

  svProject->mPassw = ui->mPassw->text();

  svProject->mVid = ui->mVid->text().toInt(0,16);
  svProject->mPid = ui->mPid->text().toInt(0,16);

  QDialog::accept();
  }


void DPrjProp::changeEvent(QEvent *e)
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
