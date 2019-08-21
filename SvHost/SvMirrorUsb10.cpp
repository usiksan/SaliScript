/*
  Проект "SaliDs"
    Визуальное программирование микроконтроллеров
  Автор
    Сибилев А.С.
  Описание
*/

#include "SvMirrorUsb10.h"
#include "Vpu/SvVpuUsbInterface.h"
#include "Vpu/SvVpuUtils.h"

#include <QMutexLocker>
//#include <QMessageBox>
#include <QByteArray>
#include <QThread>
#include <QDebug>

#ifdef ENABLE_USB10

template <typename T>
void SvMirrorUsb10::initUsbFunction(T & function, const char *functionName)
{
    /* function = (T)mUsbLib.resolve(functionName);
    if (function == nullptr){
       mUsbLibReady = false;
       setLink(QString(functionName) + QString(" not found"), false);
    }*/
}

void SvMirrorUsb10::initUsbFunctions(){

    libusb_init(nullptr);
}

QString SvMirrorUsb10::usbErrorString(int err)
{
    return libusb_strerror((libusb_error)err);
}

void SvMirrorUsb10::closeUsbDevice()
{
    auto device = libusb_get_device(mUsbDev);
    libusb_close( mUsbDev );
    libusb_unref_device(device);
}





SvMirrorUsb10::SvMirrorUsb10(bool scanTasks) :
  SvMirrorExtern( scanTasks ),
  mUsbLibReady(true),
  mVid(0),
  mPid(0),
  mUsbDev(0),
  mOpenAttempts(0),
  mVersion(0),       //Версия контроллера
  mFlashSize(0),     //Размер флеш-памяти контроллера
  mProgSize(0),      //Размер загруженной программы
  mTick(0)
  {
  if (mUsbLibReady)
    initUsbFunctions();
  if (!mUsbLibReady){
    mLinkStatus = tr("Usb functions unavailiable");
    }
  }




SvMirrorUsb10::~SvMirrorUsb10()
  {
  UsbClose();
  libusb_exit(nullptr);
  }



void SvMirrorUsb10::settings(const QString prjPath, const QString mainScript, const QString ip, int port, const QString globalName, const QString globalPassw, int vid, int pid)
{
    SvMirrorExtern::settings( prjPath, mainScript, ip, port, globalName, globalPassw, vid, pid );
    bool needDisconnect = mVid != vid || mPid != pid;
    mVid = vid;
    mPid = pid;
    if (needDisconnect)
        setLink( "Disconnect", false );
}




void SvMirrorUsb10::processing(int tickOffset)
{
    QMutexLocker locker(&mProcessingMutex);
    //Если нету библиотеки, то ничего делать нельзя
    if( !mUsbLibReady ) {
        setLink( tr("UsbLib error"), false );
        return;
    }

    if( !mLink ) {
        if( mTick <= 0 ) {
            //Пробуем подключиться
            Relink();
            mTick = 1000;
            //if( !mLink ) return;
        }
        else mTick -= tickOffset;
        return;
    }
    if( !mLink ) {
        //Подключения нету
        setLink( tr("Not connected"), false );
        return;
    }
    else {
        //Подключение есть

        //Проверить наличие подключения
        if( !ControllerReady() ){
            return;

        }


        //Проверить необходимость отправки переменных
        if( mWriteQueue.count() )
            //Отправляем переменные
            SendVars();

        //Если включено сканирование задач
        if( mScanTasks ) {

            //Выполняем отправку управления задачами
            SendDebugTask();

            //Получить состояние задач
            ReceivTasksState();
        }

        //Выполняем чтение переменных
        ReceivVars();
    }
}




//Отправить содержимое проекта
void SvMirrorUsb10::sendProject()
{
    // TODO включить исходный код проекта в прошивку контроллера
    //Отправим сигнал о завершении операции, пока
    setProcess( tr("Complete"), false );
}





//Получить содержимое проекта
void SvMirrorUsb10::receivProject()
{
    // TODO включить исходный код проекта в прошивку контроллера
    //Отправим сигнал о завершении операции, пока
    setProcess( tr("Complete"), false );
}


