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
int byte=0x20;	/* выводимый байт */
unsigned pout=0x378,fric;	/* pin=0x379 */
double dt,dt_per,prod,vol;
unsigned long i,i_max=1000000,j,paus,dlit,per,nper=1000;

printf("\n\nПрограмма,- низкочастотный (до 500 Гц) генератор TTL импульсов.");
printf("\nGolovnev A. 20.11.03 y. glav@interdacom.ru tel.(8442) 697198");
printf("\n\nРазъем порта LPT1 адрес 378h, Контакт разъема- 2.");

while(1)
	{printf("\n\nВведите исходные данные целыми числами (Для выхода введите все нули):\n");
	printf("\nПериод\tДлт.имп\tЧисло имп\n");
	printf("(млс)\t(млс)\t(0-непрерывно)\n");
	scanf("%lu%lu%lu",&per,&dlit,&nper);
	if(dlit==0) break;
	if(dlit>=per)
		{printf("\nДлительность не должна превышать период!! Повторите ввод.");
		continue;
		}
	paus=per-dlit;
	if(nper==0) nper=0xffffffff;
	printf("\nГенерируются импульсы с параметрами:");
	printf("\nПериод\t\t\t\t%lu млс",per);
	printf("\nЧастота\t\t\t\t%.3f Гц",1000/(float)per);
	printf("\nДлительность импульса\t\t%lu млс",dlit);
	printf("\nСкважность\t\t\t%.3f",(float)dlit/(float)per);
	printf("\nЧисло импульсов\t\t\t%lu",nper);
	printf("\nПродолжительность генерации\t%.2f с",(float)per*nper/1000);
	for(fric=2300;fric<=3100;fric+=200)
		{sound(fric);
		delay(1000);
		nosound();
		}
	gettime(&ts);
	for(i=0;i<nper;i++)
		{outp(pout,byte);
		delay(dlit);
		outp(pout,0);
		delay(paus);
		}
	gettime(&te);
	sound(3100);
	delay(1000);
	nosound();
	prod=diftime (ts,te);
	printf("\n\nРеально сгенерировано:");
	printf("\nПериод\t\t\t\t%.1f млс",prod*1000/i);
	printf("\nЧастота\t\t\t\t%.3f Гц",i/prod);
	printf("\nПродолжительность генерации\t%.2f с",prod);
	printf("\nОшибка \t\t\t\t%.3f %",(prod/((float)per*nper/1000)-1)*100);
	printf("\nВведите объем газа в литрах\t\t");
	scanf("%lf",&vol);
	printf("\nПолучен расход:\t\t\t%f л/с",vol*1000/dlit/nper);
	}
}



