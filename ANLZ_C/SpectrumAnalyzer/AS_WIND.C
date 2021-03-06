/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_WIND
* Describe  : �㭪樨 ����᪮�� ����䥩�
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
#include <conio.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_keys.h"
#include "as_list.h"
#include "as_fndir.h"
#include "as_wind.h"

static int  lastx,lasty;                               /* ��᫥����: ����., */
static char lastbuf [BUFLEN];                          /* ����,             */
static char far *imgbuf;                               /* ����ࠦ����.       */
static int  imgnum=0;                                  /* ����� ⥪. �३��  */
extern char far pathto_as[];
extern asHotKey hotkeys[];

void gprintfxy (int x,int y,char *fmt,...)
/***********
* Describe : ��ଠ�஢���� �뢮� � ��䨪�
* Params   : int x     - ��㤠
*          : int y     - �뢮����
*          : char *fmt - ��ப� �ଠ� (�. printf)
*          : ...       - ��ࠬ���� ��� �뢮��
* Return   : void
* Call     :
***********/
{
 va_list ap;                                           /* ᯨ᮪ ��.        */
 char buffer [BUFLEN];                                 /*                    */
 int x2,y2;                                            /*                    */
                                                       /*                    */
 va_start (ap,fmt);                                    /*                    */
 vsprintf (buffer,fmt,ap);                             /*                    */
 va_end (ap);                                          /*                    */
 x2 = x + textwidth (buffer);                          /*                    */
 y2 = y + textheight (buffer) + YSPACE*2;              /*                    */
 bar (x,y,x2,y2);                                      /* ��頥� ��� �뢮�� */
 outtextxy (x,y,buffer);                               /*                    */
}                                                      /*                    */

void textfon (int x,int y,char *fmt,...)
/***********
* Describe : "����祭��" �ଠ�஢������ ⥪��
* Params   : int x     - ��㤠
*          : int y     - �뢮����
*          : char *fmt - ��ப� �ଠ� (�. printf)
*          : ...       - ��ࠬ���� ��� �뢮��
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 va_list ap;                                           /*                    */
 extern  char lastbuf [BUFLEN];                        /*                    */
 extern int lastx,lasty;                               /*                    */
 int x2,y2;                                            /*                    */
                                                       /*                    */
 lastx = x;                                            /* ���������� ���     */
 lasty = y;                                            /* �뢮����           */
 va_start (ap,fmt);                                    /*                    */
 vsprintf (lastbuf,fmt,ap);                            /*                    */
 va_end (ap);                                          /*                    */
 x2 = x + textwidth (lastbuf);                         /* � �� �뢮����     */
 y2 = y + textheight (lastbuf) + YSPACE*2;             /*                    */
 bar (x,y,x2,y2);                                      /* ��頥� ����      */
 outtextxy (x,y,lastbuf);                              /*                    */
}                                                      /*                    */

void textfoff (void)
/***********
* Describe : "�몫�祭��" ⥪��, �뢥������� ��᫥���� �㭪. textfon
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 int x2,y2;                                            /*                    */
 extern  char lastbuf [BUFLEN];                        /*                    */
 extern int lastx,lasty;                               /*                    */
                                                       /*                    */
 x2 = lastx + textwidth (lastbuf);                     /* ⠬ ��� �뢮����   */
 y2 = lasty + textheight (lastbuf) + YSPACE*2;         /*                    */
 bar (lastx,lasty,x2,y2);                              /* ��. ��אַ㣮�쭨� */
}                                                      /*                    */

