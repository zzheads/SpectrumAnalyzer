/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MATH
* Describe  : математические функции программы СпектроАнализатор
* File      : C:\ALEX\WORK\AS_MATH.C
* Last edit : 14.08.93 00.30.48
* Remarks   : разработка А.В.Папина и А.Э.Головнева
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alloc.h>
#include <string.h>
#include <graphics.h>
#include "m_option.h"
#include "as_mem.h"
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_math.h"

extern enum asWindType weightwindow;
extern asChoice mnStd[];

void get_window (float *d,int m)
/***********
* Describe : функция взятия весового окна
* Params   : double *d - массив значений сигнала/спектра
*          : int m     -
* Return   : void
* Call     :
***********/
{
 int i;
 int m10=m/2-1;

 switch(weightwindow)
 {
  case REC: break;
  case BAR: for(i=0;i<=m10;i++)
			{
			 d[i+m10+1]*=sqrt(3.0)*(1.0-(double)i/m10);
			 d[m10-i]*=sqrt(3.0)*(1.0-(double)i/m10);
			}
			break;
  case FTP: for(i=1;i<=m10;i++)
			{
			 d[i+m10+1]*=1.067*sin(PI*i/m10)/PI/i*m10;
			 d[m10-i]*=1.067*sin(PI*i/m10)/PI/i*m10;
			}
  case PAR: for(i=0;i<=m10;i++)
			{
			 if (i<=m10/2)
			 {
			  d[i+m10+1]*=1.93*(1-6.0*i*i/m10/m10+6.0*i*i*i/m10/m10/m10);
			  d[m10-i]*=1.93*(1-6.0*i*i/m10/m10+6.0*i*i*i/m10/m10/m10);
			 }
			 else
			 {
			  d[i+m10+1]*=3.86*(1-(double)i/m10)*(1-(double)i/m10)*(1-(double)i/m10);
			  d[m10-i]*=3.86*(1-(double)i/m10)*(1-(double)i/m10)*(1-(double)i/m10);
			 }
			}
			break;
  case HAN: for(i=0;i<=m10;i++)
			{
			 d[i+m10+1]*=0.815+0.815*cos((double)PI*i/m10);
			 d[m10-i]*=0.815+0.815*cos((double)PI*i/m10);
			}
			break;
  case HAM: for(i=0;i<=m10;i++)
			{
			 d[i+m10+1]*=1.59*(0.54+0.46*cos((double)PI*i/m10));
			 d[m10-i]*=1.59*(0.54+0.46*cos((double)PI*i/m10));
			}
			break;
  case BLC: for(i=0;i<=m10;i++)
			{
			 d[i+m10+1]*=1.81*(0.42+0.5*cos((double)PI*i/m10)+0.08*cos(2.0*PI*i/m10));
			 d[m10-i]*=1.81*(0.42+0.5*cos((double)PI*i/m10)+0.08*cos(2.0*PI*i/m10));
			}
			break;
 }
}

void FFT (float *r,float *i,int n,int fl)
/***********
* Describe : преобразование Фурье
* Params   : double *r - действительная и
*          : double *i - мнимая части сигнала/спектра
*          : int n     - число дискрет в сигнале/спектре
*          : int fl    - вид преобразования 1-прямое (-1)-обратное
* Return   : void
* Call     :
***********/
{
 int m;      /* показатель степени n=2^m */
 int it,l,e0,f2,j,ou,k,j1,i1;
 double u,v,z1,c,s,p,q,ro,t,w;

 m=log((double)n)/log(2.);
 n=pow(2.,(double)m)+0.5;
 for(l=1;l<=m;l++)
 {
  e0=pow(2.,(double)(m+1-l))+0.1;
  f2=e0/2;
  u=1;
  v=0;
  z1=PI/f2;
  c=cos(z1);
  s=fl*sin(z1);
  for(j=1;j<=f2;j++)
  {
   for(it=j;it<=n;it+=e0)
   {
    ou=it+f2-1;
    p=r[it-1]+r[ou];
	q=i[it-1]+i[ou];
    ro=r[it-1]-r[ou];
    t=i[it-1]-i[ou];
    r[ou]=ro*u-t*v;
    i[ou]=t*u+ro*v;
    r[it-1]=p;
    i[it-1]=q;
   }
   w=u*c-v*s;
   v=v*c+u*s;
   u=w;
  }
 }
 j=1;
 for(it=1;it<n;it++)
 {
  if(it<j)
  {
   j1=j-1;
   i1=it-1;
   p=r[j1];
   q=i[j1];
   r[j1]=r[i1];
   i[j1]=i[i1];
   r[i1]=p;
   i[i1]=q;
  }
  k=n/2;
  while(k<j)
  {
   j-=k;
   k/=2;
  }
  j+=k;
 }
 if(fl==(1))
 {
  for(k=0;k<n;k++)
  {
   r[k]/=n;
   i[k]/=n;
  }
 }
}

