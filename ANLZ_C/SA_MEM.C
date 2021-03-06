/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : SA_MEM
* Describe  : ������ ������ � �ਡ�஬ � ࠡ�� � �������
* File      : C:\ALEX\WORK\SA_MEM.C
* Last edit : 15.04.93 00.29.51am
* Remarks   : �ਭ��� ���ଠ�� ����� ���� ����� 64 �����, ��� ����樨
*           : ⠪��� ���ᨢ� �ᯮ������� ��� 㪠��⥫�, �� ��� ࠧ���
*           : ᥣ����.
*           : ���ᨬ��쭮� �᫮ ����⮢ ᨣ���� - 2048 ����
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
* Describe : �㭪�� �⥭�� ����� ��᮪��� �஢��
* Params   : asHeadPtr *hdr - 㪠�. �� 㪠�. �� asHeader
*          : void (* func)(double) - �㭪�� ��� �⮡ࠦ���� ���뢠���, ��
*          :                         ��।��� ��業� ��⠭��� ���ଠ樨
* Return   : asRecPtr  *    - ���ᨢ 㪠��⥫�� �� ����� � �����
* Call     : getdump,numrec,addr
***********/
{                                                           /*               */
 uint last,i,j,n,off;                                       /*               */
 long size;                                                 /*               */
 byte huge *f_s,huge *s_s;
 asRecPtr huge *rec = NULL;                                 /* ���ᨢ 㪠�.  */
 static byte tmp[2048];
                                                            /* �� ������     */
 *hdr = (asHeadPtr) getdump (&size,func);                   /* �ਭ�����     */
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
 f_s = (byte huge *) (*hdr);                                /* 1 ��������    */
 s_s = (byte huge *) (*hdr)+65536L;                         /* 2 �������� �� */
 for (i=0;i<((*hdr)->numrec)-1;i++)                         /* 64 �����     */
 {
  drawprogress ( (double) i / (((*hdr)->numrec) - 1) );

  off = (uint)((*hdr)->addr)[i];                              /* ᬥ饭��    */
  if ((uint)off>(uint)65534)                      /* ��ॠ��� ���� ����� */
  {
   rec[i] = (asRecPtr) ((byte huge *)NULL);
   continue;
  }

  if ((uint)off>(uint)32767)                                  /* �� 2 ���.   */
   rec[i] = (asRecPtr) ((byte huge *)s_s+((uint)off-32768)*2);/*             */
  else                                                        /* � 1 ���.    */
   rec[i] = (asRecPtr) ((byte huge *)f_s+(uint)off*2);        /*             */

  if (rec[i]->number > 2048)
   rec[i]->number = 2048;
  if ((issignal(rec[i]))&&(rec[i]!=NULL))
  {
   rec[i]->number = rec[i]->dimfft;
   for (n=0;n<(rec[i])->number;n+=2)           /* ��� ����� */
    tmp[n] = ((byte *)rec[i]->y)[n/2];
   for (n=1;n<(rec[i])->number;n+=2)           /* ����� ����� */
    tmp[n] = ((byte *)rec[i]->y)[(uint)((rec[i])->number+n)/2];
   memcpy ((byte *)rec[i]->y,(byte *)tmp,(size_t)(rec[i])->number);
  }
  else
   if (rec[i]!=NULL)
    rec[i]->mnoj-=3;

  rec[i]->res1 = 0;  /* �㫥�� ����� ��᫥ ����� */

  for (last=7;((last>0)&&((rec[i]->name)[last]==' '));last--)
   ;         /* ��諨 ��᫥���� �㪢� - �� �஡�� */

  for (;((last>0)&&((rec[i]->name)[last]!=' '));last--)
   ;         /* ��諨 ��᫥���� �஡�� */

  if (last!=0) /* �᫨ �� �� ��諨 */
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
* Describe : �ਭ��� ���� ����� �ਡ�� � ������ ���
* Params   : word *size -  ࠧ��� �ਭ���� ������ (� �����), ����� 祬
*          :               ࠧ��� �뤥������ �����, ⠪ ��� �� ��஢���
*          :               �� �࠭��� 1024 ����
*          : PFV func    - �㭪�� ��� �⮡ࠦ���� ���뢠���, ��
*          :               ��।��� ��業� ��⠭��� ���ଠ樨
* Return   : byte huge*  - 㪠��⥫� �� ����, �㤠 ����ᠭ� �����
* Call     : input,waitstart,setaddr,getKbyte,bioscom
***********/
{                                                  /* i-⥪�騩 ����        */
 unsigned long volume,i_long;
 uint i=0,Imax=1024;                               /* Imax-����. ����       */
 byte bufer[1024];                                 /*                        */
 int err=0,rpt=0;                                  /*                        */
 byte huge *ppm=NULL,huge *cur=NULL;               /* 㪠��⥫� �� ����     */
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
   func ((double)((double)i_long/(double)Imax));   /* ��㥬 �ண���        */
  do                                               /*                        */
  {                                                /*                        */
   if (err<=8)                                     /* �᫨ �� �訡�� � �᫨  */
    if (i==0)                                      /* ���� ����� �        */
     err=getKbyte(bufer);                          /* �ਭ����� � ����      */
    else                                           /* ����                  */
    {
     err=getKbyte(cur);                            /* �ਭ����� � �����祭�� */
     if (*bufer!=*ppm)
     {
      printf ("\n There is problem, count = %u",(uint)i);
      abort ();
     }
    }
   if (err>8)                                      /* �᫨ �訡��            */
   {                                               /*                        */
    rpt++;                                         /* �᫮ ����஢         */
    if (rpt>REPEATS)                               /*                        */
    {                                              /*                        */
     if (ppm!=NULL)                                /* �᢮������� ������     */
      farfree ((byte far *)ppm);                   /* �᫨ �뤥����          */
     return (NULL);                                /* �����頥� �訡��      */
    }                                              /*                        */
   }                                               /*                        */
  }                                                /*                        */
  while (err>8);                                   /*                        */
                                                   /*                        */
  if (i==0)                                        /* �᫨ ���� �����      */
  {                                                /* �                     */
   Imax=(uint) bufer[8]+bufer[9]*256;              /* ����塞 Imax         */
   *size=(long) Imax*2L;                           /* ࠧ��� � �����        */
   volume=(unsigned long) (*size)+1024L;           /* ��ࠢ����� �� 1024 �  */
   ppm=(byte huge*) farcalloc(volume,sizeof(byte));/* �뤥�塞 ������        */
   cur = ppm;
   if (ppm==NULL)
   {
    printf ("\n Imax = %u, size = %ld",Imax,(*size));
    printf ("\n No enough memory! Function GETDUMP. ");
    printf ("\n %ul bytes free. ",farcoreleft());
    abort ();
   }
   memcpy ((byte *)ppm,bufer,1024);                /* �����㥬 �� ����     */
  }                                                /*                        */
 }                                                 /*                        */
 if (func!=NULL)                                   /*                        */
  func  ((double)1.0);                             /* ��㥬 �ண���        */
 fclose (fout);
 return ((byte huge*) ppm);                        /*                        */
}                                                  /*                        */
                                                   /*                        */
int input (byte huge *x)                           /*                        */
/***********
* Describe : �ਭ��� ���� �� ���� COM1
* Params   : byte *x - �ਭ��� ����
* Return   : int     - ��� �訡��
* Call     : bioscom
***********/
{                                                  /*                        */
 fread (x,sizeof(byte),1,fout);
 return 0;
}                                                  /*                        */
                                                   /*                        */
int waitstart(void)                                /*                        */
/***********
* Describe : ���� ��ப� "STRT" �� �ਡ��
* Params   : void -
* Return   : int  - ��� �訡��
* Call     : input,strcmp
***********/
{                                                  /*                        */
 return 0;
}                                                  /*                        */
                                                   /*                        */
void setaddr(uint x)                               /*                        */
/***********
* Describe : ��⠭�������� ���� ᫥���饣� �������� ����� ��� �ਥ��
* Params   : word x - ���� � ᫮��� (� �����*2)
* Return   : void
* Call     : bioscom
***********/
{                                                  /*                        */
 return;
}                                                  /*                        */
                                                   /*                        */
int getKbyte (byte huge *a)                        /*                        */
/***********
* Describe : �ਭ��� �������� ����� �� �ਡ��
* Params   : byte *a - ���� �ਥ��
* Return   : int     - ��� �訡��
* Call     : input
***********/
{                                                  /*                        */
 fread (a,sizeof(byte),1024,fout);
 return 0;
}                                                  /*                        */

#define BIT7  0x80    /* ��� 7 (2^7=128=0x80) */

char *modestr (asRecPtr r)
/***********
* Describe : ��।������ ०���
* Params   : asRecPtr r
* Return   : char        - ��ப�-०��
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
* Describe : ��।������ ����� ०���
* Params   : asRecPtr r
* Return   : int        - ����� ०���
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
* Describe : ��।������ �����. �ᨫ����
* Params   : asRecPtr r
* Return   : int        - �����. �ᨫ����
* Call     :
***********/
{
 return ceil(((int)r->gain)*.25);
}

int afreqnum (asRecPtr r)
/***********
* Describe : ��।������ �।��� �����
* Params   : asRecPtr r
* Return   : int        - �।��� ���� � ��
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
* Describe : ��।������ ��. ���.
* Params   : asRecPtr r
* Return   : char       - ��. ���.
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
* Describe : ��⥣�஢���� ��. ����७�� �� �� Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
 {
  r->yunit = BIT7;                  /* ��⠭����� ᪮���� */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->mode[0] = 'm';                 /* ��⠭����� ����� */
  return 0;
 }
 return (1);
}

