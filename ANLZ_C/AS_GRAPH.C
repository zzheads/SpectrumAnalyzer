/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_GRAPH
* Describe  : ����� ��䨪� ᯥ���/ᨣ����
* File      : D:\BC\WORK\AS_GRAPH.C
* Last edit : 12-10-93 07:54:39pm
* Remarks   : ���,���/���,��⥣�஢����,���. ����⠡�
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
#include "m_options.h"

extern asChoice mnCursor[];
extern asHotKey hotkeys[];
void closeall (void);
extern char pathto_as[];
extern int p_x1,p_x2,p_y1,p_y2,CUR_TYPE,scurnum;
extern asRecPtr p_rec;
int PICK;
double MRS_BEG,MRS_END,AUTO_BEG,AUTO_END;   /* ���.��������� RMS � ��⮬���⠡�� */

asChoice mnWindow[]   = {{"   Transient  ",NULL ,1},
                         {"   Exponent   ",NULL ,1},
                         {NULL            ,NULL ,0}};


/*enum asCursor CUR_TYPE;*/

int graphwindow (int y1,int y2,asFile far *file)
/***********
* Describe : ����� ��䨪� ����� � �८�ࠧ������ ���
* Params   : int y1           - ���⨪���� ���न���� ��砫�
*          : int y2           - � ���� ���� � ��䨪��
*          : asFile far *file - ᮤ�ন� 㪠��⥫� �� �㦭�� ������
* Return   : void
* Call     : infowindow
***********/
{
 static double scursor, delta;
 int i,x2,xlow,xhigh,ylow,yhigh,x1=0,num,num_mrs,numrange,numlow;
 int cursor=0,n,r_code=0,scurpos;
 double koeffx2,koeffy,maxy,miny,ynull,y,j,step,pick,mrs=0.0,drms,incr;
 char xunit[10],yunit[10],title[50],tmp[20];
 unsigned char	user;
 float far *msr=NULL,A,B,maxx,KOEFY=1.0,KOEFX=1.0,auto_end,mrs_end;
 enum asScale yscale;

 hotkeys[1].able = 0;     /* ����� ᭮�� �맢��� */

 xlow  = x1 + XSPACE*2 + XDIGIT*3;               /* ���. �࠭�� ��䨪�     */
 xhigh = xlow + 512;                             /* �ࠢ�� �࠭�� ��䨪�   */
 x2 = xhigh + XDIGIT*2;                          /* �ࠢ�� �࠭�� ����      */
 yhigh = y1 + YBUTTONSIZE*2 + YDIGIT + YSPACE*2; /* ���� ��䨪�             */
 ylow  = y2 - YDIGIT*2 - YSPACE*2;               /* ��� ��䨪�              */

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
	for (n=0;n<num;n++)                /* ���������� ���ᨢ� ���祭�� float */
	 msr[n] = (float) A*((byte)(file->record->y)[n]+B);
 }
 else
 {
	for (n=0;n<num;n++)
	 msr[n] = (float) pow(10.0,A*((byte)(file->record->y)[n]+B)/20);
 }

 strcpy (title,"Object:  ");
 strcat (title,(char *)file->record->name);
 strcat (title,";     Date:  ");
 ftimetoa(&(file->ftime),tmp);
 strcat (title,tmp);

 initcursor (fullpath("cursor.img"));
 initscursor (fullpath("scursor.img"));

XY_SCALING:
 numlow =0;
 numrange = num = file->record->number;                 // �᫮ �����
 strcpy (xunit,xunitstr(file->record));				 // ��� �� �� �
 maxx = isspectrum(file->record) ? (float)file->record->hfreq:
																	 (float)num/(file->record->hfreq*2.56);
Y_SCALING:
 strcpy (yunit,yunitstr(file->record));
 miny=maxy=0.0;
 if (issignal(file->record))    /* �᫨ ᨣ��� */
 {
	for (i=0;i<num;i++)           /* ��室�� max �� �ᥬ ���祭 */
	{
	 if(maxy<fabs((double)msr[i]))  maxy=fabs((double)msr[i]);
	}
	miny=-maxy;                   /* ����� � ���� ᨬ�����.��䨪 */
 }
 else                        /* �᫨ ᯥ��� */
 {
	auto_end=AUTO_END*num/maxx;       /* max ⮫쪮 � ��������� */
	if(auto_end>num)  auto_end=num;
	for (i=AUTO_BEG*num/maxx;i<auto_end;i++)
	{
	 if(maxy<msr[i])  maxy=msr[i];
	}
 }

