/*****************************************************************************
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