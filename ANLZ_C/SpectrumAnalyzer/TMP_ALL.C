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
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_3D
* Describe  : Трехмерная графика
* File      : C:\ALEX\WORK\AS_3D.C
* Last edit : 14.08.93 00.57.17
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <math.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_keys.h"
#include "as_rkey.h"
#include "as_wind.h"
#include "as_pen.h"
#include "as_msg.h"
#include "as_main.h"
#include "as_math.h"
#include "as_graph.h"
#include "as_3d.h"

extern char pathto_as[];
extern int p_x1,p_x2,p_y1,p_y2;
extern asRecPtr p_rec;
extern asHotKey hotkeys[];

int GIPX;    /* смещения на графике по осям                                  */
int GIPY;    /*                                                              */

void graph3D (int y1,int y2,asFile far *files,int num)
/***********
* Describe :
* Params   : int y1
*          : int y2
*          : asFile far *files
*          : int num
* Return   : void
* Call     :
***********/
{
 int i,x_a,x_b,y_a,y_b,x2,xlow1,xhigh1,ylow1,yhigh1,xlow2,xhigh2,ylow2,
     x1=0,number;
 int cursor=0,changed=1,n,cur_file=0,changed_y=1;
 double koeffx,koeffx2,koeffy,koeffz_x,koeffz_y,maxy,miny,minx=0,
	ynull,y,j,koeff_a,koeff_b,ynull_b,KOEFY=1.0;
 char user,xunit[10],yunit[10],tmp[30];
 float far *far *msr=NULL,A,B,maxx;

 msr = (float far *far *) farcalloc ((unsigned long)num,sizeof(float far *));

 x2 = getmaxx();
 x_b = x2 - XDIGIT;
 x_a = x_b - XBUTTONSIZE;
 xhigh2 = x_a - YBUTTONSIZE*2;                   /* правая граница графика   */
 xhigh1 = xhigh2 - GIPX;                         /* правая граница графика   */
 xlow1  = x1 + XSPACE*2 + XDIGIT*3;              /* лев. граница графика     */
 xlow2  = xlow1 + GIPX;                          /* лев. граница графика     */
 yhigh1 = y1 + YBUTTONSIZE*2 + YDIGIT + YSPACE*2;/* верх графика             */
 ylow1  = y2 - GIPY - YDIGIT*2 - YSPACE*2;       /* низ графика              */
 ylow2  = ylow1 + GIPY;                          /* низ графика              */

 y_b = yhigh1;
 y_a = y_b + XBUTTONSIZE;

 number = ((files[0]).record)->number;
 for (i=0;i<num;i++)
 {
  msr[i] = (float far *) farcalloc((unsigned long)number,sizeof(float));
  if (msr[i]==NULL)
   abort ();
  calckoeff (files[i].record,&A,&B);
  for (n=0;n<number;n++)
   msr[i][n] = (float) A * ((byte)((files[i]).record->y)[n] + B);
 }

SCALING:

 strcpy (xunit,xunitstr(files[0].record));
 strcpy (yunit,yunitstr(files[0].record));

 maxx = isspectrum(files[0].record) ? (float)files[0].record->hfreq:
                              (float)number/(files[0].record->hfreq*2.56);

 miny = maxy = msr[0][0];
 for (n=0;n<num;n++)
 for (i=1;i<number;i++)
 {
  maxy = (maxy>msr[n][i]) ? maxy : msr[n][i];
  miny = (miny<msr[n][i]) ? miny : msr[n][i];
 }
 if (issignal(files[0].record))
 {
  maxy = (fabs(maxy)>fabs(miny)) ? fabs(maxy) : fabs(miny);
  miny = (fabs(maxy)>fabs(miny)) ? -fabs(maxy) : -fabs(miny);
 }
 else
  if (islinear(files[0].record))
   miny = 0.0;

 if (maxy==miny)
  maxy*=2;
 koeffx = (double)(xhigh1-xlow1)/fabs(maxx-minx);
 koeffy = (double)(ylow1-yhigh1)/fabs(maxy-miny);
 koeffz_x = ((double)GIPX/(num-1));
 koeffz_y = ((double)GIPY/(num-1));
 koeffx2 = (double)(xhigh1-xlow1)/(number-1);
 koeff_a = (double)(x_b-x_a)/(num-1);
 koeff_b = (double)((y_a-2)-(y_b+2))/fabs(maxy-miny);

 if (!strcmp(yunit," mm/s "))
  KOEFY = (float) 1000;
 if (!strcmp(yunit," m/ss "))
  KOEFY = (float) 1.0;
 if (!strcmp(yunit," mkm "))
  KOEFY = (float) 1000000.0;

 ynull = yhigh1 + maxy*koeffy;
 ynull_b = (y_b+2) + maxy*koeff_b;

 setframe (x1,y1,x2,y2,(char *)files[0].record->name);
 p_x1=x1;
 p_x2=x2;
 p_y1=y1;
 p_y2=y2;
 p_rec=NULL;
 rectangle (xlow1-XSPACE,ylow1,xhigh1,yhigh1-YSPACE*2);
 line (xlow1,ylow1,xlow2,ylow2);
 line (xlow2,ylow2,xhigh2,ylow2);

 for (j=(float)0;j<=(float)maxx+maxx/10;j+=(float)maxx/5)
 {
  line (xlow2+j*koeffx,ylow2,xlow2+j*koeffx,ylow2+YSPACE*3);
  if (j>=maxx-maxx/10)
   gprintfxy (xlow2+j*koeffx-XDIGIT*1.5,ylow2+YSPACE*4,"%-5.1f",(float)j);
  else
   if (j==0)
    gprintfxy (xlow2+j*koeffx,ylow2+YSPACE*4,"%5.1f",(float)j);
   else
    gprintfxy (xlow2+j*koeffx-XDIGIT/2,ylow2+YSPACE*4,"%5.1f",(float)j);
 }
 gprintfxy (xhigh2+XSPACE*6,ylow2+YSPACE*4,"%5.5s",xunit);

 for (y=miny;y<=(double)maxy+fabs(maxy-miny)/4;y+=fabs(maxy-miny)/2)
 {
  line (xlow1-XSPACE,ynull-y*koeffy,xlow1-XSPACE*4,ynull-y*koeffy);
  gprintfxy (x1+XDIGIT/2,ynull-y*koeffy-YDIGIT/2,"%7.3f",(double)y*KOEFY);
 }
 gprintfxy (x1+XDIGIT,yhigh1-YDIGIT*2,"%5.5s",yunit);

 for (n=0;n<num;n++)
 {
  ynull = yhigh1 + maxy*koeffy + koeffz_y*n;
  for (i=0;i<number;i++)
  {
   setcolor (WHITE);
   line (xlow1+n*koeffz_x+i*koeffx2,ylow1+koeffz_y*n-1,
         xlow1+n*koeffz_x+i*koeffx2,ynull-(msr[n][i]*koeffy));
  }
  setcolor(BLACK);
  moveto (xlow1+n*koeffz_x,ynull-(msr[n][0]*koeffy));
  for (i=0;i<number;i++)
   lineto (xlow1+n*koeffz_x+i*koeffx2,ynull-(msr[n][i]*koeffy));
 }
 setfillstyle (SOLID_FILL,WHITE);
 bar (x_a,y_a,x_b,y_b);
 rectangle (x_a-2,y_a+2,x_b+2,y_b-2);
 ftimetoa (&(files[cur_file].ftime),tmp);
 gprintfxy (x_a,y_a+(YDIGIT+YSPACE*3),"D: %s",tmp);
 initcursor (fullpath("pencil.img"));

 do
 {
  if (changed)
  {
   killcursor ();
   setcursor (xlow1+cur_file*koeffz_x+cursor*koeffx2,
              yhigh1+maxy*koeffy+koeffz_y*cur_file-msr[cur_file][cursor]*koeffy);

   bar (x_a,y_a,x_b,y_b);
   moveto (x_a,ynull_b-(msr[0][cursor])*koeff_b);
   for (n=0;n<num;n++)
    lineto (x_a+n*koeff_a,ynull_b-(msr[n][cursor])*koeff_b);
   line (x_a+cur_file*koeff_a,y_a,
         x_a+cur_file*koeff_a,ynull_b-(msr[cur_file][cursor])*koeff_b);

   gprintfxy (x1+XDIGIT*3,yhigh1-YDIGIT*2," X=%5.1f %5.5s",
              (float)cursor*koeffx2/koeffx,xunit);
   gprintfxy (x1+XDIGIT*8,yhigh1-YDIGIT*2," Y=%5.1f %5.5s",
              (float)msr[cur_file][cursor]*KOEFY,yunit);
   if (changed_y)
   {
    ftimetoa (&(files[cur_file].ftime),tmp);
    gprintfxy (x_a,y_a+(YDIGIT+YSPACE*3),"D: %s",tmp);
   }
  }
  changed = 1;
  changed_y = 0;
  user = rkey ();
  switch (user)
  {
   case KB_N_LEFT    :  if (cursor>0)
                         cursor--;
                        break;
   case KB_N_RIGHT   :  if (cursor<number-1)
                         cursor++;
                        break;
   case KB_N_UP      :  if (cur_file>0)
                        {
                         cur_file--;
                         changed_y=1;
                        }
                        break;
   case KB_N_DOWN    :  if (cur_file<num-1)
                        {
                         cur_file++;
                         changed_y=1;
                        }
                        break;

   case KB_C_LEFT    :  if (cursor>=10)
                         cursor-=10;
			break;
   case KB_C_RIGHT   :  if (cursor<=number-1-10)
                         cursor+=10;
                        break;
   case KB_N_HOME    :  cursor=0;
                        break;
   case KB_N_END     :  cursor=number-1;
                        break;

   case KB_A_F10     :  killcursor();
                        closecursor();
                        killframe();
                        for (i=0;i<num;i++)
                         doIntegrate (msr[i],files[i].record);
                        goto SCALING;
   case KB_C_F10     :  killcursor();
                        closecursor();
                        killframe();
                        for (i=0;i<num;i++)
                         doDiff (msr[i],files[i].record);
                        goto SCALING;

   default           :  for (i=0;hotkeys[i].key!=KB_LAST;i++)
                         if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                         {
                          killcursor();
                          closecursor();
                          hotkeys[i].action ();
                          initcursor(fullpath("pencil.img"));
                          setcursor (xlow1+cur_file*koeffz_x+cursor*koeffx2,
                                     yhigh1+maxy*koeffy+koeffz_y*cur_file-msr[cur_file][cursor]*koeffy);
			  changed = 0;
			  statusline((char *)sl3DGraph);
                         }
                        break;
  }
 }
 while (user!=KB_N_ESC);

 for (n=0;n<num;n++)
  farfree (msr[n]);
 farfree (msr);
 killcursor ();
 closecursor ();
 killframe ();
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CAPTU
* Describe  : Запись граф. копии экрана в файл
* File      : C:\ALEX\WORK\AS_CAPTU.C
* Last edit : 14.08.93 00.59.52
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include <alloc.h>
#include <graphics.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_captu.h"

void abort (void);

void capture_screen (void)
/***********
* Describe : записать экран на диск
* Params   : void
* Return   : void
* Call     :
***********/
{
 extern int p_x1,p_y1,p_x2,p_y2;
 char far *buffer=NULL,fname[30]="capture.img";
 unsigned int volume;
 FILE *fout;

 volume = imagesize (p_x1,p_y1,p_x2,p_y2);
 buffer = (char far *) farcalloc (volume,sizeof(char));
 if (buffer==NULL)
 {
  printf (" No enough memory in function CAPTURE_SCREEN.");
  abort ();
 }
 printf ("\07");
 getimage (p_x1,p_y1,p_x2,p_y2,buffer);
 printf ("\07");

 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
           getmaxy()/4+YBUTTONSIZE*4," Enter file name: ");
 editstring (getmaxx()/4+XDIGIT,getmaxy()/4+YBUTTONSIZE*2,30,fname);
 killframe ();

 fout = fopen (fname,"wb");
 if (fout==NULL)
 {
  printf (" Problem in function CAPTURE_SCREEN. Disk full?");
  abort ();
 }
 fwrite ((char *)buffer,volume,sizeof(char),fout);
 fclose (fout);

 farfree (buffer);
 printf ("\07");
 return;
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CHECK
* Describe  : проверка на наличие в директории файлов
* File      : C:\ALEX\WORK\AS_CHECK.C
* Last edit : 14.08.93 01.02.16
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include "as_ftime.h"
#include "as_list.h"
#include "as_check.h"

void check (asList far *list,int dx)
/***********
* Describe : отмечает имеющиеся в директории файлы
* Params   : asList far *list - список имен записей
*          : int dx           - сколько из списка проверять
* Return   : void
* Call     :
***********/
{
 int i,j,last;
 static char filename[15];
 static char fname[15];

 if (list==NULL)
  return;
 for (i=0;((i<dx)&&(strcmp((list[i]).name,"LAST")));i++)
 {
  strcpy (filename,(char *)(strchr((list[i]).name,'.')+2));
  for (j=7;((j>=0)&&(filename[j]==' '));j--)
   ;
  last = j;
  for (j = last;j>=0;j--)                      /*                             */
  {                                            /*                             */
   if (filename[j]!=' ')                       /* заменяем пробелы '_'        */
    fname[j]=filename[j];                      /*                             */
   else                                        /*                             */
    fname[j]='_';                              /*                             */
  }                                            /*                             */
  fname[last+1]=0;
  strcat (fname,".spe");

  if (access(fname,0)==0)
  {
   (list[i]).mark=1;
   (list[i]).name[0]='*';
  }
  else
  {
   (list[i]).mark=0;
   if ((list[i]).name[0]=='*')
    (list[i]).name[0]=' ';
  }
 }
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CHOOS
* Describe  :
* File      : C:\ALEX\WORK\AS_CHOOS.C
* Last edit : 04.07.93 00.08.04
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <alloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_list.h"
#include "as_fndir.h"
#include "as_choos.h"

extern asChoice mnStd[2];

FTIME chooseftime (int x1,int y1,char *fname)
/***********
* Describe : выбоp записи по вpемени из файла для загpузки
* Params   : int x1      - откуда
*          : int y1      - выводить
*          : char *fname - имя файла
* Return   : FTIME       - вpемя нужной записи
* Call     : getfilelist,grlist,ftimecpy,ftimetoa
***********/
{
 asList far *list = NULL;
 FTIME  return_item={{-1,-1,-1},{-1,-1,-1}};
 int user;

 list = (asList far *) getfilelist(fname);
 if (list==NULL)
 {
  message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoTime,mnStd);
  return (return_item);
 }

 user = grlist2 (" Choose time to load: ",list,x1,y1,3,10);
 switch (user)
 {
  case menuEscape        : break;
  case menuErr_NoItems   : message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoItems,mnStd);
                           break;
  case menuErr_ImgSize   : message(" Error: ",getmaxx()/4,getmaxy()/4,msgImgSize,mnStd);
                           break;
  case menuErr_NoImgMem  : message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoImgMem,mnStd);
                           break;
  default                : if (user>0)
                            ftimecpy (&return_item,&(list[user-1].time));
                           else
                            break;
 }
 farfree (list);
 return (return_item);
}

FTIME far *chooseftimes (int x1,int y1,char *fname,int *num)
/***********
* Describe : выбоp нескольких записей (списка) по вpемени
* Params   : int x1      - откуда
*          : int y1      - выводить
*          : char *fname - имя файла
*          : int *num    - число выбpанных вpемен
* Return   : FTIME       - массив выбpанных вpемен
* Call     :
***********/
{
 asList far *list = NULL;
 FTIME  far *result = NULL;
 int user,i,j;

 *num = 0;
 list = (asList far *) getfilelist(fname);
 if (list==NULL)
 {
  message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoTime,mnStd);
  return (result);
 }

 user = grlist2 (" Choose times to load: ",list,x1,y1,3,10);
 switch (user)
 {
  case menuEscape        : break;
  case menuErr_NoItems   : message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoItems,mnStd);
                           break;
  case menuErr_ImgSize   : message(" Error: ",getmaxx()/4,getmaxy()/4,msgImgSize,mnStd);
                           break;
  case menuErr_NoImgMem  : message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoImgMem,mnStd);
                           break;
  default                : if (user>0)
                           {
                            for (i=0,(*num=0);strcmp(list[i].name,"LAST");i++)
                             if (list[i].mark)
                              (*num)++;
                            result = (FTIME far *) farcalloc
                                     ((unsigned long)(*num),sizeof(FTIME));
                            if (result==NULL)
                             return (result);
                            for (i=0,j=0;i<(*num)&&strcmp(list[j].name,"LAST");
                                 j++)
                             if (list[j].mark)
                             {
                              ftimecpy (result+i,&(list[j].time));
                              i++;
                             }
                           }
                           else
                            break;
 }
 farfree (list);
 return (result);
}

char *choosefile (int x1,int y1,char *wc,char *result)
/***********
* Describe : выбрать имя файла в тек. директории по шаблону
* Params   : int x1    - откуда
*          : int y1    - выводить
*          : char *wc  - шаблон для поиска
* Return   : char *    - имя файла, выбранного пользователем
* Call     : setframe, killframe, setbutton
***********/
{
 char fname [20];
 asList far *listfiles;
 int n,user;

 listfiles = getdir(wc,&n);
 if (n==0)
 {
  message (" Can't find *.spe ",getmaxx()/3,getmaxy()/3,(char **)errNoFiles,mnStd);
  return (NULL);
 }

 user=grlist1(" Choose file to load: ",listfiles,x1,y1,3,10);
 if (user!=menuEscape)
 {
  strcpy (result,listfiles[user-1].name);
  strcpy (fname,result);
  farfree (listfiles);
  return (fname);
 }
 else
 {
  farfree (listfiles);
  return (NULL);
 }
}

int numrecinfile (char *fname)
/***********
* Describe : число записей в файле
* Params   : char *fname - имя файла
* Return   : int         - число записей
* Call     :
***********/
{
 FILE *f=NULL;
 long ofs=0;
 int num=0;

 f = fopen (fname,"rb");
 if (f==NULL)
  return 0;

 do
 {
  if (fread (&ofs,sizeof(long),1,f)==0)
   break;
  if (fseek (f,ofs-sizeof(long),SEEK_CUR)!=0)
   break;
  num++;
 }
 while (!feof(f));
 fclose (f);
 return (num);
}

asList far *getfilelist (char *fname)
/***********
* Describe : массив стpуктуp, описывающих содеpжимое файла
* Params   : char *fname  - имя файла
* Return   : asList    - массив
* Call     :
***********/
{
 asList far *result=NULL;
 asRecord buffer;
 FILE *f=NULL;
 long ofs=0;
 int i,numrec;
 char tmp[10];

 f = fopen (fname,"rb");
 if (f==NULL)
  return NULL;

 numrec = numrecinfile (fname);
 result = (asList far *) farcalloc (numrec+1,sizeof(asList));
 if (result==NULL)
 {
  message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoMemory,mnStd);
  return (NULL);
 }

 for (i=0;i<numrec;i++)
 {
  fread (&ofs,sizeof(long),1,f);
  fread (&(result[i].time),sizeof(FTIME),1,f);
  fread (&buffer,(size_t)49,1,f);  /* длина заголовка-49 байт */

  ftimetoa(&(result[i].time),result[i].name);

  strcpy (result[i].info,isspectrum(&buffer) ? " Spe" : " Sig");
  strcat (result[i].info,islogariphm(&buffer) ? " Log " : " Lin ");
  strcat (result[i].info,itoa(buffer.hfreq,tmp,10));
  strcat (result[i].info," Hz ");
  strcat (result[i].info,modestr(&buffer));
/*  strcat (result[i].info," HFreq:");	*/
  strcat (result[i].info,itoa(buffer.number,tmp,10));
  strcat (result[i].info," Lines");

  if ((buffer.mode)[0]=='o')   /* если огибающая */
  {
   strcat (result[i].info," Envlp:");
   strcat (result[i].info,itoa(afreqnum(&buffer),tmp,10));
   strcat (result[i].info," Hz");
  }

  fseek (f,(size_t)ofs-sizeof(long)-sizeof(FTIME)-49,SEEK_CUR);
 }
 strcpy (result[numrec].name,"LAST");

 fclose (f);
 return (result);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : FINDBAD
* Describe  : Привязка к местоположению BAD-кластеров на диске
* File      : C:\ALEX\WORK\FINDBAD.C
* Last edit : 17.07.94 00.19.49
* Remarks   : А.Щербаков, "Защита от копирования", стр. 18-20.
*****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <io.h>
#include "as_main.h"
#include "as_def.h"

static int bad_secs (int *,int *);
static void comp (int *);

#define MAX_BADSECS   100

void comp (int *true)
{
 int BadSecs[MAX_BADSECS],BadBytes[MAX_BADSECS],bads=0;
 int n_BadSecs[MAX_BADSECS],n_BadBytes[MAX_BADSECS],n_bads=0,i;
 FILE *out;

 for (i=0;i<MAX_BADSECS;i++)
 {
  BadSecs [i] = 0;
  BadBytes[i] = 0;
  n_BadSecs [i] = 0;
  n_BadBytes[i] = 0;
 }
 *true = (int)0;
 n_bads = bad_secs (n_BadSecs,n_BadBytes);
 out = fopen (fullpath("as_info.nfo"),"rb");
 if (out==NULL)
  return;
 fread (&bads   ,sizeof(int),1          ,out);
 fread (BadSecs ,sizeof(int),MAX_BADSECS,out);
 fread (BadBytes,sizeof(int),MAX_BADSECS,out);
 fclose (out);
 if (bads!=n_bads)
  return;
 for (i=0;i<MAX_BADSECS;i++)
  if ((BadSecs[i]!=n_BadSecs[i])||(BadBytes[i]!=n_BadBytes[i]))
   return;
 *true = (int)1;
 return;
}

int bad_secs (int *BadSecs,int *BadBytes)
{
/* буферы для чтения секторов                                                */
 unsigned char buffer[512],buf[512];
 int tg,SecNum,ByteNum,i,j;

/* номер первого сектора FAT и число секторов в FAT                          */
 int i_Fat1Sec,i_FatSecs;
 int i_LoopOffs,i_Ind;
 int sw_fat; /* флаг типа FAT                                                 */
 int cur_disk;

/* определение текущего диска                                                */
 cur_disk=getdisk();

/* определение типа FAT                                                      */
 absread (cur_disk,1,0,buf);
 i_FatSecs = *((int *)(buf+0x16));
 if (i_FatSecs * 341 / 4096)
  sw_fat = 1; /* 16-битовая FAT                                               */
 else
  sw_fat = 0; /* 12-битовая FAT                                               */

/* определение номера начального сектора FAT                                 */
 i_Fat1Sec = * ((int *)(buf+0x0e));

/* цикл поиска BAD-кластера                                                  */
 tg=0;
 for (i=0;i<i_FatSecs;i++)
 {
 /* чтение очередного сектора FAT                                            */
  absread (cur_disk,1,i_Fat1Sec+i,buffer);
  if (sw_fat==1)
   for (j=0;j<256;j++)
   {
    if ((buffer[j*2]==0xf7)&&(buffer[j*2+1]==0xff))
    {
     SecNum = i;      /* BAD-кластер найден                                   */
     ByteNum = j*2;
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
   }
  else
  {
   i_LoopOffs = i % 3;  /* В 12-и битовой FAT некоторые элементы попадают на  */
                        /* границу между секторами, поэтому не все сектора    */
                        /* начинаются с нового элемента, перед ним могут      */
                        /* располагаться 1 или 2 байта от предыдущей пары     */
                        /* элементов                                          */
   for (j=0;j<170;j++)
   {
    i_Ind = j*3+i_LoopOffs;
    if ((buffer[i_Ind]==0xf7)&&((buffer[i_Ind+1]&0x0f)==0x0f))
    {
     SecNum = i;
     ByteNum = i_Ind; /* BAD-кластер найден                                   */
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
    if (((buffer[i_Ind+1]&0xf0)==0x70)&&(buffer[i_Ind+2]==0xff))
    {
     SecNum = i;
     ByteNum = i_Ind; /* BAD-кластер найден                                   */
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
   }
  }
 }
 return (tg);
}

int test (void)
{
 int flag_true=0;

 comp (&flag_true);
 return (1);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_DIAG
* Describe  : диагностика
* File      : C:\ALEX\WORK\AS_DIAG.C
* Last edit : 14.08.93 01.05.12
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "as_ftime.h"
#include "db_exch.h"
#include "as_diag.h"

double log (double);
double exp (double);

int N,M,K,L;

char *diagnoze (char *fname,char *field)
/***********
* Describe : выдача диагностического сообщения о параметре из БД
* Params   : char *fname - имя БД
*          : char *field - имя параметра
* Return   : char        - сообщение
* Call     :
***********/
{
 char result[100]="Unknown",tmp[30];
 unsigned int i,j,num;
 long number;
 float far *par,break_lev,danger_lev;
 struct date far* d=NULL;
 long far* time=NULL,x_danger,x_break;
 FILE *f=NULL;
 struct date tmp1,tmp2;
 double SX,SY,SXY,SX2,SXlnY,SlnY,U_lin,A_lin,B_lin,U_exp,A_exp,B_exp;

 if ((f = fopen (fname,"rb"))==NULL ||
     (par = read_dbf_number (f,field,&number))==NULL ||
     (d = read_dbf_date (f,"DATE",&number))==NULL ||
     number<4L)
  goto EXIT;
 num = (unsigned int) number;

 for (i=2;i<num;i++)
  if (par[i]<=0.0)
  {
   for (j=i+1;j<num;j++)
   {
    par [j-1] = par[j];
    d [j-1] = d[j];
    if (par[j-1]<=0.0)
    {
     j--;
     num--;
    }
   }
   num--;
  }

 for (i=0;i<num;i++)
  if (par[i]<=0.0)
   printf ("\n par[%d]=%f",i,par[i]);

 danger_lev = par[0];
 break_lev  = par[1];

 if ((time = farcalloc (num,sizeof(long)))==NULL)
  goto EXIT;
 for (i=num-N;i<num;i++)
  time[i] = diffdate ((struct date far *)d+num-N,(struct date far *)d+i);

 for (i=num-N,SX=0,SY=0,SXY=0,SX2=0,SlnY=0,SXlnY=0;i<num;i++)
 {
  SX += (double) time[i];
  SY += (double) par[i];
  SXY += (double) time[i]*par[i];
  SX2 += (double) time[i]*time[i];
  SlnY += (double) log(par[i]);
  SXlnY += (double) time[i]*log(par[i]);
 }
  /* линейная апроксимация */
 B_lin = (double)(SX*SY-N*SXY)/(SX*SX-N*SX2);
 A_lin = (double)(SY-B_lin*SX)/N;
 for (i=num-N,U_lin=0;i<num;i++)
  U_lin += (par[i]-(A_lin+B_lin*time[i]))*(par[i]-(A_lin+B_lin*time[i]));
 /* экспоненциальная апроксимация */
 B_exp = (SX*SlnY-N*SXlnY)/(SX*SX-N*SX2);
 A_exp = exp((SlnY-B_exp*SX)/N);
 for (i=num-N,U_exp=0;i<num;i++)
  U_exp += (par[i]-(A_exp+B_exp*time[i]))*(par[i]-(A_exp+B_exp*time[i]));

 if (U_lin>U_exp) /* экспоненциальный закон точнее */
  if (B_exp!=0.0)
  {
   assert (danger_lev>0.0);
   assert (break_lev>0.0);
   assert (A_exp>0.0);
   x_danger = (log(danger_lev)-log(A_exp))/B_exp;
   x_break  = (log(break_lev)-log(A_exp))/B_exp;
  }
  else
  {
   x_danger = 2000;
   x_break  = 2000;
  }
 else
  if (B_lin!=0.0)
  {
   x_danger = (danger_lev-A_lin)/B_lin;
   x_break  = (break_lev-A_lin)/B_lin;
  }
  else
  {
   x_danger = 2000;
   x_break  = 2000;
  }

 tmp1.da_day = (d+num-N)->da_day;
 tmp1.da_mon = (d+num-N)->da_mon;
 tmp1.da_year = (d+num-N)->da_year;
 full_date (day_of_year(&tmp1)+(unsigned int)x_danger,&tmp1);

 tmp2.da_day = (d+num-N)->da_day;
 tmp2.da_mon = (d+num-N)->da_mon;
 tmp2.da_year = (d+num-N)->da_year;
 full_date (day_of_year(&tmp2)+(unsigned int)x_break,&tmp2);

 strcpy (result," ");
 if (par[num-1]>=danger_lev)
  strcat (result,"  yes   ");
 else
  if ((x_danger>0)&&(x_danger<1725))
  {
   strcpy (tmp,itoa (tmp1.da_day,tmp,10));
   strcat (result,tmp);
   strcat (result,"/");
   strcpy (tmp,itoa (tmp1.da_mon,tmp,10));
   strcat (result,tmp);
   strcat (result,"/");
   strcpy (tmp,itoa (tmp1.da_year-1900,tmp,10));
   strcat (result,tmp);
  }
  else
   strcat (result,"continue ");

 strcat (result,"-");
 if (par[num-1]>=break_lev)
  strcat (result,"  yes   ");
 else
  if ((x_break>0)&&(x_break<1725))
  {
   strcpy (tmp,itoa (tmp2.da_day,tmp,10));
   strcat (result,tmp);
   strcat (result,"/");
   strcpy (tmp,itoa (tmp2.da_mon,tmp,10));
   strcat (result,tmp);
   strcat (result,"/");
   strcpy (tmp,itoa (tmp2.da_year-1900,tmp,10));
   strcat (result,tmp);
  }
  else
   strcat (result,"continue ");

 /*
 if (U_lin>U_exp)
 {
  gcvt (U_exp,6,tmp);
  strcat (result," exp: ");
  strcat (result,tmp);
 }
 else
 {
  gcvt (U_lin,6,tmp);
  strcat (result," lin: ");
  strcat (result,tmp);
 }
 */

 EXIT:
      if (f!=NULL)
       fclose (f);
      if (par!=NULL)
       farfree (par);
      if (d!=NULL)
       farfree (d);
      if (time!=NULL)
       farfree (time);
      return (result);
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_FILE
* Describe  : Работа с файлами
* File      : C:\ALEX\WORK\AS_FILE.C
* Last edit : 07.01.93 01.46.25
* Remarks   :
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <alloc.h>
#include <string.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"

int load (asFile *file,char *fname,void (*func)(char *))
/***********
* Describe : Чтение записи из файла по времени в file (или с равным временем,
*          : или первую с большим, или последнюю запись в файле)
* Params   : asFile *file - куда читать
*          : char *fname  - имя файла для чтения
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
* Call     :
***********/
{
 FILE *fout=NULL;                             /*                             */
 FTIME fa;                                    /*                             */
 long size;
                                              /*                             */
 ftimecpy (&fa,&(file->ftime));               /* нужное время сохраняем в fa */
 fout = fopen(fname,"rb");                    /*                             */
 if (fout==NULL)                              /*                             */
  return (E_READ);                            /* ошибка чтения               */
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем, какой файл читаем */
   func (fname);                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  fread(&(file->offset),sizeof(long),1,fout); /* читаем запись               */
  fread(&(file->ftime),sizeof(FTIME),1,fout); /*                             */
  size = file->offset-sizeof(long)-sizeof(FTIME);
  fread((byte *)file->record,sizeof(byte),(size_t)size,fout);
 }
 while (! ( feof(fout) || (ftimecmp(&(file->ftime),&fa)>=0) ) );
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}

int save (asFile *file,void (*func)(char *))
/***********
* Describe : Запись записи в файл, упорядочивает записи в файле по времени
* Params   : asFile *file - что записывать
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
* Call     :
***********/
{
 char fname[20]="        ";                   /*                             */
 FILE *fout=NULL;                             /*                             */
 int i;                                       /*                             */
 long pos,len;                                /*                             */
 byte far *buffer=NULL;                       /*                             */
 struct ftime t;
                                              /*                             */
 for (i=7;((i>=0)&&(file->record->name[i]==' '));i--)
  ;
 for (;i>=0;i--)                              /*                             */
 {                                            /*                             */
  if (file->record->name[i]!=' ')             /* заменяем пробелы '_'        */
   fname[i]=file->record->name[i];            /*                             */
  else                                        /*                             */
   fname[i]='_';                              /*                             */
 }                                            /*                             */
 fname[8]=0;
 strcat (fname,".spe");                       /* расширение - "spe"          */
 fout = fopen(fname,"a+b");                   /*                             */
 if (fout==NULL)                              /*                             */
 {
  printf ("\n Cant write to %s",fname);
  return (E_WRITE);                           /* ошибка записи файла         */
 }
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем какой файл пишем   */
   func (fname);                              /*                             */
 pos = setfile (fout,&(file->ftime));         /* писать с этой позиции       */
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ситываем остаток в буфер,   */
  fseek (fout,pos,SEEK_SET);                  /* обрезаем файл               */
 }
 file->offset = sizeof(long)+sizeof(FTIME)+file->record->number+49;
 fwrite(&(file->offset),sizeof(long),1,fout); /* записываем нашу запись      */
 fwrite(&(file->ftime),sizeof(FTIME),1,fout); /* в конец файла               */
 fwrite((byte *)file->record,sizeof(byte),(size_t)(file->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* дописываем остаток          */
 }                                            /*                             */
 t.ft_year  = (unsigned) (file->ftime.date.da_year-1980);
 t.ft_month = (unsigned) (file->ftime.date.da_mon);
 t.ft_day   = (unsigned) (file->ftime.date.da_day);
 if (setftime (fileno(fout),&t)!=0)
 {
  printf ("\n Error in setting file date/time");
  abort ();
 }
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

int delete (char *fname,FTIME t,void (*func)(char *))
{
 FILE *fout=NULL;                             /*                             */
 long pos1,pos2,len;                          /*                             */
 byte far *buffer=NULL;                       /*                             */
                                              /*                             */
 fout = fopen(fname,"a+b");                   /*                             */
 if (fout==NULL)                              /*                             */
  return (E_WRITE);                           /* ошибка записи файла         */
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем какой файл удаляем */
   func (fname);                              /*                             */
 pos1 = setfile (fout,&t);                    /* удалять с этой позиции      */
 ftimeinc (&t,1);
 pos2 = setfile (fout,&t);
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos2,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos2),fout);
 }
 chsize ( fileno(fout),pos1 );                /* ситываем остаток в буфер,   */
 fseek (fout,pos1,SEEK_SET);                  /* обрезаем файл               */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos2),fout);
  farfree (buffer);                           /* дописываем остаток          */
 }                                            /*                             */
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