void amplitude (float *real,float *image,int num)
/***********
* Describe : вычисление амплитуды сигнала/спектра
* Params   : double *real  - действительная и
*          : double *image - мнимая части функции
*          : int num       - число дискрет
* Return   : void          -
* Call     :
***********/
{
 int i;
 double Mult;

 Mult=sqrt(2.0);

 for (i=0;i<num;i++)
  real[i]=Mult*sqrt(real[i]*real[i]+image[i]*image[i]);
}

void integrate (float *r,float hfreq,int num)
/***********
* Describe : Деление массива r на угл. частоту, соотв. каждому элементу
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 double tmp;

 r[0]=0.0;
 for (i=1;i<num;i++)
 {
  tmp = 2*PI*(i*hfreq/num);
  r[i]/=tmp;   /* делим на 2*PI*f */
 }
}

void diff (float *r,float hfreq,int num)
/***********
* Describe :
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/
{
 int i;
 double tmp;

 for (i=0;i<num;i++)
 {
  tmp = 2.0*PI*(i*hfreq/num);
  r[i]*=tmp;
 }
}

void doFFT (float *msr,int *num)
/***********
* Describe : функция высокого уровня вычисления спектра по вр. реализации
* Params   : float *msr - измерения сигнала
*          : int *num   - число измерений
* Return   : void
* Call     : get_window,FFT,amplitude,drawprogress.
***********/
{
 float huge *img=NULL;

 drawprogress ((double)0.0);
 img = (float huge *) farcalloc ((unsigned long)*num,sizeof(float));
 if (img==NULL)
 {
  printf ("\n Error in doFFT: No enough memory!");
  abort ();
 }
 get_window ((float *)msr,*num);
 drawprogress ((double)0.3);
 FFT (msr,(float *)img,(int)*num,1);
 drawprogress ((double)0.6);
 amplitude (msr,(float *)img,(int)*num);
 drawprogress ((double)0.8);
 farfree ((float *)img);
 *num=(*num+1)/2.56;
 drawprogress ((double)1.0);
}

int doIntegrate (float *msr,asRecPtr r)
/***********
* Describe : интегрирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int 0 нормальн интегрирование, -1 ненорм.завершение
* Call     :
***********/
{
 if (inc_yunit(r))
 {
  message (" Error message: ",getmaxx()/4,getmaxy()/4,msgNoInt,mnStd);
  return (-1);
 }
 if (isspectrum(r))
 {
  drawprogress ((double)0.0);
  integrate (msr,(float)(r->hfreq),r->number);
  drawprogress ((double)1.0);
  return (0);
 }
 else
 {
  integrate_sig (msr,(float)(r->hfreq),r->number);
  return (0);
 }
}

