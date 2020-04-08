/*
  Проект     "Скриптовый язык SaliScript: упрощенный c++"
  Подпроект  "Виртуальная машина"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Функции SPI
*/
#ifndef SPI_H
#define SPI_H

int
SpiTransfer( int module, int cmd, int len, int value ) {
  //Отправить команду на исполнение
  ModuleSpiTransfer( module, cmd, len, value );
  //Ожидать пока не исполнится
  while(1) {
    //Проверить, исполнилась ли команда
    //Если исполнилась, то прекратить
    if( ModuleSpiIsBusy( module ) == 0 ) break;
    svWait();
    }
  //Вернуть результат исполнения
  return ModuleSpiValue( module );
  }

#endif // SPI_H