int copy (asFile *file_from,asFile *file_to,char *from,char *to)
{
 FILE *fout=NULL;                             /*                             */
 long pos,len;                                /*                             */
 byte far *buffer=NULL;                       /*                             */

 load (file_from,from,NULL);
 fout = fopen(to,"a+b");                      /*                             */
 if (fout==NULL)                              /*                             */
  return (E_WRITE);                           /* ошибка записи файла         */
 pos = setfile (fout,&(file_to->ftime));      /* писать с этой позиции       */
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ситываем остаток в буфер,   */
  fseek (fout,pos,SEEK_SET);                  /* обрезаем файл               */
 }
 file_from->offset = sizeof(long)+sizeof(FTIME)+file_from->record->number+49;
 fwrite(&(file_from->offset),sizeof(long),1,fout); /* записываем нашу запись      */
 fwrite(&(file_from->ftime),sizeof(FTIME),1,fout); /* в конец файла               */
 fwrite((byte *)file_from->record,sizeof(byte),(size_t)(file_from->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* дописываем остаток          */
 }                                            /*                             */
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

long setfile (FILE *f,FTIME *t)
/***********
* Describe : Установить указатель в файле на начало первой записи
*          : с большей датой, чем данная
* Params   : FILE *f  - файл
*          : FTIME *t - дата
* Return   : long     - смещение от начала файла до нужного места
* Call     :
***********/
{                                             /*                             */
 long ofs=0,now=0;                            /*                             */
 FTIME tmp;                                   /*                             */
                                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  now = ftell (f);                            /* текущий указатель в файле   */
  if (fread (&ofs,sizeof(long),1,f)!=1)       /* считываем записи из файла   */
   break;                                     /* пока не кончится файл       */
  if (fread (&tmp,sizeof(FTIME),1,f)!=1)      /* или не совпадут фремена     */
   break;                                     /* или считаное время больше   */
  if (fseek (f,ofs-sizeof(long)-sizeof(FTIME),SEEK_CUR)!=0)
   break;                                     /* искомого времени            */
 }                                            /*                             */
 while (! ( feof(f) || (ftimecmp(&tmp,t) >= 0)/*                             */ ) );
 fseek (f,now,SEEK_SET);                      /*                             */
 return now;                                  /*                             */
}                                             /*                             */

/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : _FNDIR
* Describe  : Функция поиска файлов по заданным шаблонам.
* File      : C:\ALEX\WORK\GRAPH\_FNDIR.H
* Last edit : 30.05.93 00.50.20
* Remarks   :
*****************************************************************************/

#include <io.h>
#include <fcntl.h>
#include <graphics.h>
#include <stdio.h>
#include <dir.h>
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include "as_ftime.h"
#include "as_list.h"
#include "as_fndir.h"

struct ffblk* fn_edir (char wildcard[][MAX_WILDCARD_LEN],int num_wildcard,
                       struct ffblk *files,int *num_files)
/***********
* Describe : Функция поиска файлов по заданным шаблонам.
* Params   : char wildcard[][]   - массив шаблонов длиной не более MAX_WILDCARD_LEN
*          : int num_wildcard    - число шаблонов
*          : struct ffblk *files - указатель на массив структур ffblk (см. dos.h)
*          : int *num_files      - число найденных по шаблону файлов
* Return   : struct ffblk*       - указатель на массив структур ffblk
* Call     :
***********/
{
 int n=DYN_ARRAY_SIZE;
 unsigned int blk_size=sizeof(struct ffblk);
 int i,j,k,nomatch,not_unique,found;
 struct ffblk *ptr;

 files=(struct ffblk *) calloc(n,blk_size);
 if (files==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }
 ptr=(struct ffblk *) calloc(n,blk_size);
 if (ptr==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }
 *num_files=0;
 for (i=0;i<num_wildcard;i++)
 {
  nomatch=findfirst(*(wildcard+i),ptr,0);
  while (!nomatch)
  {
   not_unique = 0;
   if (*num_files>0)
    for (j=0;(j<*num_files)&&(!not_unique);j++)
     if (!strcmp(ptr->ff_name,(files+j)->ff_name))
      not_unique=1;
   if (!not_unique)
   {
    (*num_files)++;
    if (*num_files>n)
    {
     n+=INC_ARRAY_SIZE;
     files=(struct ffblk *) realloc(files,n*blk_size);
     if (files==NULL)
     {
      closegraph ();
      printf ("\n Error reading directory: no enough memory. ");
      exit (-1);
     }
    }
    if (*num_files>1)
    {
     found = 0;
     k=*num_files-1;
     for (j=0;(j<k)&&(!found);j++)
     {
      if (strcmp(ptr->ff_name,(files+j)->ff_name)<0)
      {
       found = 1;
       memmove((files+j+1),(files+j),(k-j)*blk_size);
       *(files+j)=*ptr;
      }
     }
     if (!found)
      *(files+k)=*ptr;
    }
    else
     *files=*ptr;
   }
   nomatch=findnext(ptr);
  }
 }
 free(ptr);
 return files;
}

asList far *getdir (char *wc,int *numfiles)
/***********
* Describe : выдать список файлов в диpектоpии
* Params   : char *wc      - шаблон для поиска
*          : int *numfiles - число найденных файлов
* Return   : asList far *  - массив стpуктуp
* Call     : fn_edir
***********/
{
 struct ffblk *files=NULL;
 asList far *result=NULL;
 int i;
 char tmp[10];

 files = (struct ffblk *)fn_edir (wc,1,files,numfiles);

 result = (asList far *) farcalloc (*numfiles+1,sizeof(asList));
 if (result==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }

 for (i=0;i<*numfiles;i++)
 {
  strcpy (result[i].name,files[i].ff_name);

  strcpy (result[i].info," size: ");
  strcat (result[i].info,ltoa(files[i].ff_fsize,tmp,10));
 }
 strcpy (result[*numfiles].name,"LAST");
 farfree (files);
 return (asList far *)(result);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_FTIME
* Describe  : Описание вpемени пpогpаммы СпектpоАнализатоp
* File      : C:\ALEX\WORK\AS_FTIME.H
* Last edit : 30.05.93 00.41.01
* Remarks   :
*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "as_ftime.h"

int ftimecmp (FTIME *t1,FTIME *t2)
/***********
* Describe : сравнивает время t1 и t2
* Params   : FTIME *t1 - время1
*          : FTIME *t2 - время2
* Return   : (int) 0-времена одинаковы,1 время1 больше,(-1) время1 меньше
* Call     :
***********/
{
 if (t1->date.da_year > t2->date.da_year)     /* приоритет: год,месяц,день,  */
  return (1);                                 /* час,минуты,секунды.         */
 if (t1->date.da_year < t2->date.da_year)     /*                             */
  return (-1);                                /*                             */
 if (t1->date.da_mon > t2->date.da_mon)       /*                             */
  return (1);                                 /*                             */
 if (t1->date.da_mon < t2->date.da_mon)       /*                             */
  return (-1);                                /*                             */
 if (t1->date.da_day > t2->date.da_day)       /*                             */
  return (1);                                 /*                             */
 if (t1->date.da_day < t2->date.da_day)       /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_hour > t2->time.ti_hour)     /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_hour < t2->time.ti_hour)     /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_min > t2->time.ti_min)       /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_min < t2->time.ti_min)       /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_sec > t2->time.ti_sec)       /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_sec < t2->time.ti_sec)       /*                             */
  return (-1);                                /*                             */
 return (0);                                  /*                             */
}

void ftimecpy (FTIME *t1,FTIME *t2)
/***********
* Describe : копирует время t1 в t2
* Params   : FTIME *t1 - время1
*          : FTIME *t2 - время2
* Return   : void
* Call     :
***********/
{
 t1->date.da_year = t2->date.da_year;         /* комментарии излишни         */
 t1->date.da_mon  = t2->date.da_mon ;         /*                             */
 t1->date.da_day  = t2->date.da_day ;         /*                             */
 t1->time.ti_hour = t2->time.ti_hour;         /*                             */
 t1->time.ti_min  = t2->time.ti_min ;         /*                             */
 t1->time.ti_sec  = t2->time.ti_sec ;         /*                             */
}

void earnftime (FTIME *t)
/***********
* Describe : Устанавливает в t системное время
* Params   : FTIME *t - куда писать время
* Return   : void
* Call     :
***********/
{
 getdate (&(t->date));                        /* системная дата              */
 gettime (&(t->time));                        /* системное время             */
}

void ftimeinc (FTIME *t,int i)
/***********
* Describe : увеличить вpемя на i секунд с пеpеносом
* Params   : FTIME *t - вpемя
*          : int i    - колличество секунд
* Return   : void
* Call     :
***********/
{
 unsigned char hour,min;

 hour = i/(60*60);
 min  = i/60-hour*60;
 i    = i-hour*60*60-min*60;

 t->time.ti_sec+=i;
 t->time.ti_min+=min;
 t->time.ti_hour+=hour;

 while (t->time.ti_sec>=60)
 {
  t->time.ti_min++;
  t->time.ti_sec-=60;
 }
 while (t->time.ti_min>=60)
 {
  t->time.ti_hour++;
  t->time.ti_min=0;
 }
 while (t->time.ti_hour>=24)
 {
  t->date.da_day++;
  t->time.ti_hour=0;
 }
 while (t->date.da_day>31)
 {
  t->date.da_mon++;
  t->date.da_day=1;
 }
 while (t->date.da_mon>12)
 {
  t->date.da_year++;
  t->date.da_mon=1;
 }
}

void ftimetoa (FTIME *t,char *result)
/***********
* Describe : пpедставить вpемя в cтpоке
* Params   : FTIME *t
* Return   : char
* Call     :
***********/
{
 char tmp[20];

 strcpy (result," ");
 strcpy (tmp,itoa((int)t->date.da_day,tmp,10));
 strcat (result,tmp);
 strcat (result,"/");

 strcpy (tmp,itoa((int)t->date.da_mon,tmp,10));
 strcat (result,tmp);
 strcat (result,"/");

 strcpy (tmp,itoa((int)t->date.da_year-1900,tmp,10));
 strcat (result,tmp);
 strcat (result," ");

 strcpy (tmp,itoa((int)t->time.ti_hour,tmp,10));
 strcat (result,tmp);
 strcat (result,":");

 strcpy (tmp,itoa((int)t->time.ti_min,tmp,10));
 strcat (result,tmp);
}

void setminftime (FTIME *t)
/***********
* Describe : установить мин. вpемя
* Params   : FTIME *t
* Return   : void
* Call     :
***********/
{
 t->date.da_day  = 1;
 t->date.da_mon  = 1;
 t->date.da_year = 0;
 t->time.ti_hour = 0;
 t->time.ti_min  = 0;
 t->time.ti_sec  = 0;
}

void setmaxftime (FTIME *t)
/***********
* Describe : установить макс. вpемя
* Params   : FTIME *t
* Return   : void
* Call     :
***********/
{
 t->date.da_day  = 30;
 t->date.da_mon  = 12;
 t->date.da_year = 3000;
 t->time.ti_hour = 10;
 t->time.ti_min  = 50;
 t->time.ti_sec  = 50;
}

int incorrect (FTIME *t)
/***********
* Describe : время не правильно?
* Params   : FTIME *t1 - время
* Return   : int
* Call     :
***********/
{
 if ( (t->date.da_year<0)    || (t->date.da_mon<1)  || (t->date.da_day<1)
   || (t->date.da_year>3000) || (t->date.da_mon>12) || (t->date.da_day>31)
   || (t->time.ti_hour>24)   || (t->time.ti_min>60) || (t->time.ti_sec>60) )
  return (1);
 else
  return (0);
}

#define DAYS_IN_YEAR  365

int leap (int year)
/***********
* Describe : опpеделяет високосный ли год
* Params   : int year - год
* Return   : int      - 1-да, 0-нет
* Call     :
***********/
{
 if ((year%4==0 && year%100!=0) || (year%400==0))
  return 1;
 else
  return 0;
}

static char daytab[2][13] = {{0,31,28,31,30,31,30,31,31,30,31,30,31},
                             {0,31,29,31,30,31,30,31,31,30,31,30,31}};

int day_of_year (struct date *d)
/***********
* Describe : вычисляет номеp дня в году по дате
* Params   : struct date *d - дата
* Return   : int            - номеp дня в году
* Call     : leap
***********/
{
 int i,day;
 day = d->da_day;
 for (i=1;i<d->da_mon;i++)
  day+=daytab[leap(d->da_year)][i];
 return day;
}

void full_date (int day,struct date *d)
/***********
* Describe : вычисляет день и месяц по номеpу дня в году
* Params   : int day        - номеp дня в году
*          : struct date *d - в стpуктуpе содеpжится год
* Return   : void
* Call     : leap
***********/
{
 int i;
 for (i=1;day>daytab[leap(d->da_year)][i];i++)
  day -= daytab[leap(d->da_year)][i];
 d->da_mon = i;
 d->da_day = day;
 while (d->da_mon>12)
 {
  d->da_year++;
  d->da_mon-=12;
 }
}

char *month_name (int n)
/***********
* Describe : имя месяца
* Params   : int n - номеp месяца с 1
* Return   : char  - сиpока-имя месяца
* Call     :
***********/
{
 char *name[] = {"Unk","Jan","Feb","Mar","Apr","May","Jun",
                       "Jul","Aug","Sep","Okt","Nov","Dec"};
 return (n<1||n>12) ? name[0] : name[n];
}

long diffdate (struct date *d1,struct date *d2)
/***********
* Describe : вычисляет пpомежуток между двумя датами в днях
* Params   : struct date *d1 - пеpвая дата
*          : struct date *d2 - втоpая дата
* Return   : long            - число дней от d1 до d2
* Call     :
***********/
{
 int i;
 long days=0;

 days += (DAYS_IN_YEAR+leap(d1->da_year))*(d1->da_year!=d2->da_year) - day_of_year(d1);
 for (i=d1->da_year+1;i<d2->da_year;i++)
  days += DAYS_IN_YEAR + leap(i);
 days += day_of_year(d2);
 return days;
}

#define MIN_IN_HOUR  60
#define HOUR_IN_DAY  24

long min_of_day (struct time *t)
/***********
* Describe : вычисляет минуту дня по полному вpемени
* Params   : struct time *t - вpемя
* Return   : long           - минута дня
* Call     :
***********/
{
 int i;
 long min;

 min = t->ti_min;
 for (i=0;i<t->ti_hour;i++)
  min+=MIN_IN_HOUR;
 return min;
}

