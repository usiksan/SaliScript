/*
  Проект "Пакетный обмен по локальной сети и через интернет-мост"
  Автор
    Сибилев А.С.
  Описание
    Обмен файлами. Основное назначение - клиент для исполнения удаленных заданий по файловому обмену
*/
#ifndef SVNETCHANNELFILE_H
#define SVNETCHANNELFILE_H

#include "SvNetChannel.h"







class SvNetChannelFile : public QObject
  {
    Q_OBJECT
  public:
    SvNetChannelFile();

  signals:
    /*!
       \brief sendBlock Send block with appropriate command
       \param ch        Channel which must send block or nullptr for all connected channels
       \param cmd       Command for block
       \param ar        Data block

       Block is completed part of interchange data. Each block follow with
       block command which descripting block data. Before block transfer it
       prepare with CsPacketInfo struct which contains block command and
       block size follows block data.
     */
    void sendBlock( SvNetChannel *ch, qint8 cmd, QByteArray block );


  public slots:
    /*!
       \brief receivedBlock Block received
       \param ch            Channel which must send block or nullptr for all connected channels
       \param cmd           Command for block
       \param ar            Data block

       This signal emited on completing block reaciving. In connected classes
       in this function must be block data decoding.
     */
    void onReceivedBlock( SvNetChannel *ch, int cmd, QByteArray block );

    void setChannel( SvNetChannel *ch );

  private:
    void onDirListRequest( SvNetChannel *ch, const QByteArray bl );

    void onFile( SvNetChannel *ch, const QByteArray bl );

    void onFileRequest( SvNetChannel *ch, const QByteArray bl );
  };

#endif // SVNETCHANNELFILE_H