DRAWING:
 if (isspectrum(file->record))
 {
	mrs_end=MRS_END*num/maxx;       /* �᫨ ᯥ��� RMS ⮫쪮 � ��������� */
	if(mrs_end>num)  mrs_end=num;
	for (mrs=0.0,n=MRS_BEG*num/maxx;n<mrs_end;n++)  mrs+=(double)msr[n]*msr[n];
	mrs = sqrt(mrs);
 }
 else                            /* �᫨ ᨣ��� RMS �� �ᥬ ����⠬ */
 {
	for (mrs=0.0,n=0;n<num;n++)      mrs += (double)msr[n]*msr[n];
	mrs = sqrt(mrs/num);
 }

 step = maxy/1000;
 if (maxy==miny)
	maxy*=2;
 koeffy = (double)(ylow-yhigh)/fabs(SCALE(maxy)-SCALE(miny));
 ynull = yhigh + SCALE(maxy)*koeffy;             /* �㫥��� �஢���          */
 koeffx2 = (double)(xhigh-xlow)/numrange; /*                          */

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
 rectangle (xlow-XSPACE,ylow,xhigh,yhigh-YSPACE*2);	//��ࠬ����� ��䨪� (�� �३��)

 for (i=numlow;i<=numlow+numrange*1.1;i+=numrange/5)
 {
	line ((int)(xlow+(i-numlow)*koeffx2),ylow,(int)(xlow+(i-numlow)*koeffx2),
																							ylow+YSPACE*3);	//ࠧ��⪠ �� X
	if (i>=numlow+numrange*0.9)
	 gprintfxy ((int)(xlow+(i-numlow)*koeffx2-XDIGIT*1.5),ylow+YSPACE*4,"%-5.1f",
																			(float)KOEFX*i*maxx/num);
	else
	 gprintfxy ((int)(xlow+(i-numlow)*koeffx2-XDIGIT/2),ylow+YSPACE*4,"%5.1f",
																			(float)KOEFX*i*maxx/num);
 }
 gprintfxy (xhigh+XSPACE*6,ylow+YSPACE*4,"%5.5s",xunit);

 if (yscale)		//ࠧ��⪠ �� Y
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
 gprintfxy (x1+XDIGIT,yhigh-YDIGIT*1.7,"%5.5s",yunit);

 moveto (xlow,YINSIDE(ynull-SCALE(msr[0])*koeffy));	// ��⨬ �ਢ�� ��䨪�
 for (i=0;i<numrange;i++)
	lineto (xlow+i*koeffx2,YINSIDE(ynull-SCALE(msr[numlow+i])*koeffy));