long ftimediff (FTIME *t1,FTIME *t2)
/***********
* Describe : вычисляет pазницу между вpеменем t1 и t2 в минутах
* Params   : FTIME *t1 - пеpвое вpемя
*          : FTIME *t2 - втоpое вpемя
* Return   : long      - pазница в минутах
* Call     : diffdate,min_of_day
***********/
{
 long days_diff,min_diff;

 days_diff = diffdate (&(t1->date),&(t2->date));
 min_diff = days_diff * HOUR_IN_DAY * MIN_IN_HOUR;
 min_diff += min_of_day (&(t2->time)) - min_of_day (&(t1->time));
 return min_diff;
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_GRAPH
* Describe  : Показ графика спектра/сигнала
* File      : D:\BC\WORK\AS_GRAPH.C
* Last edit : 12-10-93 07:54:39pm
* Remarks   : БПФ,Лин/Лог,Интегрирование,Изм. масштаба
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <math.h>
#include <stdlib.h>
#include <io.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_keys.h"
#include "as_rkey.h"
#include "as_wind.h"
#include "as_pen.h"
#include "as_msg.h"
#include "as_main.h"
#include "as_math.h"
#include "_print.h"
#include "as_graph.h"

extern asHotKey hotkeys[];
void closeall (void);
extern char pathto_as[];
extern int p_x1,p_x2,p_y1,p_y2;
extern asRecPtr p_rec;
int PICK;
double MRS_BEG,MRS_END;

asChoice mnCursor[]   = {{"   Harmonic  ",NULL ,1},
                         {"   Sideband  ",NULL ,1},
                         {"   Delta     ",NULL ,1},
                         {NULL           ,NULL ,0}};
asChoice mnWindow[]   = {{"   Transient  ",NULL ,1},
                         {"   Exponent   ",NULL ,1},
                         {NULL            ,NULL ,0}};


enum asCursor cursor_type=MAIN;

int graphwindow (int y1,int y2,asFile far *file)
/***********
* Describe : показ графика записи и преобразования его
* Params   : int y1           - вертикальные координаты начала
*          : int y2           - и конца окна с графиком
*          : asFile far *file - содержит указатель на нужную запись
* Return   : void
* Call     : infowindow
***********/
{
 int i,x2,xlow,xhigh,ylow,yhigh,x1=0,num,num_mrs;
 int cursor=0,delta=MIN_DELTA,changed=1,n,r_code=0;
 double koeffx,koeffx2,koeffy,maxy,miny,minx=0,ynull,y,j,step,pick,mrs=0.0;
 char user,xunit[10],yunit[10],title[40],tmp[20];
 float far *msr=NULL,A,B,maxx,KOEFY=1.0,KOEFX=1.0;
 enum asScale yscale;

 hotkeys[1].able = 0;     /* нельзя снова вызвать */

 xlow  = x1 + XSPACE*2 + XDIGIT*3;               /* лев. граница графика     */
 xhigh = xlow + 512;                             /* правая граница графика   */
 x2 = xhigh + XDIGIT*2;                          /* правая граница окна      */
 yhigh = y1 + YBUTTONSIZE*2 + YDIGIT + YSPACE*2; /* верх графика             */
 ylow  = y2 - YDIGIT*2 - YSPACE*2;               /* низ графика              */

 yscale = islinear(file->record) ? LINEAR : LOGARIPHMIC;
 num = file->record->number;
 msr = (float far *) farcalloc((unsigned long)num,sizeof(float));
 if (msr==NULL)
  abort ();

 if (issignal(file->record))
 {
  calckoeff (file->record,&A,&B);
  pick = A*127.0;
 }
 else
 {
  setsignal(file->record);
  calckoeff (file->record,&A,&B);
  pick = A*127.0;
  setspectrum(file->record);
 }
 calckoeff (file->record,&A,&B);
 pick=ceil(pick);

 if (islinear(file->record))
 {
  for (n=0;n<num;n++)
   msr[n] = (float) A*((byte)(file->record->y)[n]+B);
 }
 else
 {
  for (n=0;n<num;n++)
   msr[n] = (float) pow(10.0,A*((byte)(file->record->y)[n]+B)/20);
 }

 strcpy (title," Record (");
 strcat (title,(char *)file->record->name);
 strcat (title,") of ");
 ftimetoa(&(file->ftime),tmp);
 strcat (title,tmp);

SCALING:
 num = file->record->number;
 strcpy (xunit,xunitstr(file->record));
 strcpy (yunit,yunitstr(file->record));

 maxx = isspectrum(file->record) ? (float)file->record->hfreq:
                                   (float)num/(file->record->hfreq*2.56);

 miny = maxy = msr[0];
 for (i=1;i<num;i++)
 {
  maxy = (maxy>msr[i]) ? maxy : msr[i];
  miny = (miny<msr[i]) ? miny : msr[i];
 }
 if (issignal(file->record))
 {
  maxy = (fabs(maxy)>fabs(miny)) ? fabs(maxy) : fabs(miny);
  miny = (fabs(maxy)>fabs(miny)) ? -fabs(maxy) : -fabs(miny);
 }
 else
  miny = 0.0;

DRAWING:
 if (isspectrum(file->record))
 {
  for (mrs=0.0,n=(int)(MRS_BEG*num/maxx);n<=(int)(MRS_END*num/maxx)&&n<num;n++)
   mrs += (double)(msr[n])*(msr[n]);
  mrs = (double)sqrt(mrs);
 }
 else
 {
  for (mrs=0.0,n=(int)(MRS_BEG*num/maxx);n<=(int)(MRS_END*num/maxx)&&n<num;n++)
   mrs += (double)(msr[n])*(msr[n]);
  num_mrs = (MRS_END-MRS_BEG)*num/maxx;
  mrs = (double)sqrt(mrs)/(num_mrs-1);
 }

 step = maxy/1000;
 if (maxy==miny)
  maxy*=2;
 koeffx = (double)(xhigh-xlow)/fabs(maxx-minx);  /* коэфф. по осям           */
 koeffy = (double)(ylow-yhigh)/fabs(SCALE(maxy)-SCALE(miny));
 ynull = yhigh + SCALE(maxy)*koeffy;             /* нулевой уровень          */
 koeffx2 = (double)(xhigh-xlow)/num;             /*                          */

 if (!strcmp(yunit," mm/s "))
  KOEFY = (float) 1000.0;
 if (!strcmp(yunit," m/ss "))
  KOEFY = (float) 1.0;
 if (!strcmp(yunit," mkm "))
  KOEFY = (float) 1000000.0;

 if (isspectrum(file->record))
  KOEFX = (float) 1.0;
 else
  KOEFX = (float) 1000.0;

 setframe (x1,y1,x2,y2,title);
 p_x1=x1;
 p_x2=x2;
 p_y1=y1;
 p_y2=y2;
 p_rec=file->record;
 rectangle (xlow-XSPACE,ylow,xhigh,yhigh-YSPACE*2);

 for (j=(float)0;j<=(float)maxx+maxx/10;j+=(float)maxx/5)
 {
  line (xlow+j*koeffx,ylow,xlow+j*koeffx,ylow+YSPACE*3);
  if (j>=maxx-maxx/10)
   gprintfxy (xlow+j*koeffx-XDIGIT*1.5,ylow+YSPACE*4,"%-5.1f",(float)j*KOEFX);
  else
   gprintfxy (xlow+j*koeffx-XDIGIT/2,ylow+YSPACE*4,"%5.1f",(float)j*KOEFX);
 }
 gprintfxy (xhigh+XSPACE*6,ylow+YSPACE*4,"%5.5s",xunit);

 if (yscale)
  for (y=SCALE(miny);y<=(double)SCALE(maxy)+fabs(SCALE(maxy)-SCALE(miny))/6;y+=fabs(SCALE(maxy)-SCALE(miny))/3)
  {
   line (xlow-XSPACE,ynull-y*koeffy,xlow-XSPACE*4,ynull-y*koeffy);
   j = (double) pow ((double)10.0,y/20) * KOEFY;
   gprintfxy (x1+XDIGIT/2,ynull-y*koeffy-YDIGIT/2,"%7.3f",j);
  }
 else
  for (y=miny;y<=(double)maxy+fabs(maxy-miny)/10;y+=fabs(maxy-miny)/5)
  {
   line (xlow-XSPACE,ynull-y*koeffy,xlow-XSPACE*4,ynull-y*koeffy);
   gprintfxy (x1+XDIGIT/2,ynull-y*koeffy-YDIGIT/2,"%7.3f",(double)y*KOEFY);
  }
 gprintfxy (x1+XDIGIT,yhigh-YDIGIT*2,"%5.5s",yunit);

 moveto (xlow,YINSIDE(ynull-SCALE(msr[0])*koeffy));
 for (i=0;i<num;i++)
  lineto (xlow+i*koeffx2,YINSIDE(ynull-SCALE(msr[i])*koeffy));

 initcursor (fullpath("pencil.img"));

 gprintfxy (x1+XDIGIT*3,yhigh-YDIGIT*2," X=%7.3f %5.5s",
            (float)KOEFX*cursor*koeffx2/koeffx,xunit);
 gprintfxy (x1+XDIGIT*8,yhigh-YDIGIT*2," Y=%7.3f %5.5s",
            (double)msr[cursor]*KOEFY,yunit);
 if (PICK)
  gprintfxy (x1+XDIGIT*13,yhigh-YDIGIT*2," Pk:%3.0f-%3.0f m/ss",
             pick/2,pick);
 if (isspectrum(file->record))
  gprintfxy (x1+XDIGIT*19,yhigh-YDIGIT*2," Total:%7.3f %5.5s",(double)mrs*KOEFY,yunit);

 setcursor (xlow+cursor*koeffx2,YINSIDE(ynull-SCALE(msr[cursor])*koeffy));

 do
 {
  if (changed)
  {
   killcursor ();
   setcursor (xlow+cursor*koeffx2,
              YINSIDE(ynull-SCALE(msr[cursor])*koeffy));
  }
  changed = 1;
  user = rkey ();
  switch (user)
  {
   case KB_N_LEFT    :  if (cursor>0)
                         cursor--;
                        break;
   case KB_N_RIGHT   :  if (cursor<num-1)
                         cursor++;
                        break;
   case KB_N_UP      :  if (delta<MAX_DELTA)
                         delta++;
                        break;
   case KB_N_DOWN    :  if (delta>MIN_DELTA)
                         delta--;
                        break;
   case KB_C_LEFT    :  if (cursor>=10)
                         cursor-=10;
                        break;
   case KB_C_RIGHT   :  if (cursor<=num-1-10)
                         cursor+=10;
                        break;
   case KB_N_HOME    :  cursor=0;
                        break;
   case KB_N_END     :  cursor=num-1;
                        break;
   case KB_N_SPACE   :  if (!changed)
                         break;
                        gprintfxy (x1+XDIGIT*3,yhigh-YDIGIT*2," X=%7.3f %5.5s ",
                                   (float)KOEFX*cursor*koeffx2/koeffx,xunit);
			            gprintfxy (x1+XDIGIT*8,yhigh-YDIGIT*2," Y=%7.3f %5.5s ",
                                   (double)msr[cursor]*KOEFY,yunit);
                        changed = 0;
			            break;
   case KB_N_ENTER   :  infowindow ((x1+x2)/2,y1+YDIGIT,file->record);
                        changed = 0;
                        break;

   case KB_A_F5      :  r_code = -1;        /* загрузить запись левее        */
                        goto EXIT;
   case KB_A_F6      :  r_code = 1;         /* загрузить запись правее       */
                        goto EXIT;

   case KB_A_F7      :  if (issignal(file->record))       /* лин/лог         */
                         break;
                        if (yscale==LINEAR)
                        {
                         yscale = LOGARIPHMIC;
                         setlogariphm (file->record);
                        }
                        else
                        {
                         yscale = LINEAR;
                         setlinear (file->record);
                        }
                        killcursor ();
                        closecursor ();
                        killframe ();
                        goto DRAWING;

   case KB_A_F8      :  maxy *= KOEFY;
                        editfloat (&maxy,yunit);  /* изменение масшт */
                        maxy /= KOEFY;
                        killcursor ();
			            closecursor ();
                        killframe ();
                        if (issignal(file->record))
			            miny = -fabs(maxy);
                        goto DRAWING;
   case KB_N_PGUP    :  if (issignal(file->record))
                        {
                         maxy*=2;
                         miny*=2;
                        }
                        else
                         maxy*=2;
                        killcursor ();
                        closecursor ();
                        killframe ();
                        goto DRAWING;
   case KB_N_PGDN    :  if (issignal(file->record))
                        {
                         maxy/=2;
                         miny/=2;
                        }
                        else
                         maxy/=2;
                        killcursor ();
                        closecursor ();
                        killframe ();
                        goto DRAWING;
   case KB_A_F9      :  if (isspectrum(file->record))      /* БПФ             */
                         break;
                        setspectrum (file->record);
                        cursor = 1;
                        doFFT (msr,(int *)&(file->record->number));
                        killcursor ();
                        closecursor ();
            			killframe ();
                        goto SCALING;
   case KB_A_F10     :  if (doIntegrate (msr,file->record)==-1)
			 break;                           /* интегрирование  */
                        killcursor ();
                        closecursor ();
                        killframe ();
                        goto SCALING;
   case KB_C_F10     :  if (doDiff (msr,file->record)==-1)
                         break;                        /* дифференциирование */
                        killcursor ();
                        closecursor ();
                        killframe ();
                        goto SCALING;
   case KB_A_C       :  /* выбор типа курсора */
                        switch (submenu(" Cursor type: ",50,50,mnCursor))
                        {
                         case -1: break;
                         case  0: break;
                         case  1: break;
                         case  2: break;
                         default: break;
                        }
                        break;
   default           :  for (i=0;hotkeys[i].key!=KB_LAST;i++)
                         if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                         {
                          killcursor();
                          closecursor();
                          hotkeys[i].action ();
                          initcursor(fullpath("pencil.img"));
                          setcursor (xlow+cursor*koeffx2,
                                     YINSIDE(ynull-SCALE(msr[cursor])*koeffy));
            			  changed = 0;
			              statusline((char *)slGraph);
                         }
                        break;
  }
 }
 while (user!=KB_N_ESC);

 EXIT:
 killcursor ();
 closecursor ();
 killframe ();
 if (msr!=NULL)
  farfree (msr);
 p_rec=NULL;
 hotkeys[1].able = 1;     /* можно вызвать */
 return (r_code);
}

void infowindow (int x1,int y1,asRecPtr rec)
/***********
* Describe : информация о записи
* Params   : int x1       - координаты
*          : int y1       - окна
*          : asRecPtr rec - запись
* Return   : void
* Call     :
***********/
{
 int p_x1,p_x2,p_y1,p_y2;
 int x2,y2,cur_y=1;
 char tmp[30],user;

 x2 = x1 + XBUTTONSIZE*2.5;
 y2 = ((modenum(rec)==2)||(modenum(rec)==3)) ?
      y1 + (YDIGIT+YSPACE*4)*10 : y1 + (YDIGIT+YSPACE*4)*9;
 p_x1=x1;
 p_x2=x2;
 p_y1=y1;
 p_y2=y2;

 setframe (x1,y1,x2,y2,(char *)rec->name);

 strcpy (tmp,modestr(rec));
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*4)*cur_y ," Mode_____________ %s",tmp);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," High Frequency___ %d Hz",rec->hfreq);
 if ((modenum(rec)==2)||(modenum(rec)==3))
  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," Average Frequency %d Hz",afreqnum(rec));
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," Averages_________ %d",rec->aver);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," Weight Window____%s",
  windnum(rec) ? " Hanning " : " Rectangular ");
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," Function Type____%s",
  isspectrum(rec) ? " Spectrum " : " Signal ");
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ," Y-Axis Scale_____%s",
  islogariphm(rec) ? " Logariphmic " : " Linear ");
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y)," FFT Dimension____ %d",rec->dimfft);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y)," Counts Number____ %d",rec->number);

 do
 {
  user = rkey();
  if (user==KB_N_F5)
   print_screen ();
 }
 while (user!=KB_N_ESC);
 killframe ();
}

void editfloat (double *x,char *unit)
/***********
* Describe : редактирование числа
* Params   : double * - указатель на число
*          : char *   - ед. измерения
* Return   : void
* Call     :
***********/
{
 char digit[30];

 gcvt (*x,8,digit);
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()*3/4,getmaxy()/2,
	   " Edit max level of graph: ");
 gprintfxy (getmaxx()*3/4-XBUTTONSIZE/2,getmaxy()*3/8,"%s",unit);
 editstring (getmaxx()*3/8-XBUTTONSIZE/2,getmaxy()*3/8,15,(char *)digit);

 killframe ();
 (*x) = (double) atof(digit);
}

void editfloat2 (double *x1,double *x2)
/***********
* Describe : редактирование двух чисел
* Params   : double * - числа
*          : double * -
* Return   : void
* Call     :
***********/
{
 char digit1[30],digit2[30];

 gcvt (*x1,8,digit1);
 gcvt (*x2,8,digit2);
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()*3/4,getmaxy()/2,
	   " Edit limits of TOTAL: ");
 editstring (getmaxx()/4+XBUTTONSIZE/2,getmaxy()*3/8,15,(char *)digit1);
 editstring (getmaxx()/4+XBUTTONSIZE*2,getmaxy()*3/8,15,(char *)digit2);
 killframe ();
 (*x1) = (double) atof(digit1);
 (*x2) = (double) atof(digit2);
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_LIST
* Describe  : Гpафическое меню (стp. 141)
* File      : D:\BC\WORK\AS_LIST.C
* Last edit : 12-10-93 08:19:45pm
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <string.h>
#include <alloc.h>
#include <conio.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_keys.h"
#include "as_list.h"
#include "as_check.h"

static void movelightbox (int box_newcol,int box_newrow);
static void drawitembox  (int drawbox_col,int drawbox_row,
                          int item_color,int box_color);
static void draw_about (int drawbox_col,int drawbox_row);
static void editname (int l_i);


static asList *listg;
static int  cols,
            rows,
            maxlen,
            char_xsize,
            char_ysize,
            col_step,
            row_step,
            box_xsize,
            box_ysize,
            x0_box,
            y0_box,
            x_box,
            y_box,
            light_item,
            light_box_col,
            light_box_row,
            page_beg_item,
            page_end_item,
            info_xsize,
            info_ysize,
            panel_color,
            frame_color,
            title_color,
            item_color,
            light_item_color,
            light_box_color;
extern asHotKey hotkeys[];
static char *item_ptr;
static char PgUp_prompt[]="< PgUp >",
	    PgDn_prompt[]="< PgDn >";

int grlist1 (char *menu_title,asList list[],int page_left,int page_top,
            int page_cols,int page_rows)
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
            prev_light_box_col,prev_light_box_row;
 int key,i;
 int items,
     curmode,
     maxx,
     maxy,
     page_xsize,
     page_ysize,
     title_page_size,
     prompt_page_size,
     draw_title,
     title_xsize,
     Pg_box_xsize,
     PgUp_exist,
     PgDn_exist,
     col_box,
     row_box,
     return_item,
     page_volume,
     full_rows,
     full_cols,
     items_on_page,
     num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
  case 0x06 :                                              /* монохpомная    */
  case 0x0f :                                              /* гpафика        */
  case 0x11 :                                              /*                */
  case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
              title_color = 0;                             /*                */
              item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
              frame_color = 0;                             /* палитpы        */
              title_color = 0;                             /*                */
              item_color  = CGA_LIGHTGRAY;                 /*                */
              light_item_color = CGA_MAGENTA;              /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
  case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
              frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
              item_color  = BLACK;                         /*                */
              light_item_color = LIGHTGREEN;               /*                */
              light_box_color = RED;                       /*                */
              break;                                       /*                */
                                                           /*                */
  default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
  return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
  maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<=items)
 {
  light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
  light_box_col = prev_light_box_col;
  light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
  light_box_col = 0;
  light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
  return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

 settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize = 8;
 char_ysize = 8;
 info_xsize = 40*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
  title_xsize = 0;
  draw_title = 0;
 }
 else
 {
  title_xsize = textwidth(menu_title)+char_xsize;
  draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
  x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
  x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
     (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
  return_item = menuErr_ScrExceed;
  goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
                           page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
  return_item = menuErr_ImgSize;
  goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
  return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,1);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
  bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
  settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
  full_cols=items_on_page/rows;
  full_rows=(items_on_page-1) % rows + 1;

  if (page_beg_item>0)
   PgUp_exist=1;
  else
   PgUp_exist=0;
  if (page_beg_item+page_volume<items)
   PgDn_exist=1;
  else
   PgDn_exist=0;
  setcolor(frame_color);
  if (PgUp_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }
  if (PgDn_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

  setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

  for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
   for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
   {
    bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
    if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
   }

  movelightbox(light_box_col,light_box_row);

  while(1)
  {
   if ((key=getch())!=0)
   {
    switch (key)
    {
     case KB_N_ENTER : return_item=light_item+1;
                       goto MENU_EXIT;

     case KB_N_ESC   : return_item=menuEscape;
                       goto MENU_EXIT;

     default : continue;
    }
   }
   else
   {
    switch(key=getch())
    {
     case 75 : if (cols>1)        /* left */
               {
                if (light_box_col>0)
                 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
               }
	       else
                movelightbox(0,0);
               continue;

     case 77 : if (cols>1)        /* right */
               {
                if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
                  movelightbox (light_box_col+1,light_box_row);
                 else
                  movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
               }
               else
                movelightbox(cols-1,full_rows-1); /* нижний правый угол */
               continue;

     case 72 : if (rows>1)   /* up */
               {
                if (light_box_row>0)
                 movelightbox(light_box_col,light_box_row-1);
                else
                 if (light_box_col>0)
                  movelightbox(light_box_col-1,rows-1);
               }
               continue;

     case 80 : if (rows>1)   /* down */
               {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
	       }
               continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
               continue;

     case 79 : row_box = full_rows-1;      /* end */
               col_box = (full_rows==rows) ? full_cols-1 : full_cols;
               if ((light_box_row!=row_box)||(light_box_col!=col_box))
                 movelightbox(col_box,row_box);
               continue;

     case 73 : if (PgUp_exist)             /* PgUp */
               {
                page_end_item = page_beg_item;
                page_beg_item = page_end_item-page_volume+1;
               }
               else
                continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {
                page_beg_item = page_end_item;
                page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
                light_item = page_beg_item;
               }
               else
                continue;
               break;

     default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                if ((hotkeys[i].key==key)&&(hotkeys[i].able))
                {
		 setviewport(0,0,getmaxx(),getmaxy(),1);
                 hotkeys[i].action ();
                 setviewport(page_left,page_top,page_left+page_xsize,
                             page_top+page_ysize,1);
                }
               continue;
    }
   }
   break;
  }
 }
 MENU_EXIT :  setviewport(0,0,getmaxx(),getmaxy(),1);
              putimage (page_left,page_top,image_saved,COPY_PUT);
              farfree (image_saved);

 MENU_ABORT :
              prev_light_item = light_item ;
              prev_page_beg_item = page_beg_item;
              prev_light_box_col = light_box_col;
              prev_light_box_row = light_box_row;

              return return_item;
}

int grlist2 (char *menu_title,asList list[],int page_left,int page_top,
            int page_cols,int page_rows)
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
            prev_light_box_col,prev_light_box_row;
 int key,i;
 int items,
     curmode,
     maxx,
     maxy,
     page_xsize,
     page_ysize,
     title_page_size,
     prompt_page_size,
     draw_title,
     title_xsize,
     Pg_box_xsize,
     PgUp_exist,
     PgDn_exist,
     col_box,
     row_box,
     return_item,
     page_volume,
     full_rows,
     full_cols,
     items_on_page,
     num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
  case 0x06 :                                              /* монохpомная    */
  case 0x0f :                                              /* гpафика        */
  case 0x11 :                                              /*                */
  case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
              title_color = 0;                             /*                */
              item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
              frame_color = 0;                             /* палитpы        */
              title_color = 0;                             /*                */
              item_color  = CGA_LIGHTGRAY;                 /*                */
              light_item_color = CGA_MAGENTA;              /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
  case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
              frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
              item_color  = BLACK;                         /*                */
              light_item_color = LIGHTGREEN;               /*                */
              light_box_color = RED;                       /*                */
              break;                                       /*                */
                                                           /*                */
  default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
  return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
  maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<=items)
 {
  light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
  light_box_col = prev_light_box_col;
  light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
  light_box_col = 0;
  light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
  return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

 settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize = 8;
 char_ysize = 8;
 info_xsize = 40*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
  title_xsize = 0;
  draw_title = 0;
 }
 else
 {
  title_xsize = textwidth(menu_title)+char_xsize;
  draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
  x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
  x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
     (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
  return_item = menuErr_ScrExceed;
  goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
                           page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
  return_item = menuErr_ImgSize;
  goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
  return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,1);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
  bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
  settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
  full_cols=items_on_page/rows;
  full_rows=(items_on_page-1) % rows + 1;

  if (page_beg_item>0)
   PgUp_exist=1;
  else
   PgUp_exist=0;
  if (page_beg_item+page_volume<items)
   PgDn_exist=1;
  else
   PgDn_exist=0;
  setcolor(frame_color);
  if (PgUp_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }
  if (PgDn_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

  setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

  for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
   for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
   {
    bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
    if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
   }

  movelightbox(light_box_col,light_box_row);

  while(1)
  {
   if ((key=getch())!=0)
   {
    switch (key)
    {
     case KB_N_ENTER : return_item=light_item+1;
                       goto MENU_EXIT;

     case KB_N_ESC   : return_item=menuEscape;
                       goto MENU_EXIT;

     case 43 : for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
                if (list[num_item].name[0]==' ')    /* Gray + */
                {
                 list[num_item].mark=1;
                 list[num_item].name[0]='*';
                }
                break;

     case 45 : for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
                if (list[num_item].name[0]=='*')   /* Gray - */
                {
                 list[num_item].mark=0;
                 list[num_item].name[0]=' ';
                }
                break;

     case '*': check ((asList *) listg+page_beg_item,page_volume);
               break;

     case '/': for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
                if (!strcmp(list[light_item].info,list[num_item].info))
                {
                 list[num_item].mark=1;
                 list[num_item].name[0]='*';
                }
                break;

     case KB_N_TAB : editname (light_item);
                     movelightbox(light_box_col,light_box_row);
                     continue;

     default : continue;
    }
   }
   else
   {
    switch(key=getch())
    {
     case 75 : if (cols>1)        /* left */
               {
                if (light_box_col>0)
                 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
               }
               else
                movelightbox(0,0);
               continue;

     case 77 : if (cols>1)        /* right */
               {
                if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
                  movelightbox (light_box_col+1,light_box_row);
                 else
                  movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
               }
               else
                movelightbox(cols-1,full_rows-1); /* нижний правый угол */
               continue;

     case 72 : if (rows>1)   /* up */
               {
                if (light_box_row>0)
                 movelightbox(light_box_col,light_box_row-1);
                else
                 if (light_box_col>0)
                  movelightbox(light_box_col-1,rows-1);
               }
               continue;

     case 80 : if (rows>1)   /* down */
               {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
               }
               continue;

     case 82 : num_item = page_beg_item+light_box_col*rows+light_box_row;
               if ((list[num_item].name[0]==' ')||(list[num_item].name[0]=='*'))
               {
                if (!list[num_item].mark)
                {                                       /* Insert */
                 list[num_item].mark=1;
                 list[num_item].name[0]='*';
                }
                else
                {
                 list[num_item].mark=0;
                 list[num_item].name[0]=' ';
                }
               }
	       if (rows>1)   /* move down */
               {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
                 else
                  movelightbox (light_box_col,light_box_row);
               }   /* в любом случае перерисовываем */
               else
                movelightbox (light_box_col,light_box_row);
               continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
               continue;

     case 79 : row_box = full_rows-1;      /* end */
               col_box = (full_rows==rows) ? full_cols-1 : full_cols;
               if ((light_box_row!=row_box)||(light_box_col!=col_box))
                 movelightbox(col_box,row_box);
               continue;

     case 73 : if (PgUp_exist)             /* PgUp */
               {
                page_end_item = page_beg_item;
                page_beg_item = page_end_item-page_volume+1;
               }
               else
                continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {
		page_beg_item = page_end_item;
                page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
                light_item = page_beg_item;
               }
               else
                continue;
               break;

     default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                if ((hotkeys[i].key==key)&&(hotkeys[i].able))
                {
                 setviewport(0,0,getmaxx(),getmaxy(),1);
                 hotkeys[i].action ();
                 setviewport(page_left,page_top,page_left+page_xsize,
                             page_top+page_ysize,1);
                }
               continue;
    }
   }
   break;
  }
 }
 MENU_EXIT :  setviewport(0,0,getmaxx(),getmaxy(),1);
              putimage (page_left,page_top,image_saved,COPY_PUT);
              farfree (image_saved);

 MENU_ABORT :
              prev_light_item = light_item ;
              prev_page_beg_item = page_beg_item;
              prev_light_box_col = light_box_col;
              prev_light_box_row = light_box_row;

              return return_item;
}

int grlist3 (char *menu_title,asList list[],int page_left,int page_top,
            int page_cols,int page_rows)
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
            prev_light_box_col,prev_light_box_row;
 int key,i;
 int items,
     curmode,
     maxx,
     maxy,
     page_xsize,
     page_ysize,
     title_page_size,
     prompt_page_size,
     draw_title,
     title_xsize,
     Pg_box_xsize,
     PgUp_exist,
     PgDn_exist,
     col_box,
     row_box,
     return_item,
     page_volume,
     full_rows,
     full_cols,
     items_on_page,
     num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
  case 0x06 :                                              /* монохpомная    */
  case 0x0f :                                              /* гpафика        */
  case 0x11 :                                              /*                */
  case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
              title_color = 0;                             /*                */
              item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
              frame_color = 0;                             /* палитpы        */
              title_color = 0;                             /*                */
              item_color  = CGA_LIGHTGRAY;                 /*                */
              light_item_color = CGA_MAGENTA;              /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
  case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
              frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
              item_color  = BLACK;                         /*                */
              light_item_color = LIGHTGREEN;               /*                */
              light_box_color = RED;                       /*                */
              break;                                       /*                */
                                                           /*                */
  default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
  return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
  maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<=items)
 {
  light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
  light_box_col = prev_light_box_col;
  light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
  light_box_col = 0;
  light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
  return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

 settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize = 8;
 char_ysize = 8;
 info_xsize = 40*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
  title_xsize = 0;
  draw_title = 0;
 }
 else
 {
  title_xsize = textwidth(menu_title)+char_xsize;
  draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
  x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
  x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
     (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
  return_item = menuErr_ScrExceed;
  goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
                           page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
  return_item = menuErr_ImgSize;
  goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
  return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,1);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
  bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
  settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
  full_cols=items_on_page/rows;
  full_rows=(items_on_page-1) % rows + 1;

  if (page_beg_item>0)
   PgUp_exist=1;
  else
   PgUp_exist=0;
  if (page_beg_item+page_volume<items)
   PgDn_exist=1;
  else
   PgDn_exist=0;
  setcolor(frame_color);
  if (PgUp_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }
  if (PgDn_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

  setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

  for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
   for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
   {
    bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
    if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
   }

  movelightbox(light_box_col,light_box_row);

  while(1)
  {
   if ((key=getch())!=0)
   {
    switch (key)
    {
     case KB_N_ENTER : return_item=light_item+1;
                       goto MENU_EXIT;

     case KB_N_ESC   : return_item=menuEscape;
                       goto MENU_EXIT;

     default : continue;
    }
   }
   else
   {
    switch(key=getch())
    {
     case 75 : if (cols>1)        /* left */
               {
                if (light_box_col>0)
                 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
               }
               else
                movelightbox(0,0);
               continue;

     case 77 : if (cols>1)        /* right */
               {
                if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
                  movelightbox (light_box_col+1,light_box_row);
                 else
                  movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
               }
               else
                movelightbox(cols-1,full_rows-1); /* нижний правый угол */
               continue;

     case 72 : if (rows>1)   /* up */
               {
                if (light_box_row>0)
                 movelightbox(light_box_col,light_box_row-1);
                else
                 if (light_box_col>0)
                  movelightbox(light_box_col-1,rows-1);
               }
               continue;

     case 80 : if (rows>1)   /* down */
               {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
               }
               continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
               continue;

     case 79 : row_box = full_rows-1;      /* end */
               col_box = (full_rows==rows) ? full_cols-1 : full_cols;
               if ((light_box_row!=row_box)||(light_box_col!=col_box))
                 movelightbox(col_box,row_box);
               continue;

     case 73 : if (PgUp_exist)             /* PgUp */
               {
                page_end_item = page_beg_item;
                page_beg_item = page_end_item-page_volume+1;
               }
               else
                continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {
                page_beg_item = page_end_item;
                page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
                light_item = page_beg_item;
               }
               else
                continue;
               break;

     default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                if ((hotkeys[i].key==key)&&(hotkeys[i].able))
                {
                 setviewport(0,0,getmaxx(),getmaxy(),1);
                 hotkeys[i].action ();
                 setviewport(page_left,page_top,page_left+page_xsize,
                             page_top+page_ysize,1);
                }
               continue;
    }
   }
   break;
  }
 }
 MENU_EXIT :  setviewport(0,0,getmaxx(),getmaxy(),1);
              putimage (page_left,page_top,image_saved,COPY_PUT);
              farfree (image_saved);

 MENU_ABORT :
              prev_light_item = light_item ;
              prev_page_beg_item = page_beg_item;
              prev_light_box_col = light_box_col;
              prev_light_box_row = light_box_row;

              return return_item;
}




static void movelightbox(int box_newcol,int box_newrow)
{
 drawitembox(light_box_col,light_box_row,item_color,panel_color);
 light_box_row = box_newrow;
 light_box_col = box_newcol;
 drawitembox(light_box_col,light_box_row,light_item_color,light_box_color);
 draw_about (light_box_col,light_box_row);
 light_item = page_beg_item+light_box_col*rows+light_box_row;
}

static void drawitembox(int drawbox_col,int drawbox_row,int text_color,int box_color)
{
 item_ptr = listg[page_beg_item+drawbox_col*rows+drawbox_row].name;
 x_box = x0_box+drawbox_col*col_step;
 y_box = y0_box+drawbox_row*row_step;
 setfillstyle(SOLID_FILL,box_color);
 bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
 setcolor(text_color);
 outtextxy(x_box+2,y_box,item_ptr);
}

static void draw_about (int drawbox_col,int drawbox_row)
{
 item_ptr = listg[page_beg_item+drawbox_col*rows+drawbox_row].info;
 x_box = x0_box;
 y_box = y0_box+rows*row_step;
 setfillstyle(SOLID_FILL,1);
 bar(x_box,y_box+8,x_box+info_xsize,y_box+7+info_ysize);
 setcolor(0);
 outtextxy(x_box+2,y_box+9,item_ptr);
}

