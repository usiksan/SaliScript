#ifndef SVMIRRORCOM_H
#define SVMIRRORCOM_H

#include "SvMirrorExtern.h"
#include "QtSerialPort/QSerialPort"

class SvMirrorCom : public SvMirrorExtern
  {
    QSerialPort    mSerialPort;
    QString        mPrefferedPort;
    QString        mControllerName;
    QString        mFactPort;

    int            mIndex;
    int            mStage;
    int            mTimeOut;
    int            mMemoryBlockIndex;
    int            mVpuIndex;

    //char padding[4];
  public:
    SvMirrorCom( const QString portName = QString{} );

    //!
    //! \brief controllerType Return current linked controller type name string
    //! \return Current linked controller type name string. If no controller linked
    //!         then empty string returned
    //!
    virtual QString     controllerType() const override;

    // SvMirror interface
  public slots:
    //!
    //! \brief linkTo Link to external controller host
    //! \param ipOrUsb IP for local net.
    //! \param port  Port for local server
    //! \param controllerName Not used
    //! \param passw Not used
    //!
    virtual void linkTo(const QString ipOrUsb, int port, const QString controllerName = QString{}, const QString controllerPassw = QString{} ) override;

    //!
    //! \brief setProgrammFlashRun Flash programm to controller and run it or paused. Here we send programm over Ethernet
    //!                            to controller
    //! \param prog                Programm which flashed to controller
    //! \param runOrPause          If true then programm automaticly started after flash, else - it paused
    //! \param flash               If true then programm flashed into external controller, else - do nothing
    //!
    virtual void setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause, bool flash ) override;

    //!
    //! \brief processing Perform periodic mirror handle. Here we check queues and send appropriate queries
    //! \param tickOffset Time in ms between previous calling this function and this one. Do'nt use there
    //!
    virtual void processing(int tickOffset) override;

    //!
    //! \brief init Called automaticly from inside execution thread to init mirror after creation
    //!
    virtual void init() override;

            void bytesRead();

  private:
            //Закрыть порт
            void portClose();

            void stageGet( int query, int nextStage, int timeOut );

            void stageMemorySet();

            void stageMemoryGet();

            void stageVpuGet();

            void stageDebugSet();

            void stageFlash();

  };

#endif // SVMIRRORCOM_H
