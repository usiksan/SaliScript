/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Модульный контроллер общего назначения"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Внешние функции-драйверы для аппаратных устройств контроллера и модулей.
    Константы для работы с функциями-драйверами.
*/
#ifndef SR6PLC_H
#define SR6PLC_H

#define DPLC_VERSION                 11

//Типы модулей
#define DMT_DISCRET_I4               1 //Дискретные входы
#define DMT_DISCRET_O4               2 //Дискретные выходы (верхний транзистор +U) (нижний транзистор -U)
#define DMT_DISCRET_PWM4             3 //ШИМ модулятор (верхний транзистор +U) (нижний транзистор -U) mode - маска выводов, задействованных под ШИМ (1-ШИМ, 0-обычный выход)
#define DMT_CAN_CH1                  4 //Модуль CAN шины
#define DMT_PCAN                     5 //Модуль PCAN
#define DMT_STEP_MOTOR               6 //Модуль шагового двигателя
#define DMT_CAPTURE_I4               7 //Измеритель длительности импульсов с дискретных входов
#define DMT_ANALOG_AD7192            8 //Измеритель аналоговых сигналов дифференциальный
#define DMT_MIDDLE_WINDOW            9 //Механизм получения среднего значения методом окна
#define DMT_ANALOG_O2               10 //Формирователь аналогового сигнала
#define DMT_ANALOG_O1               11 //Развязанный формировать аналогового сигнала

//Управление модулями
#define MBF_VERSION                 98
#define MBF_MODULE                  99
#define MBF_SLOT                   100
#define MBF_MODE                   101
#define MBF_SETMODE                102

//Канал SPI
#define MBF_SPI_TRANSFER           110
#define MBF_SPI_ISBUSY             111
#define MBF_SPI_VALUE              112


#define MBF_PIN4STATE              120
#define MBF_PIN4OUT                121
#define MBF_PIN4SET                122
#define MBF_PIN4CLR                123

#define MBF_PWM4SETUP              130
#define MBF_PWM4SET                131

#define MBF_CANRESTART             140
#define MBF_CANSTATUS              141
#define MBF_CANMASTERSDORESET      142
#define MBF_CANMASTERSDO           143
#define MBF_CANMASTERRAW           144
#define MBF_CANMASTERADDPDOHANDLER 145
#define MBF_CANMASTERSDOSTATE      146
#define MBF_CANMASTERSDOVALUE      147
#define MBF_CANCLIENTADDSDOHANDLER 148
#define MBF_CANCLIENTPDO           149

#define MBF_PIN4ANIN               150
#define MBF_PIN4ANOUT              151

#define MBF_PIN4CAPTURE            160
#define MBF_PIN4CAPTURE_SETUP      161
#define MBF_PIN4CAPTURE_STATUS     162
#define MBF_PIN4CAPTURE_FRIQ       163

#define MBF_MIDDLE_RESET           170
#define MBF_MIDDLE_APPEND          171
#define MBF_MIDDLE_GET             172



#define	CANO_REG_READ              0x40
#define	CANO_REG_READ_STRING       0x41
#define	CANO_REG_READ_4BYTE        0x43
#define	CANO_REG_READ_2BYTE        0x4B
#define	CANO_REG_READ_1BYTE        0x4F

#define	CANO_REG_WRITE_STRING      0x21
#define	CANO_REG_WRITE_4BYTE       0x23
#define	CANO_REG_WRITE_2BYTE       0x2B
#define	CANO_REG_WRITE_1BYTE       0x2F

#define	CANO_ID_NMT                 0x0
#define	CANO_ID_SYNC               0x80
#define	CANO_ID_EMERGENCY          0x81
#define	CANO_ID_EMERGENCY_END     0x100
#define	CANO_ID_PDO1_TXD          0x180
#define	CANO_ID_PDO1_RXD          0x200
#define	CANO_ID_PDO2_TXD          0x280
#define	CANO_ID_PDO2_RXD          0x300
#define	CANO_ID_PDO3_TXD          0x380
#define	CANO_ID_PDO3_RXD          0x400
#define	CANO_ID_PDO4_TXD          0x480
#define	CANO_ID_PDO4_RXD          0x500
#define	CANO_ID_SDO_TXD           0x580
#define	CANO_ID_SDO_RXD           0x600
#define	CANO_ID_DETECT            0x700
#define CANO_ID_DETECT_END        0x800

#define	CANO_ID_NODE_GUARDING    0x1792
#define	CANO_ID_HEART_NEATING    0x1792

#define MBF_ERR_FAIL                  0
#define MBF_ERR_OK                    1
#define MBF_ERR_WAIT                  2

#define CAN_1MB                       0 //Скорость по CAN-шине 1МБит
#define CAN_500KB                     6 //500КБит
#define CAN_250KB                    12 //250КБит
#define CAN_100KB                    30 //100КБит

