#include "IngDebugCalculator.h"
#include "./Host/SvMirror.h"



IngDebugMirrorVarProvider::IngDebugMirrorVarProvider(SvMirror *pMirror, QObject *parent):
    IngDebugVarProvider(parent),
    mMirror(pMirror)
{

}

void IngDebugMirrorVarProvider::setMirror(SvMirror *pMirror)
{
    mMirror = pMirror;
}

SvMirror *IngDebugMirrorVarProvider::mirror() const
{
    return mMirror;
}

int IngDebugMirrorVarProvider::getValue(const QString &name, int index)
{
    if (mMirror == nullptr)
        return 0;
    if (!mMirror->getLink())
        return 0;
    int address = mMirror->addressOfName(name);
    if (address > 0)
        return mMirror->memoryGet(address + index);
    return 0;
}

//приоритеты операторов
const QMap<QString, int> cOperatorPriority({
                                               { "!",   1 },
                                               { "~",   1 },
                                               { "*",   2 },
                                               { "/",   2 },
                                               { "%",   2 },
                                               { "+",   3 },
                                               { "-",   3 },
                                               { ">>",  4 },
                                               { "<<",  4 },
                                               { ">",   5 },
                                               { "<",   5 },
                                               { "<=",  5 },
                                               { ">=",  5 },
                                               { "==",  6 },
                                               { "!=",  6 },
                                               { "&",   7 },
                                               { "^",   8 },
                                               { "|",   9 },
                                               { "&&", 10 },
                                               { "||", 11 }

                                           });
bool isUnary(const QString & oper){
    return oper == "~" || oper == "!";
}

QStringList cOperators;
int IngDebugCalculator::operatorAction(const QString &actionOperator, int operand1, int operand2, bool & ok) const
{
    ok = true;
    if (actionOperator == "!")
        return !operand1;
    else if (actionOperator == "~")
        return ~operand1;
    else if (actionOperator == "+")
        return operand1 + operand2;
    else if (actionOperator == "-")
        return operand1 - operand2;
    else if (actionOperator == "*")
        return operand1 * operand2;
    else if (actionOperator == "/"){
        if (operand2 == 0){
            ok = false;
            return 0;
        }
        return operand1 / operand2;
    }
    else if (actionOperator == "%"){
        if (operand2 == 0){
            ok = false;
            return 0;
        }
        return operand1 % operand2;
    }
    else if (actionOperator == ">")
        return operand1 > operand2;
    else if (actionOperator == "<")
        return operand1 < operand2;
    else if (actionOperator == "==")
        return operand1 == operand2;
    else if (actionOperator == "<=")
        return operand1 <= operand2;
    else if (actionOperator == ">=")
        return operand1 >= operand2;
    else if (actionOperator == "!=")
        return operand1 != operand2;
    else if (actionOperator == "&&")
        return operand1 && operand2;
    else if (actionOperator == "||")
        return operand1 || operand2;
    else if (actionOperator == "<<")
        return operand1 << operand2;
    else if (actionOperator == ">>")
        return operand1 >> operand2;
    else if (actionOperator == "|")
        return operand1 | operand2;
    else if (actionOperator == "&")
        return operand1 & operand2;
    else if (actionOperator == "^")
        return operand1 ^ operand2;
    ok = false;
    return 0;
}

//получить оператор в выражениии expression, стоящий на месте index
QString IngDebugCalculator::getOperator(const QString &expression, int index)const
{

    foreach (auto oper, cOperators) {
        bool found = true;
        for (auto i=0; i<oper.length(); ++i){
            if (i + index >= expression.length()){
                found = false;
                break;
            }
            if (oper[i] != expression[index + i]){
                found = false;
                break;
            }
        }
        if (found)
            return oper;
    }
    return "";
}

