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
    Зеркало для внешнего VPU, работающего на контроллере, подключенного через usb.
*/

#ifndef SVMIRRORUSB10_H
#define SVMIRRORUSB10_H

#ifdef ENABLE_USB10
#include "SvMirrorExtern.h"
#include "libusb.h"


class SvMirrorUsb10 : public SvMirrorExtern
  {
    Q_OBJECT

    QMutex   mProcessingMutex;
    QMutex   mUsbTransferMutex;
    bool     mUsbLibReady;
    int      mVid;
    int      mPid;
    libusb_device_handle    *mUsbDev;
    int      mOpenAttempts;
    int      mVersion;       //Версия контроллера
    int      mFlashSize;     //Размер флеш-памяти контроллера
    int      mProgSize;      //Размер загруженной программы
    int      mTick;
  public:
    SvMirrorUsb10( bool scanTasks );
    ~SvMirrorUsb10() override;

    //Тип зеркала
    virtual int         mirrorType() const override { return SMT_USB; }

    //Настроить зеркало
    virtual void        settings( const QString ip, int port,
                                  const QString globalName, const QString globalPassw,
                                  int vid, int pid ) override;

    //Выполнить обработку узла
    virtual void        processing( int tickOffset ) override;

    // SvMirror interface
  public slots:
    //===========================
    //Раздел управления

    //Сначала сброс, затем создание корневого виртуального процессора и пуск с начального адреса
    virtual void        restart( bool runOrPause ) override;

    //Компиляция, линковка, если равно, иначе прошивка и запуск
    //Компиляция, прошивка и запуск
    virtual void        compileFlashRun( bool link, bool flash, bool runOrPause ) override;

  private:
    //Открыть USB
    bool     UsbOpen();
    libusb_device_handle    *UsbOpenDev();

    //Закрыть USB
    void     UsbClose();

    //Выполнить обмен по USB
    //Данные передаются из массива dat и считываются туда-же
    bool     UsbTransferData( quint8 *dat, int sendCount, int receivCount );

    //Выполнить передачу переменных
    void     SendVars();

    //Выполнить чтение переменных
    void     ReceivVars();

    //Выполнить отправку управления задачами
    void     SendDebugTask();

    //Получить состояние задач
    void     ReceivTasksState();

    //Переподключиться
    void     Relink();

    //Обновить информацию о контроллере
    void     ControllerInfo();

    //Загрузить программу в контроллер
    //Возвращает флаг, была ли программа равна
    bool     Flash(bool *equal);

    //Проверить готовность флэш-памяти к выполнению операций
    bool     FlashReady();

    //Проверить готовность контроллера к приему очередной команды
    bool     ControllerReady();

    template <typename T>
    void initUsbFunction(T & function, const char* functionName);
    void initUsbFunctions();
    QString usbErrorString(int err);
    void closeUsbDevice();
  };

#endif

#endif // SVMIRRORUSB10_H
