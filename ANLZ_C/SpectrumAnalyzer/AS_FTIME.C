/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_FTIME
* Describe  : ���ᠭ�� �p����� �p��p���� �����p���������p
* File      : C:\ALEX\WORK\AS_FTIME.H
* Last edit : 30.05.93 00.41.01
* Remarks   :
*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "as_ftime.h"

int ftimecmp (FTIME *t1,FTIME *t2)
/***********
* Describe : �ࠢ������ �६� t1 � t2
* Params   : FTIME *t1 - �६�1
*          : FTIME *t2 - �६�2
* Return   : (int) 0-�६��� ���������,1 �६�1 �����,(-1) �६�1 �����
* Call     :
***********/
{
 if (t1->date.da_year > t2->date.da_year)     /* �ਮ���: ���,�����,����,  */
  return (1);                                 /* ��,������,ᥪ㭤�.         */
 if (t1->date.da_year < t2->date.da_year)     /*                             */
  return (-1);                                /*                             */
 if (t1->date.da_mon > t2->date.da_mon)       /*                             */
  return (1);                                 /*                             */
 if (t1->date.da_mon < t2->date.da_mon)       /*                             */
  return (-1);                                /*                             */
 if (t1->date.da_day > t2->date.da_day)       /*                             */
  return (1);                                 /*                             */
 if (t1->date.da_day < t2->date.da_day)       /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_hour > t2->time.ti_hour)     /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_hour < t2->time.ti_hour)     /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_min > t2->time.ti_min)       /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_min < t2->time.ti_min)       /*                             */
  return (-1);                                /*                             */
 if (t1->time.ti_sec > t2->time.ti_sec)       /*                             */
  return (1);                                 /*                             */
 if (t1->time.ti_sec < t2->time.ti_sec)       /*                             */
  return (-1);                                /*                             */
 return (0);                                  /*                             */
}

void ftimecpy (FTIME *t1,FTIME *t2)
/***********
* Describe : ������� �६� t1 � t2
* Params   : FTIME *t1 - �६�1
*          : FTIME *t2 - �६�2
* Return   : void
* Call     :
***********/
{
 t1->date.da_year = t2->date.da_year;         /* �������ਨ ����譨         */
 t1->date.da_mon  = t2->date.da_mon ;         /*                             */
 t1->date.da_day  = t2->date.da_day ;         /*                             */
 t1->time.ti_hour = t2->time.ti_hour;         /*                             */
 t1->time.ti_min  = t2->time.ti_min ;         /*                             */
 t1->time.ti_sec  = t2->time.ti_sec ;         /*                             */
}

void earnftime (FTIME *t)
/***********
* Describe : ��⠭�������� � t ��⥬��� �६�
* Params   : FTIME *t - �㤠 ����� �६�
* Return   : void
* Call     :
***********/
{
 getdate (&(t->date));                        /* ��⥬��� ���              */
 gettime (&(t->time));                        /* ��⥬��� �६�             */
}

void ftimeinc (FTIME *t,int i)
/***********
* Describe : 㢥����� �p��� �� i ᥪ㭤 � ��p���ᮬ
* Params   : FTIME *t - �p���
*          : int i    - �������⢮ ᥪ㭤
* Return   : void
* Call     :
***********/
{
 unsigned char hour,min;

 hour = i/(60*60);
 min  = i/60-hour*60;
 i    = i-hour*60*60-min*60;

 t->time.ti_sec+=i;
 t->time.ti_min+=min;
 t->time.ti_hour+=hour;

 while (t->time.ti_sec>=60)
 {
  t->time.ti_min++;
  t->time.ti_sec-=60;
 }
 while (t->time.ti_min>=60)
 {
  t->time.ti_hour++;
  t->time.ti_min=0;
 }
 while (t->time.ti_hour>=24)
 {
  t->date.da_day++;
  t->time.ti_hour=0;
 }
 while (t->date.da_day>31)
 {
  t->date.da_mon++;
  t->date.da_day=1;
 }
 while (t->date.da_mon>12)
 {
  t->date.da_year++;
  t->date.da_mon=1;
 }
}

void ftimetoa (FTIME *t,char *result)
/***********
* Describe : �p���⠢��� �p��� � c�p���
* Params   : FTIME *t
* Return   : char
* Call     :
***********/
{
 char tmp[20];

 strcpy (result," ");
 strcpy (tmp,itoa((int)t->date.da_day,tmp,10));
 strcat (result,tmp);
 strcat (result,"/");

 strcpy (tmp,itoa((int)t->date.da_mon,tmp,10));
 strcat (result,tmp);
 strcat (result,"/");

 strcpy (tmp,itoa((int)t->date.da_year-1900,tmp,10));
 strcat (result,tmp);
 strcat (result," ");

 strcpy (tmp,itoa((int)t->time.ti_hour,tmp,10));
 strcat (result,tmp);
 strcat (result,":");

 strcpy (tmp,itoa((int)t->time.ti_min,tmp,10));
 strcat (result,tmp);
}

