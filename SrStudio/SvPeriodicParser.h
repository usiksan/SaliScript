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

class SvCompiler;
class WCModeEditor;
class SvPeriodicParser : public QThread
  {
    Q_OBJECT

    SvCompiler    *mCompiler;
    SvCompiler    *mCompiler0;
    SvCompiler    *mCompiler1;
    WCModeEditor *mEditor;
    bool          mNeedCompile;
    QStringList   mFileList;
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
