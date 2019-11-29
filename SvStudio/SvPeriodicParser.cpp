/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "SvPeriodicParser.h"
#include "SvCompiler/SvCompiler.h"
#include "SvProject.h"
#include "WCModeEditor.h"
#include "Highlighter.h"

#include <QAbstractEventDispatcher>
#include <QDebug>

class TPeriodicCompiler : public SvCompiler6::SvCompiler {
    WCModeEditor *mEditor;
  public:
    TPeriodicCompiler( WCModeEditor *editor ) : SvCompiler(), mEditor(editor) {}



    // TCompiler interface
  public:
    virtual SvCompiler6::SvSource *CreateSource(const QString &fname, bool current);

  };




//Источник исходного кода - строка текста
class SvStringSource : public SvCompiler6::SvSource {
    QString mSource;
  public:
    SvStringSource( const QString &src, int fileId ) :
      SvSource(fileId),
      mSource(src)
      {
      mInputStream = new QTextStream( &mSource, QIODevice::ReadOnly );
      }
  };






SvCompiler6::SvSource *TPeriodicCompiler::CreateSource(const QString &fname, bool current)
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
  mCompiler(nullptr),
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
  if( mCompiler == nullptr ) {
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
