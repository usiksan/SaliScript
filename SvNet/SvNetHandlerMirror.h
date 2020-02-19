#ifndef SVNETHANDLERMIRROR_H
#define SVNETHANDLERMIRROR_H

#include "SvNetHandler.h"
#include "SvHost/SvMirror.h"

#include <QMap>
#include <QList>

#define SVC_MIRROR_INFO            20 //
#define SVC_MIRROR_PROGRAMM        21
#define SVC_MIRROR_WRITE_VARIABLES 22
#define SVC_MIRROR_QUERY_VARIABLES 23
#define SVC_MIRROR_VARIABLES       24


struct SvNetMirrorInfo {
    QString mControllerType; //! Controller type
    QString mProgrammName;   //! Programm name loaded into controller

    SvNetMirrorInfo( const QString controllerType, const QString programmName ) :
      mControllerType(controllerType),
      mProgrammName(programmName) { }
    SvNetMirrorInfo( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetMirrorInfo object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mControllerType
         << mProgrammName;
      return ar;
      }

    //Deserialise SvNetMirrorInfo object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mControllerType
         >> mProgrammName;
      }

  };


struct SvNetMirrorProgramm {
    bool       mRunOrPause;
    QByteArray mProgramm;

    SvNetMirrorProgramm( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetMirrorProgramm object
    static QByteArray buildBlock( bool runOrPause, const QByteArray prog ) {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << runOrPause
         << prog;
      return ar;
      }

    //Deserialise SvNetMirrorProgramm object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mRunOrPause
         >> mProgramm;
      }
  };


struct SvNetMirrorWrite {
    QList<int>    mQueue;
    QMap<int,int> mMap;



    static QByteArray buildBlock( const QList<int> &que, const QMap<int,int> &mp ) {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << que
         << mp;
      return ar;
      }

    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mQueue
         >> mMap;
      }
  };


struct SvNetMirrorVariables {
    QList<int> mValues;

    SvNetMirrorVariables() : mValues() {}

    SvNetMirrorVariables( const QByteArray ar ) { parseBlock(ar); }

    //Serialise SvNetMirrorVariables object
    QByteArray buildBlock() const {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mValues;
      return ar;
      }

    //Deserialise SvNetMirrorVariables object
    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mValues;
      }
  };


class SvNetHandlerMirror : public SvNetHandler
  {
    Q_OBJECT

    SvMirrorPtr mMirror;
  public:
    SvNetHandlerMirror();

  public slots:
    void mirrorChanged( int id, SvMirrorPtr mirrorPtr );

    // SvNetHandler interface
  public slots:
    virtual void receivedBlock(SvNetChannel *ch, qint8 cmd, QByteArray block) override;
    virtual void receivedAnswer(SvNetChannel *ch, int srcCmd, qint32 answerCode, const QString msg) override;
  };

#endif // SVNETHANDLERMIRROR_H
