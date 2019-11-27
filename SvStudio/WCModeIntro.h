/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef CMODEINTRO_H
#define CMODEINTRO_H

#include <QWidget>
#include <QStringList>
#include "WBrowser.h"

class WCModeIntro : public WBrowser
  {
    Q_OBJECT
  public:
    explicit WCModeIntro(QWidget *parent = nullptr);
    ~WCModeIntro();

  signals:
    void openProject();
    void projectOpen( const QString fname );
    void newProject();

  public slots:
    void onProjectListChanged( const QStringList list );

    // WBrowser interface
  public slots:
    virtual void onLinkString(const QString &link);
  };

#endif // CMODEINTRO_H