// initcursor (fullpath("cursor.img"));
// initscursor (fullpath("scursor.img"));

 reportmemory(getmaxx(),getmaxy()-YBUTTONSIZE);
																						 /* �뢮��� ���祭�� ����� */
 gprintfxy (x1+XDIGIT*3,yhigh-YBUTTONSIZE*2+YSPACE*2,"dX=%11.5f %5.5s",
				(float)KOEFX*delta*maxx/num,xunit);
 gprintfxy (x1+XDIGIT*8,yhigh-YBUTTONSIZE*2+YSPACE*2,"%s",mnCursor[CUR_TYPE].text);

 gprintfxy (x1+XDIGIT*3,yhigh-YBUTTONSIZE," X=%11.5f %5.5s",
			(float)KOEFX*cursor*maxx/num,xunit);
 gprintfxy (x1+XDIGIT*8,yhigh-YBUTTONSIZE," Y=%11.5f %5.5s",
		(double)msr[cursor]*KOEFY,yunit);
 if (PICK)    															  /* �뢮��� ������ �஢��� */
	gprintfxy (x1+XDIGIT*13,yhigh-YBUTTONSIZE," Pk:%3.0f-%3.0f m/ss",
						 pick/2,pick);
																								/* �뢮��� ��� �஢��� */
 gprintfxy (x1+XDIGIT*19,yhigh-YBUTTONSIZE,"Total: %7.3f %5.5s",mrs*KOEFY,yunit);
																								// �뢮��� �����
 setcursor ((int)(xlow+(cursor-numlow)*koeffx2),(int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
 scurnum = 0;	// ���뢠�� ࠭�� ���������� ᯥ������

 do
 {
	user = rkey ();
	switch (user)
	{
	 case KB_N_LEFT    :  if (cursor>0)	cursor--;
												if (cursor>=numlow) 	goto CUR_MOVE;
												numlow -= numrange;
												if (numlow<0)	numlow = 0;
												killframe ();
												goto DRAWING;
	 case KB_N_RIGHT   :  if (cursor<num-1)		cursor++;
												if (cursor<numlow+numrange)	goto CUR_MOVE;
												numlow += numrange;
												if (numlow+numrange>num)	numlow = num-numrange;
												killframe ();
												goto DRAWING;
	 case KB_C_LEFT    :  if (cursor>=10)			 cursor-=10;
												if (cursor>=numlow) 	goto CUR_MOVE;
												numlow -= numrange;
												if (numlow<0)	numlow = 0;
												if (cursor < numlow)	cursor =numlow;
												killframe ();
												goto DRAWING;
	 case KB_C_RIGHT   :  if (cursor<num-10-1)		 cursor+=10;
												if (cursor<numlow+numrange)	goto CUR_MOVE;
												numlow += numrange;
												if (numlow+numrange>num)	numlow = num-numrange;
												if (cursor >= numlow+numrange)	cursor = numlow+numrange-1;
												killframe ();
												goto DRAWING;
	 case KB_N_HOME    :  cursor=numlow;
												goto CUR_MOVE;
	 case KB_N_END     :  cursor=numlow+numrange-1;

CUR_MOVE:					// ��ࠡ�⪠ ��६�饭��� ����� �� ��࠭�
	 killcursor ();
	 setcursor ((int)(xlow+(cursor-numlow)*koeffx2),
									(int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
	 gprintfxy (x1+XDIGIT*3,yhigh-YBUTTONSIZE," X=%11.5f ",
									(float)KOEFX*cursor*maxx/num);
	 gprintfxy (x1+XDIGIT*8,yhigh-YBUTTONSIZE," Y=%11.5f ",
									(double)msr[cursor]*KOEFY);
	 break;
																// ��������� ����� ᯥ�.�����
	 case KB_N_UP      :  //if (delta >= num-1)	break;
												switch (CUR_TYPE)
													{
													case HARM:	incr = delta/(numlow+numrange-1);
																			if (incr < 1.0/SCURNUMBER)	incr = 1.0/SCURNUMBER;
																			else	// �᪮७��� ��६�饭�� ᯥ�����
																				if (incr > 1.0)	incr = 1.0;
																			scursor = delta += incr;
																			break;
													case SIDE:	if (scursor<numlow+numrange/2)
																				incr = delta/(numlow+numrange-1-scursor);
																			else	incr = delta/(scursor-numlow);
																			if (incr < 2.0/SCURNUMBER)	incr = 2.0/SCURNUMBER;
																			else	// �᪮७��� ��६�饭�� ᯥ�����
																				if (incr > 1.0)	incr = 1.0;
																			delta += incr;
																			break;
													case DELT:	if ((scursor+delta/2<num-1)&&(scursor-delta/2>0))
																				delta+=2;
													}
												if (delta > num-1)
													{delta = num-1;
													if (CUR_TYPE==HARM)	scursor = delta;
													}
												goto SCUR_MOVE;

	 case KB_N_DOWN    :	//if (delta == 0)	break;
												switch (CUR_TYPE)
													{
													case HARM:	incr = delta/(numlow+numrange-1);
																			if (incr < 1.0/SCURNUMBER)	incr = 1.0/SCURNUMBER;
																			else	// �᪮७��� ��६�饭�� ᯥ�����
																				if (incr > 1.0)	incr = 1.0;
																			scursor = delta -= incr;
																			break;
													case SIDE:	if (scursor<numlow+numrange/2)
																				incr = delta/(numlow+numrange-1-scursor);
																			else	incr = delta/(scursor-numlow);
																			if (incr < 2.0/SCURNUMBER)	incr = 2.0/SCURNUMBER;
																			else	// �᪮७��� ��६�饭�� ᯥ�����
																				if (incr > 1.0)	incr = 1.0;
																			delta -= incr;
																			break;
													case DELT:	delta-=2;
													}
												if (delta<0)
													{delta = 0;
													if (CUR_TYPE==HARM)	scursor = delta;
													}
												goto SCUR_MOVE;

	 case KB_N_BS			 :	killcursor ();		// 㤠����� ᯥ����� � ��࠭�
												killscursor ();
												setcursor ((int)(xlow+(cursor-numlow)*koeffx2),
														(int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
												break;

	 case KB_N_SPACE   :    scursor = cursor;	// ��⠭���� ᯥ����� �� ���.

SCUR_MOVE:			//��ࠡ�⪠ ��६�饭�� ᯥ����� �� ��࠭�

	 killcursor ();
	 killscursor ();
//	 if (scursor >= num)	scursor = num-1;
//	 if (scursor < 0)	scursor = 0;
	 switch (CUR_TYPE)
		{
		case HARM:  delta = scursor;
								for(i=0;i<SCURNUMBER;i++)
									{
									scurpos = scursor+i*delta+0.5;
									if ((scurpos>=numlow)&&(scurpos<numlow+numrange))
									setscursor ((int)(xlow+(scurpos-numlow)*koeffx2),
									(int)(YINSIDE(ynull-SCALE(msr[scurpos])*koeffy)));
									}
								break;
		case SIDE:	for (i=1;i<=SCURNUMBER/2;i++)
									{
									scurpos = scursor+i*delta+0.5;
									if ((scurpos>=numlow)&&(scurpos<numlow+numrange))
										setscursor ((int)(xlow+(scurpos-numlow)*koeffx2),
											(int)(YINSIDE(ynull-SCALE(msr[scurpos])*koeffy)));
									scurpos = scursor-i*delta+0.5;
									if ((scurpos>=numlow)&&(scurpos<numlow+numrange))
										setscursor ((int)(xlow+(scurpos-numlow)*koeffx2),
											(int)(YINSIDE(ynull-SCALE(msr[scurpos])*koeffy)));
									 }
								break;
		case DELT:	if(scursor+delta/2>num-1)	cursor = scursor = num-1-delta/2;
								if(scursor-delta/2<0)		cursor = scursor = delta/2;
								scurpos = scursor+delta/2+0.5;
								if ((scurpos>=numlow)&&(scurpos<numlow+numrange))
									setscursor ((int)(xlow+(scurpos-numlow)*koeffx2),
											(int)(YINSIDE(ynull-SCALE(msr[scurpos])*koeffy)));
								scurpos = scursor-delta/2+0.5;
								if ((scurpos>=numlow)&&(scurpos<numlow+numrange))
									setscursor ((int)(xlow+(scurpos-numlow)*koeffx2),
											(int)(YINSIDE(ynull-SCALE(msr[scurpos])*koeffy)));
								gprintfxy (x1+XDIGIT*3,yhigh-YBUTTONSIZE*2+YSPACE*2,"dX=%11.5f",
																			 (float)KOEFX*(delta+1)*maxx/num);
																			 // ��⠥� � �뢮���  Delta RMS
								for (drms=0.0,n=scursor-delta/2;n<=scursor+delta/2;n++)
									drms+=(double)msr[n]*msr[n];
								if (isspectrum(file->record))	drms = sqrt(drms);
								else													drms = sqrt(drms/(delta+1));
								gprintfxy (x1+XDIGIT*18.75,yhigh-YBUTTONSIZE*2+YSPACE*2,
																	"dTotal: %7.3f %5.5s",drms*KOEFY,yunit);
								setcursor ((int)(xlow+(cursor-numlow)*koeffx2),
														(int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
								goto CUR_MOVE;
		}
	 gprintfxy (x1+XDIGIT*3,yhigh-YBUTTONSIZE*2+YSPACE*2,"dX=%11.5f",
																			 (float)KOEFX*delta*maxx/num);
	 setcursor ((int)(xlow+(cursor-numlow)*koeffx2),
														(int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
	 break;

	 case KB_N_ENTER   :  infowindow ((x1+x2)/2,y1+YDIGIT,file->record);
												break;

	 case KB_A_F5      :  r_code = -1;        /* ����㧨�� ������ �����        */
												goto EXIT;
	 case KB_A_F6      :  r_code = 1;         /* ����㧨�� ������ �ࠢ��       */
												goto EXIT;

	 case KB_A_F7      :  if (issignal(file->record))       /* ���/���         */
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
												killframe ();
												goto DRAWING;

	 case KB_A_F8      :  maxy *= KOEFY;
												editfloat (&maxy,yunit);  /* ��������� ����� oy */
												maxy /= KOEFY;
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
												killframe ();
												goto DRAWING;
	 case KB_N_PGDN    :  if (issignal(file->record))
												{
												 maxy/=2;
												 miny/=2;
												}
												else
												 maxy/=2;
												killframe ();
												goto DRAWING;
																								 // ��������� ����⠡� ��
	 case KB_C_PGDN    :  numrange /= 2;
												numlow = cursor-numrange/2;
												if (numlow < 0)  numlow=0;
												if ((numlow+numrange)>num)	numlow = num-numrange;
												killframe ();
												goto DRAWING;
	 case KB_C_PGUP    :  if (numrange == num)	break;
												numrange *= 2;
												if (numrange>num)	numrange = num;
												numlow = cursor-numrange/2;
												if (numlow < 0)  numlow=0;
												if ((numlow+numrange)>num)	numlow = num-numrange;
												killframe ();
												goto DRAWING;

	 case KB_A_F9      :  if (isspectrum(file->record))      /* ���             */
														 break;
												setspectrum (file->record);
												cursor = 0;
												doFFT (msr,(int *)&(file->record->number));
												killframe ();
												goto XY_SCALING;
   case KB_A_F10     :  if (doIntegrate (msr,file->record)==-1)
			 break;                           /* ��⥣�஢����  */
												killframe ();
												goto Y_SCALING;
   case KB_C_F10     :  if (doDiff (msr,file->record)==-1)
                         break;                        /* ����७樨஢���� */
												killframe ();
												goto Y_SCALING;

	 default           :  for (i=0;hotkeys[i].key!=KB_LAST;i++)
												 if ((hotkeys[i].key==user)&&(hotkeys[i].able))
												 {
													killcursor();
													killscursor();
													closecursor();
													closescursor();
													hotkeys[i].action ();        // �뢮��� �������� �����
													gprintfxy (x1+XDIGIT*8,yhigh-YBUTTONSIZE*2+YSPACE*2,"%s",mnCursor[CUR_TYPE].text);
													gprintfxy (x1+XDIGIT*18.75,yhigh-YBUTTONSIZE*2+YSPACE*2,"                     ");
													initcursor(fullpath("cursor.img"));
													initscursor (fullpath("scursor.img"));
													setcursor ((int)(xlow+(cursor-numlow)*koeffx2),
																		 (int)(YINSIDE(ynull-SCALE(msr[cursor])*koeffy)));
										statusline((char *)slGraph);
												 }
												break;
	}
 }
 while (user!=KB_N_ESC);

 EXIT:
 closecursor ();
 closescursor ();
 killframe ();
 if (msr!=NULL)
  farfree (msr);
 p_rec=NULL;
 hotkeys[1].able = 1;     /* ����� �맢��� */
 return (r_code);
}

void infowindow (int x1,int y1,asRecPtr rec)
/***********
* Describe : ���ଠ�� � �����
* Params   : int x1       - ���न����
*          : int y1       - ����
*          : asRecPtr rec - ������
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
* Describe : ।���஢���� �᫠
* Params   : double * - 㪠��⥫� �� �᫮
*          : char *   - ��. ����७��
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
* Describe : ।���஢���� ���� �ᥫ
* Params   : double * - �᫠
*          : double * -
* Return   : void
* Call     :
***********/
{
 char digit1[30],digit2[30];

 gcvt (*x1,8,digit1);
 gcvt (*x2,8,digit2);

# ifdef ENGLISH
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()*3/4,getmaxy()/2,
		 " Edit limits of frequency (Hz): ");
#else
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()*3/4,getmaxy()/2,
		 " ��⠭���� ����� �������� (��): ");
#endif

 editstring (getmaxx()/4+XBUTTONSIZE/2,getmaxy()*3/8,15,(char *)digit1);
 editstring (getmaxx()/4+XBUTTONSIZE*2,getmaxy()*3/8,15,(char *)digit2);
 killframe ();
 (*x1) = (double) atof(digit1);
 (*x2) = (double) atof(digit2);
}
