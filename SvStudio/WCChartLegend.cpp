#include "WCChartLegend.h"

#define DE_VAR_AXIS  4 //Ось
#define DE_VAR_COLOR 5 //Цвет

WCChartLegend::WCChartLegend(QWidget *parent) : WDebugTable(parent)
{
    setColumnCount(columnCount()+2);
    setHorizontalHeaderItem(DE_VAR_AXIS, new QTableWidgetItem(tr("Axis")));
    setHorizontalHeaderItem(DE_VAR_COLOR, new QTableWidgetItem(tr("Color")));
    for( int i = 0; i < 64; ++i ) {
        setItem( i, DE_VAR_AXIS, new QTableWidgetItem() );
        setItem( i, DE_VAR_COLOR, new QTableWidgetItem() );
    }
}

void WCChartLegend::setAxisName(int row, const QString &axisName)
{
    if (row >= 0 && row< rowCount())
        item(row, DE_VAR_AXIS)->setText(axisName);
}

QString WCChartLegend::getAxisName(int row) const
{
    if (row >= 0 && row< rowCount())
        return item(row, DE_VAR_AXIS)->text();
    else
        return "";
}

void WCChartLegend::setColor(int row, const QString &color)
{
    if (row >= 0 && row< rowCount())
        item(row, DE_VAR_COLOR)->setText(color);
}

QString WCChartLegend::getColor(int row) const
{
    if (row >= 0 && row< rowCount())
        return item(row, DE_VAR_COLOR)->text();
    else
        return "";
}