int IngDebugCalculator::calculate(const QString &expression, bool &ok)const
{
    //если в выражении ошибка - ничего не делать
    if (!isValidExpression(expression)){
       ok = false;
       return 0;
    }

    //если можно сразу привести выражение к результату - сделать это
    bool okToInt = false;
    auto res = expression.toInt(&okToInt);
    if (okToInt){
        ok = true;
        return res;
    }

    auto expr = expression;

    auto iMax = expr.length();

    QString firstOperand;
    QString secondOperand;

    QString* currentOperand = &firstOperand;

    QString  actionOperator;
    int actionOperatorPriority = 0;

    int numOpenedParenthesis = 0;
    bool openedParenthesis = false;

    bool openedBracket = false;
    int numOpenedBrackets = 0;
    int openedBracketVariableIndex = 0;
    QString variableName;
    QString variableIndex;

    //является ли оператор унарным
    bool isUnaryOperator = false;

    int  i = 0;
    //цикл по выражению
    for (i=0; i<iMax; ++i){
        auto c = expr[i];
        bool shouldBreak = false;

        //встретили квадратную скобку - значит дальше будет индекс элемента массива
        if (c == '['){
            if (!openedBracket){
                //считываем название переменной-массива
                for (openedBracketVariableIndex = i - 1;
                     openedBracketVariableIndex >= 0;
                     --openedBracketVariableIndex){
                    auto prevSymb = expr.
                            at(openedBracketVariableIndex);
                    //название переменной может содержать цифры и буквы
                    if (prevSymb.isDigit() || prevSymb.isLetter())
                        variableName.insert(0, prevSymb);
                    else
                        //встречаем что-то кроме цифр и букв - значит оператор, переменная записана
                        break;
                }
                openedBracketVariableIndex++;
                //переменной нет - ошибка записи выражения
                if (variableName.length() == 0){
                    ok = false;
                    return 0;
                }
                openedBracket = true;
            }
            //количество открытых квадратных скобок увеличивается
            //нужно для обработки вложенных индексов

            numOpenedBrackets++;
        }
        //встретили закрывающуюся скобку
        if (c == ']'){
            //количество открытых квадратных скобок уменьшается
            numOpenedBrackets--;
            //количество закрытых скобок больше, чем открытых при движении слева направо - ошибка в выражении
            if (numOpenedBrackets < 0){
                ok = false;
                return 0;
            }
            //количество закрытых скобок стало нулевым - закрылась скобка индекса переменной
            if (numOpenedBrackets == 0){
                variableIndex.remove(0, 1);
                if (variableIndex.length() == 0){
                    ok = false;
                    return 0;
                }
                //считаем индекс как выражение
                int arrayIndex = calculate(variableIndex, ok);
                if (!ok)
                    return 0;
                auto value = 0;
                //считаем значение перменной
                if (mValueProvider != nullptr)
                    value = mValueProvider->getValue(variableName, arrayIndex);
                else{
                    ok = false;
                    return 0;
                }
                //заменяем текст с переменной из массива и ее индексом полученным значением
                //и пересчитываем все выражение
                auto newExpression = expr.left(openedBracketVariableIndex)
                        + QString::number(value)
                        + expr.right(expr.length() - 1 - i);
                return calculate(newExpression, ok);
            }
        }
        //открылась круглая скобка
        if (c == '('){
            openedParenthesis = true;
            //количество открытых скобок увеличилось
            numOpenedParenthesis++;
        }
        //закрылась круглая скобка
        if (c == ')'){
           //количество открытых скобок уменьшилось
            numOpenedParenthesis--;
            if (numOpenedParenthesis < 0){
                ok = false;
                return 0;
            }
            //закрылась скобка, соответствующая встреченной открытой
            if (numOpenedParenthesis == 0){
                currentOperand->remove(0, 1);
                //если считывали второй операнд - перейти к расчету
                if (currentOperand == &secondOperand)
                    break;
                else{
                    //если это первый операнд и не достигли конца выражения
                    //и далее не идет никакого оператора - ошибка в выражении
                    if (i < iMax - 1){
                        auto nextOperator = getOperator(expr, i+1);
                        if (nextOperator.length() == 0){
                            ok = false;
                            return 0;
                        }
                    }
                    //если это первый операнд - перейти к расчету
                    else
                        break;
                }
            }
        }
        //если была открыта скобка - дописать символ в содержимое операнда
        if (numOpenedParenthesis > 0){
            currentOperand->append(c);
            continue;
        }
        //если был открыт индекс массива - дописать символ в индекс
        if (numOpenedBrackets > 0){
            variableIndex.append(c);
        }
        else{
            if (currentOperand == &firstOperand){
                //если сейчас записывается первый операнд
               if (c.isDigit() || c.isLetter())
                   currentOperand->append(c);
               auto oper = getOperator(expr, i);
               if (oper.length() > 0){
                   //встретили оператор
                   isUnaryOperator = isUnary(oper);
                   //унарный оператор может только начинать выражение
                   //при этом так как у них наивысший приоритет - будет всегда получаться так
                   //что рекурсивный алгоритм выделит выражение с унарным оператором для расчета,
                   //так что выражение типа 54 + !2 будет обрабатываться правильно
                   if (isUnaryOperator){
                       if (i != 0){
                           ok = false;
                           return 0;
                       }
                   }
                   else{
                       if (i == 0){
                           ok = false;
                           return 0;
                       }
                   }
                   //запоминаем оператор для выполнения действия
                   actionOperator = oper;
                   actionOperatorPriority = cOperatorPriority[oper];
                   //пропустить лишние символы  (для например && или <<)
                   i +=  oper.length() - 1;
                   //переходим к записи второго операнда
                   currentOperand = &secondOperand;
               }
            }
            else{
                //если сейчас записывается второй операнд
                auto oper = getOperator(expr, i);
                if (oper.length() > 0){
                    //встретили еще один оператор
                    //если приоритет такой же или ниже - закончить второй операнд и перейти к расчету
                    shouldBreak = actionOperatorPriority <= cOperatorPriority[oper];
                    if (!shouldBreak){
                        i += oper.length() - 1;
                        currentOperand->append(oper);
                    }
                }
                else
                    //не встретили оператор - дописать символ в операнд
                    currentOperand->append(c);


            }
        }
        if (shouldBreak)
            break;
    }
    //считали только один операнд
    if (currentOperand == &firstOperand){
        //выражение в скобках - расчитать
        if (openedParenthesis)
            return calculate(firstOperand, ok);
        //выражение с унарным оператором - расчитать
        if (isUnaryOperator){
            auto res = calculate(firstOperand, ok);
            if (!ok)
                return 0;
            return operatorAction(actionOperator, res, 0, ok);
        }
        //пробуем преобразовать считанный операнд к результату
        bool okToInt = false;
        auto value = firstOperand.toInt(&okToInt);
        if (!okToInt){
            //не получилось - значит перед нами переменная
            if (mValueProvider != nullptr)
                value = mValueProvider->getValue(firstOperand);
            else{
                ok = false;
                return 0;
            }
        }
        ok = true;
        return value;
    }
    //считали оба операнда
    auto result = 0;
    bool ok1, ok2;
    //посчитать первый и второй операнды как выражения
    auto firstResult = calculate(firstOperand, ok1);
    auto secondResult = calculate(secondOperand, ok2);
    //если вычисление какого-то операнда привело к ошибке - ничего не делать
    if (!ok1 || !ok2){
        ok = false;
        return 0;
    }
    else{
        //если выражение не было считано до конца - выполнить действие оператора, заменить часть выражения полученным результатом и выполнить расчет снова
        if (i < iMax){
            result = operatorAction(actionOperator, firstResult, secondResult, ok);
            expr = QString::number(result) + expr.right(iMax - i);
            return calculate(expr, ok);
        }
        else
            //если выражение считано до конца - вернуть результат выполнения оператора
            return operatorAction(actionOperator, firstResult, secondResult, ok);
    }
    ok = true;
    return result;
}

