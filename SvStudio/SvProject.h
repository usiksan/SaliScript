/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
    Проект. Описывает состав узлов, размещение скриптов проекта и список файлов для каждого узла.
*/

#ifndef SVPROJECT_H
#define SVPROJECT_H

#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

//Ключи для полей проекта
#define WPK_FILE_LIST        QString("FileList")
#define WPK_VISUAL_MODE      QString("VisualMode")
#define WPK_VARIABLE_LIST    QString("VarList")
#define WPK_PROJECT_PATH     QString("ProjectPath")
#define WPK_DEBUG_TYPE       QString("DebugType")
#define WPK_REMOTE_IP        QString("RemoteIP")
#define WPK_REMOTE_PORT      QString("RemotePort")
#define WPK_GLOBAL_ID        QString("GlobalID")
#define WPK_COMPILER_TYPE    QString("CompilerType")
#define WPK_CTR_VERSION      QString("CtrVersion")
#define WPK_MAIN_SCRIPT      QString("MainScript")
#define WPK_VID              QString("USB VID")
#define WPK_PID              QString("USB PID")
#define WPK_CHART_VARIABLES  QString("ChartVarialbes")
#define WPK_CHART_AXES       QString("ChartAxes")

//Типы возможных зеркал
#define SMT_LOCAL  0 //Локальный контроллер
#define SMT_USB    1 //Контроллер, подключенный по USB
#define SMT_REMOTE 2 //Удаленное зеркало



//Представляет собой проект
//Часть глобальных данных раскодируется при загрузке и может использоваться глобально
//Дополнительно все данные проекта хранятся в объекте проекта и могут быть прочитаны
// и записаны соответсвующими модулями программы
struct SvProject : public QObject
  {
    Q_OBJECT

  public:
    QString                 mProjectName;  //Имя файла проекта (только имя без расширения, не сохраняем)
    QString                 mProjectPath;  //Каталог размещения файлов проекта (не сохраняем)

    qint32                  mDebugType;    //Тип отладчика
    QString                 mRemoteIp;     //IP адрес удаленной машины
    qint32                  mRemotePort;   //Порт удаленной машины
    QString                 mGlobalId;     //Глобальный идентификатор машины в интернете
    QString                 mPassw;        //Пароль машины в интернете
    qint32                  mVid;          //VID usb
    qint32                  mPid;          //PID usb
    QString                 mCompilerType; //Тип компилятора
    qint32                  mCtrVersion;   //Исполнение контроллера
    QString                 mMainScript;   //Основной скрипт узла (без пути, только имя)

    QJsonObject             mProjectSrc;   //Нераспакованные данные

    SvProject();
    ~SvProject();

    //Открыть проект из файла
    void          openProject( const QString fname );

    //Сохранить проект в файле
    void          saveProject();

    //Добавить проект к списку последних проектов
    void          addRecentProject( const QString fname );

    //Обновить список последних проектов
    void          updateRecentProjects();

    //Очистить список последних проектов
    void          clearRecentProjects();

    //Создать пустой проект
    void          createNew( const QString fname );
  signals:

    //Обновился список последних проектов
    void          recentProjectsList( QStringList list );

  private:
    //Заполнить поля проекта по-умолчанию
    void          setDefaults();
  };

extern SvProject *svProject; //Глобальный проект

#endif // SVPROJECT_H
