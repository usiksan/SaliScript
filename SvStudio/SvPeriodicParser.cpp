/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "SvPeriodicParser.h"
#include "Compiler/SvCompiler.h"
#include "SvProject.h"
#include "WCModeEditor.h"
#include "Highlighter.h"

#include <QAbstractEventDispatcher>
#include <QDebug>

class TPeriodicCompiler : public SvCompiler {
    WCModeEditor *mEditor;
  public:
    TPeriodicCompiler( WCModeEditor *editor ) : SvCompiler(), mEditor(editor) {}



    // TCompiler interface
  public:
    virtual SvSource *CreateSource(const QString &fname, bool current);

    // SvCompiler interface
  public:
    virtual SvProgrammPtr make(const QString prjPath, const QString &mainScript);
  };




SvProgrammPtr TPeriodicCompiler::make(const QString prjPath, const QString &mainScript)
  {
  Q_UNUSED(prjPath)
  Q_UNUSED(mainScript)
  return SvProgrammPtr();
  }



SvSource *TPeriodicCompiler::CreateSource(const QString &fname, bool current)
  {
  QString file;
  if( current )
    file = mProjectPath + fname;
  else {
    //Вначале искать в пути проекта
    if( QFile::exists( mProjectPath + fname ) )
      file = mProjectPath + fname;
    else {
      //Искать в списке системных путей
      foreach( QString s, mSystemPathList )
        if( QFile::exists( s + fname ) ) {
          file = s + fname;
          break;
          }
      }
    }

  bool present;
  //Пробуем найти файл среди открытых файлов
  QString src = mEditor->getEditorText( file, present );

  if( present ) {
    return new SvStringSource(src, AddFile(file) );
    }
  return SvCompiler::CreateSource( fname, current );
  }


SvPeriodicParser::SvPeriodicParser(WCModeEditor *editor) :
  QThread(),
  mCompiler(0),
  mEditor(editor),
  mNeedCompile(false)
  {

  }

SvPeriodicParser::~SvPeriodicParser()
  {
  if( mCompiler ) delete mCompiler;
  }

void SvPeriodicParser::needCompile()
  {
  mNeedCompile = true;
  }



void SvPeriodicParser::run()
  {
  while( !isInterruptionRequested() ) {
    //обработать события (вызвать слоты если нужно)
    eventDispatcher()->processEvents(QEventLoop::AllEvents);
    //перекомпилить проект
    if( mNeedCompile ) {
      mNeedCompile = false;
      compile();
      mEditor->parsingComplete();
      emit compileComplete();
      }
    //пропуск чтобы не тормозило
    yieldCurrentThread();
    }
  }

void SvPeriodicParser::compile()
  {
  //Если еще нет компилятора, то создать
  if( mCompiler == 0 ) {
    Highlighter::mCompiler = mCompiler = mCompiler0 = new TPeriodicCompiler(mEditor);
    mCompiler1 = new TPeriodicCompiler(mEditor);
    }

  //Перевести компилятор в исходное состояние
  mCompiler->Reset();

  //Проверить наличие исходного файла
  if( svProject->mProjectName.isEmpty() || svProject->mProjectPath.isEmpty() )
    return;

  //Назначить системный путь
  mCompiler->mProjectPath = svProject->mProjectPath;

  //Выполнить компиляцию
  mCompiler->Compile( svProject->mMainScript );

  //Сообщить редактору об изменении списка файлов
  QStringList list = mCompiler->mFileTable.values();
  list.sort();

  if( mFileList != list ) {
    mFileList = list;
    mEditor->updateProjectFileList( mFileList );
    }

  //Установить компилятор в подсветке
  Highlighter::mCompiler = mCompiler;

  //Поменять компиляторы местами
  mCompiler = ( mCompiler == mCompiler0 ) ? mCompiler1 : mCompiler0;

  }
