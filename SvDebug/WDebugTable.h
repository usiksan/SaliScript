#ifndef WDEBUGTABLE_H
#define WDEBUGTABLE_H

#include <QTableWidget>
#include <QWidget>
#include <QTimer>

#define SV_MAX_DEBUG_VARS 64 //Максимальное количество переменный (адресов) для мониторинга


///
/// \brief The WDebugTable class таблица дебага
///
class WDebugTable: public QTableWidget
  {
    Q_OBJECT

    //запрет на изменение значений
    bool                mVarChangeLock;

    //Текущие значения
    int                 mValues[SV_MAX_DEBUG_VARS];

    QTimer              mTimer;      //Таймер опроса переменных
    int                 mCurrentRow; //Текущий ряд опроса переменных

  public:
    explicit WDebugTable( QWidget *parent = nullptr);


    QJsonArray storeToProject();

    void loadFromProject( const QJsonArray vars );

  signals:
    void    repaintOscillograph();

    void    queryVariable( int row, const QString name, int arrayIndex );

    void    setVariable( const QString name, int arrayIndex, int value );

  public slots:
    void    clearVariables();
    QString getName(int index);
    QString getForm(int index);
    QString getAxiz(int index);
    QString getMin(int index);
    QString getMax(int index);
    int     getValue(int index) const { return mValues[index]; }
    void    debugAppend(const QString & name);
    void    setVariable(int index, const QString name, const QString form, const QString axiz, const QString mmin, const QString mmax );
    //Установить контрольное (из архива) значение переменной, эти значения в специальном столбце
    void    setEdgeValue(int row, int value);

    void    onVariableValue( int row, int addr, int value );

  protected slots:

    //изменение значения в таблице
    void debugVariable(int row, int column);

    void onTimer();
  private:
    //отобразить значение
    void displayDebugValue(int row);
    //обновить адрес
    void updateAddress(int row);
  };

#endif // WDEBUGTABLE_H
