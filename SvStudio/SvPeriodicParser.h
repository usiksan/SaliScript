/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef SVPERIODICPARSER_H
#define SVPERIODICPARSER_H

#include <QThread>
#include <QTimer>
#include <QStringList>

namespace SvCompiler6 {
class SvCompiler;
};

class WCModeEditor;
class SvPeriodicParser : public QThread
  {
    Q_OBJECT

    SvCompiler6::SvCompiler    *mCompiler;
    SvCompiler6::SvCompiler    *mCompiler0;
    SvCompiler6::SvCompiler    *mCompiler1;
    WCModeEditor               *mEditor;
    bool                        mNeedCompile;
    QStringList                 mFileList;
  public:
    SvPeriodicParser( WCModeEditor *editor );
    ~SvPeriodicParser();


  signals:
    void compileComplete();

  public slots:
    void needCompile();

    // QThread interface
  protected:
    virtual void run();

  private:
    void compile();
  };

#endif // SVPERIODICPARSER_H
