/*
  Проект     "Скриптовый язык reduced c++ (rc++) v6"
  Подпроект  "Виртуальная машина"
  Автор
    Alexander Sibilev
  Интернет
    www.rc.saliLab.ru - домашний сайт проекта
    www.saliLab.ru
    www.saliLab.com

  Описание
    Байт-код виртуальной машины.
*/
#ifndef SRVMBYTECODE_H
#define SRVMBYTECODE_H

/*!
   \brief The SrVmByteCode enum перечень кодов команд виртуальной машины

    Байт-код представляет собой линейный массив байтов
    представляющих собой команды. Каждая команда начинается с кода команды, которая
    представлена одним байтом. Все коды команд перечислены в SrVmByteCode.
    За кодом команды может следовать параметр различной длины, а может и не следовать.
    Длина параметра составляет один, два, три или четыре байта.

    Идентификатор кода команды начинается с VBCx, где x - количество байтов команды
    включая сам код команды.
 */
enum SrVmByteCode {
  VBC1_HALT = 0,

  //Stack operations [Разместить в стеке]
  VBC1_PUSH_TOS,        //stack[sp-1] = stack[sp--];
  VBC1_PUSH_0,          //stack[--sp] = 0
  VBC1_PUSH_1,          //stack[--sp] = 1
  VBC1_PUSH_2,          //stack[--sp] = 2
  VBC1_ADD_1,           //stack[sp] += 1;
  VBC1_ADD_2,           //stack[sp] += 2;
  VBC1_ADD_3,           //stack[sp] += 3;
  VBC1_ADD_4,           //stack[sp] += 4;
  VBC2_ADD_CONST,       //stack[sp] += const8
  VBC2_PUSH_CONST,      //stack[--sp] = const8
  VBC3_PUSH_CONST,      //stack[--sp] = const16
  VBC4_PUSH_CONST,      //stack[--sp] = const24
  VBC5_PUSH_CONST,      //stack[--sp] = const32; Загрузка обычного целого
  VBC2_PUSH_B_OFFSET,   //stack[--sp] = bp + offset; Загрузка адреса локальной переменной
  VBC1_LOAD,            //stack[sp] = global[ stack[sp] ]; //Косвенная загрузка
  VBC1_STORE,           //global[ stack[sp+1] ] = stack[sp];
                        //stack[sp+1] = stack[sp++];
  VBC1_POP,             //global[ stack[sp+1] ] = stack[sp];
                        //sp += 2;
  VBC4_PUSH_GLOBAL,     //stack[--sp] = global[param]
  VBC2_PUSH_LOCAL,      //stack[--sp] = global[bp+offset]

  //Разместить код исключения в стек
  VBC1_PUSH_THROW,      //stack[--sp] = mThrow

  //Манипуляции с указателем стека
  VBC1_STACK_DN1,       //sp--
  VBC1_STACK_DN2,       //sp -= 2
  VBC1_STACK_UP1,       //sp++
  VBC1_STACK_UP2,       //sp += 2
  VBC1_STACK_UP3,       //sp += 3
  VBC1_STACK_UP4,       //sp += 4
  VBC1_STACK_UP5,       //sp += 5
  VBC1_STACK_UP6,       //sp += 6
  VBC1_STACK_UP7,       //sp += 7
  VBC1_STACK_UP8,       //sp += 8
  VBC2_STACK,           //sp += param; (param - число со знаком)
  VBC3_STACK,           //sp += param; (param - число со знаком)
  VBC2_POP_RESULT,      //stack[bp+param] = stack[sp++]; Поместить верхушку стека в результат функции
  VBC1_ALLOC_RESULT,    //stack[sp-1] = stack[sp--];

  //integer operations
  VBC1_ADD,             //global[sp+1] = global[sp+1] + global[sp]; sp++;
  VBC1_SUB,             //global[sp+1] = global[sp+1] - global[sp]; sp++;
  VBC1_AND,             //global[sp+1] = global[sp+1] & global[sp]; sp++;
  VBC1_OR,              //global[sp+1] = global[sp+1] | global[sp]; sp++;
  VBC1_XOR,             //global[sp+1] = global[sp+1] ^ global[sp]; sp++;
  VBC1_LSHIFT,          //global[sp+1] = global[sp+1] << global[sp]; sp++;
  VBC1_RSHIFT,          //global[sp+1] = global[sp+1] >> global[sp]; sp++;
  VBC1_EQU,             //global[sp+1] = global[sp+1] == global[sp]; sp++;
  VBC1_NOT_EQU,         //global[sp+1] = global[sp+1] != global[sp]; sp++;
  VBC1_LESS,            //global[sp+1] = global[sp+1] < global[sp]; sp++;
  VBC1_LESS_EQU,        //global[sp+1] = global[sp+1] <= global[sp]; sp++;
  VBC1_GREAT,           //global[sp+1] = global[sp+1] > global[sp]; sp++;
  VBC1_GREAT_EQU,       //global[sp+1] = global[sp+1] >= global[sp]; sp++;
  VBC1_MUL,             //global[sp+1] = global[sp+1] * global[sp]; sp++;
  VBC1_DIV,             //global[sp+1] = global[sp+1] / global[sp]; sp++;
  VBC1_MOD,             //global[sp+1] = global[sp+1] % global[sp]; sp++;
  VBC1_LNOT,            //global[sp] = !global[sp]
  VBC1_NOT,             //global[sp] = ~global[sp]
  VBC1_NEG,             //global[sp] = -global[sp]