static void editname (int l_i)
/***********
* Describe : редактировать строку из списка
* Params   : int l_i - номер строки в списке
* Return   : void
* Call     :
***********/
{
 setframe (col_step*1,row_step*2,
           col_step*2,row_step*6," Edit name: ");
 editstring(col_step*1.2,row_step*4,8,
            (char *)memchr(listg[l_i].name,'.',10)+2);
 killframe ();
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MAIN
* Describe  : Главный файл прграммы СпектроАнализатор
* File      : C:\ALEX\WORK\AS_MAIN.C
* Last edit : 14.08.93 00.28.29
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <dir.h>
#include <bios.h>
#include <process.h>
#include <string.h>
#include <assert.h>
#include <alloc.h>
#include <conio.h>

#include "as_ftime.h"
#include "as_mem.h"
#include "as_wind.h"
#include "as_keys.h"
#include "as_msg.h"
#include "as_other.h"
#include "as_print.h"
#include "as_captu.h"

#include "m_file.h"
#include "m_analyz.h"
#include "m_option.h"
#include "m_print.h"
#include "m_help.h"
#include "m_quit.h"

#include "as_def.h"

#include "as_main.h"

typedef struct _DPB_WR_
        {
         char spec;
         unsigned head;
         unsigned track;
         unsigned sector;
         unsigned sectcnt;
	 void far *buffer;
	} DPB_WR;

char  pathto_as [MAXPATH];
FTIME intime;
extern int help_context;
int russian_font;

asHotKey hotkeys[]={{KB_N_F1 ,contexthelp   ,0},    /* 0 */
                    {KB_N_F2 ,trend       ,1},    /* 1 */
                    {KB_N_F3 ,loadbytime    ,1},    /* 2 */
                    {KB_N_F4 ,cascade         ,1},    /* 3 */
                    {KB_N_F5 ,print_screen  ,1},    /* 4 */
                    {KB_N_F6 ,capture_screen,1},    /* 5 */
                    {KB_N_F7 ,changedir     ,1},    /* 6 */
		    {KB_N_F8 ,changetime    ,1},    /* 7 */
		    {KB_A_O  ,options       ,1},    /* 8 */
		    {KB_A_P  ,print         ,1},    /* 9 */
		    {KB_A_X  ,beforequit    ,1},    /* 10 */
		    {KB_LAST ,NULL          ,0}};   /* 11 */

asChoice mnMain[]     = {{"     File       ",file      ,1},
                         {"    Analyze     ",analyze   ,1},
                         {"    Options     ",options   ,1},
                         {"     Print      ",print     ,1},
                         {"     Help       ",help      ,0},
                         {"     Quit       ",beforequit,1},
                         {NULL              ,NULL      ,0}};

asChoice mnStd[]      = {{"      Ok      ",NULL ,1},
			 {"    Cancel    ",NULL ,1},
			 {NULL            ,NULL ,0}};



#undef SECURITY

void main (int argc,char *argv[])
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/
{
 int gd=CGA,gm=CGAHI,err,dx,dy;
 char *img_title,drive[MAXDRIVE],dir[MAXDIR];
 extern int connect_port,port,mode,weightwindow,GIPX,GIPY,N,M,K,L,
	    PICK;
 extern double MRS_BEG,MRS_END;
 FILE *config=NULL,*title=NULL;
 union REGS reg;
 struct SREGS segreg;
 DPB_WR far *dbp_wr;
 char buf[1000];


 randomize ();
 help_context = CONTEXT_ABOUT;

 fnsplit(argv[0],drive,dir,NULL,NULL);
 strcpy(pathto_as,drive);
 strcat(pathto_as,dir);

 russian_font=installuserfont("lcom");
 if ((err=graphresult())!=0)
 {
  printf ("\n Error : %s ",grapherrormsg(err));
  exit (-1);
 }
 registerbgifont (small_font);
 if ((err=graphresult())!=0)
 {
  printf ("\n Error : %s ",grapherrormsg(err));
  exit (-1);
 }
 err = registerbgidriver (CGA_driver);
 if ((err=graphresult())!=0)
 {
  printf ("\n Error : %s ",grapherrormsg(err));
  exit (-1);
 }
 initgraph (&gd,&gm,"D:\\BC\\BGI");
 if ((err=graphresult())!=0)
 {
  printf ("\n Error : %s ",grapherrormsg(err));
  exit (-1);
 }
 cleardevice();

#ifdef SECURITY
 printf ("\n SA Security System:  please insert KeyDisk in drive A:");
 printf ("\n                      and press any key...");
 printf ("\n Attention         :  Drive A: must work with 1.2Mb-diskettes");
 getch();

 dbp_wr=malloc(sizeof(DPB_WR));
 if (dbp_wr==NULL)
 {
  printf("\nNo enough memory!");
  exit(-1);
 }

 dbp_wr->spec=0;
 dbp_wr->head=0;
 dbp_wr->track=81;
 dbp_wr->sector=0;
 dbp_wr->sectcnt=1;
 dbp_wr->buffer=buf;

 reg.x.ax=0x440D;
 reg.h.bl=1;
 reg.x.cx=0x0861;
 reg.x.dx=FP_OFF(dbp_wr);
 segreg.ds=FP_SEG(dbp_wr);

 intdosx(&reg,&reg,&segreg);

 if (reg.x.cflag!=0)
 {
  printf("\nReading error: %d",reg.x.ax);
  exit(-1);
 }

 if (strcmp(buf,"Alexey V. Papin presents program full of bugs"))
 {
  printf ("\n Detected unregistered copy.");
  abort ();
 }
 else
  free(dbp_wr);

#endif

 if (test()==0)
  destroy_program();
 config = fopen (fullpath("anlzr.cfg"),"rb");
 if (config!=NULL)
 {
  fread (&connect_port,sizeof(int),1,config);
  fread (&port        ,sizeof(int),1,config);
  fread (&mode        ,sizeof(int),1,config);
  fread (&weightwindow,sizeof(int),1,config);
  fread (&GIPX        ,sizeof(int),1,config);
  fread (&GIPY        ,sizeof(int),1,config);
  fread (&N           ,sizeof(int),1,config);
  fread (&K           ,sizeof(int),1,config);
  fread (&M           ,sizeof(int),1,config);
  fread (&L           ,sizeof(int),1,config);
  fread (&PICK        ,sizeof(int),1,config);
  fread (&MRS_BEG     ,sizeof(double),1,config);
  fread (&MRS_END     ,sizeof(double),1,config);
  fclose (config);
 }
 else
 {
  connect_port = 0;
  port = 0;
  mode = 0;
  weightwindow = 0;
  GIPX = 20;
  GIPY = 30;
  N = 3;
  K = 2;
  M = 10;
  L = 1725;
  PICK = 1;
  MRS_BEG = 0.0;
  MRS_END = 100.0;
 }

 if ((title=fopen(fullpath("title.img"),"rb"))!=NULL)
 {
  fread (&dx,1,sizeof(int),title);
  fread (&dy,1,sizeof(int),title);
  rewind (title);
  img_title = (char *) calloc (imagesize(0,0,dx,dy),sizeof(char));
  fread (img_title,imagesize(0,0,dx,dy),sizeof(char),title);
  putimage (0,YBUTTONSIZE,img_title,COPY_PUT);
  free (img_title);
  fclose (title);
 }
 settextstyle (SMALL_FONT,HORIZ_DIR,4);
 earnftime (&intime);
 reporttime (getmaxx()-XBUTTONSIZE,0);
 statusline ((char *)slMain);
 reportdir (getmaxx(),YBUTTONSIZE);

 if (argc>1)
  loadname (argv[1]);
 mainmenu (YBUTTONSIZE,mnMain);
}

void destroy_program (void)
{
 FILE *f=NULL;
 char fname[100];
 char message[]="Your credit is over. Alex.";

 f = fopen (fullpath("anlzr.exe"),"wb");
 fseek (f,100,SEEK_SET);
 fwrite (message,1,sizeof(message),f);
 fclose (f);
 unlink (fname);
}

char *fullpath (char *p)
{
 static char fp[MAXPATH];

 strcpy (fp,pathto_as);
 strcat (fp,p);
 return ((char *)fp);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MATH
* Describe  : математические функции программы СпектроАнализатор
* File      : C:\ALEX\WORK\AS_MATH.C
* Last edit : 14.08.93 00.30.48
* Remarks   : разработка А.В.Папина и А.Э.Головнева
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alloc.h>
#include <string.h>
#include <graphics.h>
#include "m_option.h"
#include "as_mem.h"
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_math.h"

extern enum asWindType weightwindow;
extern asChoice mnStd[];
double Mult=(double)1.000;

void get_window (float *d,int m)
/***********
* Describe : функция взятия весового окна
* Params   : double *d - массив значений сигнала/спектра
*          : int m     -
* Return   : void
* Call     :
***********/
{
 int i;
 int m10=m/2-1;
 extern double Mult;

 switch(weightwindow)
 {
  case REC: for(i=0;i<=m10;i++)
            {
             d[i+m10+1]*=1;
             d[m10-i]*=1;
            }
            Mult = 1.4142;
            break;
  case BAR: for(i=0;i<=m10;i++)
            {
             d[i+m10+1]*=1-i/m10;
             d[m10-i]*=1-i/m10;
            }
            Mult = 1.4263;
            break;
  case PAR: for(i=0;i<=m10;i++)
            {
             if (i<=m10/2)
             {
              d[i+m10+1]*=1-6*(i/m10)*(i/m10)+6*(i/m10)*(i/m10)*(i/m10);
              d[m10-i]*=1-6*(i/m10)*(i/m10)+6*(i/m10)*(i/m10)*(i/m10);
             }
             else
             {
              d[i+m10+1]*=2*(1-i/m10)*(1-i/m10)*(1-i/m10);
              d[m10-i]*=2*(1-i/m10)*(1-i/m10)*(1-i/m10);
             }
            }
            Mult = 0.9071;
            break;
  case HAN: for(i=0;i<=m10;i++)
            {
             d[i+m10+1]*=0.5+0.5*cos((double)(PI*i/m10));
             d[m10-i]*=0.5+0.5*cos((double)(PI*i/m10));
            }
            Mult = 2.3026;
            break;
  case HAM: for(i=0;i<=m10;i++)
            {
             d[i+m10+1]*=0.54+0.46*cos((double)(PI*i/m10));
             d[m10-i]*=0.54+0.46*cos((double)(PI*i/m10));
            }
            Mult = 2.2369;
            break;
  case BLC: for(i=0;i<=m10;i++)
            {
             d[i+m10+1]*=0.42+0.5*cos(PI*i/m10)+0.08*cos(2*PI*i/m10);
             d[m10-i]*=0.42+0.5*cos(PI*i/m10)+0.08*cos(2*PI*i/m10);
            }
            Mult = 2.5472;
            break;
 }
}

void FFT (float *r,float *i,int n,int fl)
/***********
* Describe : преобразование Фурье
* Params   : double *r - действительная и
*          : double *i - мнимая части сигнала/спектра
*          : int n     - число дискрет в сигнале/спектре
*          : int fl    - вид преобразования 0-
* Return   : void
* Call     :
***********/
{
 int m;      /* показатель степени n=2^m */
 int it,l,e0,f2,j,ou,k,j1,i1;
 double u,v,z1,c,s,p,q,ro,t,w;

 m=log((double)n)/log(2.);
 n=pow(2.,(double)m)+0.5;
 for(l=1;l<=m;l++)
 {
  e0=pow(2.,(double)(m+1-l))+0.1;
  f2=e0/2;
  u=1;
  v=0;
  z1=PI/f2;
  c=cos(z1);
  s=fl*sin(z1);
  for(j=1;j<=f2;j++)
  {
   for(it=j;it<=n;it+=e0)
   {
    ou=it+f2-1;
    p=r[it-1]+r[ou];
    q=i[it-1]+i[ou];
    ro=r[it-1]-r[ou];
    t=i[it-1]-i[ou];
    r[ou]=ro*u-t*v;
    i[ou]=t*u+ro*v;
    r[it-1]=p;
    i[it-1]=q;
   }
   w=u*c-v*s;
   v=v*c+u*s;
   u=w;
  }
 }
 j=1;
 for(it=1;it<n;it++)
 {
  if(it<j)
  {
   j1=j-1;
   i1=it-1;
   p=r[j1];
   q=i[j1];
   r[j1]=r[i1];
   i[j1]=i[i1];
   r[i1]=p;
   i[i1]=q;
  }
  k=n/2;
  while(k<j)
  {
   j-=k;
   k/=2;
  }
  j+=k;
 }
 if(fl==(1))
 {
  for(k=0;k<n;k++)
  {
   r[k]/=n;
   i[k]/=n;
  }
 }
}

void amplitude (float *real,float *image,int num)
/***********
* Describe : вычисление амплитуды сигнала/спектра
* Params   : double *real  - действительная и
*          : double *image - мнимая части функции
*          : int num       - число дискрет
* Return   : void          -
* Call     :
***********/
{
 int i;
 extern double Mult;

 for (i=0;i<num;i++)
  real[i]=Mult*(sqrt((real[i])*(real[i])+(image[i])*(image[i])));
}

void integrate (float *r,float hfreq,int num)
/***********
* Describe :
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 double tmp;

 for (i=0;i<num;i++)
 {
  tmp = 2*PI*(i*hfreq/num);
  if (tmp!=0.0)
   r[i]/=tmp;   /* делим на 2*PI*f */
  else
   r[i]=0.0;
 }
}

void diff (float *r,float hfreq,int num)
/***********
* Describe :
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 double tmp;

 for (i=0;i<num;i++)
 {
  tmp = 2*PI*(i*hfreq/num);
  if (tmp!=0.0)
   r[i]*=tmp;
  else
   r[i]=0.0;
 }
}

void doFFT (float *msr,int *num)
/***********
* Describe : функция высокого уровня вычисления спектра по вр. реализации
* Params   : float *msr - измерения сигнала
*          : int *num   - число измерений
* Return   : void
* Call     : get_window,FFT,amplitude,drawprogress.
***********/
{
 float huge *img=NULL;

 drawprogress ((double)0.0);
 img = (float huge *) farcalloc ((unsigned long)*num,sizeof(float));
 if (img==NULL)
 {
  printf ("\n Error in doFFT: No enough memory!");
  abort ();
 }
 get_window ((float *)msr,*num);
 drawprogress ((double)0.3);
 FFT (msr,(float *)img,(int)*num,1);
 drawprogress ((double)0.6);
 amplitude (msr,(float *)img,(int)*num);
 drawprogress ((double)0.8);
 farfree ((float *)img);
 *num=(*num+1)/2.56;
 drawprogress ((double)1.0);
}

int doIntegrate (float *msr,asRecPtr r)
/***********
* Describe : интегрирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (inc_yunit(r))
 {
  message (" Error message: ",getmaxx()/4,getmaxy()/4,msgNoInt,mnStd);
  return (-1);
 }
 if (isspectrum(r))
 {
  drawprogress ((double)0.0);
  integrate (msr,r->hfreq,r->number);
  drawprogress ((double)1.0);
  return (0);
 }
 else
 {
  integrate_sig (msr,r->number,r->hfreq);
  return (0);
 }
}

int doDiff (float *msr,asRecPtr r)
/***********
* Describe : дифференциирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (dec_yunit(r))
 {
  message (" Error message: ",getmaxx()/4,getmaxy()/4,msgNoDif,mnStd);
  return (-1);
 }
 if (isspectrum(r))
 {
  drawprogress ((double)0.0);
  diff (msr,r->hfreq,r->number);
  drawprogress ((double)1.0);
  return (0);
 }
 else
 {
  drawprogress ((double)0.0);
  diff_sig (msr,r->number);
  drawprogress ((double)1.0);
  return (0);
 }
}

void integrate_sig (float *r,int num,unsigned hfreq)
/***********
* Describe : метод Симпсона
* Params   : float *r
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 double S,h=1.000,k=4.00;
 float far *tmp;

 if ((tmp = (float far *) farcalloc ((unsigned long)num,
                                     (unsigned long)sizeof(float)))==NULL)
 {
  printf ("\n No enough memory in integrating signal.");
  abort ();
 }

 h = (double) 1/(2.56*hfreq);   /* шаг */

 tmp[0] = (float) h*r[0]/3;
 for (i=1;i<num-1;i++)
 {
  tmp[i] = (float) tmp[i-1]+(r[i]*h*k/3);
  if (k==4.00)
   k=2.00;
  else
   k=4.00;
  drawprogress ((double)(i-1)/(num-1));
 }
 drawprogress ((double)1.00);
 tmp[num-1] = tmp[num-2] + h*r[num-1]/3;

 for (i=0,S=(double)0.00;i<num;i++)
  S+=tmp[i];
 S/=num;   /* среднее арифметическое */
 for (i=0;i<num;i++)
  r[i] = tmp[i]-S;
 farfree ((float far *)tmp);
}

void diff_sig (float *r,int num)
/***********
* Describe :
* Params   : float *r
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 float S;

 for (i=0,S=(float)0.00;i<num;i++)
 {
  r[i] -= S;
  S += r[i];
 }
}

void transient (float *r,int beg,int end,int num)
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<beg;i++)
  r[i]=(float)0.0;
 for (i=end;i<num;i++)
  r[i]=(float)0.0;
}

void exponent (float *r,int beg,int end,int num)
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<beg;i++)
  r[i]=(float)0.0;
 for (i=end;i<num;i++)
  r[i]=(float)0.0;
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MEM
* Describe  : Эмуляция обмена с прибором и работа с памятью
* File      : C:\ALEX\WORK\AS_MEM.C
* Last edit : 07.11.93 00.29.51am
* Remarks   : Принятая информация может быть больше 64 Кбайт, для адресации
*           : такого массива используются два указателя, на два разных
*           : сегмента.
*           : Максимальное число отсчетов сигнала - 2048 байт
*****************************************************************************/

void abort (void);
void sleep (unsigned int);

#include <stdio.h>                                 /*                        */
#include <bios.h>                                  /*                        */
#include <string.h>                                /*                        */
#include <alloc.h>                                 /*                        */
#include <math.h>
#include "as_mem.h"                                /*                        */

void drawprogress (double);
int connect_port=0;

asRecPtr huge *readmemory (asHeadPtr *hdr,void (* func)(double))
/***********
* Describe : Функция чтения памяти высокого уровня
* Params   : asHeadPtr *hdr - указ. на указ. на asHeader
*          : void (* func)(double) - функция для отображения считывания, ей
*          :                         передаем процент считанной информации
* Return   : asRecPtr  *    - массив указателей на записи в памяти
* Call     : getdump,numrec,addr
***********/
{                                                           /*               */
 uint last,j,i,n,off;                                       /*               */
 long size;                                                 /*               */
 byte huge *f_s,huge *s_s;
 asRecPtr huge *rec = NULL;                                 /* массив указ.  */
 static byte tmp[2048];
                                                            /* на запись     */
 *hdr = (asHeadPtr) getdump (&size,func);                   /* принимаем     */
 if (*hdr==NULL)
 {
  printf ("\n Error in readmemory 1: No enough memory! ");
  abort ();
 }
 rec = (asRecPtr huge *) farcalloc ((*hdr)->numrec-1,sizeof(asRecPtr));
 if (rec==NULL)
 {
  printf ("\n Error in readmemory 2: No enough memory! ");
  abort ();
 }
 f_s = (byte huge *) (*hdr);                                /* 1 половина    */
 s_s = (byte huge *) (*hdr)+65536L;                         /* 2 половина по */
 for (i=0;i<((*hdr)->numrec)-1;i++)                         /* 64 Кбайта     */
 {
  drawprogress ( (double) i / (((*hdr)->numrec) - 1) );

  off = (uint)((*hdr)->addr)[i];                              /* смещение    */
  if ((uint)off>(uint)65534)                      /* нереальный адрес записи */
  {
   rec[i] = (asRecPtr) ((byte huge *)NULL);
   continue;
  }

  if ((uint)off>(uint)32767)                                  /* во 2 пол.   */
   rec[i] = (asRecPtr) ((byte huge *)s_s+((uint)off-32768)*2);/*             */
  else                                                        /* в 1 пол.    */
   rec[i] = (asRecPtr) ((byte huge *)f_s+(uint)off*2);        /*             */

  if (rec[i]->number > 2048)
   rec[i]->number = 2048;
  if ((issignal(rec[i]))&&(rec[i]!=NULL))
  {
   rec[i]->number = rec[i]->dimfft;
   for (n=0;n<(rec[i])->number;n+=2)           /* четные номера */
    tmp[n] = ((byte *)rec[i]->y)[n/2];
   for (n=1;n<(rec[i])->number;n+=2)           /* нечетные номера */
    tmp[n] = ((byte *)rec[i]->y)[(uint)((rec[i])->number+n)/2];
   memcpy ((byte *)rec[i]->y,(byte *)tmp,(size_t)(rec[i])->number);
  }
  else
   if (rec[i]!=NULL)
    rec[i]->mnoj-=3;

  rec[i]->res1 = 0;  /* нулевые байты после имени */

  for (last=7;((last>0)&&((rec[i]->name)[last]==' '));last--)
   ;         /* нашли последнюю букву - не пробел */

  for (;((last>0)&&((rec[i]->name)[last]!=' '));last--)
   ;         /* нашли последний пробел */

  if (last!=0) /* если все же нашли */
   for (j=last;(rec[i]->name)[j]!=0;j++)
    switch ((rec[i]->name)[j])
    {
     case 'a':
     case 'A': ((rec[i]->name)[j])='x';
               break;
     case 'b':
     case 'B': ((rec[i]->name)[j])='y';
               break;
     case 'w':
     case 'W': ((rec[i]->name)[j])='z';
               break;
     default : break;
    }
 }
 drawprogress ((double)1.00);
 return ((asRecPtr huge *)rec);                             /*               */
}                                                           /*               */

byte huge *getdump (long *size,void (* func)(double))
/***********
* Describe : принять дамп памяти прибора в память ЭВМ
* Params   : word *size -  размер принятых данных (в байтах), меньше чем
*          :               размер выделенной памяти, так как он выровнен
*          :               на границу 1024 байта
*          : PFV func    - функция для отображения считывания, ей
*          :               передаем процент считанной информации
* Return   : byte huge*  - указатель на буфер, куда записаны данные
* Call     : input,waitstart,setaddr,getKbyte,bioscom
***********/
{                                                  /* i-текущий адрес        */
 unsigned long volume,i_long;
 uint i=0,Imax=1024;                               /* Imax-макс. адрес       */
 byte empty,bufer[1024];                           /*                        */
 int err=0,rpt=0;                                  /*                        */
 byte huge *ppm=NULL,huge *cur=NULL;               /* указатель на буфер     */
                                                   /*                        */
 bioscom (0,SETTINGS,connect_port);                /* инициализируем COM1    */
 for (i_long=0L;i_long<Imax;i_long+=512L,cur=(byte huge *)cur+1024,i=(uint)i_long)
 {
  if (func!=NULL)                                  /*                        */
   func ((double)((double)i_long/(double)Imax));   /* рисуем прогресс        */
  sleep (1);
  do                                               /*                        */
  {                                                /*                        */
   setaddr(i);                                     /* посылаем адрес начала  */
   err=waitstart();                                /* ждем строки "STRT"     */
   if (err<=8)                                     /* если не ошибка то      */
    err=input(&empty);                             /* принимаем пустой байт  */
   if (err<=8)                                     /* если не ошибка и если  */
    if (i==0)                                      /* первый Кбайт то        */
     err=getKbyte(bufer);                          /* принимаем в буфер      */
    else                                           /* иначе                  */
    {
     err=getKbyte(cur);                            /* принимаем в назначение */
     if (*bufer!=*ppm)
     {
      printf ("\n There is problem, count = %u",(uint)i);
      abort ();
     }
    }
   if (err>8)                                      /* если ошибка            */
   {                                               /*                        */
    rpt++;                                         /* число повторов         */
    if (rpt>REPEATS)                               /*                        */
    {                                              /*                        */
     if (ppm!=NULL)                                /* освобождаем память     */
      farfree ((byte far *)ppm);                   /* если выделена          */
     return (NULL);                                /* возвращаем ошибка      */
    }                                              /*                        */
   }                                               /*                        */
  }                                                /*                        */
  while (err>8);                                   /*                        */
                                                   /*                        */
  if (i==0)                                        /* если первый Кбайт      */
  {                                                /* то                     */
   Imax = (uint)bufer[8]+bufer[9]*256;             /* вычисляем Imax         */
   *size = (long)Imax*2L;                          /* размер в байтах        */
   volume = (unsigned long)(*size)+1024;           /* выделяем 128 Кбайт     */
   ppm=(byte huge*) farcalloc(volume,sizeof(byte));/* выделяем память        */
   cur = ppm;
   if (ppm==NULL)
   {
    printf ("\n Imax = %u, size = %ld",Imax,(*size));
    printf ("\n No enough memory! Function GETDUMP. ");
    printf ("\n %ul bytes free. ",farcoreleft());
    abort ();
   }
   memcpy ((byte *)ppm,bufer,1024);                /* копируем из буфера     */
  }                                                /*                        */
 }                                                 /*                        */
 if (func!=NULL)                                   /*                        */
  func  ((double)1.0);                             /* рисуем прогресс        */
 return ((byte huge*) ppm);                        /*                        */
}                                                  /*                        */
                                                   /*                        */
int input (byte huge *x)                           /*                        */
/***********
* Describe : принять байт из порта COM1
* Params   : byte *x - принятый байт
* Return   : int     - код ошибки
* Call     : bioscom
***********/
{                                                  /*                        */
 int err1=0;                                       /*                        */
 int err=0;                                        /*                        */
 do                                                /*                        */
 {                                                 /*                        */
  err1++;                                          /*                        */
  if (err1>32000)                                  /*                        */
  {                                                /*                        */
   err1=0;                                         /*                        */
   err++;                                          /*                        */
   if (err>9)                                      /*                        */
    return err;                                    /*                        */
  }                                                /*                        */
 }                                                 /*                        */
 while ((bioscom(3,0,connect_port)&0x100)==0);     /* пока не готов          */
 *x = (bioscom(2,0,connect_port))&(0xFF);          /* принимаем байт в млад- */
 return err;                                       /* ших байтах слова       */
}                                                  /*                        */
                                                   /*                        */
int waitstart(void)                                /*                        */
/***********
* Describe : ждет строки "STRT" от прибора
* Params   : void -
* Return   : int  - код ошибки
* Call     : input,strcmp
***********/
{                                                  /*                        */
 char bst[5]={' ',' ',' ',' ',0};                  /*                        */
 byte b;                                           /*                        */
 int i,err=0;                                      /*                        */
                                                   /*                        */
 do                                                /*                        */
 {                                                 /*                        */
  for (i=0;i<3;i++)                                /*                        */
   bst[i]=bst[i+1];                                /*                        */
  err=input(&b);                                   /*                        */
  if (err>5)                                       /*                        */
   return err;                                     /*                        */
  bst[3]=b;                                        /*                        */
 }                                                 /*                        */
 while (strcmp(bst,"STRT"));                       /* ждем строку            */
 return err;                                       /*                        */
}                                                  /*                        */
                                                   /*                        */
void setaddr(uint x)                               /*                        */
/***********
* Describe : устанавливает адрес следующего килобайта памяти для приема
* Params   : word x - адрес в словах (в байтах*2)
* Return   : void
* Call     : bioscom
***********/
{                                                  /*                        */
 byte y,s;                                         /*                        */
 bioscom(1,40,connect_port);                       /*                        */
 bioscom(1,40,connect_port);                       /* сброс порта COM1       */
 bioscom(1,40,connect_port);                       /*                        */
 bioscom(1,'A',connect_port);                      /* передаем A             */
 bioscom(1,'D',connect_port);                      /*          D             */
 y=x % 256;                                        /*                        */
 bioscom(1,y,connect_port);                        /* младший байт адреса    */
 s=y;                                              /*                        */
 y=x >> 8;                                         /*                        */
 s=s+y;                                            /*                        */
 bioscom(1,y,connect_port);                        /* старший байт           */
 s=s&255;                                          /*                        */
 bioscom(1,s,connect_port);                        /* контрольная сумма      */
}                                                  /*                        */
                                                   /*                        */
int getKbyte (byte huge *a)                        /*                        */
/***********
* Describe : принять килобайт памяти от прибора
* Params   : byte *a - буфер приема
* Return   : int     - код ошибки
* Call     : input
***********/
{                                                  /*                        */
 int i,err=0;                                      /*                        */
 for (i=0;i<1024;i++)                              /*                        */
  if (err<=8)                                      /*                        */
   input(a+i);                                     /*                        */
 return err;                                       /*                        */
}                                                  /*                        */

#define BIT7  0x80    /* бит 7 (2^7=128=0x80) */

char *modestr (asRecPtr r)
/***********
* Describe : определение режима
* Params   : asRecPtr r
* Return   : char        - строка-режим
* Call     :
***********/
{
 static char tmp[30];

 switch (r->mode[0])
 {
  case 'a' : if (r->mode[1]=='l')       strcpy (tmp,"inp-Lin ");
             if (r->mode[1]=='z')       strcpy (tmp,"inp-Amp ");
             break;
  case 'o' : if (r->mode[1]=='l')       strcpy (tmp,"Env-Lin ");
             if (r->mode[1]=='z')       strcpy (tmp,"Env-Amp ");
             break;
  case 'd' : strcpy (tmp,"Tacho   ");
             break;
  case 't' : strcpy (tmp,"Test    ");
             break;
  default  : strcpy (tmp,"Unknown ");
  }
             return tmp;
}