//Открыть USB
bool SvMirrorUsb10::UsbOpen()
{
    //Проверить, если не загружена библиотека, то ничего не делаем
    if( !mUsbLibReady )
        return false;

    if( mUsbDev == 0 ) {
        if( mOpenAttempts == 0 ) {
            mOpenAttempts = 3;

            mUsbDev = UsbOpenDev();

            if( mUsbDev != nullptr ) {

                libusb_set_auto_detach_kernel_driver(mUsbDev, 1);
                qDebug() << "libusb_set_auto_detach_kernel_driver: " << true;
                int result = 0;
                result = libusb_set_configuration(mUsbDev, 1);
                if (result < 0){
                    qDebug() << "libusb_set_configuration fail: " << usbErrorString(result);
                    closeUsbDevice();
                    mUsbDev = 0;
                    return false;
                } else {
                    qDebug() << "libusb_set_configuration: " << true;
                    result =  libusb_claim_interface(mUsbDev, 0);
                    if (result < 0){
                        qDebug() << "libusb_claim_interface fail: " << usbErrorString(result);
                        closeUsbDevice();
                        mUsbDev = 0;
                        return false;
                    }
                    qDebug() << "libusb_claim_interface: " << true;
                }

                qDebug() << "UsbOpen: " << true;
                mOpenAttempts = 0;
                return true;
            }
        }
        else {
            mOpenAttempts--;
            return false;
        }
    }
    else return true;
    return false;
}





libusb_device_handle *SvMirrorUsb10::UsbOpenDev()
{
    if (!mUsbLibReady)
        return nullptr;
    libusb_device_handle *handle = nullptr;

    libusb_device **list = nullptr;
    libusb_device *found = nullptr;

    ssize_t cnt = libusb_get_device_list(nullptr, &list);
    int err = 0;
    if (cnt < 0){
        qDebug () << "libusb_get_device_list error: " << usbErrorString(cnt);
        //setUsbLibErrorStatus("Usb get device list error", err);
    }

    //  qDebug () << "libusb_get_device_list count: " << cnt;
    for (auto i = 0; i < cnt; ++i) {
        libusb_device *device = list[i];
        libusb_device_descriptor  descriptor;
        libusb_get_device_descriptor(device, &descriptor);
        if (descriptor.idVendor == mVid && descriptor.idProduct == mPid) {
            found = device;
            libusb_ref_device(found);
            break;
        }
    }
    if (found) {
        err = libusb_open(found, &handle);
        if (err != 0){
            qDebug () << "libusb_open error: " << usbErrorString(err);
            libusb_unref_device(found);
        }
    }
    else{
        qDebug () << "Device with vid " << QString::number(mVid, 16) << " and pid " << QString::number(mPid, 16) << " not found";
    }
    libusb_free_device_list(list, 1);
    return handle;
}




void SvMirrorUsb10::UsbClose()
{
    if (!mUsbLibReady)
        return;
    //Проверить, если не загружена библиотека, то ничего не делаем
    if( mUsbDev != nullptr) {
        closeUsbDevice();
        mUsbDev = 0;
        setLink( tr("Not connected"), false );
    }
}






//Выполнить обмен по USB
//Данные передаются из массива dat и считываются туда-же
bool SvMirrorUsb10::UsbTransferData(quint8 *dat, int sendCount, int receivCount)
{
    if( !mUsbLibReady )
        return false;

    QMutexLocker locker(&mUsbTransferMutex);
    int transferred = 0;
    int error = 0;
    error =  libusb_bulk_transfer( mUsbDev, 0x03, dat, sendCount, &transferred, 5000 );
    if(error < 0 ) {
        return false;
    }
    error =  libusb_bulk_transfer( mUsbDev, 0x81, dat, receivCount, &transferred, 5000 );
    if(error < 0 ) {
        return false;
    }
    return true;
}




static void
FillAssign( quint8 *dat, int addr, int value ) {
    svIWrite16( dat, addr );
    svIWrite32( dat + 2, value );
}


