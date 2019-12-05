#if 0
#ifndef SVMIRRORCOM_H
#define SVMIRRORCOM_H

#include "SvMirrorExtern.h"
#include "QtSerialPort/QSerialPort"

class SvMirrorCom : public SvMirrorExtern
  {
    QSerialPort    mSerialPort;

    int            mReceivTimeOut;
    int            mLogLen;
    unsigned char  mBuf[64];
    bool           mFlash;
    bool           mReset;
    bool           mRun;
    char           mLog[1024];
    char padding[4];
  public:
    SvMirrorCom( const QString portName );

  public slots:
    //!
    //! \brief setProgrammFlashRun Flash programm to controller and run it or paused
    //! \param prog                Programm which flashed to controller
    //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
    //!
    virtual void setProgrammFlashRun( SvProgrammPtr prog, bool runOrPause ) override;



    //!
    //! \brief processing Perform periodic mirror handle
    //! \param tickOffset Time in ms between previous calling this function and this one
    //!
    virtual void processing( int tickOffset ) override;

  protected:
    void send( const unsigned char *buf );

            void          onReceived(const unsigned char *buf );

  protected:
            void          parseVersion( const unsigned char *buf );
            void          parseState( const unsigned char *buf );
            void          parseVariables( const unsigned char *buf );
            void          parseTask( const unsigned char *buf );
            void          parseFlash( const unsigned char *buf );
            void          parseFlashWriteOk( const unsigned char *buf );

            void          queryMemory();
            void          queryNextDebug();
            void          queryNextFlash(int addr);
            void          queryState();
            void          queryReset();
            void          queryFlashRead();
            void          queryRestart();
  };

#endif // SVMIRRORCOM_H
#endif