int doDiff (float *msr,asRecPtr r)
/***********
* Describe : дифференциирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/
{
 if (dec_yunit(r))
 {
  message (" Error message: ",getmaxx()/4,getmaxy()/4,msgNoDif,mnStd);
  return (-1);
 }
 if (isspectrum(r))
 {
  drawprogress ((double)0.0);
  diff (msr,(float)r->hfreq,r->number);
  drawprogress ((double)1.0);
  return (0);
 }
 else
 {
  diff_sig (msr,(float)r->hfreq,r->number);
  return (0);
 }
}

void integrate_sig (float *r,float hfreq,int num)
/***********
* Describe : метод Симпсона
* Params   : float *r
*          : int num
* Return   : void
* Call     :
***********/	/*
{
 int i;
 double S,h=1.000,k=4.00;
 float far *tmp;

 if ((tmp = (float far *) farcalloc ((unsigned long)num,
									 (unsigned long)sizeof(float)))==NULL)
 {
  printf ("\n No enough memory in integrating signal.");
  abort ();
 }

 h = (double) 1/(2.56*hfreq);  */ /* шаг */	/*

 tmp[0] = (float) h*r[0]/3;
 for (i=1;i<num-1;i++)
 {
  tmp[i] = (float) tmp[i-1]+(r[i]*h*k/3);
  if (k==4.00)
   k=2.00;
  else
   k=4.00;
  drawprogress ((double)(i-1)/(num-1));
 }
 drawprogress ((double)1.00);
 tmp[num-1] = tmp[num-2] + h*r[num-1]/3;

 for (i=0,S=(double)0.00;i<num;i++)
  S+=tmp[i];
 S/=num;   					*/ /* среднее арифметическое */	/*
 for (i=0;i<num;i++)
  r[i] = tmp[i]-S;
 farfree ((float far *)tmp);
}										*/
/***********
* Describe : метод через прямое и обратное FFT
* Params   : float *r
*          : int num
* Return   : void
* Call     : get_window,FFT,amplitude,drawprogress.
***********/
{
 float huge *img=NULL;
 int i;
 double omega,tmp;

 drawprogress ((double)0.0);
 img = (float huge *) farcalloc ((unsigned long)num,sizeof(float));
 if (img==NULL)
 {
  printf ("\n Error in integrate_sig: No enough memory!");
  abort ();
 }
 /* get_window ((float *)msr,*num);	*/
 FFT (r,(float *)img,(int)num,1);	/* прямое FFT */
 drawprogress ((double)0.5);
 r[0]=0.0;
 r[num-1]=0.0;				/* делим на угловую частоту 2 пи f */
 img[0]=0.0;				/* с учетом ее "-" знака на половине спектра */
 img[num-1]=0.0;			/* и поворачиваем на +90 градусов все составл */
 for (i=1;i<num/2;i++)		/* т.к. интеграл cos=sin, а интеграл sin=-cos */
 {                          /* т.е. реальн. и мним. части меняются */
  omega = 2.0*PI*i*hfreq*2.56/num;
  tmp=r[i];
  r[i]=(-img[i])/omega;
  img[i]=(tmp)/omega;
  tmp=r[num-1-i];
  r[num-1-i]=img[num-1-i]/omega;
  img[num-1-i]=(tmp)/(-omega);
 }
 FFT (r,(float *)img,(int)num,-1);	/* обратн FFT */
 farfree ((float *)img);
 drawprogress ((double)1.0);
 }

void diff_sig (float *r,float hfreq,int num)
/***********
* Describe : метод через прямое и обратное FFT дифференцирование сигнала
* Params   : float *r, hfreq
*          : int num
* Return   : void
* Call     : FFT,drawprogress.
***********/
{
 float huge *img=NULL;
 int i;
 double omega,tmp;

  drawprogress (0.0);
 img = (float huge *) farcalloc ((unsigned long)num,sizeof(float));
 if (img==NULL)
 {
  printf ("\n Error in diff_sig: No enough memory!");
  abort ();
 }
 /* get_window ((float *)msr,*num);	*/
 FFT (r,(float *)img,(int)num,1);	/* прямое FFT */

 drawprogress (0.5);		/* умножаем на угловую частоту 2 пи f */
							/* с учетом ее "-" знака на половине спектра */
							/* и поворачиваем на -90 градусов все составл */
 for(i=0;i<num/2;i++)
 {
  omega = 2.0*PI*i*hfreq*2.56/num;
  tmp=r[i];
  r[i]=img[i]*omega;
  img[i]=(-tmp)*omega;
  tmp=r[num-1-i];
  r[num-1-i]=img[num-1-i]*(-omega);
  img[num-1-i]=tmp*omega;
 }
 FFT (r,(float *)img,(int)num,-1);	/* обратн FFT */
 farfree ((float *)img);
 drawprogress (1.0);
 }
void transient (float *r,int beg,int end,int num)
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<beg;i++)
  r[i]=(float)0.0;
 for (i=end;i<num;i++)
  r[i]=(float)0.0;
}

void exponent (float *r,int beg,int end,int num)
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<beg;i++)
  r[i]=(float)0.0;
 for (i=end;i<num;i++)
  r[i]=(float)0.0;
}
