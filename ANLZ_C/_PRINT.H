/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : _PRINT
* Describe  :
* File      : D:\TC\WORK\_PRINT.H
* Last edit : 12-21-93 09:10:07pm
* Remarks   :
*****************************************************************************/


void print_screen (void);
/***********
* Describe : Вывод экрана в граф. режиме на принтер
* Params   : void
* Return   : void
* Call     :
***********/

void printarea (int x1,int y1,int x2,int y2);
/***********
* Describe : Вывод части экрана в граф. режиме на принтер
* Params   : int x1 - координаты прямоугольной 
*          : int y1 - области для
*          : int x2 - вывода на
*          : int y2 - печать
* Return   : void
* Call     : 
***********/

void printrow(int x,int y1,int y2);
/***********
* Describe : Печать строки номер x
* Params   : int x - номер строки
* Return   : void
* Call     :
***********/

int status(void);
/***********
* Describe : Чтение состояния принтера :
*          : Принтер подключен, не занят и есть бумага :
*          : 1x0x0xx0
*          : │││││  │
*          : │││││  └── нет ожидания (1 = if timed out)
*          : ││││└───── нет ошибки ввода-вывода (1 = if I/O error)
*          : │││└────── 1 = on-line ( on-line на принтере star при 0)
*          : ││└─────── иначе , если 1 - нет бумаги
*          : │└──────── не проверяем, хотя если 1 - подтверждение
*          : └───────── 1 = не занят
*          :
* Params   : void
* Return   : int (1) - все нормально, (0)-ошибка
* Call     :
***********/

char status_prn(void);
/***********
* Describe : Статус состояния принтера
* Params   : void
* Return   : char
* Call     :
***********/

void put_out(char character);
/***********
* Describe : Вывод символа через порт
* Params   : char character - символ
* Return   : void
* Call     :
***********/