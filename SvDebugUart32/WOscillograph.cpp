#include "SvConfig.h"
#include "WOscillograph.h"
#include "WDebugTable.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QLabel>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>

#define WV_LEFT_GAP 50
#define WV_BOTTOM_GAP 40
#define WV_TOP_GAP 40
#define WV_RIGHT_GAP 10
#define WV_GRID_SPACE 100
#define WV_GRID_SPACE_V 70
#define WV_GRID_ADDON   5



WOscillograph::WOscillograph(WDebugTable *dt, QWidget *parent) :
  QWidget(parent),
  mDebugTable(dt),
  mData(nullptr),
  mRowIndex(0), //Текущий индекс данных
  mRowStart(0), //Номер начального ряда
  mRowCount(0), //Текущее количество рядов
  mRowMax(0),   //Максимальное количество рядов
  //mStart(false),    //Когда истина, тогда работает
  mWait(false),
  mMode(0),
  mSync(0),
  mLevel(0),
  mSyncIndex(0), //Переменная, используемая для синхронизации
  mPrevLevel(0),
  mScale(1),
  mEdge(0),      //Позиция среза
  mLeft(0),      //Левая позиция
  mStartView(0) //Начальная позиция отображения
  {
  mScanTimer = new QTimer();
  QSettings s;

  QHBoxLayout *box = new QHBoxLayout();
  //Максимальное количество рядов
  mEditRowMax   = new QLineEdit( s.value(QStringLiteral(CFG_OSCIL_ROW_MAX), QString("1000")).toString() );
  //Кнопка Старт
  mEditStart    = new QCheckBox( tr("Start-Stop") );
  //Режим работы Постоянно, Однократно
  mEditMode     = new QComboBox();
  //Режим старта По кнопке, По переднему фронту, По заднему фронту, По обоим
  mEditSync     = new QComboBox();
  //Уровень синхронизации
  mEditLevel    = new QLineEdit( s.value(QStringLiteral(CFG_OSCIL_LEVEL), QString("0")).toString() );
  //Интервал сканирования значений
  mEditInterval = new QLineEdit( s.value(QStringLiteral(CFG_OSCIL_INTERVAL), QString("10")).toString() );
  //Масштаб
  mEditScale    = new QLabel( tr("Scale: 1") );


  //Заполнить режим
  mEditMode->addItem( tr("Loop") );
  mEditMode->addItem( tr("Single") );
  mEditMode->setCurrentIndex( s.value(QStringLiteral(CFG_OSCIL_MODE), 0).toInt() );

  mEditSync->addItem( tr("By button") );
  mEditSync->addItem( tr("Rising edge") );
  mEditSync->addItem( tr("Falling edge") );
  mEditSync->addItem( tr("Both edge") );
  mEditSync->setCurrentIndex( s.value(QStringLiteral(CFG_OSCIL_SYNC), 0).toInt() );

  box->addWidget( new QLabel(tr("Buffer size:")) );
  box->addWidget( mEditRowMax );
  box->addWidget( mEditStart );
  box->addWidget( new QLabel(tr("Scan mode:")) );
  box->addWidget( mEditMode );
  box->addWidget( new QLabel(tr("Start mode:")) );
  box->addWidget( mEditSync );
  box->addWidget( new QLabel(tr("Sync level:")) );
  box->addWidget( mEditLevel );
  box->addWidget( new QLabel(tr("Scan interval:")) );
  box->addWidget( mEditInterval );
  box->addWidget( mEditScale );

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->addLayout( box );
  vbox->addStretch();

  mScrollBar = new QScrollBar( Qt::Horizontal, this );
  vbox->addWidget( mScrollBar );

  //Сигналы
  connect( mEditRowMax, &QLineEdit::returnPressed, this, &WOscillograph::onEditRowMax );
  connect( mEditStart,  &QCheckBox::toggled, this, &WOscillograph::onEditStartToggle );
  connect( mEditMode,   QOverload<int>::of(&QComboBox::currentIndexChanged), this, &WOscillograph::onEditMode );
  connect( mEditSync,   QOverload<int>::of(&QComboBox::currentIndexChanged), this, &WOscillograph::onEditSync );
  connect( mEditLevel,  &QLineEdit::returnPressed, this, &WOscillograph::onEditLevel );
  connect( mEditInterval, &QLineEdit::returnPressed, this, &WOscillograph::onEditInterval );
  connect( mScanTimer,   &QTimer::timeout, this, &WOscillograph::onScan );
  connect( mScrollBar,    &QScrollBar::valueChanged, this, &WOscillograph::onScroll );
  connect( mDebugTable,  &WDebugTable::repaintOscillograph, this, QOverload<>::of(&WOscillograph::repaint) );

  onEditInterval();
  onEditRowMax();
  }




