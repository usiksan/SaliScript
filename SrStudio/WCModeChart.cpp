#include "WCModeChart.h"
#include "SvDebugThread.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>

#include "SvProject.h"

#define CHART_AXES_COUNT 10
const int cDefaultUpdateTime = 1000;

// обновить текст в таблице осей - через запятую переменные и выражения, содержащиеся на осях
void WCModeChart::updateAxesContainValues()
{
    for (int i=0; i<mAxesTable->rowCount(); ++i){
        QString axisName = QString::number(i+1);
        QString values;

        for (int j=0; j<mChartLegend->rowCount(); ++j){
            auto paramName = mChartLegend->debugName(j);
            if (isValidChartLegendItem(j) && mChartLegend->getAxisName(j) == axisName)
                values += paramName + ", ";
        }
        if (!values.isEmpty())
            values = values.left(values.length()-2);

        mAxesTable->item(i, 0)->setText(values);
    }
}



//проверка значений для графика (номер оси от 1 до 10, имя переменной непустое)
bool WCModeChart::isValidChartLegendItem(int row) const
  {
  bool isOk = false;
  auto paramName = mChartLegend->debugName(row);
  auto value = mChartLegend->getAxisName(row).toInt(&isOk);
  isOk &= !paramName.isEmpty();
  return isOk && value <= mAxesTable->rowCount() && value >= 1;
  }



void WCModeChart::hideEvent(QHideEvent *event)
  {
  Q_UNUSED(event)
  setWorking(false);
  }



void WCModeChart::showEvent(QShowEvent *event)
  {
  Q_UNUSED(event)
  setWorking(true);
  }



//получить время обновления (если в строке времени обновления плохое значение - берется значение по-умолчанию)
int WCModeChart::getUpdateTime() const
{
    bool ok = false;
    auto value = mUpdateTimeEdit->text().toInt(&ok);
    if (ok){
        if (value > 0)
           return value;
        else
           return UPDATE_ON_MEMORY_CHANGE;
    }
    else
        return cDefaultUpdateTime;
}

//получена выборка по времени
void WCModeChart::onSampleReceived(const UnSample &sample)
{
    mStorage->Append(sample);
}

//изменено значение в таблице осей
void WCModeChart::onAxisTableCellChanged(int row, int column)
{
   #ifndef DISABLE_CHARTS
    if (!mWorking)
        return;
    auto axisName = QString::number(row + 1);
    if (column == 0){
        //изменилось отображать/скрыть ось
        mMainChart->SetAxisVisible(axisName, mAxesTable->item(row, column)->checkState() ==Qt::Checked);
    }else if (column == 1){
        //изменился минимум
        bool isOk = false;
        auto value = mAxesTable->item(row, column)->text().toInt(&isOk);
        if (isOk)
            mMainChart->SetAxisMin(axisName, value);
    }else if (column == 2){
        //изменился максимум
        bool isOk = false;
        auto value = mAxesTable->item(row, column)->text().toInt(&isOk);
        if (isOk)
            mMainChart->SetAxisMax(axisName, value);
    }
    #endif
}

  //изменено значение в таблице перменных
void WCModeChart::onChartLegendCellChanged(int row, int column)
{
  #ifndef DISABLE_CHARTS
    if (!mWorking)
        return;
    auto paramName = mChartLegend->debugName(row);
    if (column == 3){
        //изменился адрес
        auto address = mChartLegend->debugAddress(row);
        //если еще нет такого параметра наблюдения - добавить его
        if (address > 0 && !paramName.contains('[')){
            mStorage->AddParameter(UnParameterDef(paramName));
            mMonitor->requestSetSample(mStorage->GetParameterDefinitions());
        }
    }
    //изменилось что-то, влияющее на отображение графика
    if (column == 0|| column == 4 || column == 5 || column == 3){
        if (column == 0){
            //изменилось имя перменной - убрать старый график
            if (mCurrentDebugNames.contains(row))
                mChartDataProvider->RemoveDisplaySettings(mCurrentDebugNames[row]);
            mCurrentDebugNames[row] = paramName;
        }
        //если все в порядке
        if (isValidChartLegendItem(row)){
            auto colorString = mChartLegend->item(row, 5)->text();
            auto axisName = mChartLegend->item(row, 4)->text();
            //если плохая строка с цветом - использовать черный
            if (!QColor::isValidColor(colorString))
                colorString = "#000000";

            auto address = mChartLegend->debugAddress(row);
            if (address > 0 && !paramName.contains('[')){
                //если это просто переменная - не нужно обрабатывать значение
                mChartDataProvider->RemoveDisplayValueProcessor(paramName);
            }
            else{
                //если это выражение - задать обработчик значений
                mChartDataProvider->SetDisplayValueProcessor(paramName, new ChartValueProcessor(mChartCalculator, mChartStorageVarProvider));
            }
            //создать настройки отображения
            mChartDataProvider->SetDisplaySettings(new UnChartDisplaySettings(nullptr, paramName, paramName, axisName, UniversalParameterDisplayLine,
                                                                              QPen(QColor(colorString))));


        }
        else{
            //если что-то не так - убрать отображение графика
            mChartDataProvider->RemoveDisplaySettings(paramName);
        }

        //при изменении перменной(выражения) или оси - обновить надпись в таблице осей
        if (column == 0 || column == 4)
            updateAxesContainValues();
    }
  #endif
}