void setframe (int x1,int y1,int x2,int y2,char *title)
/***********
* Describe : �뢮��� �३� (ࠬ��,���������)
* Params   : int x1      -  ���न���� ������
*          : int y1      -  ���孥�� 㣫�
*          : int x2      -  ���न���� �ࠢ���
*          : int y2      -  ������� 㣫�
*          : char *title -  ���������
* Return   : void
* Call     :
***********/
{
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
 if (imgbuf==NULL)                                     /* ��࠭塞 �����.   */
 {
  closegraph ();
  printf ("\n Error creating frame: no enough memory. ");
  exit (-1);
 }
 getimage (x1,y1,x2,y2,imgbuf);                        /* �᫨ ���� ������   */
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
 setfillstyle (SOLID_FILL,C_F_PAPER);                  /* �᭮����           */
 bar (x1,y1,x2,y2);                                    /*           ����     */
 setcolor (C_F_INK);                                   /*                    */
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);  /* ������� ࠬ��� ��  */
 line (x1+XSPACE,y1+textheight(title)+3*YSPACE,        /* ��� � �����.   */
       x2-XSPACE,y1+textheight(title)+3*YSPACE);       /* ���������          */

 outtextxy (x1+(x2-x1-textwidth(title)-2*XSPACE)/2,y1+YSPACE,title);
}                                                      /*                    */

void killframe (void)
/***********
* Describe : ����⮦��� ��᫥���� �뢥����� �३�
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
 putimage (lastx,lasty,imgbuf,COPY_PUT);               /* ��᫥���� �����.   */
 farfree (imgbuf);                                     /*                    */
}                                                      /*                    */

void setbutton (int x1,int y2,char *buttontext,int active,int inverse)
/***********
* Describe : �뢮��� ������
* Params   : int x1           -  ���न���� ������
*          : int y2           -  �������  㣫�
*          : char *buttontext -  ⥪�� ������
*          : int active       -  䫠� ��⨢����
* Return   : void
* Call     :
***********/
{                                                      /*                    */
 int x2,y1;                                            /*                    */
                                                       /*                    */
 x2 = x1 + XBUTTONSIZE;                                /* �ࠢ� ���孨�     */
 y1 = y2 - YBUTTONSIZE;                                /*           㣮�     */
 if (!inverse)
  if (active)                                           /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_INK);                   /* ��࠭��� ������   */
   setcolor (C_B_PAPER);                                /*                    */
  }                                                     /*                    */
  else                                                  /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_PAPER);                 /* ���. ������       */
   setcolor (C_B_INK);                                  /*                    */
  }
 else                                                   /*                    */
  if (active)                                           /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_PAPER);                 /* ��࠭��� ������   */
   setcolor (C_B_INK);                                  /*                    */
  }                                                     /*                    */
  else                                                  /*                    */
  {                                                     /*                    */
   setfillstyle (SOLID_FILL,C_B_INK);                   /* ���. ������       */
   setcolor (C_B_PAPER);                                /*                    */
  }
 bar (x1,y1,x2,y2);                                    /* ��. ���.         */
 rectangle (x1+XSPACE,y1+YSPACE,x2-XSPACE,y2-YSPACE);  /* ࠬ��              */
 outtextxy (x1+XSPACE,y1+YSPACE,buttontext);           /*                    */
}                                                      /*                    */