//Выполнить передачу переменных
void SvMirrorUsb10::SendVars()
{
    //Место под пакеты
    quint8 dat[64];

    //Блокировщик доступа к очереди
    QMutexLocker locker( &mWriteMutex );

    //Сформировать пакет на установку
    int size = qMin( mWriteQueue.count(), 10 );
    dat[0] = SVU_CMD_VARS_WRITE;
    dat[1] = size;


    for( int i = 0; i < size; i++ ) {
        int adr = mWriteQueue.at(i);
        int val = mWriteValues.value(adr);
        FillAssign( dat + 2 + i * 6, adr, val );
    }

    //Пакет сформирован, выполнить обмен
    if( !UsbTransferData( dat, 2 + size * 6, 1 ) || dat[0] != SVU_ANS_OK )
        return;

    //Пакет передан успешно, убрать переданные переменные из массива
    for( int i = 0; i < size; i++ )
        mWriteValues.remove( mWriteQueue.takeFirst() );
}





//Выполнить чтение переменных
void SvMirrorUsb10::ReceivVars()
{
    if( mMemory && mMemorySize && mProgramm->globalCount() ) {
        //Буфер для чтения переменных
        quint8 ram[270000];
        //Выполнить чтение переменных
        ram[0] = SVU_CMD_VARS_READ;
        svIWrite32( ram + 1, mProgramm->globalCount() );
        svIWrite32( ram + 5, 0 );
        int ct = mProgramm->globalCount() * 4;
        if( ct & 0x3f ) ct = (ct / 64 + 1) * 64;
        //Пакет сформирован, выполнить обмен
        if( !UsbTransferData( ram, 9, ct ) )
            return;

        // qDebug() << "received vars";
        //Обмен выполнен успешно, скопировать данные
        ct = mProgramm->globalCount();
        bool changed = false;
        int d;
        for( int i = 1; i < ct; i++ ) {
            d = svIRead32(ram+i*4);
            if( mMemory[i] != d ) {
                changed = true;
                mMemory[i] = d;
            }
        }

        //changed = true;
        //memcpy_s(mMemory+1, 4*(ct-1), ram+4, 4*(ct-1));

        //Если были изменения, то отослать сигнал об обновлении
        if( changed ) {
            emit memoryChanged();
        }
    }
}






//Выполнить отправку управления задачами
void SvMirrorUsb10::SendDebugTask()
{
    if( mVpuDebug == 0 ) return;
    quint8 dat[64];
    for( int i = 0; i < mVpuMax; i++ )
        if( mVpuDebug[i].mCommand ) {
            //Выполнить команду
            dat[1] = (quint8)i;
            svIWrite32( dat + 2, mVpuDebug[i].mParam1 );
            svIWrite32( dat + 6, mVpuDebug[i].mParam2 );
            switch(mVpuDebug[i].mCommand) {
            case SDC_RUN             : //Команда пуск (отключение отладки)
                dat[0] = SVU_CMD_DEBUG_RUN;
                if( !UsbTransferData( dat, 2, 1 ) || dat[0] != SVU_ANS_OK )
                    return;
                break;

            case SDC_RUN_STEP        : //Выполнить шаг
                dat[0] = SVU_CMD_DEBUG_STEP;
                if( !UsbTransferData( dat, 10, 1 ) || dat[0] != SVU_ANS_OK )
                    return;
                break;

            case SDC_RUN_UNTIL       : //Выполнять, пока IP вне диапазона адресов
                dat[0] = SVU_CMD_DEBUG_UNTIL;
                if( !UsbTransferData( dat, 10, 1 ) || dat[0] != SVU_ANS_OK )
                    return;
                break;

            case SDC_RUN_TRACE       : //Выполнить шаг со входом в подпрограмму
                dat[0] = SVU_CMD_DEBUG_TRACE;
                if( !UsbTransferData( dat, 10, 1 ) || dat[0] != SVU_ANS_OK )
                    return;
                break;
            }

            //Сообщить, что команда выполнена
            mVpuDebug[i].mCommand = 0;
        }
}






