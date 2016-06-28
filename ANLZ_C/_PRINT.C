/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : _PRINT
* Describe  :
* File      : D:\TC\WORK\_PRINT.C
* Last edit : 12-22-93 01:50:28pm
* Remarks   :
*****************************************************************************/

#include <conio.h>
#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include <stdlib.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_wind.h"
#include "_print.h"

extern int port;
extern int mode;
extern asChoice mnStd[];
int p_x1=0,p_x2=0,p_y1=0,p_y2=0;
asRecPtr p_rec=NULL;

static base_addr = 0x378;

void print_screen (void)
/***********
* Describe : распечатка на принтер
* Params   : void
* Return   : void
* Call     :
***********/
{
 base_addr = peek(0x0040,0x0008+port*2); // считываем из BIOS базовый адpес
					 // пpинтеpа
 printarea(p_x1,p_y1,p_x2,p_y2);
}

void printarea (int x1,int y1,int x2,int y2)
/***********
* Describe : Вывод части экрана в граф. режиме на принтер
* Params   : int x1 - координаты прямоугольной
*          : int y1 - области для
*          : int x2 - вывода на
*          : int y2 - печать
* Return   : void
* Call     : 
***********/
{
 int i=0,x,y;
 char *msg[] = {" Please turn printer on, insert paper, ",
		" and press 'Ok' button.                ",
		NULL};

 while (!status())
  if (!message(" Message: ",getmaxx()/5,getmaxy()/3,msg,mnStd))
   return;   // отмена печати

 put_out(0x07);
 put_out(0x1b);
 put_out(0x33);
 put_out(0x15);
 for (i=0; i<12; i++)
  put_out(0x0A);

 for (x = x2; x >= x1; x -= 8)
 {
  if (kbhit())
  {                    /* прервать ? */
   put_out(0x1b);
   put_out(0x32);
   getch();
   break;
  }
  printrow(x,y1,y2); /* печаем строку */
 }
 put_out(0x07);
}

void printrow(int x,int y1,int y2)
/***********
* Describe : Печать строки номер x
* Params   : int x - номер строки
* Return   : void
* Call     :
***********/
{
 unsigned char savechar, temp;
 /* в начале буфера устанавливаем пpинтеp в гpафический pежим */
 unsigned char *out_buff;
 unsigned i, j, newy, y, size, to_out;

 to_out = 80+y2-y1;
 size   = to_out+4;
 out_buff = (unsigned char *) malloc(size);
 if (out_buff==NULL)
 {
  printf ("\n Error: No enough memory.");
  abort ();
 }
 for (i=0;i<size;i++)
  out_buff[i] = 0;
 out_buff[0] = 27;
 out_buff[1] = 'K';
 out_buff[2] = to_out%256;
 out_buff[3] = to_out/256;

 for (y=y1, j=84; y<=y2; y++,j++)
 {
  savechar = 0;

  /* Определим какие иголки будут задействованы,
     8 пикселей - один байт - 8 иголок */
  for (i=0; i < 8; i++)
  {
   temp = getpixel(x-i,y);
   /* если цвет пикселя не совпадает с цветом фона -
      бит, соответствующей иголки устанавливаем в 1 */
   if (temp != BLACK)
    savechar |= 1;
   if (i != 7)
    savechar <<= 1;
  }
  out_buff[j] = savechar; /* накапливаем строку */
 }

 /* побайтный вывод строки в порт */
 for (i=0; i<size; i++)
  put_out(out_buff[i]);

 put_out('\r');
 put_out('\n');

 free (out_buff);
}

int status (void)
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
{
  union REGS regs;

  regs.x.dx = 0x00; /* Выбираем тип принтера (0:2) = 0         */
  regs.h.ah = 0x02; /* Вызов BIOS для чтения состояния принтера*/

  int86(0x17, &regs, &regs);  /* с кодом функции в регистре аh */

  if (((regs.h.ah & 0x29) > 0) || ((regs.h.ah & 0x80) == 0))
    return 0;
  else return (1);
}

char status_prn (void)
/***********
* Describe : Статус состояния принтера
* Params   : void
* Return   : char
* Call     :
***********/
{
 char temp;
 temp = inportb(base_addr+1);
 return (temp & 0x80);
}

void put_out(char character)
/***********
* Describe : Вывод символа через порт
* Params   : char character - символ
* Return   : void
* Call     :
***********/
{
 int temp;
 char *msg[] = {" Please turn printer on, insert paper, ",
		" and press 'Ok' button.                ",
		NULL};

 while (!status_prn())
  if (!message(" Message: ",getmaxx()/5,getmaxy()/3,msg,mnStd))
   return;       // отмена печати

 outportb(base_addr,character);
 outportb(base_addr+2, 0x0D);
 outportb(base_addr+2, 0x0C);
}