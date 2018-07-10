/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/
#include "SvConfig.h"
#include "WBrowser.h"
#include "WCModeHelp.h"
#include <QHBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>


WBrowser::WBrowser(const QString &path, QWidget *parent) :
  QFrame(parent)
  {
  setFrameStyle( QFrame::Box );
  setFrameShadow( QFrame::Plain );

  QHBoxLayout *box = new QHBoxLayout();
  box->setMargin(0);
  mBrowser = new QTextBrowser(this);
  mBrowser->setOpenLinks(false);
  box->addWidget( mBrowser );
  setLayout( box );

  connect( mBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)) );
  //connect( mBrowser, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)) );

  setDir( path );
  home();
  }

WBrowser::~WBrowser()
  {

  }

void WBrowser::setDir(const QString &path)
  {
  mDir = path;
  //При отсутствии слэша - добавить
  if( !mDir.endsWith( QChar('/') ) )
    mDir.append( QChar('/') );
  }



QUrl WBrowser::convertPage(const QString &page, const QString &fragment)
  {
  QSettings s;
  //Язык интерфейса
  QString lang = s.value( CFG_LANGUAGE, QVariant(QString("en")) ).toString();

  //Проверить наличие файла на языке пользователя
  if( QFile::exists( mDir + lang + "_" + page ) ) {
    QUrl url = QUrl::fromLocalFile( mDir + lang + "_" + page );
    if( !fragment.isEmpty() )
      url.setFragment( fragment );
    qDebug() << url;
    return url;
    }
  //Проверить наличие файла с анлийским языком
  else if( QFile::exists( mDir + "en_" + page ) ) {
    QUrl url = QUrl::fromLocalFile( mDir + "en_" + page );
    if( !fragment.isEmpty() )
      url.setFragment( fragment );
    qDebug() << url;
    return url;
    }

  //Никакого файла нету, выдать файл с ошибкой
  qDebug() << "no file" << (mDir + "en_" + page);
  return WCModeHelp::pageError();
  }




void WBrowser::setPage(const QString &page, const QString &fragment)
  {
  qDebug() << "setPage" << page << fragment;
  //Открыть страницу
  mBrowser->setSource( convertPage( page, fragment ) );

  }




void WBrowser::onLinkClicked(const QUrl &url)
  {
//  qDebug() << "link" << url.toLocalFile();
//  qDebug() << url.toString();
  if( url.hasFragment() ) {
    setPage( url.fileName(), url.fragment() );
    }
  else
    onLinkString( url.toString( QUrl::None ) );
  }




void WBrowser::onLinkString(const QString &link)
  {
  setPage( link, QString() );
  }




void WBrowser::backward() {
  mBrowser->backward();
  //Отправить сигнал о доступности кнопки Назад и Вперед
  emit backwardEnable( mBrowser->isBackwardAvailable() );
  emit forwardEnable( mBrowser->isForwardAvailable() );
  }



void WBrowser::forward() {
  mBrowser->forward();
  //Отправить сигнал о доступности кнопки Назад и Вперед
  emit backwardEnable( mBrowser->isBackwardAvailable() );
  emit forwardEnable( mBrowser->isForwardAvailable() );
  }



void WBrowser::setHtml(const QString &html)
  {
  mBrowser->setHtml( html );
  }

void WBrowser::onLoadFinished(bool )
  {
  //Обрабатывать линки самостоятельно
  mBrowser->setOpenLinks(false);

  //Обновить сигнал с именем
  emit setTitle( mBrowser->documentTitle() );

  //Отправить сигнал о доступности кнопки Назад и Вперед
  emit backwardEnable( mBrowser->isBackwardAvailable() );
  emit forwardEnable( mBrowser->isForwardAvailable() );
  }

