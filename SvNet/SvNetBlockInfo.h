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
#ifndef SVNETBLOCKINFO_H
#define SVNETBLOCKINFO_H


#include <QString>

/*!
   \brief The SvNetBlockInfo class - Заголовок блока

    Блок - основа обмена информацией. Любые данные, передаваемые по сети представляют
    собой блок байтов предваряемых данной структурой. В структуре описывается команда,
    которая определяет способ декодирования данных блока и длина этих данных для
    сборки блока на приемной стороне
 */
class SvNetBlockInfo
  {
    quint8 mCommand;   //! Команда, определяющая способ декодирования блока данных
    quint8 mLenght[4]; //! Длина блока данных. Длина данной структуры не входит в этот размер
  public:
    //!
    //! \brief SvNetBlockInfo Конструктор с возможностью задания команды и длины данных
    //! \param cmd            Команда
    //! \param len            Длина блока данных
    //!
    SvNetBlockInfo( int cmd = 0, int len = 0 );

    //!
    //! \brief setCommand Установить команду и длину блока данных
    //! \param cmd        Команда
    //! \param len        Длина блока данных
    //!
    //! Код команды записывается в поле mCommand, а длина - в поле mLenght
    void setCommand( int cmd, int len = 0 );


    //!
    //! \brief command Возвращает код команды
    //! \return        Код команды
    //!
    int  command() const { return mCommand; }

    //!
    //! \brief setLenght Установить длину блока данных. Длину записывает в поле mLenght
    //! \param len       Длина блока данных
    //!
    void setLenght( int len );

    //!
    //! \brief lenght Возвращает длину блока данных
    //! \return       Длина блока данных
    //!
    int  lenght() const;

  };

#endif // SVNETBLOCKINFO_H
