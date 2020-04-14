#include "SvComBook.h"
#include "SvMirrorCom.h"
#include "SvHost/SvTextStreamIn.h"
#include "SvHost/SvTextStreamOut.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

QString stage;

SvMirrorCom::SvMirrorCom(const QString portName, bool doScanTask ) :
  SvMirrorExtern(),
  mSerialPort(),
  mPrefferedPort(portName),
  mPortIndex(0),
  mFlashIndex(0),
  mTimeOut(0),
  mMemoryBlockIndex(0),
  mVpuIndex(0)
  {
  mScanTasks = doScanTask;
  //Подключить порт к приемнику данных
  connect( &mSerialPort, &QSerialPort::readyRead, this, &SvMirrorCom::bytesRead );
  }




QString SvMirrorCom::controllerType() const
  {
  if( mControllerType.isEmpty() )
    return tr("USB not found");
  return tr("USB %1 -> %2 (Serial: %3)").arg(mFactPort).arg(mControllerType).arg(mSerial);
  }




void SvMirrorCom::linkTo(const QString ipOrUsb, int port, const QString controllerName, const QString controllerPassw)
  {
  Q_UNUSED(port)
  Q_UNUSED(controllerPassw)
  mPrefferedPort = ipOrUsb;
  mControllerName = controllerName;
  mSerialPort.close();
  }




void SvMirrorCom::setProgrammFlashRun(SvProgrammPtr prog, bool runOrPause, bool flash)
  {
  if( flash ) {
    mRunOrPause = runOrPause;
    mProgramm   = prog;
    mNeedFlash  = true;
    qDebug() << "start flash";
    }
  }




void SvMirrorCom::processing(int tickOffset)
  {
  mTimeOut -= tickOffset;

  if( mSerialPort.isOpen() ) {
    //Порт открыт, продолжаем обмен
    if( mTimeOut < 0 )
      //Если время ожидания истекло, то закрываем порт
      portClose();
    }
  else {
    //Порт не открыт, пробуем открыть

    if( mPrefferedPort.isEmpty() ) {
      //Порт не задан, выбираем автоматически
      auto serialPortList = QSerialPortInfo::availablePorts();
      if( mPortIndex >= serialPortList.count() ) mPortIndex = 0;
      if( mPortIndex < serialPortList.count() ) {
        mFactPort = serialPortList.at(mPortIndex++).portName();
        }
      else
        mFactPort.clear();
      }
    else mFactPort = mPrefferedPort;

    mSerialPort.setPortName( mFactPort );
    if( mSerialPort.open( QIODevice::ReadWrite ) ) {
      //Прочитать все, что накоплено в буфере порта
      mSerialPort.readAll();
      qDebug() << "opened" << mFactPort;
//      mSerialPort.write( "primer" );
//      qDebug() << "primer sended";
      stageGet( SV_CB_INFO_GET, 200 );
      }
    }
  }



void SvMirrorCom::init()
  {

  }



