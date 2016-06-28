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

#include <dos.h>
#include "as_rkey.h"

void clrbuf (void)
/***********
* Describe : очистка буфера клавиатуры
* Params   : void
* Return   : void
* Call     : 
***********/
{
 int far *head = MK_FP(0x0040,0x001A);
 int far *tail = MK_FP(0x0040,0x001C);

 *head = *tail;
}

int rkey (void)
/***********
* Describe : чтение символа с предварительной очисткой буфера
* Params   : void
* Return   : int - считанный символ
* Call     :
***********/
{
 union REGS r;

 clrbuf ();
 r.h.ah = 0x0;
 int86 (0x16,&r,&r);

 if (r.h.al==0)
  return (r.h.ah);
 else
  return (r.h.al);
}

