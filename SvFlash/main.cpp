#include "SvFlash.h"
#include <QApplication>

#include "../SvStudio/Host/SvMirrorThread.h"
#include "../SvStudio/Host/SvMirrorUsb10.h"

SvMirror *svMirror;

int main(int argc, char *argv[])
  {
  QApplication a(argc, argv);

  //Образовать зеркало
  svMirror = new SvMirrorUsb10(false);
  svMirror->settings( QString(), QString(), QString("62.109.11.83"), 1971, QString(), QString(), 0x2743, 0x1971 );
  //Образовать задачу для связи
  SvMirrorThread *thread = new SvMirrorThread( svMirror );
  thread->start();

  SvFlash w;
  w.show();

  int res = a.exec();

  //Остановить связную задачу
  thread->quit();
  thread->wait();
  delete thread;

  return res;
  }