//изменена частота обновления
void WCModeChart::onUpdateTimeEditChanged()
{
    mMonitor->SetUpdateInterval(getUpdateTime());
}


WCModeChart::WCModeChart(QWidget *parent) : QWidget(parent),
    mWorking(true)
{
    mSplitter = new QSplitter();
#ifndef DISABLE_CHARTS
    mMainChart = new UnChartWidget();
#endif
    mChartLegend = new WCChartLegend();
    mAxesTable = new QTableWidget(10, 3);

    QStringList axesTableHeaders;
    axesTableHeaders << tr("Show") << tr("Minimum") << tr("Maximum");
    mAxesTable->setHorizontalHeaderLabels(axesTableHeaders);


    QWidget* pUpdateTimeEditor = new QWidget();
    QHBoxLayout* pUpdateTimeEditorLayout = new QHBoxLayout();
    QLabel* pText = new QLabel();
    pText->setText(tr("Update time (0 for update on memory change): "));
    mUpdateTimeEdit = new QLineEdit();
    mUpdateTimeEdit->setValidator( new QIntValidator(0, 60000, this) );
    connect(mUpdateTimeEdit, &QLineEdit::editingFinished, this, &WCModeChart::onUpdateTimeEditChanged);
    mUpdateTimeEdit->setText(QString::number(cDefaultUpdateTime));
    pUpdateTimeEditorLayout->addWidget(pText);
    pUpdateTimeEditorLayout->addWidget(mUpdateTimeEdit);
    pUpdateTimeEditor->setLayout(pUpdateTimeEditorLayout);

    auto pLeftPanelSplitter = new QSplitter(Qt::Vertical);
    pLeftPanelSplitter->addWidget(mAxesTable);
    pLeftPanelSplitter->addWidget(mChartLegend);

    QWidget* pLeftPanel = new QWidget();
    QVBoxLayout* pLeftLayout = new QVBoxLayout();
    pLeftLayout->addWidget(pUpdateTimeEditor);
    pLeftLayout->addWidget(pLeftPanelSplitter);
    pLeftPanel->setLayout(pLeftLayout);


    #ifndef DISABLE_CHARTS
    mSplitter->addWidget(mMainChart);
    #endif
    mSplitter->addWidget(pLeftPanel);
    QList<int> sizes;
    sizes << 1100 << 700;
    mSplitter->setSizes(sizes);

    QHBoxLayout* pLayout = new QHBoxLayout();
    pLayout->addWidget(mSplitter);
    setLayout(pLayout);

    mMonitor = new UnLocalMonitor(this, nullptr, cDefaultUpdateTime);
    mStorage = new UnStorage(this, false, 5000);

    mChartStorageVarProvider = new IngDebugStorageVarProvider(mStorage, this);
    mChartCalculator = new IngDebugCalculator(mChartStorageVarProvider, this);

    mMonitor->StartMonitoring();
    connect(mMonitor, &UnLocalMonitor::sampleReceived, this, &WCModeChart::onSampleReceived);

    for (int i=0; i<CHART_AXES_COUNT; ++i){
        auto axisName = QString::number(i+1);
        mAxesTable->setItem( i, 0, new QTableWidgetItem() );

        QTableWidgetItem* checkItem = new QTableWidgetItem();
        checkItem->setCheckState(Qt::Unchecked);
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        mAxesTable->setItem( i, 0, checkItem );
        mAxesTable->setItem( i, 1, new QTableWidgetItem(0) );
        mAxesTable->setItem( i, 2, new QTableWidgetItem(1000) );
        mAxesTable->setRowHeight( i, 20 );
#ifndef DISABLE_CHARTS
        mMainChart->AddAxis(new UnChartAxisDefinition(axisName, 0, 1000));
        mMainChart->SetAxisVisible(axisName, false);
#endif
    }
    connect(mAxesTable, &QTableWidget::cellChanged, this, &WCModeChart::onAxisTableCellChanged);
    connect (mChartLegend, &QTableWidget::cellChanged, this, &WCModeChart::onChartLegendCellChanged);

#ifndef DISABLE_CHARTS
    mChartDataProvider = new UnChartDataProvider();
    mChartDataProvider->SetStorage(mStorage);
    mMainChart->SetDataProvider(mChartDataProvider);
#endif

    mAxesTable->setColumnWidth(0, mAxesTable->width() - mAxesTable->columnWidth(1) - mAxesTable->columnWidth(2));


}