int modenum (asRecPtr r)
/***********
* Describe : определение номера режима
* Params   : asRecPtr r
* Return   : int        - номер режима
* Call     :
***********/
{
 int tmp;

 switch (r->mode[0])
 {
  case 'a' : tmp = 1;
             break;
  case 'o' : tmp = 2;
             break;
  case 'd' : return 4;
  case 't' : return 5;
  default  : return -1;
 }
 if (r->mode[1]=='l')
  if (tmp==1)
   return 0;
  else
   return 2;

 if (tmp==1)
  return 1;
 else
  return 3;
}

int gainnum (asRecPtr r)
/***********
* Describe : определение коэфф. усиления
* Params   : asRecPtr r
* Return   : int        - коефф. усиления
* Call     :
***********/
{
 return ceil(((int)r->gain)*.25);
}

int afreqnum (asRecPtr r)
/***********
* Describe : определение средней частоты
* Params   : asRecPtr r
* Return   : int        - средняя частота в Гц
* Call     :
***********/
{
 switch (r->afreq)
 {
  case 0   : return  3100;
  case 64  : return 10000;
  case 128 : return  8000;
  case 134 : return 16000;
  case 166 : return 10000;
  case 192 : return  6300;
  case 198 : return  8000;
  case 230 : return  6300;
  default   : return    -1;
 }
}

char *yunitstr (asRecPtr r)
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/
{
 switch (modenum(r))
 {
  case 4  :
  case 5  :
  case 0  :
  case 2  :  return (" mV ");
  case 1  :
  case 3  :  if (r->yunit)
              return (" mm/s ");
             else
              return (" m/ss ");
  default :  return (" mkm ");
 }
}

int inc_yunit (asRecPtr r)
/***********
* Describe : интегрирование ед. измерения по оси Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
 {
  r->yunit = BIT7;                  /* установили скорость */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->mode[0] = 'm';                 /* установили длину */
  return 0;
 }
 return (1);
}

char *xunitstr (asRecPtr r)
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/
{
 return ( isspectrum(r) ? " Hz " : " msec " );
}

int windnum (asRecPtr r)
/***********
* Describe : определение номера окна (0-прям.,1-Хеннинга)
* Params   : asRecPtr r
* Return   : int        - номер окна
* Call     :
***********/
{
 return (r->wind!=0);
}

int isspectrum (asRecPtr r)
/***********
* Describe : определение спектр? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ((r->type) & BIT7) >> 7 );
}

int issignal (asRecPtr r)
/***********
* Describe : определение сигнал? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ( ((r->type) & BIT7) >> 7 ) == 0);
}

int islinear (asRecPtr r)
/***********
* Describe : определение линейный масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale==0);
}

int islogariphm (asRecPtr r)
/***********
* Describe : определение логарифмический масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale!=0);
}

void setspectrum (asRecPtr r)
/***********
* Describe : установка : спектр
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = BIT7;
}

void setsignal (asRecPtr r)
/***********
* Describe : установка : сигнал
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = 0;
}

void setlinear (asRecPtr r)
/***********
* Describe : установка : линейный масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = 0;
}

void setlogariphm (asRecPtr r)
/***********
* Describe : установка : логарифмический масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = BIT7;
}

int dec_yunit (asRecPtr r)
/***********
* Describe : дифференциирование ед. измерения по оси Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
  return (1);
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->yunit = 0;                     /* установили ускорение */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mkm "))
 {
  r->mode[0] = 'a';                  /* установили скорость */
  r->mode[1] = 's';
  r->yunit = BIT7;
  return (0);
 }
 return (1);
}

void calckoeff (asRecPtr r,float *A,float *B)
/***********
* Describe : подсчет коэффициентов
* Params   : asRecPtr r
*          : float *A   - 1 множ.
*          : float *B   - 2 множ.
* Return   : void
* Call     :
***********/
{
 float Z=0,T=0;

 if (isspectrum(r))      /* если спектр */
  Z = (float) r->mnoj1+r->mnoj;
 else
  Z = (float) r->mnoj1;

 if ((modenum(r)==1)||(modenum(r)==3))  /* если вход усилителя заряда */
  T = (float) r->kdbch;
 if ((modenum(r)==0)||(modenum(r)==2))  /* если линейный вход */
  T = (float) r->kdbln;

 if (issignal(r))    /* если сигнал */
  *B = (float) -128;
 if (isspectrum(r)&&islinear(r))  /* если спектр и масштаб линейный */
  *B = (float) 0;
 if (isspectrum(r)&&islogariphm(r))  /* если спектр и масштаб логарифм. */
  *B = (float) 4*Z+T+14;

 if (islogariphm(r))   /* если масштаб логарифм. */
  *A = (float) .25;
 else             /* если масштаб линейный */
  if ((modenum(r)==1)||(modenum(r)==3))   /* при входе усилителя заряда */
   *A = (float) (7500/(float)r->kofch)*exp(Z*log(10)/20);
  else                    /* при линейном входе */
   *A = (float) (7500/(float)r->kofln)*exp(Z*log(10)/20);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MSG
* Describe  : Определения сообщений
* File      : C:\ALEX\WORK\AS_MSG.C
* Last edit : 30.06.93 00.54.11
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include "as_msg.h"

const char *slMain    =  " F1=Help F2=Trend F3=Record F4=Cascade F5=Print F6=Capture F7=ChDir F8=ChTime AltX=Quit ";
const char *slControl =  " F1=Help F5=Print F6=Capture F7=ChDir F8=ChTime AltX=Quit ";
const char *slRecord  =  " F1=Help F5=Print F6=Capture F7=ChDir F8=ChTime AltX=Quit ";
const char *slCascade =  " F1=Help F5=Print F6=Capture F7=ChDir F8=ChTime AltX=Quit ";
const char *slOptions =  " F1=Help AltX=Quit ";
const char *slPrint   =  " F1=Help AltX=Quit ";
const char *slGraph   =  " AltF5/AltF6=Before/After AltF7=Log/Lin AltF8=Scale AltF9=Spe AltF10/CtrF10=Integ/Diff ";
const char *sl3DGraph =  " F5=Print F6=Capture AltO=Options AltP=Print";
const char *sldbGraph =  " F5=Print F6=Capture AltF5/AltF6=Spe left/right AltO=Options AltP=Print";
const char *slChFtime =  " Ins=Mark +/-=MarkAll *=AutoMark Enter=Load F7=ChDir F8=ChTime Esc=Cancel ";
const char *slChFile  =  " Enter=Load F7=ChDir F8=ChTime Esc=Cancel ";
const char *slChField =  " Enter=Load F7=ChDir F8=ChTime Esc=Break ";
const char *slReceive =  " Tab=Edit Ins=Mark +/-=MarkAll *=AutoMark Enter=Save F7=ChDir F8=ChTime Esc=Break ";

char *msgMaxTime[] ={"                                  ",
                     "  You didn't choose time, so      ",
                     "  will loaded last record.        ",
                     "                                  ",
                     NULL};
char *msgEscape[] ={"                                  ",
                    "   Are You really want to         ",
                    "   delete all remaining records   ",
                    "   without saving to file?        ",
                    "                                  ",
                    NULL};
char *msgNoItems[] ={"                                ",
                     "        Not found items, so     ",
                     "        can't create menu.      ",
                     "                                ",
                     NULL};
char *msgImgSize[] ={"                                      ",
                     "       Size of menu is too large,     ",
                     "       so can't create menu.          ",
                     "                                      ",
                     NULL};
char *msgNoImgMem[] ={"                                           ",
                      "     No enough memory to save screen       ",
                      "     under menu, so can't create menu.     ",
                      "                                           ",
                      NULL};
char *msgChDir[] ={"                                           ",
                   "     Can't cross to specify directory.     ",
                   "     This directory wasn't exist.          ",
                   "                                           ",
                   NULL};
char *msgReceive[] ={"                                           ",
                     "     Make sure that PR90 ready and         ",
                     "     there's notice 'Wait AD'.             ",
                     "     Select <OK> for begin read,           ",
                     "     <Cancel> for break operation.         ",
                     "                                           ",
                     NULL};
char *msgSig[] =    {"                                           ",
                     "     Can't switch to logarithmic           ",
                     "     scale, because current function       ",
                     "     is signal. Only spectrum may be       ",
                     "     switched to logarithmic scale.        ",
                     "                                           ",
                     NULL};
char *msgLog[] =    {"                                           ",
                     "     Can't switch to logarithmic           ",
                     "     scale, because current function       ",
                     "     already is in logarithmic scale.      ",
                     "                                           ",
                     NULL};
char *msgLin[] =    {"                                           ",
                     "     Can't switch to linear scale          ",
                     "     because current function              ",
                     "     already is in linear scale.           ",
                     "                                           ",
                     NULL};
char *msgFFTSpe[] = {"                                           ",
                     "     Can't do FFT to spectrum              ",
                     "     because current function              ",
                     "     already is spectrum.                  ",
                     "                                           ",
                     NULL};
char *msgFFTSig[] = {"                                           ",
                     "     Can't do BackFFT to signal            ",
                     "     because current function              ",
                     "     already is signal.                    ",
                     "                                           ",
                     NULL};
char *msgNoDif[] =  {"                                         ",
                     "     Can't differentiate this function   ",
                     "     because this operation allowed      ",
                     "     only for spectrums of length or     ",
                     "     speed in linear scale.              ",
                     "                                         ",
                     NULL};
char *msgNoInt[]  = {"                                         ",
                     "     Can't integrating this function     ",
                     "     because this operation allowed      ",
                     "     only for spectrums of speed or      ",
                     "     acceleration in linear scale.       ",
                     "                                         ",
                     NULL};

const char *errNoMemory[] = {"                                               ",
                             "  No enough memory.                            ",
                             "  Select any button to abort operation.        ",
                             "  Unload all resident programs and try again.  ",
                             "                                               ",
                             NULL};
const char *errNoFiles[]  = {"                                        ",
                             "  Can't find *.spe files.               ",
                             "  Select any button to abort operation. ",
                             "  Check, are there these files.         ",
                             "                                        ",
                             NULL};
const char *errNoTime[]  =  {"                                        ",
                             "   Can't find time field in file .spe   ",
                             "   Please, check format of file         ",
                             "   and try again.                       ",
                             "                                        ",
                             NULL};
const char *errFormFile[] = {"                                        ",
                             "  Unknown or invalid format of file.    ",
                             "  Select any button to abort operation. ",
                             "  Check, that format of file is right.  ",
                             "                                        ",
                             NULL};
const char *errNoHelp[] = {"                                    ",
                           "    Help file not found.            ",
                           "  File as_help.hlp must be in       ",
                           "  directory from You start program. ",
                           "                                    ",
                           NULL};/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_OTHER
* Describe  :
* File      : C:\ALEX\WORK\AS_OTHER.C
* Last edit : 14.08.93 01.08.49
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_list.h"
#include "as_choos.h"
#include "as_graph.h"
#include "as_msg.h"
#include "m_help.h"
#include "as_other.h"

static void loading (char *fname)
{
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,"\" %12s \"",fname);
}

void loadname (char *fname)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 asRecPtr  rec;
 asFile    file;

 rec = (asRecPtr) farcalloc ((unsigned long)3000,(unsigned long)sizeof(byte));
 if (rec==NULL)
 {
  printf ("\n No enough memory in LOADBYTIME!");
  abort ();
 }
 file.record = rec;
 file.ftime = chooseftime (5,5,fname);
 if (incorrect(&(file.ftime)))
  return;

 setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
           getmaxy()/4+YBUTTONSIZE*4," Loading file ");
 if (load(&file,fname,loading)!=E_OK)
 {
  printf ("\nProblem in function LOAD.");
  abort ();
 }
 killframe();

 statusline((char *)slGraph);

 graphwindow (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,&file);

 farfree ((asRecPtr)rec);
 statusline((char *)slMain);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_PEN
* Describe  : Работа с куpсоpом
* File      : C:\ALEX\WORK\GRAPH\AS_PEN.C
* Last edit : 10.09.93 02.05.46
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <alloc.h>
#include <stdio.h>
#include "as_pen.h"

char far *buffer;       /* бывшее изобpажение                                */
char far *cursor;       /*      маска куpсоpа (гpузится из файла)            */
char far *screen;       /*       маска экрана (гpузится из файла)            */
int x,y;                /* последние кооpдинаты для вывода                   */

void initcursor (char *cursor_name)
/***********
* Describe : инициализиpует куpсоp: загpужает в память из файла обpаз куpсоpа
* Params   : char *cursor_name - имя файла с обpазом куpсоpа
* Return   : void
* Call     :
***********/
{
 FILE *f;
 int dx,dy;

 f = fopen(cursor_name,"rb");   /*                                           */
 fread (&dx,sizeof(int),1,f);   /* читаем pазмеpы обpаза куpсоpа             */
 fread (&dy,sizeof(int),1,f);   /*                                           */
 cursor = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* память           */
 screen = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* память           */
 buffer = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* память           */
 rewind (f);                                             /*                  */
 fread (cursor,sizeof(char),imagesize(0,0,dx,dy),f);     /* читаем обpаз     */
 fread (screen,sizeof(char),imagesize(0,0,dx,dy),f);     /* читаем обpаз     */
 fclose (f);                                             /*                  */
}

void killcursor (void)
/***********
* Describe : уничтожает pанее выведенный куpсоp (восстанавливает изобpажение)
* Params   : void
* Return   : void
* Call     :
***********/
{
 putimage (x,y,buffer,COPY_PUT);         /* восстанавливаем изобpажение      */
}

void setcursor (int x1,int y1)
/***********
* Describe : устанавливает куpсоp
* Params   : int x1 - где поместить
*          : int y1 - куpсоp
* Return   : void
* Call     :
***********/
{
 int xsize,ysize;
 x = x1;                      /* кооpдинаты для                              */
 y = y1;                      /* востановления                               */
 xsize = ((int)cursor[0]);    /* pазмеpы                                     */
 ysize = ((int)cursor[2]);    /* обpаза                                      */
 getimage (x1,y1,x1+xsize,y1+ysize,buffer);
 putimage (x1,y1,screen,AND_PUT);
 putimage (x1,y1,cursor,XOR_PUT);
}

void closecursor (void)
/***********
* Describe : заканчивает pаботу с куpсоpом
* Params   : void
* Return   : void
* Call     :
***********/
{
 farfree (cursor);
 farfree (screen);
 farfree (buffer);
}
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
* Describe : распечатка участка экрана в режиме mode
* Params   : int x1 -
*          : int y1 -  координаты участка
*          : int x2 -  экрана
*          : int y2 -
*          : int mode - режим печати
* Return   : void
* Call     :
***********/
{
 int i,j;
 unsigned char pix_byt,pix_bytini;

 if((mode&0xff)>30)              /*  24 игольчатый printer            */
     {printc (ESC);              /*  установка межстрочного интервала */
     printc ('A');               /*  равного 24 иглам                 */
     printc (24);
     if(mode<255)                   /* Single печать */
         {for (i=y1;i<=y2;i+=24)
              {
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
     printc ('A');               /*  равного 8 иглам                  */
     printc (8);
     for (i=y1;i<=y2;i+=8)
         {
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
 }/*****************************************************************************
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
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_WIND
* Describe  : Функции графического интерфейса
* File      : C:\ALEX\WORK\AS_WIND.C
* Last edit : 17.04.93 00.24.33
* Remarks   :
*****************************************************************************/

int atoi (const char *);
int strlen (const char *);
char *getcwd (char *,int);
void exit (int);
double floor (double);
double ceil  (double);
double sqrt  (double);
double fabs  (double);
char *strcpy (char *,const char *);
char *strcat (char *,const char *);
void far *farcalloc (unsigned long,unsigned long);
void farfree (void far *);
void free (void *);
char *itoa (int,char *,int);
void abort (void);
unsigned long coreleft (void);
long filelength (int);
int getch (void);

#include <stdio.h>
#include <stdarg.h>
#include <graphics.h>
#include <ctype.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_keys.h"
#include "as_list.h"
#include "as_fndir.h"
#include "as_wind.h"

static int  lastx,lasty;                               /* последние: коорд., */
static char lastbuf [BUFLEN];                          /* буфер,             */
static char far *imgbuf;                               /* изображение.       */
static int  imgnum=0;                                  /* номер тек. фрейма  */
extern char far pathto_as[];
extern asHotKey hotkeys[];

void gprintfxy (int x,int y,char *fmt,...)
/***********
* Describe : Форматированный вывод в графике
* Params   : int x     - откуда
*          : int y     - выводить
*          : char *fmt - строка формата (см. printf)
*          : ...       - параметры для вывода
* Return   : void
* Call     :
***********/
{
 va_list ap;                                           /* список арг.        */
 char buffer [BUFLEN];                                 /*                    */
 int x2,y2;                                            /*                    */
                                                       /*                    */
 va_start (ap,fmt);                                    /*                    */
 vsprintf (buffer,fmt,ap);                             /*                    */
 va_end (ap);                                          /*                    */
 x2 = x + textwidth (buffer);                          /*                    */
 y2 = y + textheight (buffer) + YSPACE*2;              /*                    */
 bar (x,y,x2,y2);                                      /* очищаем для вывода */
 outtextxy (x,y,buffer);                               /*                    */
}                                                      /*                    */

void textfon (int x,int y,char *fmt,...)
/***********
* Describe : "Включение" форматированного текста
* Params   : int x     - откуда
*          : int y     - выводить
*          : char *fmt - строка формата (см. printf)
*          : ...       - параметры для вывода
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 va_list ap;                                           /*                    */
 extern  char lastbuf [BUFLEN];                        /*                    */
 extern int lastx,lasty;                               /*                    */
 int x2,y2;                                            /*                    */
                                                       /*                    */
 lastx = x;                                            /* запоминаем где     */
 lasty = y;                                            /* выводили           */
 va_start (ap,fmt);                                    /*                    */
 vsprintf (lastbuf,fmt,ap);                            /*                    */
 va_end (ap);                                          /*                    */
 x2 = x + textwidth (lastbuf);                         /* и что выводили     */
 y2 = y + textheight (lastbuf) + YSPACE*2;             /*                    */
 bar (x,y,x2,y2);                                      /* очищаем место      */
 outtextxy (x,y,lastbuf);                              /*                    */
}                                                      /*                    */

void textfoff (void)
/***********
* Describe : "Выключение" текста, выведенного последней функ. textfon
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 int x2,y2;                                            /*                    */
 extern  char lastbuf [BUFLEN];                        /*                    */
 extern int lastx,lasty;                               /*                    */
                                                       /*                    */
 x2 = lastx + textwidth (lastbuf);                     /* там где выводили   */
 y2 = lasty + textheight (lastbuf) + YSPACE*2;         /*                    */
 bar (lastx,lasty,x2,y2);                              /* рис. прямоугольник */
}                                                      /*                    */

void setframe (int x1,int y1,int x2,int y2,char *title)
/***********
* Describe : Выводит фрейм (рамка,заголовок)
* Params   : int x1      -  координаты левого
*          : int y1      -  верхнего угла
*          : int x2      -  координаты правого
*          : int y2      -  нижнего угла
*          : char *title -  заголовок
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 long imgsz;                                           /*                    */
 extern int lastx,lasty;                               /*                    */
 extern char far *imgbuf;                              /*                    */
 extern int imgnum;                                    /*                    */
 FILE *f;                                              /*                    */
 char fname[100],tmp[20];                              /*                    */

 strcpy (fname,pathto_as);
 strcat (fname,"\\as_image.");
                                                       /*                    */
 imgsz = (long) imagesize(x1,y1,x2,y2);                /*                    */
 imgbuf = (char far *) farcalloc (imgsz,sizeof(char)); /*                    */
 if (imgbuf==NULL)                                     /* сохраняем изобр.   */
 {
  closegraph ();
  printf ("\n Error creating frame: no enough memory. ");
  exit (-1);
 }
 getimage (x1,y1,x2,y2,imgbuf);                        /* если есть память   */
 imgnum++;                                             /*                    */

 strcpy (tmp,itoa(imgnum,tmp,10));                     /*                    */
 strcat (fname,tmp);                                   /*                    */
 f = fopen (fname,"wb");                               /*                    */
 if (f==NULL)
 {
  closegraph ();
  printf ("\n Error open image file %s ",fname);
  printf ("\n Disk full, or write protected.");
  exit (-1);
 }
 fwrite (&x1,sizeof(int),1,f);                         /*                    */
 fwrite (&y1,sizeof(int),1,f);                         /*                    */
 fwrite (imgbuf,sizeof(char),(size_t)imgsz,f);         /*                    */
 fclose (f);                                           /*                    */
 farfree (imgbuf);                                     /*                    */
 setfillstyle (SOLID_FILL,C_F_PAPER);                  /* основное           */
 bar (x1,y1,x2,y2);                                    /*           окно     */
 setcolor (C_F_INK);                                   /*                    */
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);  /* обводим рамкой по  */
 line (x1+XSPACE,y1+textheight(title)+3*YSPACE,        /* краям и подчерк.   */
       x2-XSPACE,y1+textheight(title)+3*YSPACE);       /* заголовок          */

 outtextxy (x1+(x2-x1-textwidth(title)-2*XSPACE)/2,y1+YSPACE,title);
}                                                      /*                    */

void killframe (void)
/***********
* Describe : Уничтожает последний выведенный фрейм
* Params   : void
* Return   : void
* Call     :
***********/
{
 long imgsz;                                           /*                    */
 extern int imgnum;                                    /*                    */
 FILE *f;                                              /*                    */
 char fname[100],tmp[20];                              /*                    */
                                                       /*                    */
 strcpy (fname,pathto_as);
 strcat (fname,"\\as_image.");
 if (imgnum<=0)
  return ;
 strcpy (tmp,itoa(imgnum,tmp,10));                     /*                    */
 strcat (fname,tmp);                                   /*                    */
 f = fopen (fname,"rb");                               /*                    */
 if (f==NULL)
 {
  closegraph ();
  printf ("\n Error open image file %s ",fname);
  exit (-1);
 }
 imgsz = (long) filelength(fileno(f))-sizeof(int)*2;   /*                    */
 imgbuf = (char far *) farcalloc (imgsz,sizeof(char)); /*                    */
 if (imgbuf==NULL)
 {
  closegraph ();
  printf ("\n Error deleting frame: no enough memory. ");
  exit (-1);
 }
 fread (&lastx,sizeof(int),1,f);                       /*                    */
 fread (&lasty,sizeof(int),1,f);                       /*                    */
 fread (imgbuf,sizeof(char),(size_t)imgsz,f);          /*                    */
 fclose (f);                                           /*                    */
 unlink (fname);                                       /*                    */
 imgnum--;                                             /*                    */
 putimage (lastx,lasty,imgbuf,COPY_PUT);               /* последнее изобр.   */
 farfree (imgbuf);                                     /*                    */
}                                                      /*                    */

void setbutton (int x1,int y2,char *buttontext,int active,int inverse)
/***********
* Describe : Выводит кнопку
* Params   : int x1           -  координаты левого
*          : int y2           -  нижнего  угла
*          : char *buttontext -  текст кнопки
*          : int active       -  флаг активности
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 int x2,y1;                                            /*                    */
                                                       /*                    */
 x2 = x1 + XBUTTONSIZE;                                /* правый верхний     */
 y1 = y2 - YBUTTONSIZE;                                /*           угол     */
 if (!inverse)
  if (active)                                           /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_INK);                   /* выбранная кнопка   */
   setcolor (C_B_PAPER);                                /*                    */
  }                                                     /*                    */
  else                                                  /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_PAPER);                 /* норм. кнопка       */
   setcolor (C_B_INK);                                  /*                    */
  }
 else                                                   /*                    */
  if (active)                                           /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_PAPER);                 /* выбранная кнопка   */
   setcolor (C_B_INK);                                  /*                    */
  }                                                     /*                    */
  else                                                  /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_INK);                   /* норм. кнопка       */
   setcolor (C_B_PAPER);                                /*                    */
  }
 bar (x1,y1,x2,y2);                                    /* осн. прям.         */
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);  /* рамка              */
 outtextxy (x1+XSPACE,y1+YSPACE,buttontext);           /*                    */
}                                                      /*                    */

void mainmenu (int y,asChoice *ch)
/***********
* Describe : главное горизонтальное меню программы
* Params   : int y        - высота откуда выводить
*          : asChoice *ch - массив альтернатив
* Return   : void
* Call     : setbutton
***********/
{                                                      /*                    */
 int selected=0,wasselected=0,nchoices=-1,i,changed=1; /*                    */
 char user;                                            /*                    */
                                                       /*                    */
 for (i=0;(ch[i]).text!=NULL;i++)                      /*                    */
 {                                                     /* рисуем меню        */
  setbutton (i*XBUTTONSIZE,y,(ch[i]).text,(selected==i),1);
  nchoices++;                                          /* число альтернатив  */
 }                                                     /*                    */
 wasselected=nchoices;                                 /* была выбрана       */
 do                                                    /*                    */
 {                                                     /*                    */
  changed = 1;                                         /* флаг изменения     */
  user = getch();                                      /* ввод пользователя  */
  switch (user)                                        /*                    */
  {                                                    /*                    */
   case KB_N_LEFT:    wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected--;                     /*                    */
                       if (selected<0)                 /*                    */
                        selected=nchoices;             /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* перескакиваем чер. */
                      break;                           /* запрещенные кнопки */
                                                       /*                    */
   case KB_N_RIGHT:   wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected++;                     /*                    */
                       if (selected>nchoices)          /*                    */
                        selected=0;                    /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* перескакиваем чер. */
                      break;                           /* запрещенные кнопки */
                                                       /*                    */
   case KB_N_ENTER:   if ((ch[selected]).action!=NULL)
                       (ch[selected]).action ();       /* выполняем          */
                      changed = 0;
                      break;
                                                       /*                    */
   default:           for (i=0;hotkeys[i].key!=KB_LAST;i++)
                       if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                        hotkeys[i].action ();
                      changed = 0;                     /* нет изменений      */
                      break;                           /*                    */
  }                                                    /*                    */
  if (!changed)                                        /*                    */
   continue;                                           /*                    */
  setbutton (wasselected*XBUTTONSIZE,y,(ch[wasselected]).text,0,1);
  setbutton (selected*XBUTTONSIZE,y,(ch[selected]).text,1,1);
 }                                                     /* перерисов. 2 кноп. */
 while (1);                                            /* пока не Alt-X      */
}                                                      /*                    */

int submenu (char *title,int x1,int y1,asChoice *ch)
/***********
* Describe : вертикальное подменю
* Params   : char *title  - заголовок
*          : int x1       - левый
*          : int y1       - верхний угол
*          : asChoice *ch - массив альтернатив
* Return   : номер выбранной альтернативы 1..num
* Call     : setframe,killframe,setbutton
***********/
{                                                      /*                    */
 int selected=0,wasselected=0,nchoices=-1,i,x2,y2,changed=1;
 char user;                                            /*                    */
                                                       /*                    */
 for (i=0;(ch[i]).text!=NULL;i++)                      /*                    */
  nchoices++;                                          /* число альтернатив  */
 x2 = x1 + XBUTTONSIZE + 2*XSPACE;                     /*                    */
 y2 = y1 + (nchoices+1)*YBUTTONSIZE + (nchoices+6)*YSPACE + textheight(title);
 setframe (x1,y1,x2,y2,title);                         /*                    */
 for (i=0;i<=nchoices;i++)                             /* рисуем меню        */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(i+1)*(YBUTTONSIZE+YSPACE),
             (ch[i]).text,(selected==i),0);            /*                    */
 wasselected=nchoices;                                 /* была выбрана       */
 do                                                    /*                    */
 {                                                     /*                    */
  changed = 1;                                         /* флаг изменения     */
  user = getch();                                      /* ввод пользователя  */
  switch (user)                                        /*                    */
  {                                                    /*                    */
   case KB_N_UP:      wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected--;                     /*                    */
                       if (selected<0)                 /*                    */
                        selected=nchoices;             /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* перескакиваем чер. */
                      break;                           /* запрещенные кнопки */
                                                       /*                    */
   case KB_N_DOWN:    wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected++;                     /*                    */
                       if (selected>nchoices)          /*                    */
                        selected=0;                    /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* перескакиваем чер. */
                      break;                           /* запрещенные кнопки */
                                                       /*                    */
   case KB_N_ENTER:   if ((ch[selected]).action!=NULL)
                       (ch[selected]).action ();       /* выполняем          */
                      killframe ();
                      return selected;

   default:           for (i=0;hotkeys[i].key!=KB_LAST;i++)
                       if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                       {
                        hotkeys[i].action ();
                        killframe ();
                        return selected;
                       }
                      changed = 0;                     /* нет изменений      */
                      break;                           /*                    */
  }                                                    /*                    */
  if (!changed)                                        /*                    */
   continue;                                           /*                    */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(wasselected+1)*(YBUTTONSIZE+YSPACE),
             (ch[wasselected]).text,0,0);              /* перерис. только    */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(selected+1)*(YBUTTONSIZE+YSPACE),
             (ch[selected]).text,1,0);                 /* 2 измен. кнопки    */
 }                                                     /*                    */
 while (user!=KB_N_ESC);                               /* пока не ESC        */
 killframe ();                                         /*                    */
 return -1;                                            /* Esc - -1           */
}                                                      /*                    */

