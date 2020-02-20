#ifndef SVNETMIRROR_H
#define SVNETMIRROR_H

#include "SvHost/SvMirror.h"

#define SVC_MIRROR_CONTROL         20 //
#define SVC_MIRROR_STATUS          21
#define SVC_MIRROR_PROGRAMM        22


//=======================================================================================================================
//                                  SvDebugTask
//!
//! \brief The SvDebugTask struct is job for task debug. Job consist from command ("step over" or "step into" and so on),
//!                               and parameter1 and parameter2. Their purpose depends on the command
//!
struct SvDebugTask {
    int mCommand; //! Debug command
    int mParam1;  //! Parameter 1. It depends on the command
    int mParam2;  //! Parameter 2. It depends on the command

    void set( int cmd, int p1 = 0, int p2 = 0 ) { mCommand = cmd; mParam1 = p1; mParam2 = p2; }

    void clear() { mCommand = mParam1 = mParam2 = 0; }
  };




inline QDataStream &operator << ( QDataStream &os, const SvDebugTask &tsk )
  {
  os << tsk.mCommand << tsk.mParam1 << tsk.mParam2;
  return os;
  }

inline QDataStream &operator >> ( QDataStream &is, SvDebugTask &tsk )
  {
  is >> tsk.mCommand >> tsk.mParam1 >> tsk.mParam2;
  return is;
  }




//=======================================================================================================================
//                                  SvNetMirrorProgramm
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




//=======================================================================================================================
//                                  SvNetMirrorControl
struct SvNetMirrorControl {
    QVector<SvDebugTask>  mVpuDebug;    //! Debug commands for every virtual machines (tasks) [Команды отладочного управления]
    QList<int>            mWriteQueue;  //Индексы переменных, значения которых нужно записать
    QMap<int, int>        mWriteValues; //Перечень записываемых значений индекс(ключ)-значение

    SvNetMirrorControl() {}
    SvNetMirrorControl( const QByteArray ar ) { parseBlock(ar); }

    QByteArray buildBlock() {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mVpuDebug
         << mWriteQueue
         << mWriteValues;
      return ar;
      }

    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mVpuDebug
         >> mWriteQueue
         >> mWriteValues;
      }

  };





//=======================================================================================================================
//                                  SvNetMirrorStatus
struct SvNetMirrorStatus {
    QString      mControllerType; //! Current controller type
    QString      mProgrammName;   //! Current programm name
    SvVpuVector  mVpuState;       //! State of virtual machines (tasks) [Зеркало состояния виртуальных машин]
    QVector<int> mMemory;         //! Memory of global variables
    QStringList  mLog;            //! Last log segment

    SvNetMirrorStatus() {}
    SvNetMirrorStatus( const QByteArray ar ) { parseBlock(ar); }

    QByteArray buildBlock() {
      QByteArray ar;
      QDataStream os( &ar, QIODevice::WriteOnly );
      os << mControllerType
         << mProgrammName
         << mVpuState
         << mMemory
         << mLog;
      return ar;
      }

    void parseBlock( const QByteArray ar ) {
      QDataStream is(ar);
      is >> mControllerType
         >> mProgrammName
         >> mVpuState
         >> mMemory
         >> mLog;
      }

  };

#endif // SVNETMIRROR_H
