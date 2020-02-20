/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Host-система"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Зеркало для внешнего VPU, работающего через машину в локальной сети.
*/
#ifndef SVMIRRORREMOTE_H
#define SVMIRRORREMOTE_H

#include "SvMirrorExtern.h"
#include "SvNet/SvNetChannel.h"


class SvMirrorRemote : public SvMirrorExtern
  {
    Q_OBJECT
  protected:
    SvNetChannel *mChannel; //! Channel to connect to remote controller host
    QString       mIp;      //! IP of remote controller host
    int           mPort;    //! Port of remote controller host
    int           mTimeOut;
    int           mFsmCurrent;
  public:
    SvMirrorRemote();
    ~SvMirrorRemote() override;

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

  private slots:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    void receivedBlock( SvNetChannel *ch, int cmd, const QByteArray block );

  private:
    void reconnect();
  };

#endif // SVMIRRORREMOTE_H
