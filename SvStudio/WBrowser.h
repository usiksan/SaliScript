/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#ifndef WBROWSER_H
#define WBROWSER_H

#include <QWidget>
#include <QFrame>
#include <QUrl>
#include <QTextBrowser>


class WBrowser : public QFrame
  {
    Q_OBJECT

  protected:
    QTextBrowser *mBrowser;
    QString       mDir;
  public:
    WBrowser( const QString &path, QWidget *parent = 0 );
    ~WBrowser();

  signals:
    void backwardEnable( bool enable );
    void forwardEnable( bool enable );
    void setTitle( const QString title );

  public slots:
            void setDir( const QString &path );

            QUrl convertPage( const QString &page , const QString &fragment );

    virtual void setPage( const QString &page , const QString &fragment );

    virtual void contens() {}

    virtual void home() {}

    virtual void onLinkClicked( const QUrl &url );

    virtual void onLinkString( const QString &link );

    virtual void backward();

    virtual void forward();

            void setHtml( const QString &html );

            void onLoadFinished( bool ok );
  };

#endif // WBROWSER_H
