/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MAIN
* Describe  : ������ 䠩� ��ࠬ�� �����஀��������
* File      : C:\ALEX\WORK\AS_MAIN.C
* Last edit : 14.08.93 00.28.29
* Remarks   :
*****************************************************************************/

# include <mem.h>
# include <dos.h>
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
#include <time.h>

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
#include "m_export.h"
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
int help_context;
int Y_SIZEFONT;	// �-� ����⠡�஢���� ���� �� ���⨪��� �⭮�⥫쭮 8
int russian_font;
int	far G_DRIVER, G_MODE;

asHotKey hotkeys[]={{KB_N_F1 ,contexthelp   ,1},    /* 0 */
					{KB_N_F2 ,trend         ,1},    /* 1 */
					{KB_N_F3 ,loadbytime    ,1},    /* 2 */
					{KB_N_F4 ,cascade       ,1},    /* 3 */
					{KB_N_F5 ,print_screen  ,1},    /* 4 */
					{KB_N_F6 ,capture_screen,1},    /* 5 */
					{KB_N_F7 ,changedir     ,1},    /* 6 */
			{KB_N_F8 ,changetime    ,1},    /* 7 */
			{KB_A_O  ,options       ,1},    /* 8 */
				{KB_A_P  ,print         ,1},    /* 9 */
				{KB_A_X  ,beforequit    ,1},    /* 10 */
				{KB_LAST ,NULL          ,0}};   /* 11 */

#ifdef ENGLISH		// �������筠� �����

asChoice mnMain[]     = {{"     File       ",file      ,1},
												 {"    Analyze     ",analyze   ,1},
												 {"    Options     ",options   ,1},
												 {"     Print      ",print     ,1},
												 {"     Export     ",export    ,1},
												 {"     Help       ",help     ,1},
												 {NULL              ,NULL      ,0}};

asChoice mnStd[]      = {{"      Ok      ",NULL ,1},
												 {"    Cancel    ",NULL ,1},
												 {NULL            ,NULL ,0}};

#else				// ���᪮��筠� �����

asChoice mnMain[]     = {{"     ����       ",file      ,1},
												 {"    ��ᬮ��    ",analyze   ,1},
												 {"  ���䨣����  ",options   ,1},
												 {"    �ਭ��     ",print     ,1},
												 {"    ��ᯮ��     ",export    ,1},
												 {"    ������      ",help     ,1},
												 {NULL              ,NULL      ,0}};

asChoice mnStd[]      = {{"  �த������  ",NULL ,1},
												 {"   ��ࢠ��   ",NULL ,1},
												 {NULL            ,NULL ,0}};
#endif

#undef SECURITY

