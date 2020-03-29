/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "Host-система"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Зеркало для внешних контроллеров, подключенных по usb.
*/
#ifndef SVMIRRORCOM_H
#define SVMIRRORCOM_H

#include "SvMirrorExtern.h"
#include "QtSerialPort/QSerialPort"

class SvMirrorCom : public SvMirrorExtern
  {
    QSerialPort    mSerialPort;     //! Последовательный порт. Через этот объект осуществляется фактическое подключение

    QString        mPrefferedPort;  //! Порт, через который нужно выполнить подключение. Если это
                                    //! поле оставить пустым, то подключение будет выполнено через любой порт

    QString        mControllerName; //! Имя контроллера, к которому нужно подключиться
                                    //! Здесь записывается серийный номер конкретного контроллера, к которому нужно подключиться
                                    //! Если это поле оставить пустым, то подключится к любому контроллеру

    QString        mFactPort;       //! Фактический порт, через который выполнено подключение

    QString        mSerial;         //! Серийный номер подключенного контроллера

    int            mPortIndex;      //! Счетчик перебора доступных портов для подключения к контроллеру
    int            mFlashIndex;     //! Счетчик байтов прошивки программ
    int            mTimeOut;
    int            mMemoryBlockIndex;
    int            mVpuIndex;

    //char padding[4];
  public:
    SvMirrorCom( const QString portName = QString{} , bool doScanTask = false );

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

    //!
    //! \brief bytesRead Слот вызывается при наличии байтов для чтения
    //!
            void bytesRead();

  private:
            //!
            //! \brief portClose Закрыть последовательный порт
            //!
            void portClose();

            //!
            //! \brief stageGet  Отправить запрос query и перейти в стадию nextStage.
            //!                  Это обычно ожидание ответа
            //! \param query     Запрос, который необходимо отправить
            //! \param timeOut   Тайм-аут ожидания. Если не нулевой то разрешаем контроль тайм-аута
            //!
            void stageGet(int query, int timeOut );

            //!
            //! \brief stageMemorySet Перейти в стадию "Запись в память"
            //!
            void stageMemorySet();

            //!
            //! \brief stageMemoryGet Перейти в стадию "Чтение из памяти"
            //!
            void stageMemoryGet();

            //!
            //! \brief stageVpuGet Перейти в стадию "Получить состояние vpu"
            //!
            void stageVpuGet();

            //!
            //! \brief stageDebugSet Перейти в стадию "Отправка команды отладки"
            //!
            void stageDebugSet();

            //!
            //! \brief stageFlash Перейти в стадию "Прошивка flash"
            //!
            void stageFlash();

  };

#endif // SVMIRRORCOM_H
