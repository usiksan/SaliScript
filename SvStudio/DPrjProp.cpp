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

  ui->mControllerName->setText( svProject->mControllerName );
  ui->mControllerName->setToolTip( tr("Controller name through USB or global id of remote computer on connections via internet.") );

  ui->mControllerPassw->setText( svProject->mControllerPassw );
  ui->mControllerPassw->setToolTip( tr("Password of remote computer. On connections via internet.") );

  ui->mOk->setText( okTitle );

  //Связать выбор типа устройства с предустановленными свойствами
  connect( ui->mDebugRemote, &QRadioButton::clicked, this, &DPrjProp::selectedRemote );
  connect( ui->mDebugBridge, &QRadioButton::clicked, this, [this] () {
    ui->mIp->setText( DEFAULT_INTERNET_IP );
    ui->mPort->setText( QString::number( DEFAULT_REMOTE_PORT ) );
    });

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
  ui->mIp->setText( QStringLiteral("127.0.0.1") );
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

  svProject->mControllerName = ui->mControllerName->text();

  svProject->mControllerPassw = ui->mControllerPassw->text();

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