void SvMirrorCom::bytesRead()
  {
  while( mSerialPort.canReadLine() ) {
    QByteArray line = mSerialPort.readAll().simplified();
    //qDebug() << "received" << line;
    SvTextStreamIn in( line.data() );
    switch( in.getCmd() ) {
      case SV_CB_INFO : {
        //Получена информация от контроллера
        // Информация о контроллере
        // SV_CB_INFO
        // i8[20] - тип контроллера
        // i8     - количество доступных процессоров
        // i32[3] - серийный номер процессора
        // i32    - hash-значение загруженной программы
        // i8[20] - наименование загруженной программы
        char type[21];
        for( int i = 0; i < 20; i++ )
          type[i] = in.getInt8();
        type[20] = 0;

        int vpuCount = in.getInt8();
        if( mVpuState.count() != vpuCount )
          mVpuState.resize(vpuCount);

        if( mVpuDebug.count() < mVpuState.count() ) {
          //Дополнить количество отладочных ячеек до количества процессов
          mVpuDebug.resize( mVpuState.count() );
          clearDebug();
          }

        if( mMemory.count() == 0 ) {
          mMemory.resize( mMemoryCount = 100 );
          }

        int serial[3];
        serial[0] = in.getInt32();
        serial[1] = in.getInt32();
        serial[2] = in.getInt32();
        QString serialString = QString("%1%2%3").arg( serial[0], 8, 16, QChar('0')).arg( serial[1], 8, 16, QChar('0')).arg( serial[2], 8, 16, QChar('0'));
        qDebug() << "serial" << serialString;
        int hash = in.getInt32();
        Q_UNUSED(hash);
        char name[21];
        for( int i = 0; i < 20; i++ ) {
          name[i] = in.getInt8();
          //Все непечатные символы заменяем на черточку
          if( name[i] & 0x80 || name[i] < ' ' )
            name[i] = '-';
          }
        name[20] = 0;

        //Сформируем имя контроллера из его серийного номера
        if( !mControllerName.isEmpty() && mControllerName != serialString ) {
          //Задано имя контроллера и оно не совпадает с контроллером
          //Поэтому будем переходить к следующему контроллеру
          mSerialPort.close();
          return;
          }

        //Проверить тип контроллера и имя программы. Совпадают ли с текущими
        QString theControllerType = QString::fromUtf8( type );
        QString theProgrammName = QString::fromUtf8( name );
        if( mControllerType != theControllerType || mProgrammName != theProgrammName || mSerial != serialString ) {

          //Обновить тип контроллера
          mControllerType = theControllerType;
          //Обновить имя программы
          mProgrammName = theProgrammName;
          //Обновить серийный номер контроллера
          mSerial = serialString;

          //Отправить извещение о подключении
          emit linkChanged( true, controllerType(), programmName() );

          }

        stageGet( SV_CB_TICKET_GET, 100 );
        }
        break;


      case SV_CB_BUSY :
        if( in.getInt8() )
          //Пока занят, продолжаем спрашивать
          stageGet( SV_CB_BUSY_GET, 0 );
        else
          //Освободился, продолжаем загрузку
          stageFlash();
        break;


      case SV_CB_TICKET : {
        //Получено извещение
        int ticketId = in.getInt16();
        //Проверить, есть ли извещение. Если есть, то возимся дальше
        //иначе - ничего не делаем
        if( ticketId ) {
          //Получить параметры
          int p[8];
          for( auto &v : p )
            v = in.getInt32();
          //Отправить сигнал с извещением
          emit ticket( ticketId, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
          }

        //Если нужно прошить программу, то переходим к стиранию памяти
        if( mNeedFlash ) {
          qDebug() << "begin erase";
          mFlashIndex = 0;
          mNeedFlash = false;
          stageGet( SV_CB_ERASE, 10000 );
          }
        else
          stageMemorySet();
        }
        break;



      case SV_CB_MEMORY : {
        int addr = in.getInt16();
        if( mMemory.count() < addr + 12 )
          mMemory.resize( addr + 12 );
        for( int i = 0; i < 12; i++ )
          mMemory[addr + i] = in.getInt32();
        mMemoryBlockIndex = addr + 12;
        stageGet( SV_CB_TICKET_GET, 100 );
        }
        break;


      case SV_CB_VPU : {
        //Номер vpu
        int i = in.getInt8();
        if( i < mVpuState.count() ) {
          SvVmVpuState st;
          st.mIp       = in.getInt32(); //!< instruction pointer [указатель инструкций]
          st.mSp       = in.getInt32(); //!< stack pointer [указатель стека]
          st.mBp       = in.getInt32(); //!< function frame pointer [указатель базы в стеке локальных переменных для текущей функции (указывает на фрейм возврата из функции)]
          st.mTm       = in.getInt32(); //!< exception mask [маска обрабатываемых исключений]
          st.mBaseSp   = in.getInt32(); //!< stack start [Начало стека для данного процессора]
          st.mThrow    = in.getInt32(); //!< current exception [Текущее значение исключения]
          st.mDebugRun = in.getInt32(); //!< if eq 0 then in debug state else - in run state
          mVpuState[i] = st;
          }
        stageVpuGet();
        }
        break;


      case SV_CB_LOG : {
        int count = in.getInt8();
        while( count ) {
          int textIndex = in.getInt32();
          int value0    = in.getInt32();
          int value1    = in.getInt32();
          int value2    = in.getInt32();

          QString pat;

          if( mProgramm != nullptr && textIndex < mProgramm->mStrings.count() ) {
            //Получить строку
            pat = mProgramm->mStrings.at( textIndex );
            //Теперь добавляем параметры, если есть
            if( pat.contains("%3") )
              pat = pat.arg(value0).arg(value1).arg(value2);
            if( pat.contains("%2") )
              pat = pat.arg(value0).arg(value1);
            if( pat.contains("%1") )
              pat = pat.arg(value0);
            }
          else {
            //В программе нету соответствующей строки, формируем общий лог
            pat = QString("Log[%1]: %2, %3, %4").arg( textIndex ).arg( value0 ).arg( value1 ).arg( value2 );
            }

          emit log( pat );
          count--;
          }
        }
        //Теперь снова считывание информации
        stageGet( SV_CB_TICKET_GET, 100 );
        break;

      case SV_CB_DEBUG :
        //Подтверждена команда отладки, проверяем наличие следующих команд
        stageDebugSet();
        break;

      case SV_CB_MEMORY_SET :
        //Подтверждена запись переменных, проверяем следующие записи
        stageMemorySet();
        break;
      }
    }
  }




void SvMirrorCom::portClose()
  {
  qDebug() << "Time out" << stage;
  //Закрыть порт
  mSerialPort.close();
  if( !mControllerType.isEmpty() ) {
    //Сообщить, что порт закрыт и подключения нету
    mControllerType.clear();
    mSerial.clear();
    emit linkChanged( false, controllerType(), programmName() );
    }
  }



void SvMirrorCom::stageGet(int query, int timeOut )
  {
  //qDebug() << "stageGet" << query << nextStage;
  stage = QString("stage get %1 time %2").arg( QChar(query) ).arg( timeOut );
  //Сформировать запрос информации
  SvTextStreamOut out{};
  out.begin( query );
  out.end();

  //Отправить запрос
  mSerialPort.write( out.buffer() );

  //Переходим в состояние ожидания ответа
  if( timeOut )
    mTimeOut = timeOut;
  }






void SvMirrorCom::stageMemorySet()
  {
  if( mWriteQueue.count() ) {
    //Есть данные для передачи
    //Блокиратор сдвоенного доступа к очереди
    QMutexLocker locker(&mWriteMutex);

    //Сформировать запрос информации
    SvTextStreamOut out{};
    out.begin( SV_CB_MEMORY_SET );

    //Количество элементов для передачи
    int count = qMin( 8, mWriteQueue.count() );
    out.addInt8( count );

    for( int i = 0; i < count; i++ ) {
      //Адрес
      out.addInt16( mWriteQueue.first() );
      //Данные
      out.addInt32( mWriteValues.value(mWriteQueue.first()) );
      //Исключаем значение из карты
      mWriteValues.remove( mWriteQueue.first() );
      //Убрать значение из очереди
      mWriteQueue.removeFirst();
      }
    out.end();

    //Отправить запрос
    mSerialPort.write( out.buffer() );

    //Будем ожидать ответ
    mTimeOut = 100;

    stage = "stageMemorySet";
    return;
    }
  else stageMemoryGet();
  }




void SvMirrorCom::stageMemoryGet()
  {
  if( mMemoryBlockIndex < mMemoryCount ) {
    //Сформировать запрос информации
    SvTextStreamOut out{};
    out.begin( SV_CB_MEMORY_GET );
    out.addInt16( mMemoryBlockIndex );
    out.end();

    //Отправить запрос
    mSerialPort.write( out.buffer() );

    //Будем ожидать ответ
    mTimeOut = 100;

    stage = "stageMemoryGet";
    }
  else {
    //Известить, что память обновлена
    emit memoryChanged( this );

    //Подготовить следующий цикл обновления памяти
    mMemoryBlockIndex = 0;
    if( mScanTasks ) stageDebugSet();
    else             stageGet( SV_CB_LOG_GET, 100 );
    }
  }





void SvMirrorCom::stageVpuGet()
  {
  if( mVpuIndex < mVpuState.count() ) {
    //Сформировать запрос информации
    SvTextStreamOut out{};
    out.begin( SV_CB_VPU_GET );
    out.addInt8( mVpuIndex );
    out.end();

    //Отправить запрос
    mSerialPort.write( out.buffer() );

    //Будем ожидать ответ
    mTimeOut = 100;
    mVpuIndex++;

    stage = QString("stageVpuGet %1").arg(mVpuIndex);
    }
  else stageGet( SV_CB_LOG_GET, 100 );
  }




void SvMirrorCom::stageDebugSet()
  {
  //Проверить наличие команд отладчика
  for( int i = 0; i < mVpuDebug.count(); i++ )
    if( mVpuDebug.at(i).mCommand ) {
      //Есть данные для передачи
      //Блокиратор сдвоенного доступа к очереди
      QMutexLocker locker(&mVpuMutex);

      //Сформировать запрос информации
      SvTextStreamOut out{};
      out.begin( SV_CB_DEBUG );
      out.addInt8( i );
      out.addInt8( mVpuDebug.at(i).mCommand );
      out.addInt32( mVpuDebug.at(i).mParam1 );
      out.addInt32( mVpuDebug.at(i).mParam2 );
      out.end();

      //Отправить запрос
      mSerialPort.write( out.buffer() );

      stage = QString("stageDebugSet[%1] cmd=%2").arg(i).arg(mVpuDebug.at(i).mCommand);

      //Обозначить, что команда выполнена
      mVpuDebug[i].mCommand = 0;

      //Будем ожидать ответ
      mTimeOut = 1000;

      return;
      }

  //У отладчика нету команд, получить состояние процессоров
  mVpuIndex = 0;
  stageVpuGet();
  }




void SvMirrorCom::stageFlash()
  {
  if( mProgramm && mFlashIndex < mProgramm->codeCount() ) {
    int count = qMin( 48, mProgramm->codeCount() - mFlashIndex );
    //Сформировать запрос информации
    SvTextStreamOut out{};
    out.begin( SV_CB_FLASH );
    out.addInt32( mFlashIndex );
    out.addInt8( count );
    for( int i = 0; i < count; i++ )
      out.addInt8( mProgramm->getCode(mFlashIndex + i) );
    out.end();

    qDebug() << "flash" << mFlashIndex << count;
    stage = QString("stageFlash %1").arg(mFlashIndex);

    mFlashIndex += count;

    emit transferProcess( false, tr("Flash %1 bytes").arg( mFlashIndex ) );

    //Отправить запрос
    mSerialPort.write( out.buffer() );

    mTimeOut = 1000;
    }
  else {
    //Прошивка закончена, выполнить сброс
    //Сформировать запрос информации
    SvTextStreamOut out{};
    out.begin( SV_CB_RESET );
    out.addInt8( mRunOrPause ? 1 : 0 );
    out.end();

    //Отправить запрос
    mSerialPort.write( out.buffer() );

    emit transferProcess( true, QString{} );

    //Установить количество глобальных переменных в соответствии с программой
    mMemoryCount = mProgramm->globalCount();
    if( mMemoryCount > mMemory.count() )
      mMemory.resize( mMemoryCount );

    //Переходим в состояние ожидания ответа
    mTimeOut = 100;

    stage = QString("stageReset %1").arg(mRunOrPause);
    }
  }