  //Function frame [Фрейм глобальной функции]
  // result [результат]
  // param 0 [параметр 0]
  // param 1 [параметр 1]
  // ...
  // bp frame address of calling func [адрес фрейма вызывающей функции]
  // tm exception mask [маска воспринимаемых исключений]
  // ip return address [адрес возврата]

  //Function return [Возврат из функции]
  VBC1_RETURN,          //sp = bp + 3;
                        //ip = global[bp]; tm = global[bp+1]; bp = global[bp+2];


  //Function call [Вызов любой функции]
  //operand - call address offset [это смещение комплексного адреса вызова]
  //global[--sp] = bp;
  //global[--sp] = tm; tm = 0;
  //global[--sp] = ip; ip = addr;
  //bp = sp;
  //Calling address contains address of internal function or
  // index of external function [Комплексный адрес вызова содержит в одной 32-битной ячейке 2 поля]
  // 31 - 1 - external functions [внешние функции]
  //      0 - internal functions [внутренние функции]
  VBC2_CALL,            //call( address offset );
  VBC1_CALL0,           //call( offset = 0 )
  VBC1_CALL1,           //call( offset = 1 )
  VBC1_CALL2,           //call( offset = 2 )
  VBC1_CALL3,           //call( offset = 3 )
  VBC1_CALL4,           //call( offset = 4 )
  VBC1_CALL5,           //call( offset = 5 )
  VBC1_CALL6,           //call( offset = 6 )
  VBC1_CALL7,           //call( offset = 7 )



  //Inc [Инкремент]
  VBC1_INC,             //global[ stack[sp++] ]++;
  VBC4_INC,             //global[ stack[sp++] ] += param24

  //Dec [Декремент]
  VBC1_DEC,             //global[ stack[sp++] ]--;
  VBC4_DEC,             //global[ stack[sp++] ] -= param24

  //Post inc [Постинкремент]
  VBC1_POST_INC,        //stack[sp] = global[ stack[sp] ]++;
  VBC4_POST_INC,        //tmp = stack[sp]; stack[sp] = global[ tmp ]; global[tmp] += param24

  //Post dec [Постдекремент]
  VBC1_POST_DEC,        //stack[sp] = global[ stack[sp] ]--;
  VBC4_POST_DEC,        //tmp = stack[sp]; stack[sp] = global[ tmp ]; global[tmp] -= param24

  //Pred inc [Прединкремент]
  VBC1_PRED_INC,        //stack[sp] = ++global[ stack[sp] ];
  VBC4_PRED_INC,        //stack[sp] = global[ stack[sp] ] += param24;

  //Pred dec [Преддекремент]
  VBC1_PRED_DEC,        //stack[sp] = --global[ stack[sp] ];
  VBC4_PRED_DEC,        //stack[sp] = global[ stack[sp] ] -= param24;

  //Jumps [Переходы]
  VBC4_JUMP,            //ip = param
  VBC4_TRUE_JUMP,       //if( stack[sp++] != 0 ) ip = param; Переход на метку res при условии
  VBC4_TRUE_JUMP_KEEP,  //if( stack[sp] != 0 ) ip = param; else sp++;
  VBC4_FALSE_JUMP,      //if( stack[sp++] == 0 ) ip = param;
  VBC4_FALSE_JUMP_KEEP, //if( stack[sp] == 0 ) ip = param; else sp++;

  VBC1_NOP,             //Холостая команда

  //Обработка исключений
  //Объявить, что в данной функции останавливаются заданные исключения
  VBC5_CATCH,           //tm = param;
  VBC1_CATCH,           //tm = stack[sp++]

  //Возбудить исключение
  VBC1_THROW,           //mThrow = stack[sp]
  VBC5_THROW,           //mThrow = param
                        //Если mThrow == 0, то ничего не делать
                        //Делать:
                        // Возврат из функции
                        // Пока на корневая функция или маска исключения
                        //   функции не совпадет с исключением (tm & mThrow) != 0

  //Инструкции, порождаемые специальными функциями
  VBC1_WAIT,            //Передача управления другому потоку


  VBC_LAST
  };



#endif // SRVMBYTECODE_H
