#include <stdio.h>
#include <dos.h>
#include <conio.h>
/************************************************************/
/* Функция возвращает разницу между двумя моментами времени */
/* te - конечным и ts - начальным в секундах с точностью до сотых*/
/* te и ts - структуры time, возвращаемые функцией gettime() */
/*************************************************************/
float diftime (struct time ts, struct time te)
{
return ((((float)te.ti_hour-ts.ti_hour)*60+((float)te.ti_min-ts.ti_min))*60+((float)te.ti_sec-ts.ti_sec)+((float)te.ti_hund-ts.ti_hund)/100);
}
/**************************************************************/
/* Программа тестирования времени выполнения операций */
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
gettime(&ts);		/* запомнили начальный момент времени */
for(i=0;i<i_max;i++);        /* задаем число повторений операции */
for(j=0;j<j_max;j++);
/*       {   		/* начало тестируемого фрагмента */
/*       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
       for(j=0;j<j_max;j++);
	}		/* конец тестируемого фрагмента */
gettime(&te);		/* запомнили время окончания */
dt=diftime(ts,te);
printf("\nAll Time is: %.2f\t\tOne сycle time is: %f mks",dt,dt*1000000/(i+j));
getch();
}

/**************** Для ROVER 66 в ДОС-режиме */
/* пустой цикл со счетчиком long на увеличение	- 0.24 mks */
/* пустой цикл со счетчиком long на уменьшение	- 0.24 mks */
/* if(nper==2000) где nper(int)			- 0.37 mks */
/* outp(pout,0);				- 1.5 mks */
/* {
/* outp(pout,0);				- 2.79 mks */
/* outp(pout,stat);                                       */
/* }							  */
/* gettime(&tn);				- 35.7 mks */
/* printf("\rЧастота %f",dt);			- 237 mks */
/* printf("Частота %f",dt);			- 715  mks */
/* printf("\nЧастота %f",dt);			- 2581 mks */
/*							   */
/* 	stat=inp(pin)&0x20;			- 2.81 mks */
/*	if(stat!=prstat)  условие всегда выполняется	   */
/*		{outp(pout,(stat));			   */
/*		prstat=0xFF;                               */
