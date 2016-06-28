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
/*			 xpos,
			 ypos,*/
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
/* drawprogress ((float)0.125);*/
 if ((par  = read_dbf_number (f,field_name,&num))==NULL||num<3)
  return;
/* drawprogress ((float)0.25);*/
	/* считывание комментариев */
/* drawprogress ((float)0.375);*/
 if ((repair = read_dbf_logical (f,"R",&num))==NULL||num<3)
  return;
/* drawprogress ((float)0.4);*/

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
 drawprogress ((float)0.5);
 for (i=3;i<num;i++)
  time_days[i] = diffdate ((struct date far *)time+2,(struct date far *)time+i);
/* drawprogress ((float)0.6);*/

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
/* drawprogress ((float)0.7);*/

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
/* drawprogress ((float)0.775);*/

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
 gprintfxy (xlow+(time_days[num-1]-time_days[2])/2*koeff_x,ylow+YSPCE,"период %ld дней",
						time_days[(int)num-1]);
 /* первая и последняя дата */

 line (xlow-1,ylow+2,xlow-1,yhigh-1);   /* ось Y */
 for (ptr=ymin;ptr<ymax+(ymax-ymin)/10;ptr+=(ymax-ymin)/5)
 {
  line (xlow-1,ynull-ptr*koeff_y,xlow-XSPCE-1,ynull-ptr*koeff_y);
  gprintfxy (x1+XSPCE*2,ynull-ptr*koeff_y-YDIGIT/2,"%6.2f",ptr);
 }

 /* строим уровни опасности: опасный и аварийный                             */

 line (xlow,ynull-danger_level*koeff_y,xhigh,ynull-danger_level*koeff_y);
 outtextxy (xhigh+XSPCE,ynull-danger_level*koeff_y-YSPCE*2,"Warning");

 line (xlow,ynull-break_down_level*koeff_y,xhigh,ynull-break_down_level*koeff_y);
 outtextxy (xhigh+XSPCE,ynull-break_down_level*koeff_y-YSPCE*2,"Danger");

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
	 setcursor ((int)(xlow+time_days[i]*koeff_x),(int)(ynull-par[i]*koeff_y));
   if((i+1)<num)
	 moveto (xlow+time_days[i+1]*koeff_x,ynull-par[i+1]*koeff_y);
  }
 }
 closecursor ();
 strcpy (pathto_cursor,pathto_as);
 strcat (pathto_cursor,"\\cursor.img");
 initcursor (pathto_cursor);
 cursor = num-1;
																						// выводим значения курсора
 gprintfxy (x1+XDIGIT*3,y1+YDIGIT*2," %02d/%02d/%02d ",
						time[cursor].da_day,time[cursor].da_mon,time[cursor].da_year-1900);
 gprintfxy (x1+XDIGIT*6,y1+YDIGIT*2,"(%8ld дн)",
						time_days[cursor]-time_days[num-1]);
 gprintfxy (x1+XDIGIT*10,y1+YDIGIT*2,"%s = %6.2f",
						field_name,(float)par[cursor]);
 /*xpos = x1+XDIGIT*12;
 ypos = y1+YDIGIT*2;

 пишем комментарий
 settextstyle (russian_font,HORIZ_DIR,0);
 setusercharsize (1,3,1,4);
 gprintfxy (xpos,ypos,"%40s",comment[cursor]);
 settextstyle (SMALL_FONT,HORIZ_DIR,4); */

 setcursor ((int)(xlow+time_days[cursor]*koeff_x),(int)(ynull-par[cursor]*koeff_y));

 do
 {
  if ((changed)&&(graph_active==GRAPH))
  {
	killcursor ();
	setcursor ((int)(xlow+time_days[cursor]*koeff_x),(int)(ynull-par[cursor]*koeff_y));
	/*setcolor (BLACK);*/
	gprintfxy (x1+XDIGIT*3,y1+YDIGIT*2," %02d/%02d/%02d ",
		 time[cursor].da_day,time[cursor].da_mon,time[cursor].da_year-1900);
	gprintfxy (x1+XDIGIT*6,y1+YDIGIT*2,"(%8ld дн)",
									time_days[cursor]-time_days[num-1]);
	gprintfxy (x1+XDIGIT*10,y1+YDIGIT*2,"%s = %6.2f",
								   field_name,(float)par[cursor]);
/*	xpos = x1+XDIGIT*12;
	ypos = y1+YDIGIT*2;*/
	}
  else
   if (changed)
   {
    killcursor ();
		setcursor ((int)(xlow+time_days[cursor]*koeff_x),(int)(ylow+BOTTOM*(2*graph_active+1)/6+2));
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
/*   case KB_N_SPACE   :  if (!changed)
						 break;
						setcolor (BLACK);
						gprintfxy (x1+XDIGIT*3,y1+YDIGIT*2," %02d/%02d/%02d ",
								   time[cursor].da_day,time[cursor].da_mon,time[cursor].da_year-1900);
						gprintfxy (x1+XDIGIT*6,y1+YDIGIT*2,"%s = %6.2f",
								   field_name,(float)par[cursor]);
						xpos = x1+XDIGIT*12;
						ypos = y1+YDIGIT*2;
						 пишем комментарий
						settextstyle (russian_font,HORIZ_DIR,0);
						setusercharsize (1,3,1,4);
						gprintfxy (xpos,ypos,"%40s",comment[cursor]);
						settextstyle (SMALL_FONT,HORIZ_DIR,4);
						changed = 0;
						break;*/
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
												setcursor ((int)(xlow+time_days[cursor]*koeff_x),(int)(ylow+BOTTOM*(2*graph_active+1)/6+2));
                        break;
     default          : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                         if ((hotkeys[i].key==user)&&(hotkeys[i].able))
						 {
						  killcursor();
						  closecursor();
						  hotkeys[i].action ();
						  initcursor(pathto_cursor);
							setcursor ((int)(xlow+time_days[cursor]*koeff_x),
									 (int)(ylow+BOTTOM*(2*graph_active+1)/6+2));
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
 /*long diff=0;*/

 file.record = &rec;

 if (left)
	for (i=dir_num-1;((i>=0)&&(ftimecmp(/*(struct date *)*/(FTIME *)&(dir_spes[i].time.date),/*(struct date *)*/(FTIME *)now)>=0));i--)
	 ;
 else
	for (i=0;((i<dir_num)&&(ftimecmp(/*(struct date *)*/(FTIME *)&(dir_spes[i].time.date),/*(struct date *)*/(FTIME *)now)<=0));i++)
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
