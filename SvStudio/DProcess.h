/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Отображение прогресс-бара.
*/

#ifndef DPROCESS_H
#define DPROCESS_H

#include <QDialog>
#include <QTimer>


namespace Ui {
  class DProcess;
  }

class DProcess : public QDialog
  {
    Q_OBJECT

    bool mLink;       //Выполнить ли подключение к программе
    bool mFlash;      //Выполнять прошивку или просто компиляцию
    bool mRunOrPause; //Запускать приложение или поставить на паузу
  public:
    //Конструктор, обеспечивающий компиляцию, прошивку и запуск
    explicit DProcess( bool link, bool flash, bool runOrPause, QWidget *parent = 0);

    //Конструктор, обеспечивающй отправку или прием проекта
    DProcess( bool sendOrReceiv, QWidget *parent = 0 );

    ~DProcess();

  signals:
    //Компилировать проект, программировать и запустить
    void compile( bool link, bool flash, bool runOrPause );

    //Передать проект в удаленную машину или принять проект из удаленной машины
    void transferProject();

  public slots:
    //При запуске процесса (вызывается когда диалог построен)
    void onStart();

    //При изменении состояния процесса
    void onProcessChanged( const QString status, bool processStatus, const QString error );
  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::DProcess *ui;
  };

#endif // DPROCESS_H