QColor WOscillograph::axizColor(int axiz)
  {
  return ( qRgb((axiz & 0x1 ? 160 : 0) + (axiz & 0x8 ? 80 : 0),
                     (axiz & 0x2 ? 160 : 0) + (axiz & 0x10 ? 80 : 0),
                     (axiz & 0x4 ? 160 : 0) + (axiz & 0x20 ? 80 : 0)) );
  }




void WOscillograph::onEditRowMax()
  {
  int row = mEditRowMax->text().toInt();
  if( row > 1 && row < 1000000 ) {
    if( mData != nullptr ) delete mData;
    mData = new int [row * SV_MAX_DEBUG_VARS];
    mRowMax = row;
    QSettings s;
    s.setValue( QStringLiteral(CFG_OSCIL_ROW_MAX), mEditRowMax->text() );
    mScrollBar->setMinimum(0);
    mScrollBar->setMaximum(mRowMax-300);
    mScrollBar->setValue(mStartView = 0);
    mScrollBar->setPageStep(300);
    if( mEditStart->isChecked() ) restart();
    else repaint();
    }
  }




void WOscillograph::onEditStartToggle( bool checked )
  {
  if( checked )
    restart();
  }



void WOscillograph::onEditMode(int index)
  {
  QSettings s;
  s.setValue( QStringLiteral(CFG_OSCIL_MODE), index );
  if( mEditStart->isChecked() ) restart();
  }

void WOscillograph::onEditSync(int index)
  {
  QSettings s;
  s.setValue( QStringLiteral(CFG_OSCIL_SYNC), index );
  if( mEditStart->isChecked() ) restart();
  }



void WOscillograph::onEditLevel()
  {
  QString level = mEditLevel->text();
  QSettings s;
  s.setValue( QStringLiteral(CFG_OSCIL_LEVEL), level );
  if( mEditStart->isChecked() ) restart();
  }



void WOscillograph::onEditInterval()
  {
  int interval = mEditInterval->text().toInt();
  QSettings s;
  s.setValue( QStringLiteral(CFG_OSCIL_INTERVAL), mEditInterval->text() );
  mScanTimer->setInterval( interval );
  mScanTimer->start();
  if( mEditStart->isChecked() ) restart();
  }




void WOscillograph::onScan()
  {
  if( mEditStart->isChecked() ) {
    if( mWait ) {
      //Ожидаем синхронизации
      checkSyncAxiz();
      int level = mDebugTable->getValue(mSyncIndex);
      switch(mSync) {
        case 1 :
          //Нарастающий фронт
          if( mPrevLevel < mLevel && level >= mLevel ) mWait = false;
          break;
        case 2 :
          //Спадающий фронт
          if( mPrevLevel > mLevel && level <= mLevel ) mWait = false;
          break;
        case 3 :
          //Оба фронта
          if( mPrevLevel < mLevel && level >= mLevel ) mWait = false;
          if( mPrevLevel > mLevel && level <= mLevel ) mWait = false;
          break;
        }
      }

    if( !mWait ) {
      //Зарегистрировать
      for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ )
        mData[ mRowIndex * SV_MAX_DEBUG_VARS + i ] = mDebugTable->getValue(i);
      //Продвинуть индекс
      mRowIndex++;
      if( mRowIndex >= mRowMax ) mRowIndex = 0;
      mRowCount++;
      if( mRowCount >= mRowMax ) {
        mRowCount = mRowMax;
        //Дошли до конца
        if( mMode == 0 ) {
          //Регистрировать циклически
          mRowStart++;
          if( mRowStart >= mRowMax ) mRowStart = 0;
          }
        else {
          //Регистрировать однократно
          mEditStart->setChecked(false); //Остановить регистрацию
          }
        }
      repaint();
      }
    }
  mPrevLevel = mDebugTable->getValue(mSyncIndex);
  }






void WOscillograph::onScroll(int val)
  {
  mStartView = val;
  repaint();
  }




