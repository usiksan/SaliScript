/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#ifndef WNETVIEWER_H
#define WNETVIEWER_H

#include <QTextEdit>

class WNetViewer : public QTextEdit
  {
    Q_OBJECT

    bool mControl;
  public:
    WNetViewer();

  signals:
    void saveContens( const QByteArray contens, const QString srcName, const QString dstName );

    // QWidget interface
  protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
  };

#endif // WNETVIEWER_H
