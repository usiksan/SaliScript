#include "SvNetHandler.h"

SvNetHandler::SvNetHandler(QObject *parent) : QObject(parent)
  {

  }

void SvNetHandler::addNetChannel(SvNetChannel *channel)
  {
  //Make connections to channel
  connect( channel, &SvNetChannel::receivedBlock, this, &SvNetHandler::receivedBlock );
  connect( channel, &SvNetChannel::receivedAnswer, this, &SvNetHandler::receivedAnswer );
  }
