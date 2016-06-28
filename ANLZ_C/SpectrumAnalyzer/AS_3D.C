/*****************************************************************************
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
*          : int num              число записей
* Return   : void
* Call     :
***********/
{
 extern double AUTO_BEG,AUTO_END;
 int i,x_a,x_b,y_a,y_b,x2,xlow1,xhigh1,ylow1,yhigh1,xlow2,xhigh2,ylow2,
     x1=0,number;
 int cursor=0,changed=1,n,cur_file=0,changed_y=1;
 double koeffx,koeffx2,koeffy,koeffz_x,koeffz_y,maxy,miny,minx=0,
	ynull,y,j,koeff_a,koeff_b,ynull_b,KOEFY=1.0;
 char user,xunit[10],yunit[10],tmp[30];
 float far *far *msr=NULL,A,B,maxx,auto_end;

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

 number = ((files[0]).record)->number;
 strcpy (xunit,xunitstr(files[0].record));
 strcpy (yunit,yunitstr(files[0].record));
                                            /* max по оси x */
 maxx = isspectrum(files[0].record) ? (float)files[0].record->hfreq:
                              (float)number/(files[0].record->hfreq*2.56);

 miny=maxy=0.0;
 if (issignal(files[0].record))    /* если сигнал */
 {
  for (n=0;n<num;n++)           /* находим max по всем записям */
  for (i=0;i<number;i++)        /* находим max по всем значениям в записи */
  {
   if(maxy<fabs((double)msr[n][i]))  maxy=fabs((double)msr[n][i]);
  }
  miny=-maxy;                   /* вверх и вниз симметричн.график */
 }
 else                        /* если спектр */
 {
  auto_end=AUTO_END*number/maxx;       /* max только в диапазоне */
  if(auto_end>number)  auto_end=number; /* перевод диапазона в дискреты из Гц */
  for (n=0;n<num;n++)                  /* находим max по всем записям */
  for (i=AUTO_BEG*number/maxx;i<auto_end;i++)
  {
   if(maxy<msr[n][i])  maxy=msr[n][i];
  }
 }

DRAWING:
 if (maxy==miny)   maxy+=1;
 koeffx = (double)(xhigh1-xlow1)/fabs(maxx-minx);
 koeffy = (double)(ylow1-yhigh1)/fabs(maxy-miny);
 koeffz_x = ((double)GIPX/(num-1));
 koeffz_y = ((double)GIPY/(num-1));
 koeffx2 = (double)(xhigh1-xlow1)/number;
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
 {                                /* рисование и разметка оси х */
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
                                 /* рисование и разметка оси y */
 for (y=miny;y<=(double)maxy+fabs(maxy-miny)/4;y+=fabs(maxy-miny)/2)
 {
  line (xlow1-XSPACE,ynull-y*koeffy,xlow1-XSPACE*4,ynull-y*koeffy);
  gprintfxy (x1+XDIGIT/2,ynull-y*koeffy-YDIGIT/2,"%7.3f",(double)y*KOEFY);
 }
 gprintfxy (x1+XDIGIT,yhigh1-YDIGIT*2,"%5.5s",yunit);

 for (n=0;n<num;n++)               /* цикл рисования по записям */
 {
  ynull = yhigh1 + maxy*koeffy + koeffz_y*n;	/* нулевая линия n-го графика в дискретах */
  for (i=0;i<number;i++) 	                /* по точкам в кажд записи */
  {                                         /* затирание занимаемого места */
   setcolor (WHITE);
   line (xlow1+n*koeffz_x+i*koeffx2,ylow1+koeffz_y*n-1,
		 xlow1+n*koeffz_x+i*koeffx2,ynull-((msr[n][i]>maxy ? maxy : (msr[n][i]>miny ? msr[n][i] : miny))*koeffy));
  }
  setcolor(BLACK);                          /* рисование граф. по затертому */
  moveto (xlow1+n*koeffz_x,ynull-((msr[n][0]>maxy ? maxy : (msr[n][0]>miny ? msr[n][0] : miny))*koeffy));
  for (i=0;i<number;i++)
   lineto (xlow1+n*koeffz_x+i*koeffx2,ynull-((msr[n][i]>maxy ? maxy : (msr[n][i]>miny ? msr[n][i] : miny))*koeffy));
 }

 gprintfxy (x1+XDIGIT*3,yhigh1-YDIGIT*2," X=            %5.5s",xunit);
 gprintfxy (x1+XDIGIT*9,yhigh1-YDIGIT*2," Y=            %5.5s",yunit);

 setfillstyle (SOLID_FILL,WHITE);
 bar (x_a,y_a,x_b,y_b);
 rectangle (x_a-2,y_a+2,x_b+2,y_b-2);		/* рисов. правого окна */
 ftimetoa (&(files[cur_file].ftime),tmp);
 gprintfxy (x_a,y_a+(YDIGIT+YSPACE*3),"D: %s",tmp);
 initcursor (fullpath("cursor.img"));

 do
 {
  if (changed)
  {
   killcursor ();
	 setcursor ((int)(xlow1+cur_file*koeffz_x+cursor*koeffx2),
				(int)(yhigh1+maxy*koeffy+koeffz_y*cur_file-(msr[cur_file][cursor]>maxy ? maxy : (msr[cur_file][cursor]>miny ? msr[cur_file][cursor] : miny))*koeffy));

   bar (x_a,y_a,x_b,y_b);		/*стирание правого графика */
   moveto (x_a,ynull_b-(msr[0][cursor]>maxy ? maxy : (msr[0][cursor]>miny ? msr[0][cursor] : miny))*koeff_b);
   for (n=0;n<num;n++)                /* рисование правого графика */
	lineto (x_a+n*koeff_a,ynull_b-(msr[n][cursor]>maxy ? maxy : (msr[n][cursor]>miny ? msr[n][cursor] : miny))*koeff_b);
   line (x_a+cur_file*koeff_a,y_a,
		 x_a+cur_file*koeff_a,ynull_b-(msr[cur_file][cursor]>maxy ? maxy : (msr[cur_file][cursor]>miny ? msr[cur_file][cursor] : miny))*koeff_b);
									  /* печать значен курсора */

   gprintfxy (x1+XDIGIT*4,yhigh1-YDIGIT*2,"%11.5f",(float)cursor*koeffx2/koeffx);
   gprintfxy (x1+XDIGIT*10,yhigh1-YDIGIT*2,"%11.5f",(float)msr[cur_file][cursor]*KOEFY);

   if (changed_y)
   {
	ftimetoa (&(files[cur_file].ftime),tmp);
	gprintfxy (x_a,y_a+(YDIGIT+YSPACE*3),"D: %s  ",tmp);
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

   case KB_N_ENTER   :  infowindow ((x1+x2)/2,y1+YDIGIT,files[cur_file].record);
						changed = 0;
						break;

   case KB_A_F8      :  maxy *= KOEFY;
						editfloat (&maxy,yunit);  /* изменение масшт */
						maxy /= KOEFY;
						killcursor ();
						closecursor ();
						killframe ();
						if (issignal(files[0].record))
						miny = -maxy;
						goto DRAWING;
   case KB_N_PGUP    :   maxy*=2;
						 miny*=2;
						killcursor ();
						closecursor ();
						killframe ();
						goto DRAWING;
   case KB_N_PGDN    :   maxy/=2;
						 miny/=2;
						killcursor ();
						closecursor ();
						killframe ();
						goto DRAWING;
   case KB_A_F9      :  if (isspectrum(files[0].record))      /* БПФ             */
						 break;
						for(i=0;i<num;i++)
						 {
						  setspectrum (files[i].record);
						  doFFT (msr[i],&(files[i].record->number));
						 }
						cursor = 1;
						killcursor ();
						closecursor ();
						killframe ();
						goto SCALING;

   case KB_A_F10     :  for (i=0;i<num;i++)
						 if(doIntegrate (msr[i],files[i].record)) break;
						if(i!=num)	break;	/* если нельзя интегрировать */
						killcursor();       /* выход */
						closecursor();
						killframe();
						goto SCALING;
   case KB_C_F10     :  for (i=0;i<num;i++)
						 if(doDiff (msr[i],files[i].record)) break;
						if(i!=num)	break;	/* если нельзя дифферинцировать */
						killcursor();       /* выход */
						closecursor();
						killframe();
						goto SCALING;

   default           :  for (i=0;hotkeys[i].key!=KB_LAST;i++)
                         if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                         {
                          killcursor();
                          closecursor();
                          hotkeys[i].action ();
													initcursor(fullpath("cursor.img"));
													setcursor ((int)(xlow1+cur_file*koeffz_x+cursor*koeffx2),
									 (int)(yhigh1+maxy*koeffy+koeffz_y*cur_file-msr[cur_file][cursor]*koeffy));
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
}