IngDebugCalculator::IngDebugCalculator(IngDebugVarProvider* pValueProvider, QObject *parent) : QObject(parent),
    mValueProvider(pValueProvider)
{
    //отсортироваьть по уменьшению длины
    //нужно для того, чтобы при получении оператора в функции getOperator составные операторы считались раньше, чем более короткие (например в выражении 2 << 3 считался оператор <<, а не <)
    cOperators = cOperatorPriority.keys();
    qSort(cOperators.begin(), cOperators.end(), [](const QString & s1, const QString &s2){
        return s1.length() > s2.length();
    });
}

void IngDebugCalculator::setValueProvider(IngDebugVarProvider *pProvider)
{
    if (mValueProvider != nullptr)
        delete mValueProvider;
    mValueProvider = pProvider;
}

IngDebugVarProvider *IngDebugCalculator::valueProvider() const
{
    return mValueProvider;
}
//определить является ли выражение корректным по скобкам
bool IngDebugCalculator::isValidExpression(const QString &expression) const
{
    if (expression.isEmpty())
        return false;


    int numOpenedParenthesis = 0;
    int numClosedParenthesis = 0;

    int numOpenedBrackets = 0;
    int numClosedBrackets = 0;
    foreach(auto c, expression){
        if (c == '(')
            numOpenedParenthesis++;
        if (c == ')')
            numClosedParenthesis++;
        if (c == '[')
            numOpenedBrackets++;
        if (c == ']')
            numClosedBrackets++;
        //при движении слева направо количество закрывающих скобок превысило количество открывающих
        //это ошибка
        if (numClosedParenthesis > numOpenedParenthesis)
            return false;
        if (numClosedBrackets > numOpenedBrackets)
            return false;
    }

    if (numOpenedParenthesis != numClosedParenthesis ||
            numOpenedBrackets != numClosedBrackets){
        return false;
    }
    return true;
}



int IngDebugCalculator::value(const QString &expression, bool &ok)
  {
  auto expr =expression;
  expr = expr.remove(' ').remove('\t').remove('\n').remove('\r');
  return calculate(expr, ok);
  }




