/*
  Проект "SaliMt"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "WCModeIntro.h"
#include "SvProject.h"
#include <QFileInfo>
#include <QDebug>

WCModeIntro::WCModeIntro(QWidget *parent) :
  WBrowser( QString(), parent )
  {
  connect( svProject, SIGNAL(recentProjectsList(QStringList)), this, SLOT(onProjectListChanged(QStringList)) );
  }

WCModeIntro::~WCModeIntro()
  {

  }

void WCModeIntro::onProjectListChanged(const QStringList list)
  {
  //Формировать новый стартовый экран
  QString html("<html><body><h1 align=\"center\">");
  html.append( tr("Welcome to SaliLAB DS") ).append("</h1>");
  html.append( "<p>" ).append( tr("You can open existing or recent project. You can also create new project.")).append("</p>");
  html.append("<hr>");
  html.append("<table border=0 width=\"100%\"><tr><td width=\"50%\"><button><img src=\"qrc:///pic/file_yellow_open.png\" align=\"middle\"> <a href=\"load\">").append( tr("Load existing project") ).append("</a></button></td>");
  html.append("<td><button><img src=\"qrc:///pic/file_yellow_empty_new.png\" align=\"middle\"> <a href=\"new\">").append( tr("Create new project") ).append( "</a></button></td></tr></table>" );
  if( list.count() ) {
    html.append("<hr>");

    html.append("<h3>").append( tr("Recent projects") ).append("</h3>");

    foreach( const QString str, list ) {
      QFileInfo info(str);
      html.append("<p><a href=\"").append(str).append("\">").append( info.completeBaseName() ).append("</a><br>(").append(str).append(")</p>");
      }
    }

  //Установить новый стартовый экран
  setHtml( html );
  }



void WCModeIntro::onLinkString(const QString &link)
  {
  qDebug() << "onLink" << link;
  if( link.startsWith(QString("load")) ) emit openProject();
  else if( link.startsWith(QString("new")) ) emit newProject();
  else emit projectOpen( link );
  }
