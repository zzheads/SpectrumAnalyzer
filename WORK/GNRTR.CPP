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

/* не заработал на высоких частотах, т.к. пустой цикл выполняется */
/* видимо непропорциональное время при различных значениях счетчиков */
/* заработало только в низкочастотной области на функции delay() */

void main()
{
struct  time ts,te;
int byte=0x20;	/* выводимый байт */
unsigned pout=0x378,fric;	/* pin=0x379 */
double dt,dt_per,per,dlit,prod,vol;
unsigned long i,i_max=1000,j,paus_i=1000,dlit_i=1000,nper=1000;

printf("\n\nGolovnev A. 18.11.03 y. glav@interdacom.ru");
printf("\n\nПрограмма,-генератор TTL импульсов.");
printf("\nРазъем порта LPT1 адрес 378h, Контакт разъема- 2.");
printf("\n\nИдет проверка скорости процессора и настройка временных интервалов\nЖДИТЕ!!!");

gettime(&ts);		/* определяем скорость на малом числе итераций */
for(i=0;i<i_max;i++)
	{
	for(j=0;j<dlit_i;j++);
	for(j=0;j<paus_i;j++);
	}
gettime(&te);
dt=diftime(ts,te)*1000000/(i_max*(dlit_i+paus_i+3));
/*dt=diftime (ts,te);*/

/*i_max*=(3.16/dt);*/
dlit_i=paus_i=((float)10000000/i_max)/dt/2;
/*paus_i*=(3.16/dt);*/
gettime(&ts);		/* уточняем скорость на 10-ти секундном интервале */
for(i=0;i<i_max;i++)    /* sqr(10)/2 = 1.7	*/
	{
	for(j=0;j<dlit_i;j++);
	for(j=0;j<paus_i;j++);
	}
gettime(&te);
dt=diftime(ts,te)*1000000/(i_max*(dlit_i+paus_i+3));	/* время пустого цикла в мкс */
printf("\nВремя пустого цикла\t\t%f мкс",dt);

nper=i_max;  /* определяем прочие расходы времени в периоде */
gettime(&ts);		/* на интервале 10 с в 1000 циклах, т.е. 100 Гц */
for(i=0;i<nper;i++)
	{outp(pout,byte);
	for(j=0;j<dlit_i;j++);
	outp(pout,0);
	for(j=0;j<paus_i;j++);
	}
gettime(&te);
dt_per=diftime(ts,te)*1000000/nper-(dlit_i+paus_i+3)*dt;	/* прочие затраты времени */
printf("\nВремя обращений к порту\t\t%f мкс ",dt_per);
printf("\n\nТестирование закончено,");

while(1)
	{printf("\n\nВведите исходные данные (Для выхода введите все нули):\n");
	printf("\nПериод\tДлт.имп\tЧисло имп\n");
	printf("(мкс)\t(мкс)\t(0-непрерывно)\n");
	scanf("%lf%lf%lu",&per,&dlit,&nper);
	if(dlit==0) break;
	if(dlit>=per)
		{printf("\nДлительность не должна превышать период!! Повторите ввод.");
		continue;
		}
	if(nper==0) nper=0xffffffff;
	printf("\nГенерируются импульсы с параметрами:");
	printf("\nПериод\t\t\t\t%.1f мкс",per);
	printf("\nЧастота\t\t\t\t%.3f Гц",1000000/per);
	printf("\nДлительность импульса\t\t%.1f мкс",dlit);
	printf("\nСкважность\t\t\t%.3f",dlit/per);
	printf("\nЧисло импульсов\t\t\t%lu",nper);
	printf("\nПродолжительность генерации\t%.2f с",per*nper/1000000);
	dlit_i=(dlit-dt_per/2)/dt-1;
	paus_i=(per-dlit-dt_per/2)/dt-2;
	for(fric=2300;fric<=3100;fric+=200)
		{sound(fric);
		delay(1000);
		nosound();
		}
	gettime(&ts);
	for(i=0;i<nper;i++)
		{outp(pout,byte);
		for(j=0;j<dlit_i;j++);
		outp(pout,0);
		for(j=0;j<paus_i;j++);
		}
	gettime(&te);
	sound(3100);
	delay(1000);
	nosound();
	prod=diftime (ts,te);
	printf("\n\nРеально сгенерировано:");
	printf("\nПериод\t\t\t\t%.1f мкс",prod*1000000/i);
	printf("\nЧастота\t\t\t\t%.3f Гц",i/prod);
	printf("\nПродолжительность генерации\t%.2f с",prod);
	printf("\nОтклонение от заданного\t\t%.3f %",((prod*1000000/per/nper)-1)*100);
	printf("\nВведите объем газа в литрах\t\t");
	scanf("%lf",&vol);
	printf("\nПолучен расход:\t\t\t%f л/с",vol*1000000/dlit/nper);
	}
}