void reportmemory (int x2,int y1)
/***********
* Describe : сообщить о свободной памяти
* Params   : int x2 - откуда
*          : int y1 - выводить
* Return   : void
* Call     : gprintfxy
***********/
{
 int x1,y2;

 y2 = y1 + YBUTTONSIZE;
 x1 = x2 - XBUTTONSIZE;
 setfillstyle (SOLID_FILL,C_B_INK);
 setcolor (C_B_PAPER);
 bar (x1,y1,x2,y2);
 gprintfxy (x1+XSPACE,y1+YSPACE,"  Free %3d Kb ",(int)floor(coreleft()/1024));
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);
}

void reporttime (int x1,int y1)
/***********
* Describe : сообщить о дате и времени
* Params   : int x1 - откуда
*          : int y1 - выводить
* Return   : void
* Call     :
***********/
{
 char tmp [BUFLEN],buffer [BUFLEN]=" ";
 int x2,y2;
 extern FTIME intime;

 strcpy (tmp,itoa(intime.date.da_day,tmp,10));
 strcat (buffer,tmp);
 strcat (buffer,"/");
 strcpy (tmp,itoa(intime.date.da_mon,tmp,10));
 strcat (buffer,tmp);
 strcat (buffer,"/");
 strcpy (tmp,itoa(intime.date.da_year-1900,tmp,10));
 strcat (buffer,tmp);
 strcat (buffer," ");

 strcpy (tmp,itoa(intime.time.ti_hour,tmp,10));
 strcat (buffer,tmp);
 strcat (buffer,":");
 strcpy (tmp,itoa(intime.time.ti_min,tmp,10));
 strcat (buffer,tmp);

 y2 = y1+ YBUTTONSIZE;
 x2 = x1 + XBUTTONSIZE;
 setfillstyle (SOLID_FILL,C_B_INK);
 setcolor (C_B_PAPER);
 bar (x1,y1,x2,y2);
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);
 outtextxy (x1+XSPACE,y1+YSPACE,buffer);
}

void drawprogress (double pc)
/***********
* Describe : Рисует процент выполнения операции
* Params   : float pc - процент (всегда меньше 1.0)
* Return   : void
* Call     : setframe, killframe
***********/
{
 int x1,y1,x2,y2,xcur;
 char title[]=" Progress operation: ";

 if (pc>1)
  pc=1;
 x1   = getmaxx()/4;
 y1   = getmaxy()/4;
 x2   = x1 + 2*XBUTTONSIZE + XSPACE*8;
 y2   = y1 + textheight(title) + YBUTTONSIZE*2 + YSPACE*7;
 xcur = x1 + floor(2*XBUTTONSIZE*pc) ;
 if (pc==0)
 {
  setframe (x1,y1,x2,y2,title);
  setfillstyle (SOLID_FILL,C_F_INK);
  bar (x1+XSPACE*3,y1+textheight(title)+YSPACE*4,
       x2-XSPACE*3,y1+textheight(title)+YSPACE*5+YBUTTONSIZE);
  setcolor (C_F_PAPER);
  rectangle (x1+XSPACE*3,y1+textheight(title)+YSPACE*4,
             x2-XSPACE*3,y1+textheight(title)+YSPACE*5+YBUTTONSIZE);
 }
 setfillstyle (SLASH_FILL,C_F_PAPER);
 bar (x1+XSPACE*4,  y1+textheight(title)+YSPACE*6,
      xcur+XSPACE*4,y1+textheight(title)+YSPACE*3+YBUTTONSIZE);
 setcolor (C_F_INK);
 setfillstyle (SOLID_FILL,C_F_PAPER);
 gprintfxy (x1+XSPACE*3,y1+textheight(title)+YSPACE*6+YBUTTONSIZE,
            " Done %-2.1f%",pc*100);
 if (pc>=1)
  killframe ();
}

void statusline (char *text)
/***********
* Describe : строка статуса внизу экрана и сообщение о свободной памяти
* Params   : char *text   - сообщение в строке статуса
* Return   : void
* Call     : reportmemory
***********/
{
 int x1,y1,x2,y2;

 x1 = 0;
 y1 = getmaxy()-YBUTTONSIZE;
 x2 = getmaxx()-XBUTTONSIZE-XSPACE;
 y2 = getmaxy();
 setfillstyle (SOLID_FILL,C_B_INK);
 setcolor (C_B_PAPER);
 bar (x1,y1,x2,y2);
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);
 outtextxy (x1+XSPACE,y1+YSPACE,text);
 reportmemory (getmaxx(),y1);
}

int message (char *title,int x1,int y1,char *text[],asChoice ch[2])
/***********
* Describe : сообщение и 2 кнопки
* Params   : char *title    - заголовок
*          : int x1         - откуда
*          : int y1         - выводить
*          : char *text[]   - текст
*          : asChoice ch[2] - кнопки
* Return   : int Ok = 0; !Ok = 1; ESC =-1;
* Call     : setframe, killframe
***********/
{
 int nlines,i,x2,y2,x,Ok=1,changed=1;
 char user;


 x2 = x1 + textwidth(title) + XSPACE*6;
 for (i=0,nlines=0;text[i]!=NULL;i++,nlines++)
 {
  x = x1 + textwidth(text[i]) + XSPACE*6;
  x2 = max(x2,x);
 }
 x = x1+2*(XBUTTONSIZE+XSPACE)+XSPACE*6;
 x2 = max(x2,x);
 y2 = y1 + textheight (title) + YSPACE*5 + YBUTTONSIZE*(nlines+1);

 setframe (x1,y1,x2,y2,title);
 for (i=0;i<nlines;i++)
  outtextxy (x1+XSPACE,y1+textheight (title)+YSPACE*2+i*YBUTTONSIZE,text[i]);

 do
 {
  if (changed)
  {
   setbutton (x1+(x2-x1)/4-XBUTTONSIZE/2,y1+textheight(title)+nlines*YBUTTONSIZE+YSPACE*8,
              (ch[0]).text,Ok,1);
   setbutton (x1+3*(x2-x1)/4-XBUTTONSIZE/2,y1+textheight(title)+nlines*YBUTTONSIZE+YSPACE*8,
              (ch[1]).text,!Ok,1);
  }
  changed = 1;
  user = getch();
  switch (user)
  {
   case KB_N_RIGHT :
   case KB_N_LEFT  : Ok=!Ok;
                     break;

   case KB_N_ENTER : if (ch[!Ok].action!=NULL)
                      ch[!Ok].action ();
                     killframe ();
                     return Ok;

   default         : changed = 0;
                     break;
  }
 }
 while (user!=KB_N_ESC);
 killframe ();
 return (-1);
}

int editstring (int x1,int y1,int len,char *str)
/***********
* Describe : ввод стpоки
* Params   : int x1
*          : int y1
*          : int len
*          : char *str
* Return   : int
* Call     :
***********/
{
 static char buffer[MAX_LEN+1]="",ch;
 int  x2,y2,curpos=0,changed=1;

 strcpy (buffer,str);
 curpos = strlen (buffer);

 setfillstyle (SOLID_FILL,C_EDIT_PAPER);
 setcolor (C_EDIT_INK);

 x2 = x1 + (len+1) * textwidth("_") + XSPACE*2;
 y2 = y1 + YBUTTONSIZE;

 bar (x1,y1,x2,y2);
 outtextxy (x1+XSPACE*2,y1+YSPACE,buffer);

 do
 {
  if (changed)
  {
   bar (x1+XSPACE*2+textwidth(buffer)-textwidth(buffer+curpos-1),y1,x2,y2);
   if (curpos>0)
    outtextxy (x1+XSPACE*2+textwidth(buffer)-textwidth(buffer+curpos-1),
               y1+YSPACE,buffer+curpos-1);
  }

  do
  {
   ch = getch();
   if (ch == 0)
    getch ();
  }
  while (ch == 0);

  changed = 1;
  switch (ch)
  {
   case KB_N_BS   :  if (curpos<1)
                     {
                      changed = 0;
                      break;
                     }
                     curpos--;
                     buffer[curpos] = 0;
                     break;
   case KB_N_ENTER : strcpy (str,buffer);
                     return 0;
   case KB_N_ESC   : return -1;
   default         : if ( (!isascii(ch)) || (curpos>len-1) )
                     {
                      changed = 0;
                      break;
                     }
                     buffer[curpos] = ch;
                     curpos++;
                     buffer[curpos] = 0;
                     break;
  }
 }
 while (1);
}

int editint (int x1,int y1,int digit)
/***********
* Describe : ввод целого числа
* Params   : int x1
*          : int y1
*          : int digit
* Return   : int
* Call     :
***********/
{
 static char buffer[10]="";
 int err;

 strcpy (buffer,itoa(digit,buffer,10));
 err=editstring (x1,y1,2,buffer);
 if (err!=-1)
  return (atoi(buffer));
 else
  return (-1);
}

int editftime (int x1,int y1,FTIME *t)
/***********
* Describe : pедактиpовать вpемя
* Params   : int x1
*          : int y1
*          : FTIME *t
* Return   : int
* Call     :
***********/
{
 int  x2,y2;
 FTIME tmp;

 ftimecpy (&tmp,t);

 x2 = x1 + XLETSZ*40;
 y2 = y1 + YBUTTONSIZE*5;

 setcolor (WHITE);

 setframe (x1,y1,x2,y2," Set internal time: ");

 outtextxy (x1+XLETSZ*4 ,y1+YBUTTONSIZE*2,"Day");
 if ((tmp.date.da_day  = editint (x1+XLETSZ*4,y1+YBUTTONSIZE*3,tmp.date.da_day))==-1)
 {
  killframe ();
  return -1;
 }
 setcolor (BLACK);
 outtextxy (x1+XLETSZ*9,y1+YBUTTONSIZE*3,"/");
 outtextxy (x1+XLETSZ*10,y1+YBUTTONSIZE*2,"Mon");
 if ((tmp.date.da_mon  = editint (x1+XLETSZ*10,y1+YBUTTONSIZE*3,tmp.date.da_mon))==-1)
 {
  killframe ();
  return -1;
 }
 setcolor (BLACK);
 outtextxy (x1+XLETSZ*15,y1+YBUTTONSIZE*3,"/");
 outtextxy (x1+XLETSZ*16 ,y1+YBUTTONSIZE*2,"Year");
 if ((tmp.date.da_year = editint (x1+XLETSZ*16,y1+YBUTTONSIZE*3,tmp.date.da_year-1900)+1900)==-1)
 {
  killframe ();
  return -1;
 }
 setcolor (BLACK);
 outtextxy (x1+XLETSZ*28,y1+YBUTTONSIZE*2,"Hour");
 if ((tmp.time.ti_hour = editint (x1+XLETSZ*28,y1+YBUTTONSIZE*3,tmp.time.ti_hour))==255)
 {
  killframe ();
  return -1;
 }
 setcolor (BLACK);
 outtextxy (x1+XLETSZ*33,y1+YBUTTONSIZE*3,":");
 outtextxy (x1+XLETSZ*34,y1+YBUTTONSIZE*2,"Min");
 if ((tmp.time.ti_min  = editint (x1+XLETSZ*34,y1+YBUTTONSIZE*3,tmp.time.ti_min))==255)
 {
  killframe ();
  return -1;
 }

 killframe ();
 if (incorrect(&tmp))
  return (-1);
 ftimecpy (t,&tmp);
 return 0;
}

void reportdir (int x2,int y1)
/***********
* Describe : сообщить о текущей директории
* Params   : int x2 - откуда
*          : int y1 - выводить
* Return   : void
* Call     : gprintfxy
***********/
{
 int x1,y2;
 char curdir [40];

 y2 = y1 + YBUTTONSIZE;
 x1 = x2 - 50 * textwidth("_");
 setfillstyle (SOLID_FILL,C_B_INK);
 setcolor (C_B_PAPER);
 bar (x1,y1,x2,y2);
 strcpy(curdir,getcwd(curdir,40));
 gprintfxy (x1+XSPACE,y1+YSPACE," Dir: %-40.40s ",curdir);
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : DB_EXCH
* Describe  : функции pаботы (обмена) с файлами БД dBASE
* File      : W:\WORK\DBF\DB_EXCH.C
* Last edit : 18.08.93 00.44.45
* Remarks   : фоpмат dBASE файла см. в "Компьютеp Пpесс" N 1'91 стp.72
*****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include "db_exch.h"

int readfield (FILE *from,dbfField *f)
/***********
* Describe : нижний уpовень pаботы с dBASE файлами,
*          : считывает в f нужное поле в стpоковом фоpмате
* Params   : FILE *from  - dBASE файл, откpыт "rb"
*          : dbfField *f - пеpед вызовом содеpжит имя нужного поля
* Return   : int         - код ошибки
* Call     :
***********/
{
 DBF_HEADER header;
 DBF_DESCRIPTOR far *descr = NULL;
 int full_length,rec_length,i,descr_num,found=0,need;
 unsigned long offset=1;
 char term_byte;

 strupr(f->field_name);                    /* имя поля БД в веpхнем pегистpе */
 rewind(from);                                           /* с начала файла   */
 fread (&header,sizeof(header),sizeof(char),from);       /* читаем заголовок */
 full_length = header.full_length;                       /* полная длина     */
 rec_length  = header.record_length;                     /* длина записи     */
 descr_num = (full_length-sizeof(DBF_HEADER))/sizeof(DBF_DESCRIPTOR);
 descr = farcalloc ((unsigned long) descr_num,sizeof(DBF_DESCRIPTOR));
 if (descr==NULL)
  return (eNoMemory);                               /* нет памяти          */
 fread (descr,descr_num,sizeof(DBF_DESCRIPTOR),from); /* читаем дескpиптоpы  */
 fread (&term_byte,1,sizeof(char),from);                 /* байт-pазделитель */
 if (term_byte!='\r')                     /* должен быть '\r' у dBASE файла  */
 {
  farfree (descr);
  return (eFormFile);                   /* неизвестный фоpмат файла        */
 }
 for (i=0;(i<descr_num)&&(!found);i++)            /* ищем нужное поле        */
 {
  found = (!strcmp(f->field_name,descr[i].field_name));
  if (!found)
   offset+=descr[i].field_length;                /* смещение до нужного поля */
  else
   need = i;                                     /* номеp искомого поля      */
 }
 if (!found)
 {
  farfree (descr);
  return (eNotFound);                                  /* такого поля нет */
 }
 f->field_type       = descr[need].field_type;   /* тип поля                 */
 f->field_length     = descr[need].field_length; /* длина поля               */
 f->field_dec_points = descr[need].dec_points;   /* число цифp после точки   */
 f->rec_num          = header.records_num;       /* число записей в БД       */
 f->rec              = (char far **) farcalloc(f->rec_num,sizeof(char *));
 for (i=0;i<f->rec_num;i++)
 {                                       /* память под поле                  */
  f->rec[i] = (char *) calloc(f->field_length+1,sizeof(char));
  if (f->rec[i]==NULL)
  {
   for (i--;i>=0;i--)
    free (f->rec[i]);                    /* освобождаем pанее выделенную     */
   return (eNoMemory);                 /* нет памяти                       */
  }
  fseek(from,offset,SEEK_CUR);               /* смещаемся до нужного поля    */
  fread((f->rec)[i],f->field_length,1,from); /* читаем из записи             */
   (f->rec)[i][f->field_length] = '\0';      /* конец стpоки                 */
  fseek(from,(long)rec_length-offset-(f->field_length),SEEK_CUR);
 }                                           /* смещаемся до след. записи    */
 farfree (descr);
 return (eOk);                                         /* искомое поле в f */
}

char **getfieldnames (char *fname,char field_type,int *num)
{
 DBF_HEADER header;
 DBF_DESCRIPTOR far *descr = NULL;
 int full_length,i,descr_num,number_found[1024];
 char **result;
 FILE *from;

 if ((from = fopen(fname,"rb"))==NULL)
  return (NULL);

 fread (&header,sizeof(header),sizeof(char),from);       /* читаем заголовок */
 full_length = header.full_length;                       /* полная длина     */
 descr_num = (full_length-sizeof(DBF_HEADER))/sizeof(DBF_DESCRIPTOR);

 descr = farcalloc ((unsigned long) descr_num,sizeof(DBF_DESCRIPTOR));
 if (descr==NULL)
  return (NULL);                                      /* нет памяти          */
 fread (descr,descr_num,sizeof(DBF_DESCRIPTOR),from); /* читаем дескpиптоpы  */

 for (i=0,(*num)=0;i<descr_num;i++)                            /* ищем нужное поле        */
 {
  if (descr[i].field_type==field_type)
  {
   number_found[(*num)]=i;
   (*num)++;  /* число найденных полей */
  }
 }

 if (!(*num))
 {
  farfree (descr);
  return (NULL);                                  /* такого поля нет */
 }

 result = (char **) calloc ((*num),sizeof(char *));
 for (i=0;i<(*num);i++)
 {
  result[i] = (char *) calloc (12,sizeof(char));
  strcpy (result[i],descr[number_found[i]].field_name);
 }

 farfree (descr);
 fclose (from);
 return (result);
}

struct date far *read_dbf_date (FILE *db,char *name,long *num)
/***********
* Describe : читать даты из БД
* Params   : FILE *db   - откуда читать
*          : char *name - имя поля-даты
*          : long *num  - число считанных дат
* Return   : struct date far * - массив дат из БД
* Call     : readfield
***********/
{
 dbfField tmp;
 struct date far *result = NULL;
 int i,j;
 char y_str[5],m_str[3],d_str[3];

 strcpy (tmp.field_name,name);     /* готовим к поиску поля name             */
 if (readfield (db,&tmp)!=0)       /* если нет такого поля или ошибка        */
  return (NULL);
 if (tmp.field_type!='D')          /* если поле типа не дата                 */
  return (NULL);

 *num = tmp.rec_num;               /*  число пpочитанных записей             */
 result = (struct date far *) farcalloc(*num,sizeof(struct date));
 if (result==NULL)
  return (NULL);                   /* нет памяти под массив дат              */
 for (i=0;i<*num;i++)
 {
  for (j=0;j<4;j++)                /* считываем:                             */
   y_str[j] = tmp.rec[i][j];       /*            в y_str - стpоковое         */
  y_str[4] = '\0';                 /*            пpедставление года          */
  for (j=0;j<2;j++)                /*                                        */
   m_str[j] = tmp.rec[i][4+j];     /*            в m_str - стpоковое         */
  m_str[2] = '\0';                 /*            пpедставление месяца        */
  for (j=0;j<2;j++)                /*                                        */
   d_str[j] = tmp.rec[i][6+j];     /*            в d_str - стpоковое         */
  d_str[2] = '\0';                 /*            пpедставление дня           */
  result[i].da_year = atoi (y_str);
  result[i].da_mon  = atoi (m_str);  /* пеpеводим в фоpмат даты              */
  result[i].da_day  = atoi (d_str);
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);                  /* возвpащаем массив дат                  */
}

float far *read_dbf_number (FILE *db,char *name,long *num)
/***********
* Describe : считывает из БД числовые (Numeric) поля
* Params   : FILE *db   - откуда считывать
*          : char *name - имя чилового поля
*          : long *num  - число пpчитанных записей
* Return   : float far * - массив чисел из БД
* Call     : readfield
***********/
{
 dbfField tmp;
 float far *result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* готовим к поиску поля name             */
 if (readfield (db,&tmp)!=0)      /* если нет такого поля или ошибка        */
  return (NULL);
 if (tmp.field_type!='N')         /* если поле не числового типа            */
  return (NULL);

 *num = tmp.rec_num;              /* число пpочитанных записей               */
 result = (float far *) farcalloc(*num,sizeof(float));
 if (result==NULL)
  return (NULL);                  /* нет памяти под массив чисел             */
 for (i=0;i<*num;i++)
 {
  result[i] = atof (tmp.rec[i]);  /* пеpеводим стpоки в фоpмат float         */
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);                 /* возвpащаем массив чисел из БД           */
}

char far **read_dbf_string (FILE *db,char *name,long *num)
{
 dbfField tmp;
 char far **result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* готовим к поиску поля name             */
 if (readfield (db,&tmp)!=0)      /* если нет такого поля или ошибка        */
  return (NULL);
 if (tmp.field_type!='C')
  return (NULL);

 *num = tmp.rec_num;              /* число пpочитанных записей               */
 result = (char far **) farcalloc(*num,sizeof(char *));
 if (result==NULL)
  return (NULL);
 for (i=0;i<*num;i++)
 {
  if ((strlen(tmp.rec[i])==0)||tmp.rec[i]==NULL)
   continue;
  result[i] = calloc (tmp.field_length+1,sizeof(char));
  if (result==NULL)
   return (NULL);
  strcpy (result[i],tmp.rec[i]);
 }
 for (i=0;i<*num;i++)
  free (tmp.rec[i]);
 farfree (tmp.rec);
 return (result);
}

char far *read_dbf_logical (FILE *db,char *name,long *num)
{
 dbfField tmp;
 char far *result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* готовим к поиску поля name             */
 if (readfield (db,&tmp)!=0)      /* если нет такого поля или ошибка        */
  return (NULL);
 if (tmp.field_type!='L')
  return (NULL);

 *num = tmp.rec_num;              /* число пpочитанных записей               */
 result = (char far *) farcalloc(*num,sizeof(char));
 if (result==NULL)
  return (NULL);
 for (i=0;i<*num;i++)
 {
  result[i] = ((tmp.rec[i][0]=='Y')||(tmp.rec[i][0]=='T'));
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : DB_GRAPH
* Describe  : графика с данными из БД
* File      : D:\BC\WORK\DB_GRAPH.C
* Last edit : 12-10-93 08:01:29pm
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "as_ftime.h"
#include "as_mem.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_keys.h"
#include "as_list.h"
#include "as_choos.h"
#include "as_rkey.h"
#include "as_pen.h"
#include "db_exch.h"
#include "as_msg.h"
#include "m_analyz.h"
#include "as_graph.h"
#include "db_graph.h"


extern asHotKey hotkeys[];
extern int russian_font;
extern char pathto_as[];
extern p_x1,p_x2,p_y1,p_y2;

#define LL1  'x'
#define LL2  'y'
#define LL3  'z'

void draw_parameters (int y1,int y2,char *db,char *field_name)
/***********
* Describe : строит график,
* Params   : int y1
*          : int y2
*          : char *db
*          : char *field_name
* Return   : void
* Call     :
***********/
{
 FILE *f;
 asList far *xspes = NULL, far *yspes = NULL, far *zspes = NULL;
 struct date far *time=NULL;
 long far        *time_days=NULL,far *xtime=NULL,far *ytime=NULL,far *ztime=NULL;
 float far       *par=NULL;
 long  num;
 char  user,title[20],*repair=NULL,fname[20],pathto_cursor[80];
 int   i,j,
       changed=0,
       left=1,
       graph_active=GRAPH,
       cursor=2,
       x1,
       x2,
       xpos,
       ypos,
       xlow,
       xhigh,
       ylow,
       yhigh,
       xnum=0,
       ynum=0,
       znum=0;
 float koeff_x,
       koeff_y,
       ynull,
       ymin,
       ymax,
       ptr,
       danger_level,
       break_down_level;

 strcpy (title,db);
 strcat (title," (");
 strcat (title,field_name);
 strcat (title,")");

 x1 = 0;
 x2 = getmaxx();
 xlow  = x1 + SIDE;
 xhigh = x2 - SIDE;
 ylow  = y2 - BOTTOM*3/2;
 yhigh = y1 + TOP;

 if ((f = fopen (db,"rb"))==NULL)
  return;
 drawprogress ((float)0.0);
 if ((time = read_dbf_date (f,"DATE",&num))==NULL||num<3)
  return;
 drawprogress ((float)0.125);
 if ((par  = read_dbf_number (f,field_name,&num))==NULL||num<3)
  return;
 drawprogress ((float)0.25);
  /* считывание комментариев */
 drawprogress ((float)0.375);
 if ((repair = read_dbf_logical (f,"R",&num))==NULL||num<3)
  return;
 drawprogress ((float)0.4);

 danger_level = par[0];
 break_down_level = par[1];

 for (i=2;i<num;i++)
  if (par[i]<=0.0)
  {                                 /* выбрасываем нулевые значения */
   for (j=i+1;j<num;j++)
   {
    par  [j-1] = par[j];
    time [j-1] = time[j];
    repair [j-1] = repair[j];
    if (par[j-1]<=0.0)
    {
     j--;
     num--;
    }
   }
   num--;
  }

 ymin = ymax = par[0];
 for (i=1;i<num;i++)
 {
  ymin = min (ymin,par[i]);
  ymax = max (ymax,par[i]);
 }
 if (ymin>0.0)
  ymin = 0.0;

 if ((time_days = farcalloc (num,sizeof(long)))==NULL)
  abort ();
 drawprogress ((float)0.525);

 for (i=3;i<num;i++)
  time_days[i] = diffdate ((struct date far *)time+2,(struct date far *)time+i);
 drawprogress ((float)0.6);

 strcpy (fname,spefile(db,field_name,LL1));
 xspes = getfilelist (fname);          /*                      */
 if (xspes!=NULL)
 {
  for (xnum=0;strcmp(xspes[xnum].name,"LAST");xnum++)
   ;
  if ((xtime = farcalloc (xnum,sizeof(long)))==NULL)
   abort ();
  for (i=0;i<xnum;i++)
   xtime[i] = diffdate ((struct date far *)time+2,(struct date far *)&(xspes[i].time.date));
 }
 drawprogress ((float)0.7);

 strcpy (fname,spefile(db,field_name,LL2));
 yspes = getfilelist (fname);
 if (yspes!=NULL)
 {
  for (ynum=0;strcmp(yspes[ynum].name,"LAST");ynum++)
   ;
  if ((ytime = farcalloc (ynum,sizeof(long)))==NULL)
   abort ();
  for (i=0;i<ynum;i++)
   ytime[i] = diffdate ((struct date far *)time+2,(struct date far *)&(yspes[i].time.date));
 }
 drawprogress ((float)0.775);

 strcpy (fname,spefile(db,field_name,LL3));
 zspes = getfilelist (fname);
 if (zspes!=NULL)
 {
  for (znum=0;strcmp(zspes[znum].name,"LAST");znum++)
   ;
  if ((ztime = farcalloc (znum,sizeof(long)))==NULL)
   abort ();
  for (i=0;i<znum;i++)
   ztime[i] = diffdate ((struct date far *)time+2,(struct date far *)&(zspes[i].time.date));
 }
 drawprogress ((float)1.00);

 strlwr (title+1);
 strlwr (field_name+1);
 setframe (x1,y1,x2,y2,title);
 p_x1=x1;
 p_x2=x2;
 p_y1=y1;
 p_y2=y2;

 if ((time_days[(int)num-1]==0)||((ymax-ymin)==0))
  goto EXIT;
 koeff_x = (float) (xhigh-xlow)/time_days[(int)num-1];
 koeff_y = (double)(ylow-yhigh)/fabs(ymax-ymin);
 ynull = yhigh + ymax*koeff_y;

 line (xlow,ylow+2,xhigh,ylow+2);   /* ось X */
 gprintfxy (xlow+time_days[2]*koeff_x,ylow+YSPCE,"%02d/%02d/%02d",
            time[2].da_day,time[2].da_mon,time[2].da_year);
 line (xlow+time_days[2]*koeff_x,ylow+2,xlow+time_days[2]*koeff_x,ylow+YSPCE+2);
 gprintfxy (xlow+time_days[(int)num-1]*koeff_x-YDIGIT*4,ylow+YSPCE,"%02d/%02d/%02d",
            time[(int)num-1].da_day,time[(int)num-1].da_mon,time[(int)num-1].da_year);
 line (xlow+time_days[(int)num-1]*koeff_x,ylow+2,xlow+time_days[(int)num-1]*koeff_x,ylow+YSPCE+2);
 /* первая и последняя дата */

 line (xlow-1,ylow+2,xlow-1,yhigh-1);   /* ось Y */
 for (ptr=ymin;ptr<ymax+(ymax-ymin)/10;ptr+=(ymax-ymin)/5)
 {
  line (xlow-1,ynull-ptr*koeff_y,xlow-XSPCE-1,ynull-ptr*koeff_y);
  gprintfxy (x1+XSPCE*2,ynull-ptr*koeff_y-YDIGIT/2,"%6.2f",ptr);
 }

 /* строим уровни опасности: опасный и аварийный                             */

 line (xlow,ynull-danger_level*koeff_y,xhigh,ynull-danger_level*koeff_y);
 outtextxy (xhigh+XSPCE,ynull-danger_level*koeff_y-YSPCE*2,"danger");

 line (xlow,ynull-break_down_level*koeff_y,xhigh,ynull-break_down_level*koeff_y);
 outtextxy (xhigh+XSPCE,ynull-break_down_level*koeff_y-YSPCE*2,"break down");

 /* строим оси внизу графика с отметками имеющихся на диске спектров         */
 /* с соответствующими датами                                                */

 outtextxy (xlow-XSPCE*5,ylow-YDIGIT*2/3+BOTTOM*3/6+2,"(X)");
 line (xlow,ylow+BOTTOM*3/6+2,xhigh,ylow+BOTTOM*3/6+2); /* X spes */
 for (i=0;i<xnum;i++)
  line (xlow+xtime[i]*koeff_x,ylow+BOTTOM*3/6+2,
        xlow+xtime[i]*koeff_x,ylow+BOTTOM*3/6+YSPCE+2);

 outtextxy (xlow-XSPCE*5,ylow-YDIGIT*2/3+BOTTOM*5/6+2,"(Y)");
 line (xlow,ylow+BOTTOM*5/6+2,xhigh,ylow+BOTTOM*5/6+2); /* Y spes */
 for (i=0;i<ynum;i++)
  line (xlow+ytime[i]*koeff_x,ylow+BOTTOM*5/6+2,
        xlow+ytime[i]*koeff_x,ylow+BOTTOM*5/6+YSPCE+2);

 outtextxy (xlow-XSPCE*5,ylow-YDIGIT*2/3+BOTTOM*7/6+2,"(Z)");
 line (xlow,ylow+BOTTOM*7/6+2,xhigh,ylow+BOTTOM*7/6+2); /* Z spes */
 for (i=0;i<znum;i++)
  line (xlow+ztime[i]*koeff_x,ylow+BOTTOM*7/6+2,
        xlow+ztime[i]*koeff_x,ylow+BOTTOM*7/6+YSPCE+2);

 /* строим график параметра используя курсор                                 */

 strcpy (pathto_cursor,pathto_as);
 strcat (pathto_cursor,"\\repair.img");
 initcursor (pathto_cursor);

 moveto (xlow,ynull-par[2]*koeff_y);
 for (i=3;i<num;i++)
 {
  lineto (xlow+time_days[i]*koeff_x,ynull-par[i]*koeff_y);
  if (repair[i])
  {
   setcursor (xlow+time_days[i]*koeff_x,ynull-par[i]*koeff_y);
   i++;
   moveto (xlow+time_days[i]*koeff_x,ynull-par[i]*koeff_y);
  }
 }
 closecursor ();
 strcpy (pathto_cursor,pathto_as);
 strcat (pathto_cursor,"\\pencil.img");
 initcursor (pathto_cursor);

 gprintfxy (x1+XDIGIT*3,y1+YDIGIT*2," %02d/%02d/%02d ",
            time[cursor].da_day,time[cursor].da_mon,time[cursor].da_year-1900);
 gprintfxy (x1+XDIGIT*6,y1+YDIGIT*2,"%s = %6.2f",
            field_name,(float)par[cursor]);
 xpos = x1+XDIGIT*12;
 ypos = y1+YDIGIT*2;

 /* пишем комментарий
 settextstyle (russian_font,HORIZ_DIR,0);
 setusercharsize (1,3,1,4);
 gprintfxy (xpos,ypos,"%40s",comment[cursor]);
 settextstyle (SMALL_FONT,HORIZ_DIR,4); */

 setcursor (xlow+time_days[cursor]*koeff_x,ynull-par[cursor]*koeff_y);

 do
 {
  if ((changed)&&(graph_active==GRAPH))
  {
   killcursor ();
   setcursor (xlow+time_days[cursor]*koeff_x,ynull-par[cursor]*koeff_y);
  }
  else
   if (changed)
   {
    killcursor ();
    setcursor (xlow+time_days[cursor]*koeff_x,ylow+BOTTOM*(2*graph_active+1)/6+2);
   }
  left = changed = 1;
  user=rkey ();
  switch (user)
  {
   case KB_N_UP      :  if (graph_active!=GRAPH)
                         graph_active--;
                        break;
   case KB_N_DOWN    :  if (graph_active!=ZSPE_LINE)
                         graph_active++;
                        break;
   case KB_N_LEFT    :  if (cursor>2)
                         cursor--;
                        break;
   case KB_N_RIGHT   :  if (cursor<num-1)
                         cursor++;
                        break;
   case KB_N_HOME    :  cursor=2;
			break;
   case KB_N_END     :  cursor=(int)num-1;
                        break;
   case KB_N_SPACE   :  if (!changed)
                         break;
                        setcolor (BLACK);
                        gprintfxy (x1+XDIGIT*3,y1+YDIGIT*2," %02d/%02d/%02d ",
                                   time[cursor].da_day,time[cursor].da_mon,time[cursor].da_year-1900);
                        gprintfxy (x1+XDIGIT*6,y1+YDIGIT*2,"%s = %6.2f",
                                   field_name,(float)par[cursor]);
                        xpos = x1+XDIGIT*12;
			ypos = y1+YDIGIT*2;
                        /* пишем комментарий
                        settextstyle (russian_font,HORIZ_DIR,0);
                        setusercharsize (1,3,1,4);
                        gprintfxy (xpos,ypos,"%40s",comment[cursor]);
                        settextstyle (SMALL_FONT,HORIZ_DIR,4); */
                        changed = 0;
                        break;
   case KB_N_ENTER   :  changed = 0;
                        break;

   case KB_A_F6      :  left = 0;
   case KB_A_F5      :  changed = 0;
                        killcursor ();
                        closecursor ();
                        switch (graph_active)
                        {
                         case GRAPH    :
                         case XSPE_LINE:  if (xspes!=NULL)
                                          {
                                           strcpy (fname,spefile(db,field_name,LL1));
					   loadspe (left,fname,
                                                    xnum,xspes,time+cursor);
					  }
                                          break;
                         case YSPE_LINE:  if (yspes!=NULL)
                                          {
                                           strcpy (fname,spefile(db,field_name,LL2));
                                           loadspe (left,fname,
                                                    ynum,yspes,time+cursor);
                                          }
                                          break;
                         case ZSPE_LINE:  if (zspes!=NULL)
                                          {
					   strcpy (fname,spefile(db,field_name,LL3));
                                           loadspe (left,fname,
                                                    znum,zspes,time+cursor);
                                          }
                                          break;
                        }
                        initcursor (pathto_cursor);
                        setcursor (xlow+time_days[cursor]*koeff_x,ylow+BOTTOM*(2*graph_active+1)/6+2);
                        break;
     default          : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                         if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                         {
                          killcursor();
                          closecursor();
                          hotkeys[i].action ();
                          initcursor(pathto_cursor);
                          setcursor (xlow+time_days[cursor]*koeff_x,
                                     ylow+BOTTOM*(2*graph_active+1)/6+2);
			  changed = 0;
			  statusline((char *)sldbGraph);
                         }
			break;

  }
 }
 while (user!=KB_N_ESC);

 killcursor ();
 closecursor ();

EXIT:
 killframe ();

 farfree (par);
 farfree (time);

 farfree (repair);

 if (time_days!=NULL)
  farfree (time_days);

 fclose(f);

 if (xspes!=NULL)
  farfree (xspes);
 if (yspes!=NULL)
  farfree (yspes);
 if (zspes!=NULL)
  farfree (zspes);

 if (xtime!=NULL)
  farfree (xtime);
 if (ytime!=NULL)
  farfree (ytime);
 if (ztime!=NULL)
  farfree (ztime);
}

void loadspe (int left,char *fname,int dir_num,asList far *dir_spes,struct date *now)
/***********
* Describe : загружает ближайший по дате к курсору спектр (x,y,z)
* Params   : int left              - 1-слева,0-справа
*          : char *fname           - имя файла спектра
*          : int dir_num           - число
*          : asList far *dir_spes  - список спектров данного направления
*          : struct date *now      - время на которое указ. курсор
* Return   : void
* Call     : graphwindow,loading,diffdate,load,statusline,setframe,killframe
***********/
{
 asRecord  rec;
 asFile    file;
 int i,res;
 long diff=0;

 file.record = &rec;

 if (left)
  for (i=dir_num-1;((i>=0)&&(ftimecmp((struct date *)&(dir_spes[i].time.date),(struct date *)now)>=0));i--)
   ;
 else
  for (i=0;((i<dir_num)&&(ftimecmp((struct date *)&(dir_spes[i].time.date),(struct date *)now)<=0));i++)
   ;

 LOAD_GRAPH:
 ftimecpy(&(file.ftime),&(dir_spes[i].time));

 setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
	   getmaxy()/4+YBUTTONSIZE*4," Loading file ");
 load(&file,fname,loading);
 killframe();
 statusline((char *)slGraph);
 res=graphwindow (YBUTTONSIZE*4,getmaxy()-YBUTTONSIZE,&file);
 if (res!=0)
 {
  i += res;
  if (i<0)
   i = 0;
  if (i>=dir_num)
   i = dir_num - 1;
  goto LOAD_GRAPH;
 }
 statusline((char *)sldbGraph);
}

char *spefile (char *dbfile,char *field,char dir)
/***********
* Describe : имя .spe-файла по имени файла БД
* Params   : char *dbfile - название БД
*          : char *field  - название поля
*          : char dir     - направление x,y,z
* Return   : char
* Call     :
***********/
{
 static char result[13];
 char tmp[6];
 int i,j;

 strcpy (result,dbfile);
 result [strlen(result)-4]='_';
 result [strlen(result)-3]='\0';

 for (i=0;((i<strlen(field))&&(isdigit(field[i])==0));i++)
  ;
 for (j=0;isdigit(field[i]);j++,i++)
  tmp[j] = field[i];
 tmp[j] = '\0';
 strcat (result,tmp);

 tmp [0] = dir;
 tmp [1] = '\0';
 strcat (result,tmp);
 strcat (result,".spe");

 return (result);
}
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_list.h"
#include "as_choos.h"
#include "db_exch.h"
#include "as_diag.h"
#include "as_graph.h"
#include "db_graph.h"
#include "as_3d.h"
#include "as_msg.h"
#include "as_def.h"
#include "as_main.h"
#include "m_help.h"
#include "m_analyz.h"

extern int help_context;

asChoice mnAnalyze[]  = {{" Trend        ",trend   ,1},
                         {" Record       ",loadbytime,1},
                         {" Cascade      ",cascade     ,1},
                         {NULL            ,NULL      ,0}};

void analyze (void)
{
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_ANALYZE;
 submenu ("  Analyze  ",XBUTTONSIZE,0,mnAnalyze);
 help_context = was_c;
}

void trend (void)
/***********
* Describe : пpосмотp паpаметpов агpегата из БД по агpегату
* Params   : void
* Return   : void
* Call     : choosefile,getfieldnames,submenu,statusline,draw_parameters
***********/
{
 char fname [30],*field_name;
 char **fields=NULL;
 int was_c,num,choosed,i;
 asList far *fields_list=NULL;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_CONTROL;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 statusline((char *)slChFile);

 if (test()==0)
  destroy_program();

 while (choosefile(XSPACE,YSPACE*3,"*.dbf",fname)!=NULL)
 {
  fields = getfieldnames (fname,'N',&num);
  if (num==0)
  {
   return;                   /* нет полей numeric в базе данных */
  }

  fields_list = (asList far *) farcalloc (num+1,sizeof(asList));
  for (i=0;i<num;i++)
  {
   strcpy (fields_list[i].name,fields[i]);
   strcat (fields_list[i].name,diagnoze(fname,fields[i]));
   fields_list[i].mark=0;
  }
  strcpy(fields_list[num].name,"LAST");

  do
  {
   statusline((char *)slChField);
   choosed = grlist3(" Field: ",fields_list,XSPACE,YBUTTONSIZE*2+YDIGIT,1,7);
   if (choosed<1)
    break;
   field_name = fields[choosed-1];
   statusline((char *)sldbGraph);
   draw_parameters (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,fname,field_name);
  }
  while (choosed>=1);
  if (fields_list!=NULL)
   farfree (fields_list);

  for (i=0;i<num;i++)
   free(fields[i]);
  free(fields);
 }

 help_context = was_c;
 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 statusline((char *)slMain);
 return;
}

void loading (char *fname)
{
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,"\" %12s \"",fname);
}

void loadbytime (void)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 asFile    file;
 char fname [30],timestr[30];
 int was_c,res,num,select;
 asList far *spes=NULL;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_RECORD;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;

 if (test()==0)
  destroy_program();

 file.record = (asRecPtr) farcalloc ((unsigned long)3000,(unsigned long)sizeof(byte));
 if (file.record==NULL)
 {
  printf ("\n No enough memory in LOADBYTIME!");
  abort ();
 }

 statusline((char *)slChFile);
 while (choosefile(XSPACE,YSPACE*3,"*.spe",fname)!=NULL)
 {
  spes = getfilelist (fname);
  for (num=0;strcmp(spes[num].name,"LAST");num++)
   ;
  do
  {
   statusline((char *)slChFtime);
   file.ftime=chooseftime(5,5,fname);

   LOAD_GRAPH:
   if (incorrect(&(file.ftime)))
   {
    /* printf ("\n Incorrect ftime"); */
    break;
   }

   for (select=0;ftimecmp(&(spes[select].time),&(file.ftime))!=0;select++)
    ;
   if ((select<0)||(select>=num))
   {
    printf ("\nProblem in function LOAD. #1");
    abort ();
   }

   setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
	     getmaxy()/4+YBUTTONSIZE*4," Loading file ");
   if (load(&file,fname,loading)!=E_OK)
   {
    printf ("\nProblem in function LOAD. #2");
    abort ();
   }
   killframe();
   statusline((char *)slGraph);
   res=graphwindow (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,&file);
   if (res!=0)
   {
    select += res;
    if (select<0)
     select = 0;
    if (select>=num)
     select = num-1;
    ftimecpy (&(file.ftime),&(spes[select].time));
    goto LOAD_GRAPH;
   }
  }
  while (!incorrect(&(file.ftime)));
  if (spes!=NULL)
   farfree (spes);
 }
 if (file.record!=NULL)
  farfree ((asRecPtr) file.record);

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 help_context = was_c;
 statusline((char *)slMain);
}

