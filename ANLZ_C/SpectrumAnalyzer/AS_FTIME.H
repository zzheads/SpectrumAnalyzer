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

#include <dos.h>

typedef struct
{
 struct date date;
 struct time time;
} FTIME;


void ftimecpy (FTIME *t1,FTIME *t2);
/***********
* Describe : ������� �६� t1 � t2
* Params   : FTIME *t1 - �६�1
*          : FTIME *t2 - �६�2
* Return   : void
* Call     :
***********/

void earnftime (FTIME *t);
/***********
* Describe : ��⠭�������� � t ��⥬��� �६�
* Params   : FTIME *t - �㤠 ����� �६�
* Return   : void
* Call     :
***********/

void ftimeinc (FTIME *t,int i);
/***********
* Describe : 㢥����� �p��� �� 1 ᥪ㭤�
* Params   : FTIME *t - �६�
* Return   : void
* Call     :
***********/

void ftimetoa (FTIME *t,char *result);
/***********
* Describe : �p���⠢��� �p��� � c�p���
* Params   : FTIME *t
* Return   : char
* Call     :
***********/

int ftimecmp (FTIME *t1,FTIME *t2);
/***********
* Describe : �ࠢ������ �६� t1 � t2
* Params   : FTIME *t1 - �६�1
*          : FTIME *t2 - �६�2
* Return   : (int) 0-�६��� ���������,1 �६�1 �����,(-1) �६�1 �����
* Call     :
***********/

void setminftime (FTIME *t);
/***********
* Describe : ��⠭����� ���. �p���
* Params   : FTIME *t
* Return   : void
* Call     :
***********/

void setmaxftime (FTIME *t);
/***********
* Describe : ��⠭����� ����. �p���
* Params   : FTIME *t
* Return   : void
* Call     :
***********/

int incorrect (FTIME *t);
/***********
* Describe : �६� �� �ࠢ��쭮?
* Params   : FTIME *t
* Return   : int
* Call     :
***********/

long diffdate (struct date *d1,struct date *d2);
/***********
* Describe : ������ �p�����⮪ ����� ���� ��⠬� � ����
* Params   : struct date *d1 - ��p��� ���
*          : struct date *d2 - ��p�� ���
* Return   : long            - �᫮ ���� �� d1 �� d2
* Call     :
***********/

long min_of_day (struct time *t);
/***********
* Describe : ������ ������ ��� �� ������� �p�����
* Params   : struct time *t - �p���
* Return   : long           - ����� ���
* Call     : 
***********/

long ftimediff (FTIME *t1,FTIME *t2);
/***********
* Describe : ������ p������ ����� �p������ t1 � t2 � ������
* Params   : FTIME *t1 - ��p��� �p���
*          : FTIME *t2 - ��p�� �p���
* Return   : long      - p����� � ������
* Call     : diffdate,min_of_day
***********/

void full_date (int day,struct date *d);
/***********
* Describe : ������ ���� � ����� �� ����p� ��� � ����
* Params   : int day        - ����p ��� � ����
*          : struct date *d - � ��p���p� ᮤ�p����� ���
* Return   : void
* Call     : leap
***********/

int day_of_year (struct date *d);
/***********
* Describe : ������ ����p ��� � ���� �� ���
* Params   : struct date *d - ���
* Return   : int            - ����p ��� � ����
* Call     : leap
***********/