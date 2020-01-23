#include "WDPortSettings.h"
#include "ui_WDPortSettings.h"

WDPortSettings::WDPortSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::WDPortSettings)
  {
  ui->setupUi(this);
  }

WDPortSettings::~WDPortSettings()
  {
  delete ui;
  }