void cascade (void)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 FTIME  far * ftimes;
 asFile far * files;
 char fname [30];
 int was_c,num,i,j;
 int hfreq,type,scale,number;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_TRENDS;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;

 if (test()==0)
  destroy_program();

 statusline((char *)slChFile);
 if (choosefile(XSPACE,YSPACE*3,"*.spe",fname)==NULL)
  return;

 statusline((char *)slChFtime);
 ftimes = chooseftimes (5,5,fname,&num);
 if (ftimes==NULL || num<2)
  return;
 files = (asFile far *) farcalloc ((unsigned long)num,
				  (unsigned long)sizeof(asFile));
 if (files==NULL)
 {
  printf ("\n No enough memory in TREND!");
  abort();
 }

 for (i=0;i<num;i++)
 {
  (files[i]).record = (asRecPtr) farcalloc (1L,3000L);
  if ((files[i]).record==NULL)
  {
   printf ("\n No enough memory in TREND for records!");
   abort();
  }
  ftimecpy (&((files[i]).ftime),&(ftimes[i]));
  if (incorrect(&((files[i]).ftime)))
   return;
  if (load(&(files[i]),fname,NULL)!=E_OK)
  {
   printf ("\nProblem in function TREND of loading!");
   abort ();
  }

  if (i==0)
  {
   hfreq  = files[0].record->hfreq;
   type   = files[0].record->type;
   scale  = files[0].record->scale;
   number = files[0].record->number;
  }
  else
   if (hfreq !=files[i].record->hfreq  ||
       type  !=files[i].record->type   ||
       scale !=files[i].record->scale  ||
       number!=files[i].record->number)
    {
     num--;
     for (j=i;j<num;j++)
      ftimecpy (&((files[j]).ftime),&(ftimes[j+1]));
     i--;
     continue;
    }
  drawprogress ((double)i/num);
 }
 drawprogress ((double)1.00);
 farfree (ftimes);

 if (num>1)
 {
  statusline((char *)sl3DGraph);
  graph3D (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,files,num);
 }

 for (i=0;i<num;i++)
  farfree ((files[i]).record);
 farfree (files);

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 help_context = was_c;
 statusline((char *)slMain);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_FILE
* Describe  : меню FILE пpогpаммы СпектpоАнализатоp ПР90
* File      : C:\ALEX\WORK\M_FILE.C
* Last edit : 30.06.93 00.08.05
* Remarks   :
*****************************************************************************/

#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <graphics.h>
#include <ctype.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_list.h"
#include "as_check.h"
#include "as_choos.h"
#include "as_fndir.h"
#include "as_graph.h"
#include "as_3d.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_quit.h"
#include "as_main.h"
#include "db_exch.h"
#include "db_graph.h"
#include "as_wind.h"
#include "as_diag.h"
#include "m_file.h"

extern int help_context;
extern asChoice mnStd[2];
extern asHotKey hotkeys[];
extern FTIME intime;

asChoice mnFile[]     = {{" Receive      ",receive    ,1},
			 {" Change dir   ",changedir  ,1},
			 {" Change time  ",changetime ,1},
			 {" OS shell     ",osshell    ,1},
			 {" Copy         ",rcopy      ,0},
			 {" Delete       ",rdelete    ,0},
			 {" Quit         ",beforequit ,1},
			 {NULL             ,NULL       ,0}};

void file (void)
/***********
* Describe : пункт главного меню FILE
* Params   : void
* Return   : void
* Call     : подменю
***********/
{
 int was_c;
 was_c = help_context;
 help_context = CONTEXT_FILE;
 submenu ("  File  ",0,0,mnFile);
 help_context = was_c;
}

void rcopy (void)
{
 asFile    file_from,file_to;
 char from[30],to[30];
 int was_c;

 was_c = help_context;
 help_context = 0;

 file_from.record = (asRecPtr) farcalloc ((unsigned long)3000,(unsigned long)sizeof(byte));
 if (file_from.record==NULL)
 {
  printf ("\n No enough memory in COPY!");
  abort ();
 }

 if (choosefile(XSPACE,YSPACE*3,"*.spe",from)!=NULL)
 {
  file_from.ftime=chooseftime(5,5,from);
  if (incorrect(&(file_from.ftime)))
  {
   /* printf ("\n Incorrect ftime"); */
   goto EXIT;
  }
 }
 else
  goto EXIT;

 if (choosefile(XSPACE,YSPACE*3,"*.spe",to)!=NULL)
 {
  editftime (getmaxx()/4,getmaxy()/4,&(file_to.ftime));
  if (incorrect(&(file_to.ftime)))
  {
   /* printf ("\n Incorrect ftime"); */
   goto EXIT;
  }
 }
 goto EXIT;

 if (copy(&file_from,&file_to,from,to)!=E_OK)
 {
  printf ("\nProblem in function COPY. #1");
  abort ();
 }

 EXIT:
 if (file_from.record!=NULL)
  farfree ((asRecPtr) file_from.record);
 help_context = was_c;
 statusline((char *)slMain);
}

void rdelete (void)
{
 FTIME ftime;
 char fname[30];
 int was_c;

 was_c = help_context;
 help_context = 0;

 if (choosefile(XSPACE,YSPACE*3,"*.spe",fname)!=NULL)
 {
  ftime=chooseftime(5,5,fname);
  if (incorrect(&ftime))
  {
   /* printf ("\n Incorrect ftime"); */
   return;
  }
 }
 else
  return;

 if (delete(fname,ftime,NULL)!=E_OK)
 {
  printf ("\nProblem in function DELETE. #1");
  abort ();
 }

 help_context = was_c;
 statusline((char *)slMain);
}

void saving (char *fname)
{
 setcolor (BLACK);
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2," %12s ",fname);
}

void receive (void)
/***********
* Describe : пpием инфоpмации из пpибоpа и запись ее в файлы (специфициpуя
*          : диpектоpию-агpегат/цех/завод и вpемя записи)
* Params   : void
* Return   : void
* Call     : message,readmemory,setframe,gprintfxy,killframe,statusline,
*          : grlist,ftimecpy,ftimeinc,save
***********/
{
 asRecPtr huge *rec = NULL;
 asHeadPtr hdr = NULL;
 asFile    file;
 asList    *list = NULL;
 int i,user,remains,was_context;
 char tmp[20];

 was_context = help_context;
 help_context = CONTEXT_RECEIVE;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[4].able=0;
 hotkeys[5].able=0;

 i = message (" Warning: ",getmaxx()/4,getmaxy()/4,msgReceive,mnStd);
 if ((i==0)||(i==-1))
  return;

 rec = (asRecPtr huge *) readmemory (&hdr,drawprogress);

 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
	   getmaxy()/4+YBUTTONSIZE*5," Received information: ");
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,
	    " Volume information: %6.3f Kbytes ",(float)(hdr->volume)/512);
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*3,
	    " Number of records:  %u records  ",(uint)(hdr->numrec)-1);
 getch ();
 killframe ();

 list = (asList *) calloc (hdr->numrec,sizeof(asList));

 for (i=0;i<hdr->numrec-1;i++)
 {
  strcpy(list[i].name,"  ");
  strcpy(tmp,itoa(i+1,tmp,10));
  strcat(list[i].name,tmp);
  strcat(list[i].name,". ");
  strcat(list[i].name,(char *)rec[i]->name);
 }
 strcpy(list[hdr->numrec-1].name,"LAST");
 remains = hdr->numrec-1;

 while (remains>0)
 {
  statusline((char *)slReceive);

  user = grlist2(" Mark records to save: ",list,XSPACE,YDIGIT,4,10);

  if (user==menuEscape)
  {
   i=message(" Verify: ",getmaxx()/3,getmaxy()/4,msgEscape,mnStd);
   if (i==1)
    remains=0;
   continue;
  }

  changetime ();
  changedir  ();
  reportdir (getmaxx(),YBUTTONSIZE);

  setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
	    getmaxy()/4+YBUTTONSIZE*4," Saving file ");
  for (i=0;i<hdr->numrec-1;i++)
   if ((list[i].mark)&&(rec[i]!=NULL))
   {
    strcpy(rec[i]->name,(char *)memchr(list[i].name,'.',10)+2);
    file.record=rec[i];
    ftimecpy (&(file.ftime),&intime);
    ftimeinc (&(file.ftime),i);
    file.ftime.time.ti_hund = 1;     /* номер версии */
    save (&file,saving);

    strcpy(tmp,itoa(i+1,tmp,10));
    strcat(tmp,".( saved )");
    strcpy(list[i].name,"  ");
    strcat(list[i].name,tmp);
    (list[i]).name[0]=1;

    list[i].mark=0;
    remains--;
   }
  killframe ();
 }

 if (list!=NULL)
  free ((asList *) list);
 if (hdr!=NULL)
  farfree ((byte far *) hdr);
 if (rec!=NULL)
  farfree ((asRecPtr *) rec);

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[4].able=1;
 hotkeys[5].able=1;

 help_context = was_context;
 statusline((char *)slMain);
}

void changedir (void)
/***********
* Describe : смена текущей диpектоpии
* Params   : void
* Return   : void
* Call     :
***********/
{
 static char path[MAX_LEN];
 int err,j;
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_CHDIR;
 statusline((char *)slMain);

 getcwd(path,MAX_LEN);
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
	   getmaxy()/4+YBUTTONSIZE*4," Enter full path to dir: ");
 err=editstring (getmaxx()/4+XDIGIT,getmaxy()/4+YBUTTONSIZE*2,MAX_LEN,path);
 killframe ();

 if (err)
 {
  help_context = was_c;
  return;
 }

 for (j=0;path[j]!='\0';j++)
  if (path[j]==':')
   setdisk (tolower(path[j-1])-'a');

 err = chdir (path);
 if (err)
  message(" Error: ",getmaxx()/4,getmaxy()/4,msgChDir,mnStd);
 else
  reportdir (getmaxx(),YBUTTONSIZE);
 help_context = was_c;
 statusline((char *)slMain);
}

void changetime (void)
/***********
* Describe : смена текущего внутpеннего вpемени (котоpое используется
*          : пpи записи в файлы пpинятой инфоpмации)
* Params   : void
* Return   : void
* Call     :
***********/
{
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_CHTIME;
 statusline((char *)slMain);
 editftime (getmaxx()/4,getmaxy()/4,&intime);
 reporttime (getmaxx()-XBUTTONSIZE,0);
 help_context = was_c;
 statusline((char *)slMain);
}

