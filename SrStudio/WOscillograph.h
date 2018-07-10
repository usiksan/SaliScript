/*
Project "SvStudio"

Author
  Sibilev Alexander S.

Web
  www.saliLab.com
  www.saliLab.ru

Description
  Осциллограф для наблюдения за сигналами
*/
#ifndef WOSCILLOGRAPH_H
#define WOSCILLOGRAPH_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QCheckBox>
#include <QLabel>
#include <QScrollBar>

class WDebugTable;

class WOscillograph : public QWidget
  {
    Q_OBJECT

    WDebugTable *mDebugTable;   //Таблица с переменными отладки
    QLineEdit   *mEditRowMax;   //Максимальное количество рядов
    QCheckBox   *mEditStart;    //Кнопка Старт
    QComboBox   *mEditMode;     //Режим работы Постоянно, Однократно
    QComboBox   *mEditSync;     //Режим старта По кнопке, По переднему фронту, По заднему фронту, По обоим
    QLineEdit   *mEditLevel;    //Уровень синхронизации
    QLineEdit   *mEditInterval; //Интервал опроса переменных
    QLabel      *mEditScale;    //Текущий масштаб изображения
    QScrollBar  *mScrollBar;    //Скролл бар для прокрутки

    QTimer      *mScanTimer;    //Таймер сканирования данных


    int         *mData;     //Массив данных осциллографа

    int          mRowIndex; //Текущий индекс данных
    int          mRowStart; //Номер начального ряда
    int          mRowCount; //Текущее количество рядов
    int          mRowMax;   //Максимальное количество рядов

    bool         mWait;
    int          mMode;
    int          mSync;
    int          mLevel;
    int          mSyncIndex; //Переменная, используемая для синхронизации
    int          mPrevLevel;

    int          mScale;     //Масштабирование
    int          mEdge;      //Позиция среза
    int          mLeft;      //Левая позиция
    int          mStartView; //Начальная позиция отображения
  public:
    explicit WOscillograph( WDebugTable *dt, QWidget *parent = nullptr);

    static QColor axizColor( int axiz );

  signals:

  public slots:
    void onEditRowMax();
    void onEditStartToggle(bool checked);
    void onEditMode( int index );
    void onEditSync( int index );
    void onEditLevel();
    void onEditInterval();

    void onScan();

    void onScroll( int val );

  private:
    void restart();
    void checkSyncAxiz();
    void paint();
    int  getValue( int row, int index ) const;
    //void paintSeries( )


    // QWidget interface
  protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
  };

#endif // WOSCILLOGRAPH_H