void mainmenu (int y,asChoice *ch)
/***********
* Describe : ������� ��ਧ��⠫쭮� ���� �ணࠬ��
* Params   : int y        - ���� ��㤠 �뢮����
*          : asChoice *ch - ���ᨢ ����ୠ⨢
* Return   : void
* Call     : setbutton
***********/
{                                                      /*                    */
 int selected=0,wasselected=0,nchoices=-1,i,changed=1; /*                    */
 char user;                                            /*                    */
                                                       /*                    */
 for (i=0;(ch[i]).text!=NULL;i++)                      /*                    */
 {                                                     /* ��㥬 ����        */
  setbutton (i*XBUTTONSIZE,y,(ch[i]).text,(selected==i),1);
  nchoices++;                                          /* �᫮ ����ୠ⨢  */
 }                                                     /*                    */
 wasselected=nchoices;                                 /* �뫠 ��࠭�       */
 do                                                    /*                    */
 {                                                     /*                    */
  changed = 1;                                         /* 䫠� ���������     */
	user = getch();                                      /* ���� ���짮��⥫�  */
  switch (user)                                        /*                    */
  {                                                    /*                    */
   case KB_N_LEFT:    wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected--;                     /*                    */
                       if (selected<0)                 /*                    */
                        selected=nchoices;             /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* ���᪠������ ��. */
                      break;                           /* ����饭�� ������ */
                                                       /*                    */
   case KB_N_RIGHT:   wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected++;                     /*                    */
                       if (selected>nchoices)          /*                    */
                        selected=0;                    /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* ���᪠������ ��. */
                      break;                           /* ����饭�� ������ */
                                                       /*                    */
   case KB_N_ENTER:   if ((ch[selected]).action!=NULL)
                       (ch[selected]).action ();       /* �믮��塞          */
                      changed = 0;
                      break;
                                                       /*                    */
   default:           for (i=0;hotkeys[i].key!=KB_LAST;i++)
                       if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                        hotkeys[i].action ();
                      changed = 0;                     /* ��� ���������      */
                      break;                           /*                    */
  }                                                    /*                    */
  if (!changed)                                        /*                    */
   continue;                                           /*                    */
  setbutton (wasselected*XBUTTONSIZE,y,(ch[wasselected]).text,0,1);
	setbutton (selected*XBUTTONSIZE,y,(ch[selected]).text,1,1);
 }                                                     /* ����ᮢ. 2 ����. */
 while (1);                                            /* ���� �� Alt-X      */
}                                                      /*                    */

int submenu (char *title,int x1,int y1,asChoice *ch)
/***********
* Describe : ���⨪��쭮� �������
* Params   : char *title  - ���������
*          : int x1       - ����
*          : int y1       - ���孨� 㣮�
*          : asChoice *ch - ���ᨢ ����ୠ⨢
* Return   : ����� ��࠭��� ����ୠ⨢� 1..num
* Call     : setframe,killframe,setbutton
***********/
{                                                      /*                    */
 int selected=0,wasselected=0,nchoices=-1,i,x2,y2,changed=1;
 char user;                                            /*                    */

// x2 = textwidth ((char far *) title);									 /*                    */
 for (i=0;(ch[i]).text!=NULL;i++)                      /*                    */
	{
	nchoices++;                                          /* �᫮ ����ୠ⨢  */
//	if(x2 < textwidth ((char far *)ch[i].text))
//		x2 = textwidth ((char far *)ch[i].text);
	}
 x2 = x1 + XBUTTONSIZE + 2*XSPACE;                     /*                    */
// x2 += x1 + 2*XSPACE;
 y2 = y1 + (nchoices+1)*YBUTTONSIZE + (nchoices+6)*YSPACE + textheight((char far *)title);
 setframe (x1,y1,x2,y2,title);                         /*                    */
 for (i=nchoices;i>=0;i--)                             /* ��㥬 ����        */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(i+1)*(YBUTTONSIZE+YSPACE),
             (ch[i]).text,(selected==i),0);            /*                    */
 wasselected=nchoices;                                 /* �뫠 ��࠭�       */
 do                                                    /*                    */
 {                                                     /*                    */
  changed = 1;                                         /* 䫠� ���������     */
  user = getch();                                      /* ���� ���짮��⥫�  */
  switch (user)                                        /*                    */
  {                                                    /*                    */
   case KB_N_UP:      wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected--;                     /*                    */
                       if (selected<0)                 /*                    */
                        selected=nchoices;             /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* ���᪠������ ��. */
                      break;                           /* ����饭�� ������ */
                                                       /*                    */
   case KB_N_DOWN:    wasselected=selected;            /*                    */
                      do                               /*                    */
                      {                                /*                    */
                       selected++;                     /*                    */
                       if (selected>nchoices)          /*                    */
                        selected=0;                    /*                    */
                      }                                /*                    */
                      while (!(ch[selected]).able);    /* ���᪠������ ��. */
                      break;                           /* ����饭�� ������ */
                                                       /*                    */
   case KB_N_ENTER:   if ((ch[selected]).action!=NULL)
                       (ch[selected]).action ();       /* �믮��塞          */
                      killframe ();
											return selected;

	case KB_N_LEFT:
	case KB_N_RIGHT:	 i=ungetch (user);	break;

   default:           for (i=0;hotkeys[i].key!=KB_LAST;i++)
                       if ((hotkeys[i].key==user)&&(hotkeys[i].able))
                       {
                        hotkeys[i].action ();
                        killframe ();
                        return selected;
                       }
                      changed = 0;                     /* ��� ���������      */
                      break;                           /*                    */
  }                                                    /*                    */
  if (!changed)                                        /*                    */
   continue;                                           /*                    */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(wasselected+1)*(YBUTTONSIZE+YSPACE),
             (ch[wasselected]).text,0,0);              /* �����. ⮫쪮    */
  setbutton (x1+XSPACE,y1+textheight(title)+YSPACE*3+(selected+1)*(YBUTTONSIZE+YSPACE),
             (ch[selected]).text,1,0);                 /* 2 �����. ������    */
 }                                                     /*                    */
 while ((user!=KB_N_ESC)&&(user!=KB_N_LEFT)&&(user!=KB_N_RIGHT));/* ���� �� ESC        */
 killframe ();                                         /*                    */
 return -1;                                            /* Esc - -1           */
}                                                      /*                    */