//Переподключиться
void SvMirrorUsb10::Relink()
{
    if (mVid == 0 && mPid == 0)
        return;
    quint8 dat[64];

    setLink( tr("Reconnecting"), false );

    //Закрыть USB
    UsbClose();

    //Открыть USB
    if( !UsbOpen() ) {
        setLink( tr("Usb not open"), false );
        return;
    }

    //Контроллер подключен, получить конфигурационную информацию
    dat[0] = SVU_CMD_VERSION;

    if( UsbTransferData( dat, 1, 32 ) && dat[31] == SVU_ANS_OK ) {
#ifndef Q_PROCESSOR_ARM
        qDebug() << "cmdVersion ok";
#endif
        //ответ [0-3]   номер версии
        mVersion = svIRead32( dat );

        //      [4-7]   размер памяти программ
        mFlashSize = svIRead32( dat + 4 );

        //      [8-11]  размер памяти данных
        int varsSize  = svIRead32( dat + 8 );

        //      [12]    тип контроллера (процессор и количество слотов), из этой информации определяются возможности контроллера
        int controllerType = dat[12];

        //      [13]    максимальное количество VPU
        int vpuMax = dat[13];

        //      [14]    фактическое количество активных vpu
        int vpuCount = dat[14];

        //      [15]
        //      [16-23] сигнатура установленной программы
        char sig[9];
        for( int i = 0; i < 8; i++ )
            if( dat[16+i] & 0x80 || dat[16+i] < ' ' ) sig[i] = '-';
            else sig[i] = (char)(dat[16+i]);
        sig[8] = 0;

        //      [24-27] размер прошитой программы
        mProgSize = svIRead32( dat + 24 );

        //      [28-31] количество глобальных переменных
        int varsCount = svIRead32( dat + 28 );

        setupMirror( vpuMax, vpuCount, varsSize, varsCount );

        //Обновить текстовую информацию о контроллере
        setLink( tr("Type %1, os version %2, ram %3, flash %4, programm \'%5\'").arg( controllerType )
                 .arg(mVersion).arg(varsSize).arg(mFlashSize).arg(sig), true );
    }
    else {

        setLink( tr("No answer controller"), false );
    }
}





//Обновить информацию о контроллере
void SvMirrorUsb10::ControllerInfo()
{
    quint8 dat[64];

    //Контроллер подключен, получить конфигурационную информацию
    dat[0] = SVU_CMD_VERSION;

    if( UsbTransferData( dat, 1, 32 ) && dat[31] == SVU_ANS_OK ) {
        //ответ [0-3]   номер версии
        mVersion = svIRead32( dat );

        //      [4-7]   размер памяти программ
        mFlashSize = svIRead32( dat + 4 );

        //      [8-11]  размер памяти данных
        int varsSize  = svIRead32( dat + 8 );

        //      [12]    тип контроллера (процессор и количество слотов), из этой информации определяются возможности контроллера
        int controllerType = dat[12];

        //      [13]    максимальное количество VPU
        int vpuMax = dat[13];

        //      [14]    фактическое количество активных vpu
        int vpuCount = dat[14];

        //      [15]
        //      [16-23] сигнатура установленной программы
        char sig[9];
        for( int i = 0; i < 8; i++ )
            if( dat[16+i] & 0x80 || dat[16+i] < ' ' ) sig[i] = '-';
            else sig[i] = (char)(dat[16+i]);
        sig[8] = 0;

        //      [24-27] размер прошитой программы
        mProgSize = svIRead32( dat + 24 );

        //      [28-31] количество глобальных переменных
        int varsCount = svIRead32( dat + 28 );

        setupMirror( vpuMax, vpuCount, varsSize, varsCount );

        //Обновить текстовую информацию о контроллере
        setLink( tr("Type %1, os version %2, ram %3, flash %4, programm \'%5\'").arg( controllerType )
                 .arg(mVersion).arg(varsSize).arg(mFlashSize).arg(sig), true );

    }
}






