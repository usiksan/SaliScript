#include "SvNetHandlerMirror.h"

SvNetHandlerMirror::SvNetHandlerMirror(QObject *parent) : QObject(parent)
  {

  }

void SvNetHandlerMirror::setChannel(SvNetChannel *ch)
  {
  connect( ch, &SvNetChannel::receivedBlock, this, &SvNetHandlerMirror::onReceivedBlock );
  connect( this, &SvNetHandlerMirror::sendBlock, ch, &SvNetChannel::sendBlock );
  }
