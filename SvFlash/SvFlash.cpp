#include "SvFlash.h"
#include "ui_SvFlash.h"
#include "../SvStudio/Host/SvMirror.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

SvFlash::SvFlash(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvFlash)
  {
  ui->setupUi(this);

  ui->mStatus->setText( tr("Not connected") );

  //Попробуем найти файл для загрузки
  QDir dir( QCoreApplication::applicationDirPath() );
  QFileInfoList list = dir.entryInfoList( { QString("*.vpu5prg") } );
  if( !list.isEmpty() )
    ui->mFile->setText( list.first().filePath() );

  connect( this, &SvFlash::compileFlashRun, svMirror, &SvMirror::compileFlashRun );
  connect( svMirror, &SvMirror::linkStatusChanged, this, &SvFlash::onLinkStatus );
  connect( svMirror, &SvMirror::processChanged, this, &SvFlash::onProcessing );
  connect( ui->mFlash, &QPushButton::clicked, this, &SvFlash::onFlash );
  connect( ui->mSelect, &QPushButton::clicked, this, &SvFlash::onSelectFile );
  //При нажатии кнопки Выход - закрываем окно
  connect( ui->mExit, &QPushButton::clicked, this, &SvFlash::close );
  }



SvFlash::~SvFlash()
  {
  delete ui;
  }



//Выбор файла для загрузки
void SvFlash::onSelectFile()
  {
  ui->mFlashStatus->clear();
  QString fname = QFileDialog::getOpenFileName( this, tr("Выберите файл для прошивки"), QString(), tr("Файлы прошивки (*.vpu5prg)") );
  if( !fname.isEmpty() ) {
    //Сохранить выбранный файл в диалоге
    ui->mFile->setText( fname );
    }
  }




//Прошивка
void SvFlash::onFlash()
  {
  //Получить файл для загрузки
  QString fname = ui->mFile->text();
  QFileInfo info( fname );
  if( info.exists() ) {
    ui->mFlashStatus->clear();
    svMirror->settings( info.path() + QChar('/'), info.fileName(), QString("62.109.11.83"), 1971, QString(), QString(), 0x2743, 0x1971 );
    emit compileFlashRun( false, true, true );
    }
  else QMessageBox::warning( this, tr("Внимание!"), tr("Указанный файл не существует. Выберите другой.") );
  }



//При изменении состояния подключения
void SvFlash::onLinkStatus(bool link, QString state)
  {
  Q_UNUSED(link);
  ui->mStatus->setText( state );
  }




//При изменении процесса
void SvFlash::onProcessing(const QString status, bool processStatus, const QString error)
  {
  Q_UNUSED(processStatus);

  ui->mFlashStatus->setText( status + QString("  ") + error );
  }