//Загрузить программу в контроллер
bool SvMirrorUsb10::Flash( bool *equal )
{
    *equal = false;
    //Проверить соответствие загруженной программы
    if( mProgramm->codeCount() ) {
        if( mProgramm->codeCount() == mProgSize ) {
            QByteArray bdat( mFlashSize, 0xff );
            setProcess( tr("Compare programm") );
            //Прочитаем всю память
            bdat[0] = SVU_CMD_FLASH_READ;
            int len = ((mProgSize / 64) + 1) * 64;
            if( len > mFlashSize ) {
                setProcess( tr("Error"), false, tr("Fatal controller flash size") );
                return false;
            }
            svIWrite32( (quint8*)(bdat.data() + 1), len );
            svIWrite32( (quint8*)(bdat.data() + 5), 0 );
            if( UsbTransferData( (quint8*)(bdat.data()), 9, len ) ) {
                //Память прочитана, сравнить
                const char *src = mProgramm->getCode().constData();
                int mc = memcmp( src, bdat.data(), mProgSize );
                if( mc == 0 ) {
                    *equal = true; //Вернуть флаг, что программы равны
                    return true;
                }
            }
            else {
                setProcess( tr("Error"), false, tr("Reading programm error") );
                return false;
            }
        }

        //Требуется загрузка программы

        //Выполнить сброс, чтобы остановить программу
        quint8 dat[64];
        dat[0] = SVU_CMD_VPU_RESET;
        setProcess( tr("Reset programm.") );
        if( !UsbTransferData( dat, 1, 1 ) || dat[0] != SVU_ANS_OK ) {
            //Ошибка передачи данных
            setProcess( tr("Error"), false, tr("Failed reset VPUs") );
            return false;
        }

        if( !ControllerReady() ) {
            //Ошибка ожидания исполнения команды
            setProcess( tr("Error"), false, tr("Reset VPUs not complete") );
            return false;
        }

        setProcess( tr("Erase programm...") );

        //Стираем прежнюю программу
        dat[0] = SVU_CMD_FLASH_ERASE;
        if( !UsbTransferData( dat, 1, 1 ) || dat[0] != SVU_ANS_OK ) {
            //Ошибка передачи данных
            setProcess( tr("Error"), false, tr("Failed erase memory") );
            return false;
        }

        //Необходимо ожидание некоторого времени
        QThread::msleep(3000);

        //Проверить свободность контроллера
        if( !ControllerReady() ) {
            //Ошибка ожидания исполнения команды
            setProcess( tr("Error"), false, tr("Erase memory not complete") );
            return false;
        }

        //Отправляем данные поблочно
        int addr = 0;
        while( addr < mProgramm->codeCount() ) {
            setProcess( tr("Programming %1").arg(addr) );
            //Подготовим данные
            dat[0] = SVU_CMD_FLASH_WRITE;
            svIWrite32( dat + 1, addr );
            for( int i = 0; i < 56; ++i )
                if( addr + i < mProgramm->codeCount() )
                    dat[i+5] = mProgramm->getCode().at(addr + i);
                else
                    dat[i+5] = 0xff;

            //Проверить свободность контроллера
            if( !ControllerReady() ) {
                //Ошибка ожидания исполнения команды
                setProcess( tr("Error"), false, tr("Flash block not complete") );
                return false;
            }

            //Проверить свободность контроллера
            if( !FlashReady() ) {
                setProcess( tr("Error"), false, tr("Failed wait flash block") );
                return false;
            }

            //Отправить блок данных
            if( !UsbTransferData( dat, 61, 1 ) ) {
                //Ошибка передачи данных
                setProcess( tr("Error"), false, tr("Failed transfer flash block") );
                return false;
            }

            if( dat[0] == SVU_ANS_OK ) {
                //Данные переданы успешно и должны прошиться
                addr += 56;
            }
        }

        //Проверить свободность контроллера
        if( !ControllerReady() ) {
            //Ошибка ожидания исполнения команды
            setProcess( tr("Error"), false, tr("Flash final block not complete") );
            return false;
        }

        //В контроллер размещена программа (заново или проверена)
        //Проверить свободность контроллера
        if( !FlashReady() ) {
            setProcess( tr("Error"), false, tr("Failed final flash block") );
            return false;
        }
    }

    //Обновить информацию о контроллере
    ControllerInfo();

    return true;
}



//Проверить готовность флэш-памяти к выполнению операций
bool SvMirrorUsb10::FlashReady()
{
    unsigned char dat[64];

    for( int i = 0; i < 600; i++ ) {
        dat[0] = SVU_CMD_FLASH_STATE;
        if( UsbTransferData( dat, 1, 1 ) ) {
            if( dat[0] == SVU_ANS_OK ) {
                return true;
            }
            else continue;
        }
        else break;
    }
    UsbClose();
    return false;
}




