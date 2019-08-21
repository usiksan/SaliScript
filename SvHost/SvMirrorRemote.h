/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Зеркало для внешнего VPU, работающего через машину в локальной сети.
*/

#ifndef SVMIRRORREMOTE_H
#define SVMIRRORREMOTE_H

#include "SvMirrorExtern.h"
#include "SvNetClientMirror.h"

#include <QStringList>

struct SvRemoteCmd {
    int mCmd;
    int mStart;
  };

class SvMirrorRemote : public SvMirrorExtern
  {
    Q_OBJECT
  protected:
    int         mRemoteId;    //Id удаленной машины
    int         mRemotePassw; //Passw удаленной машины
    int         mTime;
    QString     mRemoteDir;   //Директорий проекта на удаленной машине
  public:
    SvMirrorRemote( bool scanTasks );
    ~SvMirrorRemote();


    //Тип зеркала
    virtual int         mirrorType() const override { return SMT_REMOTE; }

    //Настроить зеркало
    virtual void        settings( const QString prjPath, const QString mainScript,
                                  const QString ip, int port,
                                  const QString globalName, const QString globalPassw,
                                  int vid, int pid ) override;

    //Выполнить обработку узла
    virtual void        processing( int tickOffset ) override;

  signals:
    //Подключиться к мосту
    void linkToBridge();

    //Подключиться к удаленному узлу
    void linkTo( int id, int passw );

    //Получить директорий проекта
    void getProjectDir();

    //Отправить список операций
    void pushList( const SvNetOperationList list );

    //Отправить запрос на получение переменных
    void receivVars();

    //Отправить запрос на получение информации текущего состояния vpu
    void sendVpuStateGet();

    //Отправить программу
    void sendCompileFlashRun( bool link, bool flash, bool runOrPause );

    //Передать массив переменных с адресами
    void sendVars( const QByteArray ar );

    //Передать старт
    void sendRestart( bool run );

  protected:
    //Выполнить обмен с узлом
    void changeData();

    //Получить состояние задач
    void receivTask();

    // SvMirror interface
  public slots:
    //===========================
    //Раздел управления

    //Отправить содержимое проекта
    virtual void        sendProject() override;

    //Получить содержимое проекта
    virtual void        receivProject() override;

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void        restart( bool runOrPause ) override;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void        compileFlashRun( bool link, bool flash, bool runOrPause ) override;

    //Изменение состояния связи
    void linkStatus( int id, int passw, QString bridgeName, QString linkName );

    //Переменные получены
    void receivedVars( const QByteArray v );

    //Получен список состояний задач
    void receivedTasks( const QByteArray v );

    //Получен лог
    void receivedLog( const QString msg );

    //Получено состояние vpu
    void receivedVpuState( const QByteArray v );
  };

#endif // SVMIRRORREMOTE_H
