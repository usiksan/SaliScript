/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "SvNet управление скриптами по сети"
  Автор
    Alexander Sibilev
  Интернет
    www.saliLab.ru
    www.saliLab.com

  Описание
    SvNetBlockInfo - заголовок пакета.
    Пакет - основа обмена информацией. Любые данные, передаваемые по сети представляют
    собой блок байтов предваряемых данной структурой. В структуре описывается команда,
    которая определяет способ декодирования блока данных и длина этого блока для
    сборки пакета на приемной стороне
*/
#include "SvNetBlockInfo.h"


//!
//! \brief SvNetBlockInfo Конструктор с возможностью задания команды и длины данных
//! \param cmd            Команда
//! \param len            Длина блока данных
//!
SvNetBlockInfo::SvNetBlockInfo(int cmd, int len)
  {
  setCommand( cmd, len );
  }




//!
//! \brief setCommand Установить команду и длину блока данных
//! \param cmd        Команда
//! \param len        Длина блока данных
//!
//! Код команды записывается в поле mCommand, а длина - в поле mLenght
void SvNetBlockInfo::setCommand(int cmd, int len)
  {
  //Установить команду
  mCommand = static_cast<quint8>(cmd);
  //и установить длину данных
  setLenght( len );
  }





//!
//! \brief setLenght Установить длину блока данных. Длину записывает в поле mLenght
//! \param len       Длина блока данных
//!
void SvNetBlockInfo::setLenght(int len)
  {
  mLenght[0] = static_cast<quint8>(len & 0xff);
  mLenght[1] = static_cast<quint8>((len >> 8) & 0xff);
  mLenght[2] = static_cast<quint8>((len >> 16) & 0xff);
  mLenght[3] = static_cast<quint8>((len >> 24) & 0xff);
  }




//!
//! \brief lenght Возвращает длину блока данных
//! \return       Длина блока данных
//!
int SvNetBlockInfo::lenght() const
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