//Проверить готовность контроллера к приему очередной команды
bool SvMirrorUsb10::ControllerReady()
{
    unsigned char dat[64];

    for( int i = 0; i < 100; i++ ) {
        dat[0] = SVU_CMD_STATE;
        if( UsbTransferData( dat, 1, 2 ) ) {
            if( dat[1] == SVU_ANS_OK ) {
                mVpuCount = dat[0];
                return true;
            }
            else continue;
        }
        else break;
    }
    UsbClose();
    return false;
}













//Получить состояние задач
void SvMirrorUsb10::ReceivTasksState()
{
    static int divider = 0;
    divider--;
    if( divider > 0 ) return;
    divider = 10;
    if( mVpuCount == 0 ) return;
    //Место под пакет
    quint8 dat[64 * 256];

    dat[0] = SVU_CMD_VPU_READ  ;
    dat[1] = (quint8)mVpuCount;
    dat[2] = 0;
    if( UsbTransferData( dat, 3, 64 * mVpuCount) ) {
        //Получено состояние всех vpu, расшифровать
        for( int i = 0; i < mVpuCount; i++ ) {
            int off = 64 * i;
            mVpuState[i].mIp = svIRead32( dat + off + 0 );
            mVpuState[i].mSp = svIRead32( dat + off + 4 );
            mVpuState[i].mBp = svIRead32( dat + off + 8 );
            mVpuState[i].mTp = svIRead32( dat + off + 12 );
            mVpuState[i].mTm = svIRead32( dat + off + 16 );
            mVpuState[i].mThrow = svIRead32( dat + off + 20 );
            mVpuState[i].mIsRun = dat[off + 24];
        }

        //Отправить сигнал об изменении состояния задач
        emit taskChanged();
    }
}



void SvMirrorUsb10::restart(bool runOrPause)
{
    //Если контроллер не подключен, то ничего не делаем
    if( !mLink ) {
        setProcess( tr("Error"), false, tr("Controller is not linked") );
        return;
    }

    setProcess( tr("Starting") );
    //Иначе отправляем команду
    unsigned char dat[64];
    dat[0] = SVU_CMD_VPU_RESTART;
    dat[1] = runOrPause ? 1 : 0; //Без отладки
    if( UsbTransferData( dat, 2, 1 ) )
        if( dat[0] == SVU_ANS_OK ) {
            //Все нормально - завершить
            setProcess( tr("Complete"), false );
            return;
        }
    setProcess( tr("Error"), false, tr("Can't restart run controller") );
    UsbClose();
}



void SvMirrorUsb10::compileFlashRun(bool link, bool flash, bool runOrPause)
{
    QMutexLocker locker(&mProcessingMutex);
    //QMutexLocker(&);
    //Отправим сигнал о начале операции
    setProcess( tr("Compile %1 in %2").arg(mMainScript).arg(mPrjPath) );

    //Выполнить компиляцию
    make();

    QString errorString;
    if( mProgramm->mErrors.count() ) {
        //При наличии ошибок прекращаем
        foreach (auto error, mProgramm->mErrors) {
            errorString += error.mError + " in " + error.mFile + ": " + error.mLine + "\n";
            qDebug() << error.mError << " in " << error.mFile << ": " << error.mLine;
        }
        setProcess( tr("Complete"), false, tr("Compile errors: ") + errorString );
        return;
    }

    setProcess(tr("Compilation success"), false);

    if( flash || link ) {
        //Если контроллер не подключен, то ничего не делаем
        if( !mLink ) {
            setProcess( tr("Error"), false, tr("Controller is not linked") );
            return;
        }

        bool equal;
        //Сравнить программы, прошить, если разные
        if( Flash( &equal ) ) {
            //Если не задана линковка или программа была прошита, то выполнить рестарт
            if( !link || !equal ){
                qDebug() << "restarting on pause: " << runOrPause;
                restart( runOrPause );
            }
            else
                setProcess( tr("Compile flash success"), false );
        }
    }
    else
        setProcess( tr("Compile flash success"), false );
}



#endif
