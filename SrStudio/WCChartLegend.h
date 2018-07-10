#ifndef WCCHARTLEGEND_H
#define WCCHARTLEGEND_H

#include "WDebugTable.h"

#include <QWidget>

///
/// \brief The WCChartLegend class легенда графика - 2 дополнительные поля к таблице дебага (ось и цвет)
///
class WCChartLegend : public WDebugTable
{

    Q_OBJECT
public:
    explicit WCChartLegend(QWidget *parent = 0);

    void setAxisName(int row, const QString & axisName);
    QString getAxisName(int row)const;

    void setColor(int row, const QString & color);
    QString getColor(int row)const;

signals:
public slots:
};

#endif // WCCHARTLEGEND_H
