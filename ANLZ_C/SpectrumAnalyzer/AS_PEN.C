/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_PEN     
* Describe  : ����� � ��p�p��
* File      : C:\ALEX\WORK\GRAPH\AS_PEN.C        
* Last edit : 10.09.93 02.05.46
* Remarks   : 
*****************************************************************************/

#include <graphics.h>
#include <alloc.h>
#include <stdio.h>
#include "as_pen.h"
											 // ��६���� ��� �������� �����
char far *buffer;       /* ��襥 ����p������                                */
char far *cursor;       /*      ��᪠ ��p�p� (�p㧨��� �� 䠩��)            */
char far *screen;       /*       ��᪠ ��࠭� (�p㧨��� �� 䠩��)            */
int x,y;                /* ��᫥���� ���p������ ��� �뢮��                   */


char far *sbuffer[SCURNUMBER];  /* ��訥 ����p������                                */
char far *scursor;       /*      ��᪠ ��p�p� (�p㧨��� �� 䠩��)            */
char far *sscreen;       /*       ��᪠ ��࠭� (�p㧨��� �� 䠩��)            */
int sx[SCURNUMBER],sy[SCURNUMBER];  /* ��᫥���� ���p������ ��� �뢮��                   */
int scurnum;	// ����⢨⥫쭮� ������⢮ �뢥������ ����ࠦ���� ᯥ�����





void initcursor (char *cursor_name)
/***********
* Describe : ���樠����p�� ��p�p: ���p㦠�� � ������ �� 䠩�� ��p�� ��p�p�
* Params   : char *cursor_name - ��� 䠩�� � ��p���� ��p�p�
* Return   : void
* Call     :
***********/
{
 FILE *f;
 int dx,dy;

 f = fopen(cursor_name,"rb");   /*                                           */
 fread (&dx,sizeof(int),1,f);   /* �⠥� p����p� ��p��� ��p�p�             */
 fread (&dy,sizeof(int),1,f);   /*                                           */
 cursor = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
 screen = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
 buffer = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
 rewind (f);                                             /*                  */
 fread (cursor,sizeof(char),imagesize(0,0,dx,dy),f);     /* �⠥� ��p��     */
 fread (screen,sizeof(char),imagesize(0,0,dx,dy),f);     /* �⠥� ��p��     */
 fclose (f);                                             /*                  */
}

void killcursor (void)
/***********
* Describe : 㭨�⮦��� p���� �뢥����� ��p�p (����⠭�������� ����p������)
* Params   : void		�� �������⭮� ���饭�� ����� �뢮���� ���㦭��
* Return   : void		����ࠦ���� �� ����� ᮧ����� ��㭮�
* Call     :
***********/
{
 putimage (x,y,buffer,COPY_PUT);         /* ����⠭�������� ����p������      */
}

void setcursor (int x1,int y1)
/***********
* Describe : ��⠭�������� ��p�p
* Params   : int x1 - ��� ��������
*          : int y1 - ��p�p
* Return   : void
* Call     :
***********/
{
 int xsize,ysize;
 x = x1;                      /* ���p������ ���                              */
 y = y1;                      /* ���⠭�������                               */
 xsize = ((int)cursor[0]);    /* p����p�                                     */
 ysize = ((int)cursor[2]);    /* ��p���                                      */
 getimage (x1,y1,x1+xsize,y1+ysize,buffer);
 putimage (x1,y1,screen,AND_PUT);
 putimage (x1,y1,cursor,XOR_PUT);
}

void closecursor (void)
/***********
* Describe : �����稢��� p����� � ��p�p��
* Params   : void
* Return   : void
* Call     :
***********/
{
 farfree (cursor);
 farfree (screen);
 farfree (buffer);
}




void initscursor (char *cursor_name)
/***********
* Describe : ���樠����p�� ᯥ��p�p: ���p㦠�� � ������ �� 䠩�� ��p�� ��p�p�
* Params   : char *cursor_name - ��� 䠩�� � ��p���� ��p�p�
* Return   : void
* Call     :
***********/
{
 FILE *f;
 int dx,dy;

 f = fopen(cursor_name,"rb");   /*                                           */
 fread (&dx,sizeof(int),1,f);   /* �⠥� p����p� ��p��� ��p�p�             */
 fread (&dy,sizeof(int),1,f);   /*                                           */
 scursor = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
// sscreen = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
 rewind (f);                                             /*                  */
 fread (scursor,sizeof(char),imagesize(0,0,dx,dy),f);     /* �⠥� ��p��     */
// fread (sscreen,sizeof(char),imagesize(0,0,dx,dy),f);     /* �⠥� ��p��     */
 fclose (f);                                             /*                  */
 for (scurnum=0;scurnum<SCURNUMBER;scurnum++)
	 sbuffer[scurnum] = farcalloc (imagesize(0,0,dx,dy),sizeof(char)); /* ������           */
 scurnum = 0;
}

void killscursor (void)
/***********
* Describe : 㭨�⮦��� �� p���� �뢥����� ᯥ��p�p� (����⠭�������� ����p������)
* Params   : void		� �⫨稥 �� �����, ����୮� ���饭�� �� �����
* Return   : void		�뢮�� �� ��࠭
* Call     :
***********/
{
while (scurnum>0)
	{
	scurnum--;
	putimage (sx[scurnum],sy[scurnum],sbuffer[scurnum],COPY_PUT);         /* ����⠭�������� ����p������      */
	}
}

void setscursor (int x1,int y1)
/***********
* Describe : ��⠭�������� ᯥ��p�p, ���� ᤥ���� �⮫쪮 ���饭��,
* Params   : int x1 - ��� ��������              ᪮�쪮 ����ࠦ���� �.����
*          : int y1 - ��p�p
* Return   : void
* Call     :
***********/
{
 if (scurnum == SCURNUMBER)	return;
 sx[scurnum] = x1;                      /* ���p������ ���                              */
 sy[scurnum] = y1;                      /* ���⠭�������                               */
// xsize = ((int)scursor[0]);    /* p����p�                                     */
// ysize = ((int)scursor[2]);    /* ��p���                                      */
 getimage (x1,y1,x1+(int)scursor[0],y1+(int)scursor[2],sbuffer[scurnum]);
// putimage (x1,y1,sscreen,AND_PUT);
// putimage (x1,y1,scursor,XOR_PUT);
 putimage (x1,y1,scursor,AND_PUT);
 scurnum++;
}

void closescursor (void)
/***********
* Describe : �����稢��� p����� � ��p�p��
* Params   : void
* Return   : void
* Call     :
***********/
{
 farfree (scursor);
// farfree (sscreen);
 for (scurnum=0;scurnum<SCURNUMBER;scurnum++)
	farfree (sbuffer[scurnum]);
}
