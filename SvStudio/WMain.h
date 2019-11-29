/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef WMAIN_H
#define WMAIN_H

#include "SvConfig.h"
#include "WCModeIntro.h"
#include "WCModeBoard.h"
#include "WCModeEditor.h"
#include "WCModeHelp.h"
#include "SvHost/SvMirrorManager.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>

extern SvMirrorManager *svMirrorManager;

class WMain : public QMainWindow
  {
    Q_OBJECT

    WCModeIntro     *mCModeIntro;  //Режим. Быстрая навигация по предыдущим проектам
    WCModeBoard     *mCModeBoard;  //Режим. Обмен файлами с целевым компьютером
    WCModeEditor    *mCModeEditor; //Режим. Текстовые редакторы и отладчик
    WCModeHelp      *mCModeHelp;   //Режим. Система помощи
    QStackedWidget  *mCentral;     //Центральная область - стековый widget, с помощью которого переключаемся между режимами
  public:
    WMain( QWidget *parent = nullptr );
    ~WMain();

    void restorePositions();

  signals:
    void setMirror( int mirrorType );

    void setProgrammFlashRun( SvProgrammPtr prog, bool link, bool flash, bool runOrPause );

  public slots:
    void modeIntro();
    void modeBoard();
    void modeEditor();
    void modeChart();
    void modeHelp();

    void fileOpen();
    void fileOpenProject();
    void fileSave();
    void fileSaveAs();
    void fileNew();
    void fileNewProject();
    void fileRecentProject();
    void fileRecentFile();
    void fileSendProject();
    void fileReceivProject();
    void fileSaveProject();
    void fileCloseAll();
    void fileClose();
    void fileProjectOpen( const QString fname );

    void editUndo();
    void editRedo();
    void editCopy();
    void editCut();
    void editPaste();
    void editSelectAll();
    void editSearchText();
    void editReplaceText();
    void editAutoIndent();
    void editAutoIndentSettings();
    void editSvQmlGeneration();

    void chartsClear();

    void debugMode();

    void buildBuild();
    void buildAndRun();
    void buildAndPause();
    void buildAndLink();


    void helpContext();
    void helpContens();
    void helpWeb();
    void helpAbout();

    //Установлен новый тип зеркала
    void onMirrorChanged( int id, SvMirrorPtr mirror );

    //Активировать режим, подсветить соответствующую кнопку
    void activateModeIntro();
    void activateModeBoard();
    void activateModeEditor();
    void activateModeHelp();

  private:

    void compile(bool link, bool flash, bool runOrPause );

    //Установить заголовок в соотвествии с проектом
    void setupTitle();

    //Истина, если текущий режим текстовый
    bool isText() const;


    // QWidget interface
  protected:
    virtual void closeEvent(QCloseEvent *ev);

  };

#endif // WMAIN_H