void WCModeChart::setWorking(bool working)
{
    mWorking = working;
    if (!working){
      #ifndef DISABLE_CHARTS
        mChartDataProvider->RemoveAll();
      #endif
    }else{
        for (auto i=0; i<64; ++i){
            onChartLegendCellChanged(i, 4);
        }
    }
}

void WCModeChart::openFromProject()
{
    //Заполнить список переменных для отладки
    mStorage->Clear();

    QJsonArray vars = svProject->mProjectSrc.value( WPK_CHART_VARIABLES ).toArray();
    if( vars.count() == mChartLegend->rowCount() * 4 ) {
        for( int i = 0; i < mChartLegend->rowCount(); i++ ) {
            mChartLegend->setVariable(i, vars.at(i*4).toString(), vars.at(i*4+1).toString());
            mChartLegend->setAxisName(i, vars.at(i*4 + 2).toString());
            mChartLegend->setColor(i, vars.at(i*4 + 3).toString());
        }
    }
    else{
        vars = svProject->mProjectSrc.value( WPK_VARIABLE_LIST).toArray();
        if( vars.count() == mChartLegend->rowCount() * 2 ) {
            for( int i = 0; i < mChartLegend->rowCount(); i++ ) {
                mChartLegend->setVariable(i, vars.at(i*2).toString(), vars.at(i*2+1).toString());
            }
        }
    }

    QJsonArray axes = svProject->mProjectSrc.value( WPK_CHART_AXES ).toArray();
    if( axes.count() == mAxesTable->rowCount() * 3 ) {
        for( int i = 0; i < mAxesTable->rowCount(); i++ ) {
            mAxesTable->item(i, 0)->setCheckState(axes.at(i*3).toBool() ? Qt::Checked : Qt::Unchecked);
            mAxesTable->item(i, 1)->setText(axes.at(i*3 + 1).toString());
            mAxesTable->item(i, 2)->setText(axes.at(i*3 + 2).toString());
        }
    }
}


void WCModeChart::setupMirror(SvMirror *pMirror){
    mMonitor->setMirror(pMirror);
    mChartLegend->setupMirror(pMirror);
}

void WCModeChart::connectVars(bool link)
{
    if( link ) {
        //Пройти по списку переменных, получить адреса, получить значения
        //регистрировать для мониторинга

        mChartLegend->debugVariables();
    }
    else {
        mChartLegend->clearVariables();
    }

  }





//Очистка накопленных данных
void WCModeChart::clearData()
  {
  mStorage->Clear();
  }





void WCModeChart::storeToProject()
{
    //Сохранить переменные отладчика в проекте
    QJsonArray vars;
    for( int i = 0; i < mChartLegend->rowCount(); i++ ) {
        vars.append( QJsonValue(mChartLegend-> getName(i)));
        vars.append( QJsonValue(mChartLegend->getForm(i)));
        vars.append( QJsonValue(mChartLegend-> getAxisName(i)));
        vars.append( QJsonValue(mChartLegend-> getColor(i)));
    }

    //Сохранить список в проекте
    svProject->mProjectSrc.insert( WPK_CHART_VARIABLES, QJsonValue(vars) );
    QJsonArray axes;

    for( int i = 0; i < mAxesTable->rowCount(); i++ ) {
        axes.append( QJsonValue(mAxesTable->item(i, 0)->checkState() == Qt::Checked));
        axes.append( QJsonValue(mAxesTable->item(i, 1)->text()));
        axes.append( QJsonValue(mAxesTable->item(i, 2)->text()));
    }
    svProject->mProjectSrc.insert( WPK_CHART_AXES, QJsonValue(axes) );

}




#ifndef DISABLE_CHARTS
WCModeChart::ChartValueProcessor::ChartValueProcessor(IngDebugCalculator *pCalcualator, IngDebugStorageVarProvider *varProvider):
  mChartCalculator(pCalcualator),
  mChartStorageVarProvider(varProvider)
  {

  }


QVariant WCModeChart::ChartValueProcessor::process(const UnStorage *storage, const QString &parameterName, int index)
  {
  Q_UNUSED(storage)
  mChartStorageVarProvider->setCurrentIndex(index);
  bool isOk =  false;
  return mChartCalculator->value(parameterName, isOk);
  }
#endif