char *xunitstr (asRecPtr r)
/***********
* Describe : ��।������ ��. ���.
* Params   : asRecPtr r
* Return   : char       - ��. ���.
* Call     :
***********/
{
 return ( isspectrum(r) ? " Hz " : " msec " );
}

int windnum (asRecPtr r)
/***********
* Describe : ��।������ ����� ���� (0-���.,1-��������)
* Params   : asRecPtr r
* Return   : int        - ����� ����
* Call     :
***********/
{
 return (r->wind!=0);
}

int isspectrum (asRecPtr r)
/***********
* Describe : ��।������ ᯥ���? (1-��,0-���)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ((r->type) & BIT7) >> 7 );
}

int issignal (asRecPtr r)
/***********
* Describe : ��।������ ᨣ���? (1-��,0-���)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return ( ( ((r->type) & BIT7) >> 7 ) == 0);
}

int islinear (asRecPtr r)
/***********
* Describe : ��।������ ������� ����⠡? (1-��,0-���)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale==0);
}

int islogariphm (asRecPtr r)
/***********
* Describe : ��।������ �����䬨�᪨� ����⠡? (1-��,0-���)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 return (r->scale!=0);
}

void setspectrum (asRecPtr r)
/***********
* Describe : ��⠭���� : ᯥ���
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = BIT7;
}

void setsignal (asRecPtr r)
/***********
* Describe : ��⠭���� : ᨣ���
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->type) = 0;
}

void setlinear (asRecPtr r)
/***********
* Describe : ��⠭���� : ������� ����⠡
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = 0;
}

void setlogariphm (asRecPtr r)
/***********
* Describe : ��⠭���� : �����䬨�᪨� ����⠡
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 (r->scale) = BIT7;
}

int dec_yunit (asRecPtr r)
/***********
* Describe : ����७樨஢���� ��. ����७�� �� �� Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (!strcmp(yunitstr(r)," m/ss "))
  return (1);
 if (!strcmp(yunitstr(r)," mm/s "))
 {
  r->yunit = 0;                     /* ��⠭����� �᪮७�� */
  return 0;
 }
 if (!strcmp(yunitstr(r)," mkm "))
 {
  r->mode[0] = 'a';                  /* ��⠭����� ᪮���� */
  r->mode[1] = 's';
  r->yunit = BIT7;
  return (0);
 }
 return (1);
}

