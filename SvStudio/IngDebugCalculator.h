#ifndef INGDEBUGCALCULATOR_H
#define INGDEBUGCALCULATOR_H

#include <QObject>
#include <QVariantMap>

class SvMirror;
class UnStorage;

///
/// \brief The IngDebugVarProvider class предоставляет значения переменных для расчета. Базовый класс
///
class IngDebugVarProvider: public QObject{
    Q_OBJECT
public:
    explicit IngDebugVarProvider(QObject *parent = 0):QObject(parent) {}
    virtual int getValue(const QString & name, int index = 0) { Q_UNUSED(name); Q_UNUSED(index); return 0; }
};

///
/// \brief The IngDebugDebugVarProvider class возвращает заданное руками в mValues значение
///
class IngDebugDebugVarProvider: public IngDebugVarProvider{
public:
    QVariantMap mValues;
    explicit IngDebugDebugVarProvider(QObject* parent = 0): IngDebugVarProvider(parent) {}
    virtual int getValue(const QString &name, int index)override{
        if (!mValues.contains(name))
            return 0;
        auto vals = mValues[name].toList();
        if (vals.length() > 0 && index < vals.length())
            return vals[index].toInt();
        return mValues[name].toInt();
    }
};

///
/// \brief The IngDebugMirrorVarProvider class возвращает значение переменной с контроллера
///
class IngDebugMirrorVarProvider: public IngDebugVarProvider{
    SvMirror* mMirror;
public:
    explicit IngDebugMirrorVarProvider(SvMirror* pMirror, QObject* parent = 0);
    void setMirror(SvMirror* pMirror);
    SvMirror* mirror()const;
    virtual int getValue(const QString &name, int index)override;
};


///
/// \brief The IngDebugCalculator class для расчета значения выражений с поддержкой операторов C (кроме присваивания и инкрементов)
///         Поддерживаются скобки и переменные, в том числе массивы
///         Значения переменных предоставляются mValueProvider
class IngDebugCalculator : public QObject
{
    IngDebugVarProvider* mValueProvider;
    Q_OBJECT


    //действие по оператору actionOperator с операндами operand1 и operand2, ok - успешное выполнение
    int operatorAction(const QString &  actionOperator, int operand1, int operand2, bool & ok)const;
    //получить оператор в выражениии expression, стоящий на месте index
    QString getOperator(const QString & expression, int index)const;
    //расчет выражения
    int calculate(const QString & expression, bool &ok)const;
public:
    explicit IngDebugCalculator(IngDebugVarProvider* pValueProvider, QObject *parent = 0);
    void setValueProvider(IngDebugVarProvider* pProvider);
    IngDebugVarProvider *valueProvider()const;

    //определить является ли выражение корректным по скобкам
    bool isValidExpression(const QString & expression)const;
signals:


public slots:
    //расчет значения выражения expression
    int value(const QString & expression, bool & ok);
};

#endif // INGDEBUGCALCULATOR_H
