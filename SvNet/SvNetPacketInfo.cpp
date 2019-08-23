/*
  Проект "Сервер сервисного обслуживания тележек"
  Автор
    Сибилев А.С.
  Описание
    Пакетный обмен между сервером и тележкой.
    Основной принцип: запрос-ответ

*/
#include "SvNetPacketInfo.h"

SvNetPacketInfo::SvNetPacketInfo(int cmd, int len)
  {
  setCommand( cmd, len );
  }



//Command set
void SvNetPacketInfo::setCommand(int cmd, int len)
  {
  //Setup command
  mCommand = static_cast<quint8>(cmd);
  //and set lenght
  setLenght( len );
  }





//Lenght access
void SvNetPacketInfo::setLenght(int len)
  {
  mLenght[0] = static_cast<quint8>(len & 0xff);
  mLenght[1] = static_cast<quint8>((len >> 8) & 0xff);
  mLenght[2] = static_cast<quint8>((len >> 16) & 0xff);
  mLenght[3] = static_cast<quint8>((len >> 24) & 0xff);
  }



int SvNetPacketInfo::lenght() const
  {
  int val;
  val = mLenght[3];
  val <<= 8;
  val |= mLenght[2];
  val <<= 8;
  val |= mLenght[1];
  val <<= 8;
  val |= mLenght[0];
  return val;
  }
