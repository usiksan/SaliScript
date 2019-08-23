/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Структура пакета
*/
#ifndef SVNETPACKETINFO_H
#define SVNETPACKETINFO_H

#include <QString>

class SvNetPacketInfo
  {
    quint8 mCommand;   //Command with block description
    quint8 mLenght[4]; //Block lenght
  public:
    SvNetPacketInfo( int cmd = 0, int len = 0 );

    //Command access
    void setCommand( int cmd, int len = 0 );
    int  command() const { return mCommand; }

    //Lenght access
    void setLenght( int len );
    int  lenght() const;
  };

#endif // SVNETPACKETINFO_H
