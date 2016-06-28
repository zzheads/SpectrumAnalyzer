/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_PRINT
* Describe  : библиотека работы с принтером
* File      : C:\ALEX\WORK\PRINT\AS_PRINT.C
* Last edit : 21.01.94 00.46.04
* Remarks   :
*****************************************************************************/

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_wind.h"
#include "as_print.h"

extern int port;
extern int mode;
extern int margin;
extern asChoice mnStd[];
int p_x1=0,p_x2=0,p_y1=0,p_y2=0;
asRecPtr p_rec=NULL;

#define ESC (0x1B)
#define CR  (0x0D)
#define LF  (0x0A)

void init_print (int mode,int num)
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
{
 unsigned char hb,lb;

 hb=(int)num>>8;                  /* старший и младший                       */
 lb=(int)num%256;                 /*                   байты                 */

 printc (ESC);
 printc ('*');                      /* установка режима граф. печати       */
 printc ((char)mode);
 printc (lb);
 printc (hb);
}

char *status_msg (int i)
/***********
* Describe : строка - сообщение о состоянии принтера
* Params   : int i - байт состояния
* Return   : char
* Call     :
***********/
{
 static char msg[100];

 switch (i)
 {
  case 144: strcpy(msg,"Printer ready.");
            break;

  case  48: strcpy(msg,"Printer isn't connected.");
            break;

  case 136: strcpy(msg,"Printer is off.");
            break;

  case  24: strcpy(msg,"Printer is off-line, I/O error.");
            break;

  case  56: strcpy(msg,"Out of paper.");
            break;

  default:  strcpy (msg,"Status: ");
            if (i&1)
             strcat(msg,"time-out;");
            else
             strcat(msg,"no time-out;");
            if (i&8)
             strcat(msg,"I/O error;");
            else
             strcat(msg,"no I/O error;");
            if (i&16)
             strcat(msg,"select;");
            else
             strcat(msg,"not select;");
            if (i&32)
             strcat(msg,"out of paper;");
            else
             strcat(msg,"paper ready;");
            if (i&64)
             strcat(msg,"confirm;");
            else
             strcat(msg,"no confirm;");
            if (i&128)
             strcat(msg,"not busy.");
            else
             strcat(msg,"busy.");
            break;
 }

 return (msg);
}

int printc (char ch)
/***********
* Describe : печать символа
* Params   : char ch - символ для печати
* Return   : int     - состояние принтера
* Call     :
***********/
{
 extern int port;
 return (biosprint(0,ch,port));
}

int printw (char *w)
/***********
* Describe : печать слова
* Params   : char ch - слово для печати
* Return   : int     - состояние принтера
* Call     :
***********/
{
 extern int port;
 int i,j;
 for (i=0,j=144;((j==144)||(j==16))&&(w[i]!='\0');i++)
  j=biosprint(0,w[i],port);
 printc(CR);
 printc(LF);
 return (j);
}

char print_vert (int x,int y)
/***********
* Describe : возвращает байт, описывающий строку высотой в 8 точек
*          : с координатами x,y
* Params   : int x - левый верхний
*          : int y - угол сканируемой верт. линии
* Return   : char
* Call     :
***********/
{
 int num,i;
 char res;

 for (i=y,num=0,res=0;num<8;num++,i++)
  if (getpixel(x,i)==BLACK)
   res+=1<<(7-num);

 return (res);
}

void printline (char high)
{
 int i;
 init_print(mode,640);
 for (i=0;i<640;i++)
  printc(high);
}

