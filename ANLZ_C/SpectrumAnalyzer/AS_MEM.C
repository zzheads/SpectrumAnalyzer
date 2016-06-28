/*****************************************************************************
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
//#include <bios.h>                                  /*                        */
#include <dos.h>
#include <string.h>                                /*                        */
#include <alloc.h>                                 /*                        */
#include <math.h>
#include "as_mem.h"                                /*                        */

void drawprogress (double);
int connect_port=0;            // определяет номер порта 0 - СОМ1, 1 - СОМ2
unsigned base_addr;            // будет хранить базовый адрес порта

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
 reportmemory(getmaxx(),getmaxy()-textheight("X")-5);
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
																				 // считываем базовый адрес порта
 base_addr = *((unsigned far *)((unsigned far *)0x00400000L+connect_port));
 if (base_addr==0)	              			 // СОМ1 в 40:00 СОМ2 в 42:00
	{printf("\nCOM-%d is absent!",connect_port);
	return 0;
	}
 // bioscom (0,SETTINGS,connect_port);       /* инициализируем COM    */
 outportb (base_addr+3,0x83);	              // 8byt 1stop N
 outportb (base_addr,12);                   // скорость обмена 115200/12=9600
 outportb (base_addr+1,0);
 outportb (base_addr+3,0x03);                // конец установки скорости
 empty = inportb (base_addr+4);
 outportb (base_addr+4,0);                   // Запрет прерывания от порта

 for (i_long=0L;i_long<Imax;i_long+=512L,cur=(byte huge *)cur+1024,i=(uint)i_long)
 {
	if (func!=NULL)                                  /*                        */
	 func ((double)((double)i_long/(double)Imax));   /* рисуем прогресс        */
	sleep (1);
	do                                               /*                        */
	{                                                /*                        */
	 setaddr(i);                               /* посылаем адрес начала  */
	 err=waitstart();                          /* ждем строки "STRT"     */
	 if (err==0)                                     /* если не ошибка то      */
		err=input(&empty);                             /* принимаем пустой байт  */
	 if (err==0)                                     /* если не ошибка и если  */
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
	 if (err != 0)                                   /* если ошибка            */
	 {                                               /*                        */
		rpt++;                                         /* число повторов         */
		if (rpt>REPEATS)                               /*                        */
		{                                              /*                        */
		 if (ppm!=NULL)                                /* освобождаем память     */
			farfree ((byte far *)ppm);                   /* если выделена          */
			printf("\nCommunication ERROR %X",(unsigned)err);
		 return (NULL);                                /* возвращаем ошибка      */
		}                                              /*                        */
	 }                                               /*                        */
	}                                                /*                        */
	while (err!=0);                                  /*                        */
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
	 reportmemory(getmaxx(),getmaxy()-textheight("X")-5);
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
 int p_stat,err=0;                                /*                        */

 do                                                /*                        */
 {                                                 /*                        */
	//p_stat = bioscom(3,0,connect_port);             //читаем состояние порта
	p_stat = inportb (base_addr+5);
	err1++;                                          /*                        */
	if (err1>32000)                                  /*                        */
	{                                                /*                        */
	 err1=0;                                         /*                        */
	 err++;                                          /*                        */
	 if (err>19)                                     /* если порт не готов     */
		{if (p_stat==0)	p_stat = 0x80;                // после 32000 х 19 чтений
		return p_stat;                                 /* выход по ошибке !=0    */
		}
	}                                                /*                        */
 }                                                 /*                        */
 while ((p_stat&1)==0);                         /* пока не готовы данные  */

 //p_stat = bioscom(2,0,connect_port);          /* принимаем байт в млад- */
 //*x = p_stat & (0xFF);
 *x = inportb (base_addr);
 return 0;                                    /* ших байтах слова       */
}

void out (char x)                               /*                        */
/***********
* Describe : передает в прибор один байт
* Params   : char - передаваемый байт
* Return   : void
* Call     : outportb inportb
***********/
{                                                  /*                        */
 int i=0;
																									// ждем пока опустеют
 while ((inportb (base_addr+5) & 0x60) != 0x60);	// буф.и сдвиг. регистры порта
 while (i++ < 20000);	                            // ждем еще
 outportb (base_addr,x);                          // передаем
}

