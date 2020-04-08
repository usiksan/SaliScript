/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
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


  usb k3 k2 k1 k0  k3 k2 k1 k0  k3 k2 k1 k0  k3 k2 k1 k0   k3 k2
               5            3            8            9

      2            7            4            6             1
  + - k0 k1 k2 k3  k0 k1 k2 k3  k0 k1 k2 k3  k0 k1 k2 k3   k0 k1

pwm: 2, 4, 5, 6
0.5pwm: 9
can: 3, 6
usart: 5, 7, 8
*/
#ifndef SR6PLC_H
#define SR6PLC_H

#define SV_PLC_VERSION              11

//Типы модулей
#define SVM_DISCRET_I4               1 //Дискретные входы
#define SVM_DISCRET_O4               2 //Дискретные выходы (верхний транзистор +U) (нижний транзистор -U)
#define SVM_DISCRET_PWM4             3 //ШИМ модулятор (верхний транзистор +U) (нижний транзистор -U) mode - маска выводов, задействованных под ШИМ (1-ШИМ, 0-обычный выход)
#define SVM_CAN_RAW                  4 //Модуль CAN шины с raw доступом
#define SVM_CAN_OPEN                 5 //Модуль CAN шины с поддержкой CanOpen
#define SVM_STEP_MOTOR_L6470         6 //Модуль шагового двигателя
#define SVM_CAPTURE_I4               7 //Измеритель длительности импульсов с дискретных входов
#define SVM_ANALOG_AD7192            8 //Измеритель аналоговых сигналов дифференциальный
#define SVM_MIDDLE_WINDOW            9 //Механизм получения среднего значения методом окна
#define SVM_ANALOG_O2               10 //Формирователь аналогового сигнала
#define SVM_ANALOG_O1               11 //Развязанный формировать аналогового сигнала

//Управление модулями
#define MBF_VERSION                100
#define MBF_MODULE                 101
#define MBF_SLOT                   102
#define MBF_MODE                   103

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
#define MBF_CANOPEN_SDORESET       142
#define MBF_CANOPEN_SDO            143
#define MBF_CANOPEN_RAW            144
#define MBF_CANOPEN_ADDHANDLER     145
#define MBF_CANOPEN_SDOSTATE       146
#define MBF_CANOPEN_SDOVALUE       147

#define MBF_PIN4ANIN               150
#define MBF_PIN4ANOUT              151

#define MBF_PIN4CAPTURE            160
#define MBF_PIN4CAPTURE_SETUP      161
#define MBF_PIN4CAPTURE_STATUS     162
#define MBF_PIN4CAPTURE_FRIQ       163

#define MBF_MIDDLE_RESET           170
#define MBF_MIDDLE_APPEND          171
#define MBF_MIDDLE_GET             172

#define MBF_CANRAW_WRBEGIN         180
#define MBF_CANRAW_WR8             181
#define MBF_CANRAW_WR16LS          182
#define MBF_CANRAW_WR16MS          183
#define MBF_CANRAW_WR32LS          184
#define MBF_CANRAW_WR32MS          185
#define MBF_CANRAW_WREND           186

#define MBF_CANRAW_RDCOUNT         187
#define MBF_CANRAW_RDBEGIN         188
#define MBF_CANRAW_RD8             189
#define MBF_CANRAW_RD16LS          190
#define MBF_CANRAW_RD16MS          191
#define MBF_CANRAW_RD32LS          192
#define MBF_CANRAW_RD32MS          193


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
import MBF_VERSION                 int  ModuleLibVersion();

import MBF_MODULE                  int  Module( int slot, int type, int mode );
import MBF_SLOT                    int  ModuleSlot( int module );
import MBF_MODE                    int  ModuleMode( int module );

import MBF_SPI_TRANSFER            void ModuleSpiTransfer( int module, int cmd, int len, int value );
import MBF_SPI_ISBUSY              int  ModuleSpiIsBusy( int module );
import MBF_SPI_VALUE               int  ModuleSpiValue( int module );

import MBF_PIN4STATE               int  ModulePin4State( int module );
import MBF_PIN4OUT                 void ModulePin4Out( int module, int pins );
import MBF_PIN4SET                 void ModulePin4Set( int module, int pins );
import MBF_PIN4CLR                 void ModulePin4Clr( int module, int pins );

import MBF_PIN4CAPTURE             int  ModulePin4Capture( int module, int pin );
import MBF_PIN4CAPTURE_STATUS      int  ModulePin4CaptureStatus( int module, int pin );
import MBF_PIN4CAPTURE_SETUP       int  ModulePin4CaptureSetup( int module, int pin, int mode );
import MBF_PIN4CAPTURE_FRIQ        int  ModulePin4CaptureFriq( int module, int friqHz );

import MBF_PWM4SETUP               void ModulePwm4Setup( int module, int prescaler, int period );
import MBF_PWM4SET                 void ModulePwm4Set( int module, int pin, int pwm );

import MBF_CANRESTART              void ModuleCanRestart( int module );
import MBF_CANSTATUS               int  ModuleCanStatus( int module );

import MBF_CANOPEN_SDORESET        void ModuleCanOpenSdoReset( int module, int id );
import MBF_CANOPEN_SDO             void ModuleCanOpenSdo( int module,  int id, int mode, int index, int subIndex, int value );
import MBF_CANOPEN_RAW             void ModuleCanOpenRaw( int module,  int id, int len, int value0, int value1 );
import MBF_CANOPEN_ADDHANDLER      void ModuleCanOpenAddHandler( int module, int id, void (*fun)( int value0, int value1 ) );
import MBF_CANOPEN_SDOSTATE        int  ModuleCanOpenSdoState( int module, int id );
import MBF_CANOPEN_SDOVALUE        int  ModuleCanOpenSdoValue( int module, int id );


import MBF_PIN4ANIN                int  ModulePin4AnIn( int module, int pin );
import MBF_PIN4ANOUT               int  ModulePin4AnOut( int module, int pin, int value );

import MBF_MIDDLE_RESET            int  ModuleMiddleReset( int module );
import MBF_MIDDLE_APPEND           int  ModuleMiddleAppend( int module, int value );
import MBF_MIDDLE_GET              int  ModuleMiddleGet( int module );


import MBF_CANRAW_WRBEGIN          void ModuleCanRawWrBegin( int module, int id );
import MBF_CANRAW_WR8              void ModuleCanRawWr8( int module, int value );
import MBF_CANRAW_WR16LS           void ModuleCanRawWr16LS( int module, int value );
import MBF_CANRAW_WR16MS           void ModuleCanRawWr16MS( int module, int value );
import MBF_CANRAW_WR32LS           void ModuleCanRawWr32LS( int module, int value );
import MBF_CANRAW_WR32MS           void ModuleCanRawWr32MS( int module, int value );
import MBF_CANRAW_WREND            void ModuleCanRawWrEnd( int module );

import MBF_CANRAW_RDCOUNT          int  ModuleCanRawRdCount( int module );
import MBF_CANRAW_RDBEGIN          int  ModuleCanRawRdBegin( int module );
import MBF_CANRAW_RD8              int  ModuleCanRawRd8( int module );
import MBF_CANRAW_RD16LS           int  ModuleCanRawRd16LS( int module );
import MBF_CANRAW_RD16MS           int  ModuleCanRawRd16MS( int module );
import MBF_CANRAW_RD32LS           int  ModuleCanRawRd32LS( int module );
import MBF_CANRAW_RD32MS           int  ModuleCanRawRd32MS( int module );


#endif //VPU_COMPILER

#endif //SR6PLC_H