#define CAN_ERR_OK                    MBF_ERR_OK
#define CAN_ERR_WRONG_ANSWER          10
#define CAN_ERR_NO_DLL                11
#define CAN_ERR_FAIL_INIT             12

#define PCAN_ISA_1CH                   0 // ISA 1 Channel
#define PCAN_ISA_2CH                   1 // ISA 2 Channels
#define PCAN_PCI_1CH                   2 // PCI 1 Channel
#define PCAN_PCI_2CH                   3 // PCI 2 Channels
#define PCAN_PCC_1CH                   4 // PCC 1 Channel
#define PCAN_PCC_2CH                   5 // PCC 2 Channels
#define PCAN_USB_1CH                   6 // USB 1st Channel
#define PCAN_USB_2CH                   7 // USB 2nd Channel
#define PCAN_DNP                       8 // DONGLE PRO
#define PCAN_DNG                       9 // DONGLE

#define CAPTURE_POLARITY_RISING        0
#define CAPTURE_POLARITY_FALLING       0x20
#define CAPTURE_POLARITY_BOTH          0xa0

#define CAPTURE_DIV1                   0
#define CAPTURE_DIV2                   0x400
#define CAPTURE_DIV4                   0x800
#define CAPTURE_DIV8                   0xc00

#define CAPTURE_DIRECT_TI              0x1000
#define CAPTURE_INDIRECT_TI            0x2000
#define CAPTURE_TRC                    0x3000


#ifdef VPU_COMPILER
import MBF_VERSION                 int ModuleLibVersion();

import MBF_MODULE                  int Module( int slot, int type, int mode );
import MBF_SLOT                    int ModuleSlot( int module );
import MBF_MODE                    int ModuleMode( int module );
import MBF_SETMODE                 int ModuleSetMode( int module, int mode );

import MBF_SPI_TRANSFER            int ModuleSpiTransfer( int module, int cmd, int len, int value );
import MBF_SPI_ISBUSY              int ModuleSpiIsBusy( int module );
import MBF_SPI_VALUE               int ModuleSpiValue( int module );

import MBF_PIN4STATE               int ModulePin4State( int module );
import MBF_PIN4OUT                 int ModulePin4Out( int module, int pins );
import MBF_PIN4SET                 int ModulePin4Set( int module, int pins );
import MBF_PIN4CLR                 int ModulePin4Clr( int module, int pins );

import MBF_PIN4CAPTURE             int ModulePin4Capture( int module, int pin );
import MBF_PIN4CAPTURE_STATUS      int ModulePin4CaptureStatus( int module, int pin );
import MBF_PIN4CAPTURE_SETUP       int ModulePin4CaptureSetup( int module, int pin, int mode );
import MBF_PIN4CAPTURE_FRIQ        int ModulePin4CaptureFriq( int module, int friqHz );

import MBF_PWM4SETUP               int ModulePwm4Setup( int module, int prescaler, int period );
import MBF_PWM4SET                 int ModulePwm4Set( int module, int pin, int pwm );

import MBF_CANRESTART              int ModuleCanRestart( int module );
import MBF_CANSTATUS               int ModuleCanStatus( int module );
import MBF_CANMASTERSDORESET       int ModuleCanMasterSdoReset( int module, int id );
import MBF_CANMASTERSDO            int ModuleCanMasterSdo( int module,  int id, int mode, int index, int subIndex, int value );
import MBF_CANMASTERRAW            int ModuleCanMasterRaw( int module,  int id, int len, int value0, int value1 );
import MBF_CANMASTERADDPDOHANDLER  int ModuleCanMasterAddPdoHandler( int module, int id, void (*fun)( int value0, int value1 ) );
import MBF_CANMASTERSDOSTATE       int ModuleCanMasterSdoState( int module, int id );
import MBF_CANMASTERSDOVALUE       int ModuleCanMasterSdoValue( int module, int id );
import MBF_CANCLIENTADDSDOHANDLER  int ModuleCanClientAddSdoHandler( int module, int id, void (*fun)( int value0, int value1 ) );
import MBF_CANCLIENTPDO            int ModuleCanClientPdo( int module, int id, int len, int value0, int value1 );

import MBF_PIN4ANIN                int ModulePin4AnIn( int module, int pin );
import MBF_PIN4ANOUT               int ModulePin4AnOut( int module, int pin, int value );

import MBF_MIDDLE_RESET            int ModuleMiddleReset( int module );
import MBF_MIDDLE_APPEND           int ModuleMiddleAppend( int module, int value );
import MBF_MIDDLE_GET              int ModuleMiddleGet( int module );


#endif //VPU_COMPILER

#endif //SR6PLC_H