int waitstart(void)
/***********
* Describe : ждет строки "STRT" от прибора
* Params   : void -
* Return   : int  - код ошибки
* Call     : input,strcmp
***********/
{                                                  /*                        */
 char bst[5]={' ',' ',' ',' ',0};                  /*                        */
 byte b;                                           /*                        */
 int i,err,count=0;                                /*                        */
																									 /*                        */
 do                                                /*                        */
 {                                                 /*                        */
	for (i=0;i<3;i++)	bst[i]=bst[i+1];               /*                        */
	err=input(&b);                                   /*                        */
	if (err !=0)	                                   /*                        */
		{if (count > 100)	return err;
		count++;
		continue;
		}
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
 //int p_stat;

 // bioscom(1,40,connect_port);                       /*                        */
 // bioscom(1,40,connect_port);                       /* сброс порта COM1       */
 // bioscom(1,40,connect_port);                       /*                        */
 // bioscom(1,'A',connect_port);                      /* передаем A             */
 // bioscom(1,'D',connect_port);                      /*          D             */
 out ((char)0x28);
 out ((char)0x28);
 out ((char)0x28);
 out ('A');
 out ('D');
 y=x % 256;                                        /*                        */
 //p_stat = bioscom(1,y,connect_port);               /* младший байт адреса    */
 out (y);
 //if((p_stat & 0x8E00) != 0)	return p_stat;
 s=y;                                              /*                        */
 y=x >> 8;                                         /*                        */
 s=s+y;                                            /*                        */
 //p_stat = bioscom(1,y,connect_port);                        /* старший байт           */
 out (y);
 //if((p_stat & 0x8E00) != 0)	return p_stat;
 s=s&255;                                          /*                        */
 //p_stat = bioscom(1,s,connect_port);                        /* контрольная сумма      */
 out (s);
 //if((p_stat & 0x8E00) != 0)	return p_stat;
 //return 0;
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
	if (err==0)		err = input(a+i);                  /*                        */
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

#ifdef ENGLISH
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

#else
 switch (r->mode[0])
 {
	case 'a' : if (r->mode[1]=='l')       strcpy (tmp,"вх-Лин ");
						 if (r->mode[1]=='z')       strcpy (tmp,"вх-Усл ");
						 break;
	case 'o' : if (r->mode[1]=='l')       strcpy (tmp,"Огб-Лин ");
						 if (r->mode[1]=='z')       strcpy (tmp,"Огб-Усл ");
						 break;
	case 'd' : strcpy (tmp,"Тахо    ");
						 break;
	case 't' : strcpy (tmp,"Тест    ");
						 break;
	default  : strcpy (tmp,"Неизвес ");
	}

#endif
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
/* switch (modenum(r))		старая функция, нет перехода от м/сс к мм/с
 {                          при интегрировании графика
  case 4  :
	case 5  :
  case 0  :
  case 2  :  return (" mV ");
  case 1  :
  case 3  :  if ((r->yunit&0x80)&&(r->type&0x80))
			  return (" mm/s ");
			 else
			  return (" m/ss ");
  default :  return (" mkm ");
 }                                      */

						/* новая функция, где r->yunit: */
 switch (modenum(r))    /* 0-ускорение, 1-скорость, 2-смещение, */
 {                      /* 0x80-скорость при интегрировании в ПР-90  */
  case 4  :             /* а 0, 1 и 2 устанавливаются функциями */
  case 5  :             /* интегрирования и дифферинцирования */
  case 0  :
	case 2  :  return (" mV ");
	case 1  :
	case 3  :  if ((r->yunit==0)||((r->yunit&0x80)&&(r->type==0)))
								return (" m/ss ");
			 if ((r->yunit==1)||((r->yunit&0x80)&&(r->type&0x80)))
								return (" mm/s ");
			 if (r->yunit==2)	return (" mkm ");
	default :  return (NULL);
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
/*  r->yunit = BIT7;*/                  /* установили скорость */
  r->yunit=1;
  return 0;
 }
 if (!strcmp(yunitstr(r)," mm/s "))
 {
/*  r->mode[0] = 'm';  */               /* установили длину */
  r->yunit=2;
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
 return (  ((r->type) & BIT7)  == 0);
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
 /* r->mode[0] = 'a';   */               /* установили скорость */
/*  r->mode[1] = 's';
  r->yunit = BIT7;*/
  r->yunit=1;
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
