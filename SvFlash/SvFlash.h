#ifndef SVFLASH_H
#define SVFLASH_H

#include <QDialog>

class SvMirror;
extern SvMirror *svMirror;

namespace Ui {
  class SvFlash;
}

class SvFlash : public QDialog
  {
    Q_OBJECT

  public:
    explicit SvFlash(QWidget *parent = nullptr);
    ~SvFlash();

  signals:
    void compileFlashRun( bool link, bool flash, bool runOrPause );

  public slots:
    //Выбор файла для загрузки
    void onSelectFile();

    //Прошивка
    void onFlash();

    //При изменении состояния подключения
    void onLinkStatus( bool link, QString state );

    //При изменении процесса
    void onProcessing( const QString status, bool processStatus, const QString error );

  private:
    Ui::SvFlash *ui;
  };

#endif // SVFLASH_H
