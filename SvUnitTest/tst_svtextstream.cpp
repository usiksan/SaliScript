#include <QtTest>
#include <QDebug>

// add necessary includes here
#include "../SvHost/SvTextStreamIn.h"
#include "../SvHost/SvTextStreamOut.h"

class SvTextStream : public QObject
  {
    Q_OBJECT

  public:
    SvTextStream();
    ~SvTextStream();

  private slots:
    void test_case1();

  };

SvTextStream::SvTextStream()
  {

  }

SvTextStream::~SvTextStream()
  {

  }

void SvTextStream::test_case1()
  {
  qDebug() << "";
  SvTextStreamOut out;
  out.begin('a');
  out.addInt8(0x80);
  out.addInt8(0x7f);
  out.addInt8(0x01);
  out.addInt16(0x1234);
  out.addInt32(0x87654321);
  out.addBlock( "primer", 7 );
  out.addInt8(0x21);
  out.end();

  SvTextStreamIn in( out.buffer() );
  QCOMPARE( in.getCmd(), 'a' );
  QCOMPARE( in.getInt8(), 0x80 );
  QCOMPARE( in.getInt8(), 0x7f );
  QCOMPARE( in.getInt8(), 0x01 );
  QCOMPARE( in.getInt16(), 0x1234 );
  QCOMPARE( in.getInt32(), 0x87654321 );
  char buf[10];
  in.getBlock( buf, 7 );
  QCOMPARE( strcmp( buf, "primer" ), 0 );
  QCOMPARE( in.getInt8(), 0x21 );
  }

QTEST_APPLESS_MAIN(SvTextStream)

#include "tst_svtextstream.moc"
