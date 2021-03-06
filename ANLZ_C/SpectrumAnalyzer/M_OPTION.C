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
#include <process.h>

#include "as_main.h"
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_graph.h"
#include "as_rkey.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_option.h"
#include "as_keys.h"

enum asCurType CUR_TYPE;
enum  asWindType   weightwindow;
extern int help_context;
extern int GIPX,GIPY,N,M,K,L,PICK,REC_TYPE;
extern double MRS_BEG,MRS_END;
extern double AUTO_BEG,AUTO_END;

#ifdef ENGLISH
asChoice mnOptions[]  = {{" Cursor type  ",cursortype   ,1},
												 {" Window type  ",windowtype   ,1},
												 {" 3-D Params   ",set_gips     ,1},
												 {" Data port    ",set_port     ,1},
												 {" Tables       ",set_tables   ,1},
												 {" Graph mode   ",set_graphmode,1},
												 {" Graph        ",set_graph    ,1},
												 {" Save config  ",save_config  ,1},
												 {" View config  ",view_config  ,1},
												 {NULL            ,NULL         ,0}};

asChoice mnWind[]     = {{" Rectangular  ",NULL ,1},
												 {" Bartlett     ",NULL ,1},
												 {" Parzen       ",NULL ,1},
												 {" Hanning      ",NULL ,1},
												 {" Hamming      ",NULL ,1},
												 {" Blackman     ",NULL ,1},
												 {" Flat top     ",NULL ,1},
												 {NULL            ,NULL ,0}};

asChoice mnCursor[]     = {{" Harmonic    ",NULL ,1},
													 {" Sideband    ",NULL ,1},
													 {" Delta       ",NULL ,1},
													 {NULL           ,NULL ,0}};

asChoice mnGraph[]   = {{" Allow pick   ",NULL ,1},
												{" Forbid pick  ",NULL ,1},
												{" RMS range    ",NULL ,1},
												{" Autorange    ",NULL ,1},
												{NULL            ,NULL ,0}};

asChoice mnGraphmode[]   = {{" BW 640 x 200 ",NULL ,1},
														{" BW 640 x 480 ",NULL ,1},
														{NULL            ,NULL ,0}};

asChoice mnTables[]  = {{" Approximation",NULL ,1},
												{" Jump         ",NULL ,1},
												{NULL            ,NULL ,0}};

#else

asChoice mnOptions[]  = {{" ��� �����  ",cursortype   ,1},
												 {" ��� ����     ",windowtype   ,1},
												 {" 3-� �. �஥��",set_gips     ,1},
												 {" ��� �ਡ��  ",set_rec_type ,1},
												 {" ���� ������  ",set_port     ,1},
												 {" �������      ",set_tables   ,1},
												 {" ����. ०��",set_graphmode,1},
												 {" ��䨪 �����",set_graph    ,1},
												 {" ���࠭. ����.",save_config  ,1},
												 {" ��ᬮ�� ����",view_config  ,1},
												 {NULL            ,NULL         ,0}};

asChoice mnWind[]     = {{" ��אַ㣮�쭮�",NULL ,1},
												 {" ���⫥��    ",NULL ,1},
												 {" ��৥��      ",NULL ,1},
												 {" ��������     ",NULL ,1},
												 {" ��������     ",NULL ,1},
												 {" ��������     ",NULL ,1},
												 {" ���᪠� ����.",NULL ,1},
												 {NULL            ,NULL ,0}};

asChoice mnCursor[]     = {{" ��ମ����   ",NULL ,1},
													 {" ������     ",NULL ,1},
													 {" �����      ",NULL ,1},
													 {NULL           ,NULL ,0}};

asChoice mnRec_type[]     = {{" ��90        ",NULL ,1},
														 {" ��90.59�    ",NULL ,1},
														 {NULL           ,NULL ,0}};

asChoice mnGraph[]   = {{" �뢮� pick   ",NULL ,1},
												{" ������. pick ",NULL ,1},
												{" �������� ��� ",NULL ,1},
												{" ������. ��� ",NULL ,1},
												{NULL            ,NULL ,0}};

asChoice mnGraphmode[]   = {{" �� 640 x 200 ",NULL ,1},
														{" �� 640 x 480 ",NULL ,1},
														{NULL            ,NULL ,0}};

asChoice mnTables[]  = {{" ���பᨬ���",NULL ,1},
												{" ���祪       ",NULL ,1},
												{NULL            ,NULL ,0}};