void print_area (int x1,int y1,int x2,int y2,int mode)
/***********
* Describe : распечатка участка экрана в режиме mode, margin
* Params   : int x1 -
*          : int y1 -  координаты участка
*          : int x2 -  экрана
*          : int y2 -
*          : int mode - режим печати
* Return   : void
* Call     :
***********/
{
 int i,j,n;
 unsigned char pix_byt,pix_bytini;

 if((mode&0xff)>30)              /*  24 игольчатый printer            */
     {printc (ESC);              /*  установка межстрочного интервала */
     printc ('3');               /*  равного 24 иглам                 */
     printc (72);
     if(mode<255)                   /* Single печать */
         {for (i=y1;i<=y2;i+=24)
              {
              for(n=0;n<margin;n++)       printc (' ');
              init_print (mode,(x2-x1+1)*3);
              for (j=x1;j<=x2;j++)
                  {printc (print_vert(j,i));
                  if(i+8<=y2)   printc (print_vert(j,i+8));
                  else          printc (0);
                  if(i+16<=y2)  printc (print_vert(j,i+16));
                  else          printc (0);
                  }
              printc (CR);
              printc (LF);
              }
         }
     else                           /* Double печать */
         {for (i=y1;i<=y2;i+=12)
              {
              for(n=0;n<margin;n++)       printc (' ');
              init_print (mode,(x2-x1+1)*3);
              for (j=x1;j<=x2;j++)
                  {
                  pix_bytini=print_vert(j,i);
                  pix_byt=0;
                  pix_byt+=(pix_bytini&0x10)>>3;
                  pix_byt+=(pix_bytini&0x20)>>2;
                  pix_byt+=(pix_bytini&0x40)>>1;
                  pix_byt+=pix_bytini&0x80;
                  pix_byt+=pix_byt>>1;
                  printc (pix_byt);
                  pix_byt=0;
                  pix_byt+=(pix_bytini&0x8)<<3;
                  pix_byt+=(pix_bytini&0x4)<<2;
                  pix_byt+=(pix_bytini&0x2)<<1;
                  pix_byt+=pix_bytini&0x1;
                  pix_byt+=pix_byt<<1;
                  printc (pix_byt);

                  if(i+8<=y2)
                     {
                     pix_bytini=print_vert(j,i+8);
                     pix_byt=0;
                     pix_byt+=(pix_bytini&0x10)>>3;
                     pix_byt+=(pix_bytini&0x20)>>2;
                     pix_byt+=(pix_bytini&0x40)>>1;
                     pix_byt+=pix_bytini&0x80;
                     pix_byt+=pix_byt>>1;
                     printc (pix_byt);
                     }
                  else          printc (0);
                  }
               printc (CR);
               printc (LF);
               }
          }
     }

 else                            /*  8 игольчатый printer             */
     {printc (ESC);              /*  установка межстрочного интервала */
     printc ('3');               /*  равного 8 иглам                  */
     printc (24);
     for (i=y1;i<=y2;i+=8)
         {
         for(n=0;n<margin;n++)       printc (' ');
         init_print (mode,x2-x1+1);
         if(mode<255)                   /* Single печать */
             for (j=x1;j<=x2;j++)       printc (print_vert(j,i));
         else                           /* Double печать */
             {
             for (j=x1;j<=x2;j++)
                 {
                 pix_bytini=print_vert(j,i);
                 pix_byt=0;
                 pix_byt+=(pix_bytini&0x10)>>3;
                 pix_byt+=(pix_bytini&0x20)>>2;
                 pix_byt+=(pix_bytini&0x40)>>1;
                 pix_byt+=pix_bytini&0x80;
                 pix_byt+=pix_byt>>1;
                 printc (pix_byt);
                 }
             printc (CR);
             printc (LF);
             for(n=0;n<margin;n++)       printc (' ');
             init_print (mode,x2-x1+1);
             for (j=x1;j<=x2;j++)
                 {
                 pix_bytini=print_vert(j,i);
                 pix_byt=0;
                 pix_byt+=(pix_bytini&0x8)<<3;
                 pix_byt+=(pix_bytini&0x4)<<2;
                 pix_byt+=(pix_bytini&0x2)<<1;
                 pix_byt+=pix_bytini&0x1;
                 pix_byt+=pix_byt<<1;
                 printc (pix_byt);
                 }
             }
          printc (CR);
          printc (LF);
          }
     }
}

void print_screen ()
/***********
* Describe : распечатка экрана
* Return   : void
* Call     :
***********/
{
 char msg[][100]={{" Printer isn't ready. Solve problem and press 'Ok' for print. "},
		  {"                                                              "},
		  {" For break printing press 'Cancel' or press 'Esc'.            "},
		  {"                                                              "},
		  {NULL}};
 int err,x1,y1,x2,y2,cur_y=1;

/* biosprint(1,1,port);
 while ((err=biosprint(2,1,port))!=144)
 {
  strcpy(msg[1],status_msg(err));
  if (!message(" Message: ",getmaxx()/5,getmaxy()/3,msg,mnStd))
   return;
 }                                   исключено                        */

 printc(CR);
 print_area(p_x1,p_y1,p_x2,p_y2,mode);
 printc(CR);
 printc(LF);
 }