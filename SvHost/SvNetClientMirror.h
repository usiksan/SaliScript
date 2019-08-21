/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  Net client for communicating with remote mirror.
*/
#ifndef SVNETCLIENTMIRROR_H
#define SVNETCLIENTMIRROR_H

#include "SvNetClientFile.h"
#include "SvMirror.h"

//Команды для обмена управлением
#define SVC_PROJECT_DIR_GET      (SVC_FILE_LAST + 1) //Получить директорий проекта
#define SVC_PROJECT_DIR          (SVC_FILE_LAST + 2) //Директорий проекта

#define SVC_COMPILE_FLASH_RUN    (SVC_FILE_LAST + 3) //Отправить программу с указаниями

#define SVC_VAR_GET              (SVC_FILE_LAST + 4) //Запрос на получение переменных
#define SVC_VAR_LIST             (SVC_FILE_LAST + 5) //Список переменных (снимок памяти)
#define SVC_VAR_SET              (SVC_FILE_LAST + 6) //Установить переменные (до 8 штук на пакет)

#define SVC_TASK_GET             (SVC_FILE_LAST + 7) //Запрос на получение списка задач

#define SVC_LOG                  (SVC_FILE_LAST + 8) //Отправка лога

#define SVC_RESTART              (SVC_FILE_LAST + 9) //Рестарт программы

#define SVC_VPU_STATE_GET        (SVC_FILE_LAST + 10) //Запрос информации о состоянии vpu (максимальное количество vpu,
                                                      //максимальное количество памяти, количество задач и их состояние,
                                                      //состояние памяти
#define SVC_VPU_STATE            (SVC_FILE_LAST + 11) //Состояние vpu согласно запроса

//Данные флаги служат для упаковки сообщения о компиляции
#define SP_LINK                  0x1                 //Флаг линковки программы
#define SP_FLASH                 0x2                 //Флаг прошивки программы
#define SP_RUN_OR_PAUSE          0x4                 //Запустить программу или поставить на паузу

class SvNetClientMirror : public SvNetClientFile
  {
    Q_OBJECT

    SvMirror *mMirror;      //Зеркало, обслуживаемое данным агентом в режиме агента
  public:
    SvNetClientMirror(SvMirror *mirror = nullptr, int passw = 0, int id = 0, QString ip = QString("62.109.11.83"), int port = 1971, bool en = true, QObject *parent = 0);

    void setMirror( SvMirror *mirror );
  signals:
    //Выполнить компиляцию и прошивку
    void receivedCompileFlashRun( bool link, bool flash, bool runOrPause );

    //Пришло сообщение с директорием проекта
    void receivedProjectDir( const QString dir );

    //Пришли переменные
    void receivedVars( const QByteArray v );

    //Сообщить о получении log-сообщения
    void receivedLog( const QString msg );

    //Пришла команда рестарта
    void receivedRestart( bool run );

    //Пришло состояние vpu
    void receivedVpuState( const QByteArray v );
  public slots:
    //Отправить запрос на получение директория проекта
    void getProjectDir();

    //Отправить запрос на получение переменных
    void receivVars();

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    void compileFlashRun( bool link, bool flash, bool runOrPause );

    //Передать установленные переменные
    void sendVars( const QByteArray array );

    //Прием логического сообщения из зеркала
    void onLog( const QString msg );

    //Отправить рестарт программы
    void restart( bool run );

    //Отправить запрос на получение информации о VPU
    void sendVpuStateGet();

    // SvNetClient interface
  protected:
    virtual void        onReadControl(SvNetPacketBlock &pk) override;
    virtual const char *getTitle() const override;
    virtual void        clearLink() override;

    // SvNetClientFile interface
  protected:
    virtual bool operation(const SvNetOperation &op, QString &opDescr) override;
  };


extern SvNetClientMirror *svNetClientMirror;

#endif // SVNETCLIENTMIRROR_H