void main (int argc,char *argv[])
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/
{
 int err,dx,dy;
 long now;
 char *img_title,drive[MAXDRIVE],dir[MAXDIR];
 extern int connect_port,port,mode,margin,weightwindow,GIPX,GIPY,N,M,K,L,
			PICK,CUR_TYPE,REC_TYPE;
 extern double MRS_BEG,MRS_END,AUTO_BEG,AUTO_END;
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

/* russian_font=installuserfont("e:\bc\bgi\trip.chr");
 if ((err=graphresult())!=0)
 {
	printf ("\n Error : %s ",grapherrormsg(err));
	exit (-1);
 }                                                    */
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


#ifdef SECURITY /**********************************************************
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

 if (test()==0)
  destroy_program();************************************************/

#endif

/* �஢�ઠ BIOS ��⥬� �� �ࠢ����� � 䠩��� anlzr.lp */

 srand((int)time(&now)%37);    // �� ��砩���� ������ (�� �� ��� �����)
 if(rand()<10000&&prot()!=0)       // � 1/3 ��砥�
	{
	puts("\nNon registered copy !!!\n");
	if(rand()<3000)	destroy_program();	// � 1/11 ��砥�
	exit(-1);
	}

 config = fopen (fullpath("anlzr.cfg"),"rb");   /* reed file config */
 if (config!=NULL)
 {
	fread (&connect_port,sizeof(int),1,config);
	fread (&port        ,sizeof(int),1,config);
  fread (&mode        ,sizeof(int),1,config);
  fread (&margin      ,sizeof(int),1,config);
	fread (&weightwindow,sizeof(int),1,config);
	fread (&GIPX        ,sizeof(int),1,config);
	fread (&GIPY        ,sizeof(int),1,config);
	fread (&N           ,sizeof(int),1,config);
	fread (&K           ,sizeof(int),1,config);
	fread (&M           ,sizeof(int),1,config);
	fread (&L           ,sizeof(int),1,config);
	fread (&PICK        ,sizeof(int),1,config);
	fread (&MRS_BEG     ,sizeof(double),1,config);  	/* �࠭��� ���⭮�� */
	fread (&MRS_END     ,sizeof(double),1,config);    /* ��������� RMS */
	fread (&AUTO_BEG     ,sizeof(double),1,config);   /* �࠭��� ���⭮�� */
	fread (&AUTO_END     ,sizeof(double),1,config);   /* ��������� ��⮭�ନ�. ��䨪� */
	fread (&CUR_TYPE    ,sizeof(int),1,config);				// ⨯ �����
	fread (&G_DRIVER    ,sizeof(int),1,config);				// ����.�ࠩ���
	fread (&G_MODE      ,sizeof(int),1,config);				// ����. ०��
	fread (&REC_TYPE    ,sizeof(int),1,config);				// ��� �������. �ਡ�� � �ଠ� �����
	fclose (config);
 }
 else
 {
	connect_port = 0;
	port = 0;
	mode = 0;
	margin=0;
	weightwindow = 0;
	GIPX = 20;
	GIPY = 30;
	N = 3;
	K = 2;
	M = 10;
	L = 1725;
	PICK = 1;
	MRS_BEG = 0.0;
	MRS_END = 20000.0;
	AUTO_BEG = 0.0;
	AUTO_END = 20000.0;
	CUR_TYPE = HARM;
	G_DRIVER = CGA;
	G_MODE = CGAHI;
	REC_TYPE = 0; // ��90
	}

 initgraph (&G_DRIVER,&G_MODE,"");	// ���樨�㥬 ��䨪�
 if (graphresult()!=0)             // �᫨ ��㤠筮, � � CGAHI ०���
	{G_DRIVER = CGA;                 // �஡㥬 ���樨஢��� ᭮��
	G_MODE = CGAHI;
	initgraph (&G_DRIVER,&G_MODE,"");
	if ((err=graphresult())!=0)
		{
		printf ("\n Error : %s ",grapherrormsg(err));
		exit (-1);
		}
	}
 cleardevice();

	settextstyle (SMALL_FONT,HORIZ_DIR,0);	// �롨ࠥ� ࠧ��� ����
 switch (getmaxy())
	{
	case	349:
	case	199:	Y_SIZEFONT = 8;
							break;
	case	479:	Y_SIZEFONT = 14;
	}
 setusercharsize (1,1,Y_SIZEFONT,8);		// � ��⠭�������� ���

 if ((title=fopen(fullpath("title.img"),"rb"))!=NULL)
 {									/* �뢮��� ���⠢�� �� 䠩�� */
	fread (&dx,1,sizeof(int),title);
	fread (&dy,1,sizeof(int),title);
	rewind (title);
	img_title = (char *) calloc (imagesize(0,0,dx,dy),sizeof(char));
	fread (img_title,imagesize(0,0,dx,dy),sizeof(char),title);
	dy = YBUTTONSIZE*2;		//���न��� ���孥� ��� ���⠢��
	putimage (0,dy,img_title,COPY_PUT);
	free (img_title);
	fclose (title);

	setfillstyle(EMPTY_FILL,BLACK);	/* ��ࠥ� ���� ����� ���ᨨ */
	bar(180,dy+54,250,dy+74);				/* � ��襬 ���� */
	settextstyle (SMALL_FONT,HORIZ_DIR,7);
	outtextxy(200,dy+50,"2.2");
	settextstyle (SMALL_FONT,HORIZ_DIR,5);
	outtextxy(315,dy+57,"A.Papin, A.Golovnev");
	outtextxy(315,dy+72,"fax.(8442) 69 71 98");

	settextstyle (SMALL_FONT,HORIZ_DIR,0);	// ����⠭�������� ࠧ��� ����
	setusercharsize (1,1,Y_SIZEFONT,8);
 }

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
}

int prot()
	{
   int		j,r1,r2;
   unsigned	mem[512],fl[512],i,tmp;
   FILE 		*fprot;

   fprot=fopen(fullpath("anlzr.lp"),"rb");
   if(fprot==NULL)
		{puts("\n������ ������ 䠩�");
      return(-1);
			}
	 for(j=5;j>=1;j--)
   	{
      srand(j*4729); 		/* ���樨஢���� ���.�ᥫ � �⥭�� �� BIOS */
		 for(i=0;i<512;i++)	mem[i]=peek(0xfe00,j*1024+i*2);
		for(i=0;i<512;i++)	/* ࠭�������� ���ᨢ� */
      	{
         r1=rand()/64;	r2=rand()/64;
				 tmp=mem[r1];	mem[r1]=mem[r2];	mem[r2]=tmp;
         }
		if(fread(fl,sizeof(fl),1,fprot)!=1)	/* �⥭�� �� 䠩�� */
				{puts("\n���� �� �⠥���");
				 fclose(fprot);
         return(-1);
	 }
      if(memcmp(mem,fl,sizeof(mem))!=0)	/* �ࠢ����� */
      	{
         fclose(fprot);
         return(-1);       /* �� ࠧ����� ������ -1, �ᯥ� 0 */
         }
      }
	 fclose(fprot);
	 return(0);
   }
