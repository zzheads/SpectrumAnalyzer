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
* Describe : �ᯥ�⪠ �� �ਭ��
* Params   : void
* Return   : void
* Call     :
***********/
{
 base_addr = peek(0x0040,0x0008+port*2); // ���뢠�� �� BIOS ������ ��p��
					 // �p���p�
 printarea(p_x1,p_y1,p_x2,p_y2);
}

void printarea (int x1,int y1,int x2,int y2)
/***********
* Describe : �뢮� ��� ��࠭� � ���. ०��� �� �ਭ��
* Params   : int x1 - ���न���� ��אַ㣮�쭮�
*          : int y1 - ������ ���
*          : int x2 - �뢮�� ��
*          : int y2 - �����
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
   return;   // �⬥�� ����

 put_out(0x07);
 put_out(0x1b);
 put_out(0x33);
 put_out(0x15);
 for (i=0; i<12; i++)
  put_out(0x0A);

 for (x = x2; x >= x1; x -= 8)
 {
  if (kbhit())
  {                    /* ��ࢠ�� ? */
   put_out(0x1b);
   put_out(0x32);
   getch();
   break;
  }
  printrow(x,y1,y2); /* ��砥� ��ப� */
 }
 put_out(0x07);
}

void printrow(int x,int y1,int y2)
/***********
* Describe : ����� ��ப� ����� x
* Params   : int x - ����� ��ப�
* Return   : void
* Call     :
***********/
{
 unsigned char savechar, temp;
 /* � ��砫� ���� ��⠭�������� �p���p � �p���᪨� p���� */
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

  /* ��।���� ����� ������ ���� ������⢮����,
     8 ���ᥫ�� - ���� ���� - 8 ������ */
  for (i=0; i < 8; i++)
  {
   temp = getpixel(x-i,y);
   /* �᫨ 梥� ���ᥫ� �� ᮢ������ � 梥⮬ 䮭� -
      ���, ᮮ⢥�����饩 ������ ��⠭�������� � 1 */
   if (temp != BLACK)
    savechar |= 1;
   if (i != 7)
    savechar <<= 1;
  }
  out_buff[j] = savechar; /* ����������� ��ப� */
 }

 /* ������� �뢮� ��ப� � ���� */
 for (i=0; i<size; i++)
  put_out(out_buff[i]);

 put_out('\r');
 put_out('\n');

 free (out_buff);
}

int status (void)
/***********
* Describe : �⥭�� ���ﭨ� �ਭ�� :
*          : �ਭ�� ������祭, �� ����� � ���� �㬠�� :
*          : 1x0x0xx0
*          : �����  �
*          : �����  ��� ��� �������� (1 = if timed out)
*          : ���������� ��� �訡�� �����-�뢮�� (1 = if I/O error)
*          : ���������� 1 = on-line ( on-line �� �ਭ�� star �� 0)
*          : ���������� ���� , �᫨ 1 - ��� �㬠��
*          : ���������� �� �஢��塞, ��� �᫨ 1 - ���⢥ত����
*          : ���������� 1 = �� �����
*          :
* Params   : void
* Return   : int (1) - �� ��ଠ�쭮, (0)-�訡��
* Call     :
***********/
{
  union REGS regs;

  regs.x.dx = 0x00; /* �롨ࠥ� ⨯ �ਭ�� (0:2) = 0         */
  regs.h.ah = 0x02; /* �맮� BIOS ��� �⥭�� ���ﭨ� �ਭ��*/

  int86(0x17, &regs, &regs);  /* � ����� �㭪樨 � ॣ���� �h */

  if (((regs.h.ah & 0x29) > 0) || ((regs.h.ah & 0x80) == 0))
    return 0;
  else return (1);
}

char status_prn (void)
/***********
* Describe : ����� ���ﭨ� �ਭ��
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
* Describe : �뢮� ᨬ���� �१ ����
* Params   : char character - ᨬ���
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
   return;       // �⬥�� ����

 outportb(base_addr,character);
 outportb(base_addr+2, 0x0D);
 outportb(base_addr+2, 0x0C);
}