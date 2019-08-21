#ifndef WDEBUGTABLE_H
#define WDEBUGTABLE_H

#include "IngDebugCalculator.h"

#include <QTableWidget>
#include <QWidget>

#define SV_MAX_DEBUG_VARS 64 //Максимальное количество переменный (адресов) для мониторинга


///
/// \brief The WDebugTable class таблица дебага
///
class WDebugTable: public QTableWidget
  {
    Q_OBJECT

    //зеркало, с которого берутся значения
    SvMirror           *mMirror;

    //запрет на изменение значений
    bool                mVarChangeLock;

    //расчет выражений
    IngDebugCalculator *mDebugCalculator;

    //Текущие значения
    int                 mValues[SV_MAX_DEBUG_VARS];

  public:
    explicit WDebugTable(QWidget *parent = nullptr);

  signals:
    void    repaintOscillograph();

  public slots:
    void    debugVariables();
    void    clearVariables();
    void    updateVariables();
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

    void    setupMirror(SvMirror* pNewMirror);

  protected slots:
    //изменение значения в таблице
    void debugVariable(int row, int column);
    //изменение памяти на зеркале
    void onMemoryChanged();
    void onLinkStatusChanged(bool link, const QString linkStatus);

  private:
    //отобразить значение
    void displayDebugValue(int row);
    //обновить адрес
    void updateAddress(int row);
  };

#endif // WDEBUGTABLE_H