#endif

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

 was_c = help_context;
 help_context = CONTEXT_OPTIONS;
#ifdef ENGLISH
 submenu ("  Options  ",XBUTTONSIZE*2,0,mnOptions);
#else
 submenu (" ���䨣���� ",XBUTTONSIZE*2,0,mnOptions);
#endif

 help_context = was_c;
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
#ifdef ENGLISH
 j = submenu (" Window type ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnWind);
#else
 j = submenu (" ��� ����    ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnWind);
#endif
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
  case FTP: weightwindow = FTP;
			break;
 }
 help_context = was_c;
}


void cursortype (void)	// ��⠭���� ⨯� �����
{
 int was_c,j;
 was_c = help_context;
 help_context = CONTEXT_CURSORTYPE;

#ifdef ENGLISH
 j = submenu (" Cursor type ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnCursor);
#else
 j = submenu (" ��� ����� ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnCursor);
#endif
 switch (j)
 {
	case HARM: CUR_TYPE = HARM;
			break;
	case SIDE: CUR_TYPE = SIDE;
			break;
	case DELT: CUR_TYPE = DELT;
			break;
 }
 help_context = was_c;
}


#define GIPX_MIN (0)
#define GIPX_MAX (xhigh2-xlow1+XSPACE)
#define GIPY_MIN (0)
#define GIPY_MAX (ylow2-yhigh1+YSPACE*2)

void set_gips (void) 	/* ��⠭���� �ਥ��樨 �ᥩ � 3 ��୮� ��䨪� */
{                       /* �१ �����樥��� GIPX - ᬥ饭�� ��� ��㭪�� */
						/* �� �� � � GIPY -�� Y, �� �⮬ ���� ��䨪�� */
						 /* ��।����� �� ���� ������ ��� ����. ���頤� */
 char user;
 int y1,y2,x_a,x_b,x2,xlow1,xhigh1,ylow1,yhigh1,xlow2,xhigh2,ylow2,x1=0;

 y1=YBUTTONSIZE;
 y2=getmaxy()-YBUTTONSIZE;
 x2 = getmaxx();
 x_b = x2 - XDIGIT;
 x_a = x_b - XBUTTONSIZE;
 xhigh2 = x_a - YBUTTONSIZE*2;                   /* �ࠢ�� �࠭�� ��䨪�   */
 xhigh1 = xhigh2 - GIPX;                         /* �ࠢ�� �࠭�� ��䨪�   */
 xlow1  = x1 + XSPACE*2 + XDIGIT*3;              /* ���. �࠭�� ��䨪�     */
 xlow2  = xlow1 + GIPX;                          /* ���. �࠭�� ��䨪�     */
 yhigh1 = y1 + YBUTTONSIZE*2 + YDIGIT + YSPACE*2;/* ���� ��䨪�             */
 ylow1  = y2 - GIPY - YDIGIT*2 - YSPACE*2;       /* ��� ��䨪�              */
 ylow2  = ylow1 + GIPY;                          /* ��� ��䨪�              */

#ifdef ENGLISH
 setframe (x1,y1,x2,y2," Graphic orientation set:");
#else
 setframe (x1,y1,x2,y2," ��⠭���� �ਥ��樨 �ᥩ:");
#endif
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2            ,  "     Left   Right");
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2+YBUTTONSIZE,  "Ctrl Left   Right");
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2+YBUTTONSIZE*2,"     Home   End");
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2+YBUTTONSIZE*4,"     Up     Down");
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2+YBUTTONSIZE*5,"Page Up     Down");
 gprintfxy (xhigh2+XBUTTONSIZE/5,yhigh1-YSPACE*2+YBUTTONSIZE*7,"Enter   =   Quit");
 rectangle (xhigh2+XBUTTONSIZE*0.1,yhigh1-YSPACE*2,xhigh2+XBUTTONSIZE*0.1+110,yhigh1-YSPACE*2+YBUTTONSIZE*3);
 rectangle (xhigh2+XBUTTONSIZE*0.1,yhigh1-YSPACE*2+YBUTTONSIZE*4,xhigh2+XBUTTONSIZE*0.1+110,yhigh1-YSPACE*2+YBUTTONSIZE*6);
 rectangle (xhigh2+XBUTTONSIZE*0.1,yhigh1-YSPACE*2+YBUTTONSIZE*7,xhigh2+XBUTTONSIZE*0.1+110,yhigh1-YSPACE*2+YBUTTONSIZE*8);
 rectangle (xhigh2+XBUTTONSIZE*0.1-3,yhigh1-YSPACE*2-3,xhigh2+XBUTTONSIZE*0.1+113,yhigh1-YSPACE*2+YBUTTONSIZE*8+3);

 do
 {
 bar (xlow1-XSPACE,ylow2,xhigh2,yhigh1-YSPACE*2);	  	/*��࠭�� �ࠢ��� ��䨪� */

 xhigh1 = xhigh2 - GIPX;                         /* �ࠢ�� �࠭�� ��䨪�   */
 xlow2  = xlow1 + GIPX;                          /* ���. �࠭�� ��䨪�     */
 ylow1  = y2 - GIPY - YDIGIT*2 - YSPACE*2;       /* ��� ��䨪�              */
 ylow2  = ylow1 + GIPY;                          /* ��� ��䨪�              */

 rectangle (xlow1-XSPACE,ylow1,xhigh1,yhigh1-YSPACE*2);
 line (xlow1,ylow1,xlow2,ylow2);
 line (xlow2,ylow2,xhigh2,ylow2);
 line (xhigh2,ylow2,xhigh1,ylow1);

	user = rkey ();
	switch (user)
	{
   case KB_N_RIGHT    :  if (GIPX>GIPX_MIN) 	 GIPX--;
						break;
	 case KB_N_LEFT    :  if (GIPX<GIPX_MAX)	 GIPX++;
						break;
   case KB_N_UP      :  if (GIPY<GIPY_MAX)	 GIPY++;
						break;
	 case KB_N_DOWN    :  if (GIPY>GIPY_MIN)	 GIPY--;
						break;
	 case KB_N_PGUP    :  GIPY=(GIPY<GIPY_MAX-10) ?  GIPY+10 : GIPY_MAX;
						break;
	 case KB_N_PGDN    :  GIPY=(GIPY>GIPY_MIN+10) ?  GIPY-10 : GIPY_MIN;
						break;
	 case KB_C_RIGHT   :  GIPX=(GIPX>GIPX_MIN+10) ? GIPX-10 : GIPX_MIN;
						break;
	 case KB_C_LEFT    :  GIPX=(GIPX<GIPX_MAX-10) ? GIPX+10 : GIPX_MAX;
						break;
	 case KB_N_END     :  GIPX=GIPX_MIN;
						break;
   case KB_N_HOME    :  GIPX=GIPX_MAX;
						break;
   case KB_N_ESC     :
	 case KB_N_ENTER   :	user=KB_N_ESC;
	}
 }
 while (user!=KB_N_ESC);

 killframe ();
}



