#include <stdio.h>
#include <dos.h>
#include <conio.h>
/************************************************************/
/* �㭪�� �����頥� ࠧ���� ����� ���� �����⠬� �६��� */
/* te - ������ � ts - ��砫�� � ᥪ㭤�� � �筮���� �� ����*/
/* te � ts - �������� time, �����頥�� �㭪樥� gettime() */
/*************************************************************/
float diftime (struct time ts, struct time te)
{
return ((((float)te.ti_hour-ts.ti_hour)*60+((float)te.ti_min-ts.ti_min))*60+((float)te.ti_sec-ts.ti_sec)+((float)te.ti_hund-ts.ti_hund)/100);
}
/**************************************************************/
/* �ணࠬ�� ���஢���� �६��� �믮������ ����権 */
/***************************************************************/

void main()
{
struct  time ts,te,tn;
float dt;
unsigned long i,i_max,j,j_max,n,n_max;
int nper=0,stat,prstat;
unsigned pout=0x378,pin=0x379;

stat=inp(pin)&0x20;
prstat=0xFF;
stat=0x20;
i_max=1000000;
j_max=1000000;
n_max=500;
gettime(&ts);		/* ��������� ��砫�� ������ �६��� */
for(i=0;i<i_max;i++);        /* ������ �᫮ ����७�� ����樨 */
for(j=0;j<j_max;j++);
/*       {   		/* ��砫� ����㥬��� �ࠣ���� */
/*       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
	}		/* ����� ����㥬��� �ࠣ���� */
gettime(&te);		/* ��������� �६� ����砭�� */
dt=diftime(ts,te);
printf("\nAll Time is: %.2f\t\tOne �ycle time is: %f mks",dt,dt*1000000/(i+j));
getch();
}

/**************** ��� ROVER 66 � ���-०��� */
/* ���⮩ 横� � ���稪�� long �� 㢥��祭��	- 0.24 mks */
/* ���⮩ 横� � ���稪�� long �� 㬥��襭��	- 0.24 mks */
/* if(nper==2000) ��� nper(int)			- 0.37 mks */
/* outp(pout,0);				- 1.5 mks */
/* {
/* outp(pout,0);				- 2.79 mks */
/* outp(pout,stat);                                       */
/* }							  */
/* gettime(&tn);				- 35.7 mks */
/* printf("\r����� %f",dt);			- 237 mks */
/* printf("����� %f",dt);			- 715  mks */
/* printf("\n����� %f",dt);			- 2581 mks */
/*							   */
/* 	stat=inp(pin)&0x20;			- 2.81 mks */
/*	if(stat!=prstat)  �᫮��� �ᥣ�� �믮������	   */
/*		{outp(pout,(stat));			   */
/*		prstat=0xFF;                               */
