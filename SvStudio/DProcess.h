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

    bool mCompleted;
  public:
    //Конструктор
    explicit DProcess( QWidget *parent = nullptr);

    ~DProcess();

    bool isCompleted() const { return mCompleted; }
  public slots:
    //При запуске процесса (вызывается когда диалог построен)
    void onStart();

    //При изменении состояния процесса
    void onTransferProcess( bool complete, const QString msg );
  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::DProcess *ui;
  };

#endif // DPROCESS_H