void WOscillograph::restart()
  {
  mMode = mEditMode->currentIndex();
  mSync = mEditSync->currentIndex();
  mLevel = mEditLevel->text().toInt();

  checkSyncAxiz();
  mPrevLevel = mDebugTable->getValue(mSyncIndex);

  //Ожидание включаем только когда синхронизируемся от фронта
  mWait = mSync != 0;
  mRowCount = mRowIndex = mRowStart = 0;
  mEditStart->setChecked(true);
  repaint();
  }





void WOscillograph::checkSyncAxiz()
  {
  if( mDebugTable->getAxiz(mSyncIndex).toInt() != 1 ) {
    //Синхронизирующая дорожка изменена, искать новую
    for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ )
      if( mDebugTable->getAxiz(i).toInt() == 1 ) {
        mSyncIndex = i;
        break;
        }
    }
  }


inline int vpos( int vmin, int h, int vstep, int v ) {
  //Проверить ограничение снизу
  if( v < vmin ) return 0;
  //Смещение
  v -= vmin;
  //Преобразовать к области отображения
  v /= vstep;
  //Проверить ограничение сверху
  if( v > h ) return h;
  return v;
  }


void WOscillograph::paint()
  {
  QPainter painter(this);
  QSettings settings;
  //Закрасить цветом фона
  QColor colorBack = QColor( static_cast<QRgb>(settings.value( KEY_COLOR_BACK, QVariant(QColor(Qt::white).rgb()) ).toUInt()) );
  painter.fillRect( QRect( QPoint(), size() ), colorBack );


  //Цвет осей
  QColor colorAxiz = QColor( static_cast<QRgb>(settings.value( KEY_COLOR_AXIZ, QVariant(QColor(Qt::black).rgb()) ).toUInt()) );

  //Рисовать сетку серым цветом
  QColor colorGrid = QColor( static_cast<QRgb>(settings.value( KEY_COLOR_GRID, QVariant(QColor(Qt::gray).rgb()) ).toUInt()) );
  QPoint p1,p2,pt;

  int gridSpace = 200;
  int h = size().height() - WV_BOTTOM_GAP - WV_TOP_GAP;

  //Область отображения графиков
  QRect border;


  //Горизонтальные линии
  p1.rx() = WV_LEFT_GAP - WV_GRID_ADDON;
  p2.rx() = size().width() - WV_RIGHT_GAP;
  border.setLeft( mLeft = p1.x() + WV_GRID_ADDON );
  border.setRight( p2.x() );
  pt.rx() = 2;
  for( int y = h; y > 0; y -= gridSpace ) {
    p1.ry() = p2.ry() = y + WV_TOP_GAP;
    if( y == h ) {
      //Ось X
      painter.setPen( colorAxiz );
      painter.drawLine( QPoint(p1.x()+WV_GRID_ADDON,p1.y()), p2 );
      }
    else {
      painter.setPen( colorGrid );
      painter.drawLine( p1, p2 );
      }
    //Подпись температуры
//    pt.ry() = p1.y() + 5;
//    painter->setPen( colorAxiz );
//    painter->drawText( pt, QString::number( temper / 10.0, 'f', 1) );
//    temper += tstep;
    }

  //Вертикальные линии
  int w = size().width() - WV_LEFT_GAP - WV_RIGHT_GAP;
  gridSpace = 400;// w / (w / WV_GRID_SPACE);
  p1.ry() = WV_TOP_GAP;
  p2.ry() = size().height() - WV_BOTTOM_GAP + WV_GRID_ADDON;
  border.setTop( p1.y() );
  border.setBottom( p2.y()-WV_GRID_ADDON );
  QRect rt( QPoint(), QSize(WV_GRID_SPACE-10,WV_BOTTOM_GAP-10) );
  for( int x = 0; x < w; x += gridSpace ) {
    p1.rx() = p2.rx() = x + WV_LEFT_GAP;
    if( x == 0 ) {
      //Ось Y
      painter.setPen( colorAxiz );
      painter.drawLine( p1, QPoint(p2.x(),p2.y()-WV_GRID_ADDON) );
      }
    else {
      painter.setPen( colorGrid );
      painter.drawLine( p1, p2 );
      }

    //Подпись времени
//    if( mPeriod ) {
//      QDateTime t = mPeriod->Start().addSecs( (mStart + x / 10) * mScale );
//      //Переместить границы подписи
//      rt.moveCenter( QPoint(p1.x(),p2.y() + WV_BOTTOM_GAP/2 - WV_GRID_ADDON) );
//      //Рисовать
//      painter->setPen( colorAxiz );
//      painter->drawText( rt, Qt::AlignHCenter | Qt::AlignVCenter, t.toString("dd-MM-yy\nhh:mm") );

//      //Подписать величину периода
//      if( x == 0 ) {
//        rt.moveCenter( QPoint(p1.x()+60,p2.y() + WV_BOTTOM_GAP/2 - WV_GRID_ADDON) );
//        int minute = (1125 * mScale + 500) / 600;
//        if( minute >= 60 )
//          painter->drawText( rt, Qt::AlignHCenter | Qt::AlignVCenter, QString("<-%1ч->").arg((minute + 5) / 60) );
//        else
//          painter->drawText( rt, Qt::AlignHCenter | Qt::AlignVCenter, QString("<-%1мин->").arg(minute) );
//        }
//      }
    }

  //Рисовать графики
  for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ ) {
    int axiz = mDebugTable->getAxiz(i).toInt();
    if( axiz > 0 ) {
      //Данный график отображается
      //Минимальное и максимальное значения
      QString str = mDebugTable->getMin(i);
      int mmin = -1000000000;
      if( !str.isEmpty() ) mmin = str.toInt();
      str = mDebugTable->getMax(i);
      int mmax = 1000000000;
      if( !str.isEmpty() ) mmax = str.toInt();

      //Отобразить график
      int vstep = mmax / border.height() - mmin / border.height();
      if( vstep < 1 ) vstep = 1;
      int prev = border.bottom() - vpos(mmin, border.height(), vstep, getValue( mStartView, i) );
      int next;
      //Цвет графика
      painter.setPen( axizColor(axiz) );
      for( int x = 0; x < border.width(); x++ ) {
        if( (x + 1 + mStartView) * mScale >= mRowCount ) break;
        if( mScale > 1 ) {
          //Минимальное и максимальные значения за период
          int vmin = mmax;
          int vmax = mmin;
          for( int t = 0; t < mScale; t++ ) {
            int v = getValue( mStartView + x * mScale + t, i );
            if( v < vmin ) vmin = v;
            if( v > vmax ) vmax = v;
            }
          //Рисуем линию от мин до макс
          next = border.bottom() - vpos(mmin, border.height(), vstep, vmin );
          painter.drawLine( border.left() + x, next,
                            border.left() + x, border.bottom() - vpos(mmin, border.height(), vstep, vmax) );
          }
        else {
          next = border.bottom() - vpos(mmin, border.height(), vstep, getValue( mStartView + x, i ) );
          }
        //Линию из предыдущей точки в текущую
        if( x )
          painter.drawLine( border.left() + x - 1, prev, border.left() + x, next );
        prev = next;
        }
      }
    }
  }