void calckoeff (asRecPtr r,float *A,float *B)
/***********
* Describe : ������ �����樥�⮢
* Params   : asRecPtr r
*          : float *A   - 1 ����.
*          : float *B   - 2 ����.
* Return   : void
* Call     :
***********/
{
 float Z=0,T=0;

 if (isspectrum(r))      /* �᫨ ᯥ��� */
  Z = (float) r->mnoj1+r->mnoj;
 else
  Z = (float) r->mnoj1;

 if ((modenum(r)==1)||(modenum(r)==3))  /* �᫨ �室 �ᨫ�⥫� ���鸞 */
  T = (float) r->kdbch;
 if ((modenum(r)==0)||(modenum(r)==2))  /* �᫨ ������� �室 */
  T = (float) r->kdbln;

 if (issignal(r))    /* �᫨ ᨣ��� */
  *B = (float) -128;
 if (isspectrum(r)&&islinear(r))  /* �᫨ ᯥ��� � ����⠡ ������� */
  *B = (float) 0;
 if (isspectrum(r)&&islogariphm(r))  /* �᫨ ᯥ��� � ����⠡ ������. */
  *B = (float) 4*Z+T+14;

 if (islogariphm(r))   /* �᫨ ����⠡ ������. */
  *A = (float) .25;
 else             /* �᫨ ����⠡ ������� */
  if ((modenum(r)==1)||(modenum(r)==3))   /* �� �室� �ᨫ�⥫� ���鸞 */
   *A = (float) (7500/(float)r->kofch)*exp(Z*log(10)/20);
  else                    /* �� �������� �室� */
   *A = (float) (7500/(float)r->kofln)*exp(Z*log(10)/20);
}