void reportmemory (int x2,int y1)
/***********
* Describe : ᮮ���� � ᢮������ �����
* Params   : int x2 - ��㤠
*          : int y1 - �뢮����
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
* Describe : ᮮ���� � ��� � �६���
* Params   : int x1 - ��㤠
*          : int y1 - �뢮����
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
* Describe : ����� ��業� �믮������ ����樨
* Params   : float pc - ��業� (�ᥣ�� ����� 1.0)
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
* Describe : ��ப� ����� ����� ��࠭� � ᮮ�饭�� � ᢮������ �����
* Params   : char *text   - ᮮ�饭�� � ��ப� �����
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
* Describe : ᮮ�饭�� � 2 ������
* Params   : char *title    - ���������
*          : int x1         - ��㤠
*          : int y1         - �뢮����
*          : char *text[]   - ⥪��
*          : asChoice ch[2] - ������
* Return   : int
* Call     : setframe, killframe
***********/
{
 int nlines,i,x2,y2,x,Ok=1,changed=1;
 char user;


 x2 = textwidth(title);
 for (i=0,nlines=0;text[i]!=NULL;i++,nlines++)
 {
	x = textwidth(text[i]);
	x2 = max(x2,x);
 }
 x2 += x1 + XSPACE*6 + textwidth("W")*2;
 x = x1+2*(XBUTTONSIZE+XSPACE)+XSPACE*6;
 x2 = max(x2,x);
 y2 = y1 + YSPACE*5 + YBUTTONSIZE*(nlines+2);

 setframe (x1,y1,x2,y2,title);
 for (i=0;i<nlines;i++)
	outtextxy (x1+XSPACE+textwidth("W"),y1+(i+1)*YBUTTONSIZE,text[i]);

 do
 {
	if (changed)
	{
	 setbutton (x1+(x2-x1)/4-XBUTTONSIZE/2,y1+(nlines+2)*YBUTTONSIZE,
							(ch[0]).text,Ok,1);
	 setbutton (x1+3*(x2-x1)/4-XBUTTONSIZE/2,y1+(nlines+2)*YBUTTONSIZE,
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
* Describe : ���� ��p���
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

 ch = getch();	// �� ��砫� ����� ᨬ�����, ���� ��������
 if(isgraph(ch))
	{
	buffer[0] = ch;
	buffer[1] = 0;
	curpos = 1;
	bar (x1,y1,x2,y2);
	outtextxy (x1+XSPACE*2,y1+YSPACE,buffer);
	}
 else
	ungetch(ch);

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
	 case KB_N_HOME	 : curpos = 0;
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
* Describe : ���� 楫��� �᫠
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
* Describe : p�����p����� �p���
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
* Describe : ᮮ���� � ⥪�饩 ��४�ਨ
* Params   : int x2 - ��㤠
*          : int y1 - �뢮����
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
