/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef DPRJPROP_H
#define DPRJPROP_H

#include <QDialog>

namespace Ui {
  class DPrjProp;
  }

class DPrjProp : public QDialog
  {
    Q_OBJECT

  public:
    explicit DPrjProp( const QString &okTitle, QWidget *parent = nullptr);
    ~DPrjProp();

  public slots:
    //Выбрать файл для главного скрипта
    void selectScript();

    //Выбран интерфейс по локальной сети или интернет
    void selectedRemote();
  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::DPrjProp *ui;

    // QDialog interface
  public slots:
    virtual void accept();
  };

#endif // DPRJPROP_H
