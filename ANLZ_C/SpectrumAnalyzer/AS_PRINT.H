/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_PRINT
* Describe  :
* File      : C:\ALEX\WORK\PRINT\AS_PRINT.H
* Last edit : 21.01.94 00.49.31
* Remarks   :
*****************************************************************************/

void init_print (int mode,int num);
/***********
* Describe : инициализация принтера и установка гр. режима печати
* Params   : int mode - режим:
*          :          0 - одинарная плотность, 60 т/д.
*          :          1 - двойная плотность, 120 т/д.
*          :          2 - двойная плотность, 120 т/д. (ускоренно)
*          :          3 - учетверенная плотность, 240 т/д.
*          : int num  - число байт для печати
* Return   : void
* Call     :
***********/

char *status_msg (int i);
/***********
* Describe : строка - сообщение о состоянии принтера
* Params   : int i - байт состояния
* Return   : char
* Call     :
***********/

int printc (char ch);
/***********
* Describe : печать символа
* Params   : char ch - символ для печати
* Return   : int     - состояние принтера
* Call     : 
***********/

int printw (char *w);
/***********
* Describe : печать слова
* Params   : char ch - слово для печати
* Return   : int     - состояние принтера
* Call     :
***********/

void print_screen (void);