void setminftime (FTIME *t)
/***********
* Describe : ��⠭����� ���. �p���
* Params   : FTIME *t
* Return   : void
* Call     :
***********/
{
 t->date.da_day  = 1;
 t->date.da_mon  = 1;
 t->date.da_year = 0;
 t->time.ti_hour = 0;
 t->time.ti_min  = 0;
 t->time.ti_sec  = 0;
}

void setmaxftime (FTIME *t)
/***********
* Describe : ��⠭����� ����. �p���
* Params   : FTIME *t
* Return   : void
* Call     :
***********/
{
 t->date.da_day  = 30;
 t->date.da_mon  = 12;
 t->date.da_year = 3000;
 t->time.ti_hour = 10;
 t->time.ti_min  = 50;
 t->time.ti_sec  = 50;
}

int incorrect (FTIME *t)
/***********
* Describe : �६� �� �ࠢ��쭮?
* Params   : FTIME *t1 - �६�
* Return   : int
* Call     :
***********/
{
 if ( (t->date.da_year<0)    || (t->date.da_mon<1)  || (t->date.da_day<1)
   || (t->date.da_year>3000) || (t->date.da_mon>12) || (t->date.da_day>31)
   || (t->time.ti_hour>24)   || (t->time.ti_min>60) || (t->time.ti_sec>60) )
  return (1);
 else
  return (0);
}

#define DAYS_IN_YEAR  365

int leap (int year)
/***********
* Describe : ��p������ ��᮪��� �� ���
* Params   : int year - ���
* Return   : int      - 1-��, 0-���
* Call     :
***********/
{
 if ((year%4==0 && year%100!=0) || (year%400==0))
  return 1;
 else
  return 0;
}

static char daytab[2][13] = {{0,31,28,31,30,31,30,31,31,30,31,30,31},
                             {0,31,29,31,30,31,30,31,31,30,31,30,31}};

int day_of_year (struct date *d)
/***********
* Describe : ������ ����p ��� � ���� �� ���
* Params   : struct date *d - ���
* Return   : int            - ����p ��� � ����
* Call     : leap
***********/
{
 int i,day;
 day = d->da_day;
 for (i=1;i<d->da_mon;i++)
  day+=daytab[leap(d->da_year)][i];
 return day;
}

void full_date (int day,struct date *d)
/***********
* Describe : ������ ���� � ����� �� ����p� ��� � ����
* Params   : int day        - ����p ��� � ����
*          : struct date *d - � ��p���p� ᮤ�p����� ���
* Return   : void
* Call     : leap
***********/
{
 int i;
 for (i=1;day>daytab[leap(d->da_year)][i];i++)
  day -= daytab[leap(d->da_year)][i];
 d->da_mon = i;
 d->da_day = day;
 while (d->da_mon>12)
 {
  d->da_year++;
  d->da_mon-=12;
 }
}

char *month_name (int n)
/***********
* Describe : ��� �����
* Params   : int n - ����p ����� � 1
* Return   : char  - �p���-��� �����
* Call     :
***********/
{
 char *name[] = {"Unk","Jan","Feb","Mar","Apr","May","Jun",
                       "Jul","Aug","Sep","Okt","Nov","Dec"};
 return (n<1||n>12) ? name[0] : name[n];
}

long diffdate (struct date *d1,struct date *d2)
/***********
* Describe : ������ �p�����⮪ ����� ���� ��⠬� � ����
* Params   : struct date *d1 - ��p��� ���
*          : struct date *d2 - ��p�� ���
* Return   : long            - �᫮ ���� �� d1 �� d2
* Call     :
***********/
{
 int i;
 long days=0;

 days += (DAYS_IN_YEAR+leap(d1->da_year))*(d1->da_year!=d2->da_year) - day_of_year(d1);
 for (i=d1->da_year+1;i<d2->da_year;i++)
  days += DAYS_IN_YEAR + leap(i);
 days += day_of_year(d2);
 return days;
}

#define MIN_IN_HOUR  60
#define HOUR_IN_DAY  24

long min_of_day (struct time *t)
/***********
* Describe : ������ ������ ��� �� ������� �p�����
* Params   : struct time *t - �p���
* Return   : long           - ����� ���
* Call     :
***********/
{
 int i;
 long min;

 min = t->ti_min;
 for (i=0;i<t->ti_hour;i++)
  min+=MIN_IN_HOUR;
 return min;
}

long ftimediff (FTIME *t1,FTIME *t2)
/***********
* Describe : ������ p������ ����� �p������ t1 � t2 � ������
* Params   : FTIME *t1 - ��p��� �p���
*          : FTIME *t2 - ��p�� �p���
* Return   : long      - p����� � ������
* Call     : diffdate,min_of_day
***********/
{
 long days_diff,min_diff;

 days_diff = diffdate (&(t1->date),&(t2->date));
 min_diff = days_diff * HOUR_IN_DAY * MIN_IN_HOUR;
 min_diff += min_of_day (&(t2->time)) - min_of_day (&(t1->time));
 return min_diff;
}