/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_DIAG     
* Describe  : диагностика
* File      : C:\ALEX\WORK\AS_DIAG.C
* Last edit : 14.08.93 01.05.12
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "as_ftime.h"
#include "db_exch.h"
#include "as_diag.h"

double log (double);
double exp (double);

int N,L,K,M;	// Экстраполяция по N последним замерам. Если до ур.аварии
							// меньше L дней, то предупреждение. Также, если скачек более
							// К раз отн. средн уровня по М последним замерам
char *diagnoze (char *fname,char *field)
/***********
* Describe : выдача диагностического сообщения о параметре из БД
* Params   : char *fname - имя БД
*          : char *field - имя параметра
* Return   : char        - сообщение
* Call     :
***********/
{
#ifdef ENGLISH
static char *overflow="Overflow";
#else
static char *overflow="Превышен";
#endif

 static char result[100]="?        ";
 char tmp[30];
 unsigned int i,j,num;
 long number;
 float par_dif;
 float far *par,break_lev,danger_lev;
 struct date far* d=NULL;
 long far* time=NULL,x_danger,x_break;
 FILE *f=NULL;
 struct date tmp1,tmp2;
 double SX,SY,SXY,SX2,SXlnY,SlnY,U_lin,A_lin,B_lin,U_exp,A_exp,B_exp;

											/* Диагноз остается ?, если */
 if ((f = fopen (fname,"rb"))==NULL ||		/* не открылся файл, в нем нет */
	 (par = read_dbf_number (f,field,&number))==NULL || /* числ. полей, */
	 (d = read_dbf_date (f,"DATE",&number))==NULL ||    /* число замеров менее */
	 number<4L)							/* двух + две строки уровней Warning & Danger*/
	goto EXIT;
 num = (unsigned int) number;

 for (i=2;i<num;i++)
	if (par[i]<=0.0)
	{
	 for (j=i+1;j<num;j++)
	 {
		par [j-1] = par[j];
		d [j-1] = d[j];
		if (par[j-1]<=0.0)
    {
     j--;
     num--;
    }
	 }
   num--;
  }

 for (i=0;i<num;i++)
	if (par[i]<=0.0)
	 printf ("\n par[%d]=%f",i,par[i]);

 danger_lev = par[0];
 break_lev  = par[1];

 if ((time = farcalloc (num,sizeof(long)))==NULL)
	goto EXIT;
 for (i=num-N;i<num;i++)
	time[i] = diffdate ((struct date far *)d+num-N,(struct date far *)d+i);

 for (i=num-N,SX=0,SY=0,SXY=0,SX2=0,SlnY=0,SXlnY=0;i<num;i++)
 {
	SX += (double) time[i];
  SY += (double) par[i];
	SXY += (double) time[i]*par[i];
  SX2 += (double) time[i]*time[i];
  SlnY += (double) log(par[i]);
	SXlnY += (double) time[i]*log(par[i]);
 }
  /* линейная апроксимация */
 B_lin = (double)(SX*SY-N*SXY)/(SX*SX-N*SX2);
 A_lin = (double)(SY-B_lin*SX)/N;
 for (i=num-N,U_lin=0;i<num;i++)
	U_lin += (par[i]-(A_lin+B_lin*time[i]))*(par[i]-(A_lin+B_lin*time[i]));
 /* экспоненциальная апроксимация */
 B_exp = (SX*SlnY-N*SXlnY)/(SX*SX-N*SX2);
 A_exp = exp((SlnY-B_exp*SX)/N);
 for (i=num-N,U_exp=0;i<num;i++)
//	U_exp += (par[i]-(A_exp+B_exp*time[i]))*(par[i]-(A_exp+B_exp*time[i]));
	 U_exp += (par[i]-(A_exp*exp(B_exp*time[i])))*(par[i]-(A_exp*exp(B_exp*time[i])));

 if (U_lin>U_exp) 		/* экспоненциальный закон точнее */
  if (B_exp!=0.0)
  {
   assert (danger_lev>0.0);
	 assert (break_lev>0.0);
   assert (A_exp>0.0);
   x_danger = (log(danger_lev)-log(A_exp))/B_exp;
	 x_break  = (log(break_lev)-log(A_exp))/B_exp;
	}
  else
  {
   x_danger = 2000;
	 x_break  = 2000;
	}
 else                   /* линейный закон точнее */
  if (B_lin!=0.0)
	{
	 x_danger = (danger_lev-A_lin)/B_lin;
	 x_break  = (break_lev-A_lin)/B_lin;
  }
	else
	{
   x_danger = 2000;
   x_break  = 2000;
	}

 tmp1.da_day = (d+num-N)->da_day;           /* полная дата */
 tmp1.da_mon = (d+num-N)->da_mon;           /* уровня danger */
 tmp1.da_year = (d+num-N)->da_year;
 full_date (day_of_year(&tmp1)+(unsigned int)x_danger,&tmp1);

 tmp2.da_day = (d+num-N)->da_day;           /* уровня break */
 tmp2.da_mon = (d+num-N)->da_mon;
 tmp2.da_year = (d+num-N)->da_year;
 full_date (day_of_year(&tmp2)+(unsigned int)x_break,&tmp2);

 strcpy (result," ");
 if (par[num-1]>=danger_lev)
	strcat (result,overflow);
 else
	if ((x_danger>0)&&(x_danger<1725))
	{
	 strcpy (tmp,itoa (tmp1.da_day,tmp,10));
	 strcat (result,tmp);
	 strcat (result,"/");
	 strcpy (tmp,itoa (tmp1.da_mon,tmp,10));
	 strcat (result,tmp);
	 strcat (result,"/");
	 strcpy (tmp,itoa (tmp1.da_year-1900,tmp,10));
	 strcat (result,tmp);
	}
	else
	 strcat (result,"        ");

 strcat (result," ");
 if (par[num-1]>=break_lev)		// если превышение уровня аварии
	strcat (result,overflow);
 else
	if ((x_break>0)&&(x_break<1725))
	{
	 strcpy (tmp,itoa (tmp2.da_day,tmp,10));
	 strcat (result,tmp);
	 strcat (result,"/");
	 strcpy (tmp,itoa (tmp2.da_mon,tmp,10));
	 strcat (result,tmp);
	 strcat (result,"/");
	 strcpy (tmp,itoa (tmp2.da_year-1900,tmp,10));
	 strcat (result,tmp);
	}
 strncat (result,"             ",20-strlen(result));		// добавляем до 18 символов

			 // после 18 символа строка уходит в подсказку
 if (par[num-1]>=break_lev)		// если превышение уровня аварии
	sprintf (result+18,"* %d%% превыш; ",(int)((par[num-1]/break_lev-1)*100));
 else
	{
	if ((x_break<=time[num-1]+L)&&(x_break>time[num-1]))	// если прогноз < L дней
		sprintf (result+18,"* Прогноз %d дн; ",(int)(x_break-time[num-1]));
	}

 for(i=num-M-1,par_dif=0;i<num-1;i++)
	par_dif += par[i];
	par_dif = (int)((par[num-1]/par_dif*M-1)*100);		// скачек в % отн.средн. по М последним замерам
 if (abs((int)par_dif)>=K)
	{
	result[18] = '*';
	sprintf (result+strlen(result),"Скачек %d%% ; ",(int)par_dif);
	}

 if ((U_lin>U_exp)&&(B_exp>0))				// если экспоненциальный рост
 {
	strcat (result,"Эксп-та;");
 }

 EXIT:
			if (f!=NULL)
       fclose (f);
	  if (par!=NULL)
       farfree (par);
      if (d!=NULL)
       farfree (d);
      if (time!=NULL)
       farfree (time);
			return (result);
}
