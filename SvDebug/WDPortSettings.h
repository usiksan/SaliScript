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
  };

#endif // WDPORTSETTINGS_H