void osshell (void)
/***********
* Describe : вpеменный выход в ДОС
* Params   : void
* Return   : void
* Call     :
***********/
{
 char cmdln [50];
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_OSSHELL;
 closegraph ();
 printf ("\nFor return to Spectrum Analyzer type EXIT ... ");
 strcpy (cmdln,searchpath("command.com"));
 system (cmdln);
 help_context = was_c;
 main (1,(char **)NULL);
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_HELP
* Describe  : Контекстная помощь для пpогpаммы СпектpоАнализатоp
* File      : C:\ALEX\WORK\M_HELP.C
* Last edit : 02.07.93 02.28.55
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_main.h"
#include "m_help.h"

int help_context = CONTEXT_ABOUT;
extern char far pathto_as[];
extern asChoice mnStd[2];

asChoice mnHelp[]     = {{"  Context   ",contexthelp,0},
                         {"  Topic     ",NULL       ,0},
                         {NULL          ,NULL       ,0}};

asChoice mnUpDn[2] = {{"    Next    ",next    ,1},
                      {"  Previous  ",previous,1}};

#define STRLEN   101                    /* число символов в стpоке help'а    */
#define STRNUM   11                     /* число стpок help'а                */

void help (void)
/***********
* Describe :
* Return   : void
* Call     :
***********/
{
 help_context = CONTEXT_HELP;
 submenu (" Help ",XBUTTONSIZE*4,0,mnHelp);
}

void contexthelp (void)
/***********
* Describe : контекстная помощь
* Params   : void использует пеpеменную help_context
* Return   : void
* Call     : message, next, previous
***********/
{
 FILE *fhelp=NULL;
 char help_title[STRLEN],far *help_text[STRNUM];
 int x1=20,y1=20,i,was_c;
 extern int russian_font;

 was_c = help_context;
 help_context = CONTEXT_CONTEXT;

 settextstyle (russian_font,HORIZ_DIR,0);
 setusercharsize (1,3,1,4);
 for (i=0;i<STRNUM-1;i++)
  help_text[i] = (char far *) farcalloc (STRLEN,sizeof(char));
 help_text[STRNUM-1]=NULL;

 fhelp = fopen(fullpath("as_help.hlp"),"rb");
 if (fhelp==NULL)
 {
  message (" Error: ",x1,y1,(char **)errNoHelp,mnStd);
  goto EXIT;
 }

 do
 {
  fseek (fhelp,help_context*STRLEN*STRNUM,SEEK_SET);
  fread (help_title,sizeof(char),STRLEN,fhelp);
  help_title[STRLEN-1]=0;
  for (i=0;i<STRNUM-1;i++)
  {
   fread (help_text[i],sizeof(char),STRLEN,fhelp);
   help_text[i][STRLEN-1]=0;
  }
 }
 while (message(help_title,x1,y1,help_text,mnUpDn)!=-1);

 EXIT: fclose(fhelp);
       for (i=0;i<STRNUM-1;i++)
        farfree(help_text[i]);
       settextstyle (SMALL_FONT,HORIZ_DIR,4);
      help_context = was_c;
}

void next (void)
/***********
* Describe : увеличить help_context на 1
* Params   : void
* Return   : void
* Call     :
***********/
{
 if (help_context<MAX_CONTEXT)
  help_context++;
}

void previous (void)
/***********
* Describe : уменьшить help_context на 1
* Params   : void
* Return   : void
* Call     :
***********/
{
 if (help_context>0)
  help_context--;
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_OPTION
* Describe  :
* File      : C:\ALEX\WORK\M_OPTION.C
* Last edit : 30.06.93 01.07.21
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_graph.h"
#include "as_rkey.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_option.h"

enum  asWindType   weightwindow;
extern int help_context;
extern int GIPX,GIPY,N,M,K,L,PICK;
extern double MRS_BEG,MRS_END;

asChoice mnOptions[]  = {{" Window type    ",windowtype   ,1},
			 {" 3-D Params     ",set_gips     ,1},
                         {" Port           ",set_port     ,1},
                         {" Approx         ",set_approx   ,1},
                         {" Graph          ",set_graph    ,1},
                         {" Save config    ",save_config  ,1},
                         {" View config    ",view_config  ,1},
                         {NULL              ,NULL         ,0}};

asChoice mnWind[]     = {{"  Rectangular   ",NULL ,1},
                         {"  Bartlett      ",NULL ,1},
                         {"  Parzen        ",NULL ,1},
                         {"  Hanning       ",NULL ,1},
                         {"  Hamming       ",NULL ,1},
                         {"  Blackman      ",NULL ,1},
                         {NULL              ,NULL ,0}};

asChoice mnGraph[]   = {{" Allow pick   ",NULL ,1},
                        {" Forbid pick  ",NULL ,1},
                        {" RMS limits   ",NULL ,1},
                        {NULL            ,NULL ,0}};

asChoice mnCPort[]   = {{"  COM 1       ",NULL ,1},
                        {"  COM 2       ",NULL ,1},
                        {NULL            ,NULL ,0}};

void options (void)
{
 int was_c;
 extern asHotKey hotkeys[];

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[4].able=0;
 hotkeys[5].able=0;
 hotkeys[6].able=0;
 hotkeys[7].able=0;
 hotkeys[8].able=0;
 statusline((char *)slOptions);

 was_c = help_context;
 help_context = CONTEXT_OPTIONS;

 submenu ("  Options  ",XBUTTONSIZE*2,0,mnOptions);

 help_context = was_c;
 statusline((char *)slMain);
 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[4].able=1;
 hotkeys[5].able=1;
 hotkeys[6].able=1;
 hotkeys[7].able=1;
 hotkeys[8].able=1;

}

void windowtype (void)
{
 int was_c,j;
 was_c = help_context;
 help_context = CONTEXT_WINDOWTYPE;
 j = submenu (" Window type ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnWind);
 switch (j)
 {
  case REC: weightwindow = REC;
            break;
  case BAR: weightwindow = BAR;
            break;
  case PAR: weightwindow = PAR;
            break;
  case HAN: weightwindow = HAN;
            break;
  case HAM: weightwindow = HAM;
            break;
  case BLC: weightwindow = BLC;
            break;
 }
 help_context = was_c;
}

void set_gips (void)
{
 int was_c;
 was_c = help_context;
 help_context = CONTEXT_3DPARAMS;
 setframe (40,50,340,110," Enter your GIPX and GIPY: ");
 GIPX = editint(70,80,GIPX);
 GIPY = editint(160,80,GIPY);
 killframe();
 if (GIPX<0 || GIPX>100)
  GIPX = 30;
 if (GIPY<0 || GIPY>100)
  GIPX = 20;
 help_context = was_c;
}

void set_port (void)
{
 int j,was_c;
 extern int connect_port;

 was_c = help_context;
 help_context = CONTEXT_PORT;
 j=submenu (" Connect to: ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE*2,mnCPort);
 if (j>-1)
  connect_port=j;
 help_context = was_c;
}

void set_approx (void)
{
 int was_c;
 extern int N,K,M,L;

 was_c = help_context;
 help_context = CONTEXT_PORT;
 setframe (40,50,340,110," Enter your N, K, M, L: ");
 N = editint(100,80,N);
 K = editint(160,80,K);
 M = editint(220,80,M);
 L = editint(280,80,L);
 if (N<2||N>10)
  N = 3;
 if (K<1)
  K = 2;
 if (M<2)
  M = 3;
 if (L<1)
  L = 1725;
 killframe();

 help_context = was_c;
}

void set_graph (void)
{
 int was_c,j;
 extern int PICK;
 extern double MRS_BEG,MRS_END;

 was_c = help_context;
 help_context = CONTEXT_PORT;

 j = submenu (" Graph sets ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnGraph);
 switch (j)
 {
  case 0: PICK = 1;
          break;
  case 1: PICK = 0;
	  break;
  case 2: editfloat2 (&MRS_BEG,&MRS_END);
          if (MRS_BEG<0)
           MRS_BEG = 0;
          if (MRS_END<0)
           MRS_END = 0;
          break;
 }
 help_context = was_c;
}

void save_config (void)
{
 FILE *config=NULL;
 extern int connect_port,port,mode,GIPX,GIPY;
 extern double MRS_BEG,MRS_END;
 extern enum asWindType weightwindow;

 config = fopen ("anlzr.cfg","wb");
 if (config!=NULL)
 {
  fwrite (&connect_port,sizeof(int)       ,1,config);
  fwrite (&port        ,sizeof(int)       ,1,config);
  fwrite (&mode        ,sizeof(int)       ,1,config);
  fwrite (&weightwindow,sizeof(int)       ,1,config);
  fwrite (&GIPX        ,sizeof(int)       ,1,config);
  fwrite (&GIPY        ,sizeof(int)       ,1,config);
  fwrite (&N           ,sizeof(int)       ,1,config);
  fwrite (&K           ,sizeof(int)       ,1,config);
  fwrite (&M           ,sizeof(int)       ,1,config);
  fwrite (&L           ,sizeof(int)       ,1,config);
  fwrite (&PICK        ,sizeof(int)       ,1,config);
  fwrite (&MRS_BEG     ,sizeof(double)    ,1,config);
  fwrite (&MRS_END     ,sizeof(double)    ,1,config);
  fclose (config);
 }
 else
 {
  printf ("\n Problem in function SAVE_CONFIG. Disk full ?");
  abort ();
 }
}

void view_config (void)
{
 extern int connect_port,port,mode,GIPX,GIPY,N,M,K,L,PICK;
 extern double MRS_BEG,MRS_END;
 extern enum asWindType weightwindow;
 int x1,y1,x2,y2,cur_y=1;
 char tmp1[30],tmp2[30];

 x1 = XBUTTONSIZE;
 y1 = YBUTTONSIZE*3;
 x2 = x1 + XBUTTONSIZE*4;
 y2 = y1 + (YDIGIT+YSPACE*4)*12;

 setframe (x1,y1,x2,y2," Current options: ");

 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*4)*cur_y ,
            " PR90 connection port____%s ",connect_port ? " COM2 " : " COM1 ");

 switch (port)
 {
  case 0 : gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Printer port____________LPT1    ");
           break;
  case 1 : gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Printer port____________LPT2    ");
           break;
  case 2 : gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
		      " Printer port____________LPT3    ");
           break;
  default: gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Printer port____________Unknown ");
           break;
 }

 switch (mode)
 {
  case 0 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
		      " Quality of print________Single         ");
            break;
  case 1 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________Double 1       ");
            break;
  case 2 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________Double 2       ");
            break;
  case 3 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________Quater         ");
            break;
  case 4 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________CRT Graphic  I ");
            break;
  case 5 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________Plottergraphic ");
            break;
  case 6 :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________CRT Graphic II ");
            break;
  default:  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Quality of print________Unknown        ");
            break;
 }

 switch (weightwindow)
 {
  case REC :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Rectangular ");
            break;
  case BAR :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Bartlett    ");
            break;
  case PAR :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Parzen      ");
	    break;
  case HAN :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Hanning     ");
            break;
  case HAM :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
		      " Weight window________Hamming     ");
            break;
  case BLC :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Blackman    ");
            break;
  default  :  gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
                      " Weight window________Unknown     ");
            break;
 }

 strcpy (tmp1,itoa(GIPX,tmp1,10));
 strcpy (tmp2,itoa(GIPY,tmp2,10));
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " 3D Displ by axis's : 0X__%s ; 0Y__%s ",tmp1,tmp2);

 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " Last measures for approx : ___%d ",N);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " Jump : _______________________%d ",K);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
	    " Last measures for average : __%d ",M);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " Actual days : ________________%d ",L);
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " Pick : ________________%s ",PICK?"ON":"OFF");
 gprintfxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
            " Total limits : ______________%7.3f to %7.3f ",MRS_BEG,MRS_END);

 while (rkey()!=27);
 killframe ();
}/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_PRINT
* Describe  :
* File      : C:\ALEX\WORK\M_PRINT.C
* Last edit : 30.06.93 01.05.35
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_print.h"

extern int help_context;
int port;
int mode;

asChoice mnPrint[]    = {{"  Length     ",quality ,1},
                         {"  Height     ",height  ,1},
                         {"  Port       ",setport ,1},
                         {NULL           ,NULL    ,0}};

asChoice mnQual[]    = {{"  8dot 270mm  ",NULL,1},
                        {"  8dot 225mm  ",NULL,1},
                        {"  8dot 205mm Q",NULL,1},
                        {"  8dot 180mm Q",NULL,1},
                        {"  8dot 135mm  ",NULL,1},
                        {"  8dot 135mm Q",NULL,1},
                        {"  8dot  70mm  ",NULL,1},
                        {" 24dot 270mm  ",NULL,1},
                        {" 24dot 180mm  ",NULL,1},
                        {" 24dot 135mm  ",NULL,1},
                        {" 24dot  90mm  ",NULL,1},
                        {" 24dot  45mm  ",NULL,1},
                        {NULL            ,NULL,0}};

asChoice mnHeig[]    = {{"  Single      ",NULL,1},
                        {"  Double      ",NULL,1},
                        {NULL            ,NULL,0}};

asChoice mnPort[]    = {{"  LPT1   ",NULL,1},
                        {"  LPT2   ",NULL,1},
                        {"  LPT3   ",NULL,1},
                        {NULL       ,NULL,0}};

void print (void)
{
 int was_c;
 extern asHotKey hotkeys[];
 was_c = help_context;
 help_context = CONTEXT_PRINT;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[4].able=0;
 hotkeys[5].able=0;
 hotkeys[6].able=0;
 hotkeys[7].able=0;
 hotkeys[8].able=0;
 statusline((char *)slPrint);

 submenu (" Print ",(int)XBUTTONSIZE*3,0,mnPrint);

 statusline((char *)slMain);
 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[4].able=1;
 hotkeys[5].able=1;
 hotkeys[6].able=1;
 hotkeys[7].able=1;
 hotkeys[8].able=1;

 help_context = was_c;
}

void quality (void)     /* установка длины печати, мл байт mode */
{
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_QUALITY;
 j=submenu (" Length  ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE,mnQual);
 mode=mode&0xf00;
 switch(j)
  {case   0:        mode+=0; break;
  case    1:        mode+=5; break;
  case    2:        mode+=4; break;
  case    3:        mode+=6; break;
  case    4:        mode+=1; break;
  case    5:        mode+=2; break;
  case    6:        mode+=3; break;
  case    7:        mode+=32; break;
  case    8:        mode+=38; break;
  case    9:        mode+=33; break;
  case    10:       mode+=39; break;
  case    11:       mode+=40; break;
  default:          mode+=1;
  }
 help_context = was_c;
}

void height (void)      /* установка высоты печати 0-single, 1-double */
{                       /* код в ст. байте mode                       */
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_QUALITY;
 j=submenu (" Height  ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*2,mnHeig);
 if(j>=0)	mode=(mode&0xff)+(j<<8);
 help_context = was_c;
}

void setport (void)
{
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_PORT;
 j=submenu (" Output to: ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*3,mnPort);
 if (j>-1)
  port=j;
 help_context = was_c;
}
/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_QUIT
* Describe  :
* File      : C:\ALEX\WORK\M_QUIT.C
* Last edit : 02.07.93 02.45.48
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include <graphics.h>
#include <process.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "m_help.h"
#include "m_quit.h"

extern int help_context;

/*     asChoice mnQuit[] = {{"     Yes     ",quit ,1},
                     {"     No      ",NULL ,1},
                     {NULL           ,NULL ,0}};        */


void beforequit (void)
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<30;i++)
  killframe ();
 closegraph ();
 exit (0);
}

/*void beforequit (void)
***********                           исключено
* Describe :
* Params   : void
* Return   : void
* Call     :
***********
{
 int tmp;
 tmp = help_context;
 help_context = CONTEXT_QUIT;
 submenu (" Are You sure? ",XBUTTONSIZE*5,0,mnQuit);
 help_context = tmp;
}                                                         *//*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : SA_MEM
* Describe  : Эмуляция обмена с прибором и работа с памятью
* File      : C:\ALEX\WORK\SA_MEM.C
* Last edit : 15.04.93 00.29.51am
* Remarks   : Принятая информация может быть больше 64 Кбайт, для адресации
*           : такого массива используются два указателя, на два разных
*           : сегмента.
*           : Максимальное число отсчетов сигнала - 2048 байт
*****************************************************************************/

void abort (void);
void sleep (unsigned int);

#include <stdio.h>                                 /*                        */
#include <bios.h>                                  /*                        */
#include <string.h>                                /*                        */
#include <alloc.h>                                 /*                        */
#include <math.h>
#include "as_mem.h"                                /*                        */

FILE *fout;
void drawprogress (double);
int connect_port=0;

asRecPtr huge *readmemory (asHeadPtr *hdr,void (* func)(double))
/***********
* Describe : Функция чтения памяти высокого уровня
* Params   : asHeadPtr *hdr - указ. на указ. на asHeader
*          : void (* func)(double) - функция для отображения считывания, ей
*          :                         передаем процент считанной информации
* Return   : asRecPtr  *    - массив указателей на записи в памяти
* Call     : getdump,numrec,addr
***********/
{                                                           /*               */
 uint last,i,j,n,off;                                       /*               */
 long size;                                                 /*               */
 byte huge *f_s,huge *s_s;
 asRecPtr huge *rec = NULL;                                 /* массив указ.  */
 static byte tmp[2048];
                                                            /* на запись     */
 *hdr = (asHeadPtr) getdump (&size,func);                   /* принимаем     */
 if (*hdr==NULL)
 {
  printf ("\n Error in readmemory 1: No enough memory! ");
  abort ();
 }
 rec = (asRecPtr huge *) farcalloc ((*hdr)->numrec-1,sizeof(asRecPtr));
 if (rec==NULL)
 {
  printf ("\n Error in readmemory 2: No enough memory! ");
  abort ();
 }
 f_s = (byte huge *) (*hdr);                                /* 1 половина    */
 s_s = (byte huge *) (*hdr)+65536L;                         /* 2 половина по */
 for (i=0;i<((*hdr)->numrec)-1;i++)                         /* 64 Кбайта     */
 {
  drawprogress ( (double) i / (((*hdr)->numrec) - 1) );

  off = (uint)((*hdr)->addr)[i];                              /* смещение    */
  if ((uint)off>(uint)65534)                      /* нереальный адрес записи */
  {
   rec[i] = (asRecPtr) ((byte huge *)NULL);
   continue;
  }

  if ((uint)off>(uint)32767)                                  /* во 2 пол.   */
   rec[i] = (asRecPtr) ((byte huge *)s_s+((uint)off-32768)*2);/*             */
  else                                                        /* в 1 пол.    */
   rec[i] = (asRecPtr) ((byte huge *)f_s+(uint)off*2);        /*             */

  if (rec[i]->number > 2048)
   rec[i]->number = 2048;
  if ((issignal(rec[i]))&&(rec[i]!=NULL))
  {
   rec[i]->number = rec[i]->dimfft;
   for (n=0;n<(rec[i])->number;n+=2)           /* четные номера */
    tmp[n] = ((byte *)rec[i]->y)[n/2];
   for (n=1;n<(rec[i])->number;n+=2)           /* нечетные номера */
    tmp[n] = ((byte *)rec[i]->y)[(uint)((rec[i])->number+n)/2];
   memcpy ((byte *)rec[i]->y,(byte *)tmp,(size_t)(rec[i])->number);
  }
  else
   if (rec[i]!=NULL)
    rec[i]->mnoj-=3;

  rec[i]->res1 = 0;  /* нулевые байты после имени */

  for (last=7;((last>0)&&((rec[i]->name)[last]==' '));last--)
   ;         /* нашли последнюю букву - не пробел */

  for (;((last>0)&&((rec[i]->name)[last]!=' '));last--)
   ;         /* нашли последний пробел */

  if (last!=0) /* если все же нашли */
   for (j=last;(rec[i]->name)[j]!=0;j++)
    switch ((rec[i]->name)[j])
    {
     case 'a':
     case 'A': ((rec[i]->name)[j])='x';
               break;
     case 'b':
     case 'B': ((rec[i]->name)[j])='y';
               break;
     case 'w':
     case 'W': ((rec[i]->name)[j])='z';
               break;
     default : break;
    }
 }
 drawprogress ((double)1.00);
 return ((asRecPtr huge *)rec);                                  /*               */
}                                                           /*               */

byte huge *getdump (long *size,void (* func)(double))
/***********
* Describe : принять дамп памяти прибора в память ЭВМ
* Params   : word *size -  размер принятых данных (в байтах), меньше чем
*          :               размер выделенной памяти, так как он выровнен
*          :               на границу 1024 байта
*          : PFV func    - функция для отображения считывания, ей
*          :               передаем процент считанной информации
* Return   : byte huge*  - указатель на буфер, куда записаны данные
* Call     : input,waitstart,setaddr,getKbyte,bioscom
***********/
{                                                  /* i-текущий адрес        */
 unsigned long volume,i_long;
 uint i=0,Imax=1024;                               /* Imax-макс. адрес       */
 byte bufer[1024];                                 /*                        */
 int err=0,rpt=0;                                  /*                        */
 byte huge *ppm=NULL,huge *cur=NULL;               /* указатель на буфер     */
                                                   /*                        */
 fout=fopen("memory.bin","rb");
 if (fout==NULL)
 {
  printf ("\n Can't found file memory.bin. "
          "\n Try find it in current directory. ");
  abort ();
 }
 for (i_long=0L;i_long<Imax;i_long+=512L,cur=(byte huge *)cur+1024,i=(uint)i_long)
 {
  if (func!=NULL)                                  /*                        */
   func ((double)((double)i_long/(double)Imax));   /* рисуем прогресс        */
  do                                               /*                        */
  {                                                /*                        */
   if (err<=8)                                     /* если не ошибка и если  */
    if (i==0)                                      /* первый Кбайт то        */
     err=getKbyte(bufer);                          /* принимаем в буфер      */
    else                                           /* иначе                  */
    {
     err=getKbyte(cur);                            /* принимаем в назначение */
     if (*bufer!=*ppm)
     {
      printf ("\n There is problem, count = %u",(uint)i);
      abort ();
     }
    }
   if (err>8)                                      /* если ошибка            */
   {                                               /*                        */
    rpt++;                                         /* число повторов         */
    if (rpt>REPEATS)                               /*                        */
    {                                              /*                        */
     if (ppm!=NULL)                                /* освобождаем память     */
      farfree ((byte far *)ppm);                   /* если выделена          */
     return (NULL);                                /* возвращаем ошибка      */
    }                                              /*                        */
   }                                               /*                        */
  }                                                /*                        */
  while (err>8);                                   /*                        */
                                                   /*                        */
  if (i==0)                                        /* если первый Кбайт      */
  {                                                /* то                     */
   Imax=(uint) bufer[8]+bufer[9]*256;              /* вычисляем Imax         */
   *size=(long) Imax*2L;                           /* размер в байтах        */
   volume=(unsigned long) (*size)+1024L;           /* выравненный по 1024 б  */
   ppm=(byte huge*) farcalloc(volume,sizeof(byte));/* выделяем память        */
   cur = ppm;
   if (ppm==NULL)
   {
    printf ("\n Imax = %u, size = %ld",Imax,(*size));
    printf ("\n No enough memory! Function GETDUMP. ");
    printf ("\n %ul bytes free. ",farcoreleft());
    abort ();
   }
   memcpy ((byte *)ppm,bufer,1024);                /* копируем из буфера     */
  }                                                /*                        */
 }                                                 /*                        */
 if (func!=NULL)                                   /*                        */
  func  ((double)1.0);                             /* рисуем прогресс        */
 fclose (fout);
 return ((byte huge*) ppm);                        /*                        */
}                                                  /*                        */
                                                   /*                        */
int input (byte huge *x)                           /*                        */
/***********
* Describe : принять байт из порта COM1
* Params   : byte *x - принятый байт
* Return   : int     - код ошибки
* Call     : bioscom
***********/
{                                                  /*                        */
 fread (x,sizeof(byte),1,fout);
 return 0;
}                                                  /*                        */
                                                   /*                        */
int waitstart(void)                                /*                        */
/***********
* Describe : ждет строки "STRT" от прибора
* Params   : void -
* Return   : int  - код ошибки
* Call     : input,strcmp
***********/
{                                                  /*                        */
 return 0;
}                                                  /*                        */
                                                   /*                        */
void setaddr(uint x)                               /*                        */
/***********
* Describe : устанавливает адрес следующего килобайта памяти для приема
* Params   : word x - адрес в словах (в байтах*2)
* Return   : void
* Call     : bioscom
***********/
{                                                  /*                        */
 return;
}                                                  /*                        */
                                                   /*                        */
int getKbyte (byte huge *a)                        /*                        */
/***********
* Describe : принять килобайт памяти от прибора
* Params   : byte *a - буфер приема
* Return   : int     - код ошибки
* Call     : input
***********/
{                                                  /*                        */
 fread (a,sizeof(byte),1024,fout);
 return 0;
}                                                  /*                        */

#define BIT7  0x80    /* бит 7 (2^7=128=0x80) */

char *modestr (asRecPtr r)
/***********
* Describe : определение режима
* Params   : asRecPtr r
* Return   : char        - строка-режим
* Call     :
***********/
{
 static char tmp[30];

 switch (r->mode[0])
 {
  case 'a' : strcpy (tmp,"");
             break;
  case 'o' : strcpy (tmp,"Envlp-");
             break;
  case 'd' : strcpy (tmp,"Impulses");
             return tmp;
  case 't' : strcpy (tmp,"Test");
             return tmp;
  default  : strcpy (tmp,"Unknown");
             return tmp;
 }
 if (r->mode[1]=='l')
  strcat (tmp,"Linear");
 else
  strcat (tmp,"Amplifier");
 return tmp;
}

int modenum (asRecPtr r)
/***********
* Describe : определение номера режима
* Params   : asRecPtr r
* Return   : int        - номер режима
* Call     :
***********/
{
 int tmp;

 switch (r->mode[0])
 {
  case 'a' : tmp = 1;
             break;
  case 'o' : tmp = 2;
             break;
  case 'd' : return 4;
  case 't' : return 5;
  default  : return -1;
 }
 if (r->mode[1]=='l')
  if (tmp==1)
   return 0;
  else
   return 2;

 if (tmp==1)
  return 1;
 else
  return 3;
}

int gainnum (asRecPtr r)
/***********
* Describe : определение коэфф. усиления
* Params   : asRecPtr r
* Return   : int        - коефф. усиления
* Call     :
***********/
{
 return ceil(((int)r->gain)*.25);
}

int afreqnum (asRecPtr r)
/***********
* Describe : определение средней частоты
* Params   : asRecPtr r
* Return   : int        - средняя частота в Гц
* Call     :
***********/
{
 switch (r->afreq)
 {
  case 0   : return  3100;
  case 64  : return 10000;
  case 128 : return  8000;
  case 134 : return 16000;
  case 166 : return 10000;
  case 192 : return  6300;
  case 198 : return  8000;
  case 230 : return  6300;
  default   : return    -1;
 }
}

char *yunitstr (asRecPtr r)
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/
{
 switch (modenum(r))
 {
  case 4  :
  case 5  :
  case 0  :
  case 2  :  return (" mV ");
  case 1  :
  case 3  :  if (r->yunit)
              return (" mm/s ");
             else
              return (" m/ss ");
  default :  return (" mkm ");
 }
}

int inc_yunit (asRecPtr r)
/***********
* Describe : интегрирование ед. измерения по оси Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
 {
  r->yunit = BIT7;                  /* установили скорость */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->mode[0] = 'm';                 /* установили длину */
  return 0;
 }
 return (1);
}

char *xunitstr (asRecPtr r)
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/
{
 return ( isspectrum(r) ? " Hz " : " msec " );
}

int windnum (asRecPtr r)
/***********
* Describe : определение номера окна (0-прям.,1-Хеннинга)
* Params   : asRecPtr r
* Return   : int        - номер окна
* Call     :
***********/
{
 return (r->wind!=0);
}

int isspectrum (asRecPtr r)
/***********
* Describe : определение спектр? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ((r->type) & BIT7) >> 7 );
}

int issignal (asRecPtr r)
/***********
* Describe : определение сигнал? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ( ((r->type) & BIT7) >> 7 ) == 0);
}

int islinear (asRecPtr r)
/***********
* Describe : определение линейный масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale==0);
}

int islogariphm (asRecPtr r)
/***********
* Describe : определение логарифмический масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale!=0);
}

void setspectrum (asRecPtr r)
/***********
* Describe : установка : спектр
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = BIT7;
}

void setsignal (asRecPtr r)
/***********
* Describe : установка : сигнал
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = 0;
}

void setlinear (asRecPtr r)
/***********
* Describe : установка : линейный масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = 0;
}

void setlogariphm (asRecPtr r)
/***********
* Describe : установка : логарифмический масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = BIT7;
}

int dec_yunit (asRecPtr r)
/***********
* Describe : дифференциирование ед. измерения по оси Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
  return (1);
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->yunit = 0;                     /* установили ускорение */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mkm "))
 {
  r->mode[0] = 'a';                  /* установили скорость */
  r->mode[1] = 's';
  r->yunit = BIT7;
  return (0);
 }
 return (1);
}

void calckoeff (asRecPtr r,float *A,float *B)
/***********
* Describe : подсчет коэффициентов
* Params   : asRecPtr r
*          : float *A   - 1 множ.
*          : float *B   - 2 множ.
* Return   : void
* Call     :
***********/
{
 float Z=0,T=0;

 if (isspectrum(r))      /* если спектр */
  Z = (float) r->mnoj1+r->mnoj;
 else
  Z = (float) r->mnoj1;

 if ((modenum(r)==1)||(modenum(r)==3))  /* если вход усилителя заряда */
  T = (float) r->kdbch;
 if ((modenum(r)==0)||(modenum(r)==2))  /* если линейный вход */
  T = (float) r->kdbln;

 if (issignal(r))    /* если сигнал */
  *B = (float) -128;
 if (isspectrum(r)&&islinear(r))  /* если спектр и масштаб линейный */
  *B = (float) 0;
 if (isspectrum(r)&&islogariphm(r))  /* если спектр и масштаб логарифм. */
  *B = (float) 4*Z+T+14;

 if (islogariphm(r))   /* если масштаб логарифм. */
  *A = (float) .25;
 else             /* если масштаб линейный */
  if ((modenum(r)==1)||(modenum(r)==3))   /* при входе усилителя заряда */
   *A = (float) (7500/(float)r->kofch)*exp(Z*log(10)/20);
  else                    /* при линейном входе */
   *A = (float) (7500/(float)r->kofln)*exp(Z*log(10)/20);
}
