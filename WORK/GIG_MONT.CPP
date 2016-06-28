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
	 /* отслеживание управляющих импульсов системы впрыска Primera */

void main()
 {
struct  time ts,te;
unsigned pout=0x378,pin=0x379;
int stat,prstat,buf[15000][2],ibuf=0,i,test;
unsigned long front,count=0;
float dt,sum_count,count_pms;
FILE *file;

front=50;	/* 140 мкс на дребезг, на макс.скоростях и нагрузках будет */
		/* работать неправильно, попробовать без задержки и если */
		/* все нормально лучше ее отключить, т.к. дребезга вероятно нет!! */

stat=prstat=inp(pin)&0x20;
while(1)
	{
	gettime(&ts);		/* запомнили время начала */
	while(ibuf<15000)	/* одна запись около 100 с на 3000 об/мин*/
		{while ((stat=inp(pin)&0x20)==prstat)
			{count++; /* Цикл 1.686 mks, значит */
/* тестовая строка, убрать *//*if(count==1000)	{if(test==0) test=stat=0x20; else test=stat=0; break;}*/
			}    /* 1мс = 593 цикла, 5мс = 2966 циклов */
		/*if(count<front) continue;    /* если дребезг, запросить порт снова */
		prstat=stat;	/* запоминаем состояние форсунки */
		if(stat==0)	/* если команда на закрытие */
			{buf[ibuf][0]=count;
			}
		else	/* если команда на открытие */
			{buf[ibuf][1]=count;
			ibuf++; /* предыдущий цикл закончен */
			}
				/* запомнили время */
		count=0;	/* обнуляем счетчик циклов */
		}
	gettime(&te);		/* запомнили время окончания */
	dt=diftime(ts,te);	/* время всей записи сек */
	sum_count=0;
	for(i=0;i<ibuf;i++)	sum_count+=(float)buf[i][0]+buf[i][1]; /* общее число циклов за запись */
	count_pms=sum_count/dt/1000;	/* число циклов за мс */
	file=fopen("gig1.txt","a+");
	fprintf(file,"\nbegin, Record len=%.2f s, Number cucles= %d, Cucle per ms= %.2f",dt,ibuf,count_pms);
	for(i=0;i<ibuf;i++)
		fprintf(file,"\n%.2f\t%.2f",(float)buf[i][0]/count_pms,(float)buf[i][1]/count_pms);
	fclose(file);
	ibuf=0;
	stat=prstat=inp(pin)&0x20;
	}
}


