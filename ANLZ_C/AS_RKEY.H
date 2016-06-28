/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_RKEY     
* Describe  : Работа с клавиатурой
* File      : C:\ALEX\WORK\AS_RKEY.C        
* Last edit : 14.08.93 01.10.59
* Remarks   : 
*****************************************************************************/

void clrbuf (void);
/***********
* Describe : очистка буфера клавиатуры
* Params   : void
* Return   : void
* Call     : 
***********/

int rkey (void);
/***********
* Describe : чтение символа с предварительной очисткой буфера
* Params   : void
* Return   : int - считанный символ
* Call     : 
***********/