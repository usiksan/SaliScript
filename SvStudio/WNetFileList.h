/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#ifndef WNETFILELIST_H
#define WNETFILELIST_H

#include "SvNet/SvNetServiceFileSlave.h"
#include "SvHost/SvDir.h"

#include <QTableWidget>
#include <QDir>

class WNetFileList : public QTableWidget
  {
    Q_OBJECT

    SvDir mCurrentDir; //Текущий директорий
  public:
    WNetFileList( QWidget *parent = nullptr);

    QString       getCurrentFile();
    SvNetDirInfo  getSelectedFiles() const;

  signals:
    void changeDirectory( const QString dir );
    void fileView();
    void fileEdit();
    void fileCopy();
    void fileMove( const SvNetDirInfo fileList );
    void fileMkDir();
    void fileDelete( const SvNetDirInfo fileList );
    void focused();
    void focusOther();

  public slots:
    void listClear();
    void onDirList( const QString dir, const SvNetDirInfo fileList );
    void onDirEnter( int row, int column );

  private:
    void addFile(const SvNetFileInfo &file, int row );

    // QWidget interface
  protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
  };

#endif // WNETFILELIST_H
