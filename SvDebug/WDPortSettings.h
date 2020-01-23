/*
  Проект "SvDebug"
    IDE отладки проектов в динамике
  Автор
    Сибилев А.С.
  Описание
    Диалоговое окно настроек порта
*/
#ifndef WDPORTSETTINGS_H
#define WDPORTSETTINGS_H

#include <QDialog>

namespace Ui {
  class WDPortSettings;
}

class WDPortSettings : public QDialog
  {
    Q_OBJECT

  public:
    explicit WDPortSettings(QWidget *parent = nullptr);
    ~WDPortSettings();

  private:
    Ui::WDPortSettings *ui;

    // QDialog interface
  public slots:
    virtual void accept() override;
  };

#endif // WDPORTSETTINGS_H