int WOscillograph::getValue(int row, int index) const
  {
  if( row >= mRowMax ) return 0;
  row += mRowStart;
  if( row >= mRowMax ) row -= mRowMax;
  return mData[ row * SV_MAX_DEBUG_VARS + index ];
  }




void WOscillograph::mousePressEvent(QMouseEvent *event)
  {
  int x = event->x() - mLeft;
  if( x < 0 ) x = 0;
  mEdge = mStartView + x * mScale;
  if( mEdge > mRowCount ) mEdge = mRowCount - 1;
  if( mEdge >= 0 ) {
    //Заполнить значения в таблице отладки из хранилища
    for( int i = 0; i < SV_MAX_DEBUG_VARS; i++ )
      mDebugTable->setEdgeValue( i, getValue(mEdge,i) );
    }
  }




void WOscillograph::mouseMoveEvent(QMouseEvent *event)
  {
  Q_UNUSED(event)
  }




void WOscillograph::wheelEvent(QWheelEvent *event)
  {
  if( event->angleDelta().y() > 0 ) mScale++;
  else mScale--;
  if( mScale < 1 ) mScale = 1;
  if( mScale > 100 ) mScale = 100;
  mEditScale->setText( tr("Scale: %1").arg(mScale) );
  repaint();
  }




void WOscillograph::paintEvent(QPaintEvent *event)
  {
  Q_UNUSED(event);
  paint();
  }

