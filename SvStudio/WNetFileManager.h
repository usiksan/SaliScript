/*
Project "Electronic schematic and pcb CAD"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
*/
#ifndef WNETFILEMANAGER_H
#define WNETFILEMANAGER_H

#include "WNetFileList.h"
#include "SvHost/SvMirrorManager.h"

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>


class WNetFileManager : public QWidget
  {
    Q_OBJECT

    QLineEdit    *mId;
    QLineEdit    *mPassw;
    QLineEdit    *mBridgeIp;
    QLineEdit    *mLinkId;
    QLineEdit    *mLinkPassw;
    QLabel       *mNetStatus;
    QLineEdit    *mLeftDir;
    QLineEdit    *mRightDir;
    WNetFileList *mLeftPanel;
    WNetFileList *mRightPanel;
    QCheckBox    *mLeftRemote;
    QCheckBox    *mRightRemote;
    SvNetChannel *mChannel;
    QString       mLocalDir;     //Начальный директорий для локальной машины
    QString       mRemoteDir;    //Начальный директорий для удаленной машины
    bool          mEdit;         //Истина, если требуется редактирование файла
    bool          mLeftFocus;    //Активна левая панель
    char padding[6];
  public:
    explicit WNetFileManager( SvMirrorManager *manger, QWidget *parent = nullptr);

  signals:
    void netLink( int id, int passw );
    void netLinkBridge();
    void remoteDirGetCur();
    void remoteDirGetList( const QString dir );
    void remoteGetFile( const QString fname );

    void remoteDirCreate( const QString dir );

  public slots:
    void cmdView();
    void cmdEdit();
    void cmdViewEdit( bool edit, bool remote, const QString fname );
    void cmdCopy();
//    void cmdCopyLeft();
//    void cmdCopyRight();
//    void cmdCopyLocal( const SvNetFileList list );
//    void cmdLoad( const SvNetFileList list );
//    void cmdSave( const SvNetFileList list );
    void cmdMove();
    void cmdMkDir();
    void cmdMkDirInt( bool remote, const QString src, const QString dst );
    void cmdDelete();
    void cmdLink();
    void cmdUnlink();
    void onNetStatus( int id, int passw, QString bridge, QString link );
    void setLeftDir( const QString dir );
    void setRightDir( const QString dir );
    void setLeftRemote( bool remote );
    void setRightRemote( bool remote );
    void viewContens( const QString fname, const QByteArray contens );
    void viewRemoteContens( const QString fname, const QByteArray contens );
//    void updateLeftDir();
//    void updateRightDir();
    void setRemoteDir( const QString dir );
    void setLocalDir( const QString dir );
    void updateLocalDir( const QString dir );
    void updateRemoteDir( const QString dir );
    //void setDir( const QString dir );
  };

#endif // WNETFILEMANAGER_H