void set_rec_type (void) /* ��⠭���� ⨯� �ਡ�� (� ᫥����⥫쭮 */
{                        /* �ଠ� �����), � ���ண� �������� ����� */
 int j,was_c;
 extern int REC_TYPE;

 was_c = help_context;
 help_context = CONTEXT_DATAREC_TYPE;

#ifdef ENGLISH
#else
 j=submenu (" ����� ��:   ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE*2,mnRec_type);
#endif
 if (j>-1)
	REC_TYPE=j;
 help_context = was_c;
}





void set_port (void)
{
 int j,was_c;
 extern int connect_port;

 was_c = help_context;
 help_context = CONTEXT_DATAPORT;

#ifdef ENGLISH
 j=submenu (" Connect to: ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE*2,mnCPort);
#else
 j=submenu (" ����� �१: ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE*2,mnCPort);
#endif
 if (j>-1)
	connect_port=j;
 help_context = was_c;
}



void set_tables (void)
{
 int j,was_c,err;
 extern int N,K,M,L;
 char buffer[10];

 was_c = help_context;
 help_context = CONTEXT_TABLES;

 j = submenu (" �������      ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnTables);

 switch (j)
 {
	case 0: setframe (40,50,500,110," ������ �᫮ �祪 ���பᨬ�樨 � ��ਮ� �।�०����� (���): ");
					strcpy (buffer,itoa(N,buffer,10));
					err=editstring (100,80,4,buffer);
					if (err!=-1)
						N = atoi(buffer);
					if (N<2)	N = 3;
					strcpy (buffer,itoa(L,buffer,10));
					err=editstring (280,80,4,buffer);
					if (err!=-1)
						L = atoi(buffer);
					if (L<1)	L = 1725;
					break;

	case 1: setframe (40,50,500,110," ������ �᫮ �祪 ��।����� � �஢��� �।�०����� (%): ");
					strcpy (buffer,itoa(M,buffer,10));
					err=editstring (100,80,4,buffer);
					if (err!=-1)
						M = atoi(buffer);
					if (M<2)	M = 3;
					strcpy (buffer,itoa(K,buffer,10));
					err=editstring (280,80,4,buffer);
					if (err!=-1)
						K = atoi(buffer);
					if (K<1)	K = 100;
 }

 killframe();
 help_context = was_c;
}




void set_graphmode (void)	// ��⠭���� ����᪮�� ०���
{
 extern int	far G_DRIVER, G_MODE;
 int was_c,j,was_gd,was_gm,i;

 was_c = help_context;
 help_context = CONTEXT_GRAPHMODE;

 was_gd = G_DRIVER;
 was_gm = G_MODE;

#ifdef ENGLISH
 j = submenu (" Graph mode ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnGraphmode);
#else
j = submenu (" ����. ०��",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnGraphmode);
#endif

 switch (j)
 {
	case 0: G_DRIVER = CGA;
					G_MODE   = CGAHI;
			break;

	case 1: G_DRIVER = MCGA;
					G_MODE   = MCGAHI;
			break;
 }
 if ((G_DRIVER==was_gd)&&(G_MODE==was_gm)) // �᫨ ०�� �� ���������
		help_context = was_c;
 else													// �᫨ ���������
		{
		if (G_DRIVER==CGA)	GIPY *= (200.0-79)/(480.0-121);	// �ਢ���� � ᮮ⢥��⢨�
		else	GIPY *= (480.0-121)/(200.0-79);		// �-�� ��� ��嬥�. ��䨪�
														// ������� �� ����. ࠧ�襭�� � �� ࠧ��� ᨬ����� ����
		for (i=0;i<30;i++)	killframe();			// ����뢠�� ���
		closegraph ();
		save_config ();   // �㤥� ������ ��䥪�, ������ ����� ���䨣��樨, � �� ⮫쪮 �����०���
		execl (fullpath("anlzr.exe"),fullpath("anlzr.exe"),NULL);	// ������ �����
		}
}


void set_graph (void)
{
 int was_c,j;
 extern int PICK;
 extern double MRS_BEG,MRS_END;
 extern double AUTO_BEG,AUTO_END;

 was_c = help_context;
 help_context = CONTEXT_GRAF;

#ifdef ENGLISH
 j = submenu (" Graph sets ",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnGraph);
#else
 j = submenu (" ��䨪 �����",(int)XBUTTONSIZE*2+XSPACE*10,1+YBUTTONSIZE*2,mnGraph);
#endif
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
  case 3: editfloat2 (&AUTO_BEG,&AUTO_END);
					if (AUTO_BEG<0)
           AUTO_BEG = 0;
          if (AUTO_END<0)
           AUTO_END = 0;
					break;
 }
 help_context = was_c;
}


void save_config (void)
{
 FILE *config=NULL;
 extern int connect_port,port,mode,margin,GIPX,GIPY,REC_TYPE;
 extern int far G_DRIVER,G_MODE;
 extern double MRS_BEG,MRS_END;
 extern double AUTO_BEG,AUTO_END;
 extern enum asWindType weightwindow;
 extern enum asCurType CUR_TYPE;

 config = fopen (fullpath("anlzr.cfg"),"wb");
 if (config!=NULL)
 {
	fwrite (&connect_port,sizeof(int)       ,1,config);
  fwrite (&port        ,sizeof(int)       ,1,config);
  fwrite (&mode        ,sizeof(int)       ,1,config);
	fwrite (&margin      ,sizeof(int)       ,1,config);
	fwrite (&weightwindow,sizeof(int)       ,1,config);
  fwrite (&GIPX        ,sizeof(int)       ,1,config);
  fwrite (&GIPY        ,sizeof(int)       ,1,config);
  fwrite (&N           ,sizeof(int)       ,1,config);
  fwrite (&K           ,sizeof(int)       ,1,config);
	fwrite (&M           ,sizeof(int)       ,1,config);
	fwrite (&L           ,sizeof(int)       ,1,config);
	fwrite (&PICK        ,sizeof(int)       ,1,config);
	fwrite (&MRS_BEG     ,sizeof(double)    ,1,config);		/* ��砫� � ����� � �� */
	fwrite (&MRS_END     ,sizeof(double)    ,1,config);   // ��������� ���� RMS
	fwrite (&AUTO_BEG     ,sizeof(double)    ,1,config);	// � �� ��� ��⮭�ନ஢����
	fwrite (&AUTO_END     ,sizeof(double)    ,1,config);  // ��䨪� �� ����
	fwrite (&CUR_TYPE     ,sizeof(int)       ,1,config);	// ⨯ ᯥ�����
	fwrite (&G_DRIVER     ,sizeof(int)       ,1,config);	// ����᪨� �ࠩ���
	fwrite (&G_MODE       ,sizeof(int)       ,1,config);  // � ०��
	fwrite (&REC_TYPE     ,sizeof(int)       ,1,config);  // ⨯ �����.�ਡ��
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
 extern int connect_port,port,mode,margin,GIPX,GIPY,N,M,K,L,PICK,G_DRIVER,G_MODE;
 extern int REC_TYPE;
 extern enum asCurType CUR_TYPE;
 extern enum  asWindType   weightwindow;
 extern double MRS_BEG,MRS_END;
 extern double AUTO_BEG,AUTO_END;   /* �������� �����⠭���� ����⠡� */
 extern enum asWindType weightwindow;
 extern asChoice mnQual[], mnHeig[], mnRec_type[];
 int x1,y1,x2,y2,x1_2,cur_y=0,i;
 char tmp1[30],tmp2[30];

 x1 = XBUTTONSIZE;
 y1 = 0;
 x2 = x1 + XBUTTONSIZE*4;
 y2 = y1 + (YDIGIT+YSPACE*2)*20+3;
 x1_2 = x1+(x2-x1)*0.66;		// ��砫� ����� ���祭�� ��ࠬ��஢

#ifdef ENGLISH

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

#else

 setframe (x1,y1,x2,y2," ������ ���䨣����: ");

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*3)*(++cur_y) ,
																	" ����᪨� ०�� ��ᯫ��:");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*4)*cur_y ,"%s %d x %d",
										G_DRIVER == VGA ? "�� ":"�� ",getmaxx()+1,getmaxy()+1);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y)," ���� ����� ������:");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,
										connect_port ? "COM2":"COM1");

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y)," ��� �ਡ�� �����饣� �����:");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,mnRec_type[REC_TYPE].text+1);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y)," �뢮� �� �ਭ��:");
 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -����");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"LPT%d",port+1);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -����� ��䨪�");
 switch(mode&0x0ff)
		{case   0:        i=0; break;
		case    5:        i=1; break;
		case    4:        i=2; break;
		case    6:        i=3; break;
		case    1:        i=4; break;
		case    2:        i=5; break;
		case    3:        i=6; break;
		case   32:        i=7; break;
		case   38:        i=8; break;
		case   33:        i=9; break;
		case   39:        i=10; break;
		case   40:        i=11; break;
		default:          i=4;
		}
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,mnQual[i].text+2);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -���� ��䨪�");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,mnHeig[mode>>8].text+2);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -����� ᫥��");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%d ����",margin);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y)," ��ᬮ�� �����:");
 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -�������� ��⮭�ନ஢����");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%.1f - %.1f ��",AUTO_BEG,AUTO_END);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -�������� ���� ���");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%.1f - %.1f ��",MRS_BEG,MRS_END);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -������ �஢���");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,PICK ? "�뢮����":"�� �뢮����");

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -⨯ �����");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,mnCursor[CUR_TYPE].text+1);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -⨯ ���.���� ��� ���� ��-���");
 outtextxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,mnWind[weightwindow].text+1);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y)," ��ᬮ�� �७���:");
 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -����࠯����� �� ��᫥����");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%d ����ࠬ",N);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -�।�०����� �� �����, 祬 ��");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%d ��⮪",L);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -᪠祪 ��.�।���� �� ��᫥����");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%d ����ࠬ",M);

 outtextxy (x1+XSPACE*3,y1+(YDIGIT+YSPACE*2)*(++cur_y),"     -�।�०����� �� ᪠窥 �����");
 gprintfxy (x1_2,y1+(YDIGIT+YSPACE*2)*cur_y ,"%d %%",K);

 while (rkey()!=27);
 killframe ();
#endif
}
