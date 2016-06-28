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


void main()
 {
struct  time ts,te;
unsigned pout=0x378,pin=0x379;
int stat,prstat,nper=0,buf[1000],ibuf=0;
float dt,prolong,qlong,lic,flow,k_press;
unsigned long i,j,front,count=0,timeo=1000,timec=20000,n_flow,min=20000000,max=0;

		/* ИСХОДНЫЕ ДАННЫЕ: */
prolong=0;	/* к-т пропорц.увеличения врем.открытия в % */
qlong=0;	/* к-т квадр.увеличения врем.открытия в % */
lic=0.4;        /* утечка в % от макс.расхода */
flow=15;	/* начальное знач.расхода в % от макс */
n_flow=50;	/* число циклов для эксп.усреднения расхода */
k_press=25;	/* к-т добавки врем.открытия из-за падения давления в % */

outp(pout,0);	/* сначала закрываем форсунку */
prolong/=100;
qlong/=296600;
lic/=100;
flow/=100;
k_press/=100;
front=50;	/* 140 мкс на дребезг, на макс.скоростях и нагрузках будет */
		/* работать неправильно, попробовать без задержки и если */
		/* все нормально лучше ее отключить, т.к. дребезга вероятно нет!! */

stat=prstat=inp(pin)&0x20;
while (1)			/* Цикл 1.686 mks, значит */
begin:	{stat=inp(pin)&0x20;    /* 1мс = 593 цикла, 5мс = 2966 циклов */
	count++;		/* наращиваем счетчик циклов */
	if(count==2000000)	/* двигатель остановлен */
		{printf("\n\nДлительность импульсов в пределах %f - %f ms\n",(float)min/593,(float)max/593);
		return;
		}
	if(stat!=prstat)  	/* если уровень не изменился, цикл 1.6 мкс */
		{if(count<front) continue;    /* если дребезг, запросить порт снова */
		prstat=stat;	/* запоминаем состояние форсунки */
		if(stat!=0)	/* если есть входн. импульс на открытие */
			{			/* задерживаем его */
			timec=count;	/* запомнили время закрыт.сост */
			i=lic*timec;
			count=0;
			while (count<i)
				{stat=inp(pin)&0x20;
				count++;
				}
			outp(pout,0x20);	/* и открываем форсунку */
			}
		else     	/* если команда на закрытие форсунки */
			{	/* задерживаем ее */
			flow=flow-flow/n_flow+(double)timeo/(timeo+timec)/n_flow;
			if(count<min)	min=count;
			else	{if(count>max)	max=count;}
			/* среднее давление за n_flow циклов*/
/*			buf[ibuf]=count;
			ibuf++;
			if(ibuf==1000)
				{for(ibuf=0;ibuf<250;ibuf++)
				printf("\t%d",buf[ibuf]);
				return;
				}	*/
/*			i=count*(prolong+count*qlong); */	/*число циклов задержки */
			i=count*(prolong+count*qlong+k_press*flow);
			timeo=count+i; /* запоминаем время открыт.сост */
			count=0;
			while (count<i)
				{stat=inp(pin)&0x20;
				count++;          /* если еще не закрыли, а */
/*				if(stat!=prstat)  /* пора опять открывать */
  /*					{
					prstat=stat;
					count=0;
					goto begin; /* идем в начало цикла */
/*					}	*/
				}
			outp(pout,0);	/* и закрываем форсунку */
			}
		count=0;	/* обнуляем счетчик циклов */
		}
	}
}


