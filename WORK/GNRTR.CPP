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

/* �� ��ࠡ�⠫ �� ��᮪�� �����, �.�. ���⮩ 横� �믮������ */
/* ������ ���ய��樮���쭮� �६� �� ࠧ����� ���祭��� ���稪�� */
/* ��ࠡ�⠫� ⮫쪮 � ��������⭮� ������ �� �㭪樨 delay() */

void main()
{
struct  time ts,te;
int byte=0x20;	/* �뢮���� ���� */
unsigned pout=0x378,fric;	/* pin=0x379 */
double dt,dt_per,per,dlit,prod,vol;
unsigned long i,i_max=1000,j,paus_i=1000,dlit_i=1000,nper=1000;

printf("\n\nGolovnev A. 18.11.03 y. glav@interdacom.ru");
printf("\n\n�ணࠬ��,-������� TTL �����ᮢ.");
printf("\n���ꥬ ���� LPT1 ���� 378h, ���⠪� ࠧꥬ�- 2.");
printf("\n\n���� �஢�ઠ ᪮��� ������ � ����ன�� �६����� ���ࢠ���\n�����!!!");

gettime(&ts);		/* ��।��塞 ᪮���� �� ����� �᫥ ���権 */
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
gettime(&ts);		/* ���塞 ᪮���� �� 10-� ᥪ㭤��� ���ࢠ�� */
for(i=0;i<i_max;i++)    /* sqr(10)/2 = 1.7	*/
	{
	for(j=0;j<dlit_i;j++);
	for(j=0;j<paus_i;j++);
	}
gettime(&te);
dt=diftime(ts,te)*1000000/(i_max*(dlit_i+paus_i+3));	/* �६� ���⮣� 横�� � ��� */
printf("\n�६� ���⮣� 横��\t\t%f ���",dt);

nper=i_max;  /* ��।��塞 ��稥 ��室� �६��� � ��ਮ�� */
gettime(&ts);		/* �� ���ࢠ�� 10 � � 1000 横���, �.�. 100 �� */
for(i=0;i<nper;i++)
	{outp(pout,byte);
	for(j=0;j<dlit_i;j++);
	outp(pout,0);
	for(j=0;j<paus_i;j++);
	}
gettime(&te);
dt_per=diftime(ts,te)*1000000/nper-(dlit_i+paus_i+3)*dt;	/* ��稥 ������ �६��� */
printf("\n�६� ���饭�� � �����\t\t%f ��� ",dt_per);
printf("\n\n����஢���� �����祭�,");

while(1)
	{printf("\n\n������ ��室�� ����� (��� ��室� ������ �� �㫨):\n");
	printf("\n��ਮ�\t���.���\t��᫮ ���\n");
	printf("(���)\t(���)\t(0-�����뢭�)\n");
	scanf("%lf%lf%lu",&per,&dlit,&nper);
	if(dlit==0) break;
	if(dlit>=per)
		{printf("\n���⥫쭮��� �� ������ �ॢ���� ��ਮ�!! ������ ����.");
		continue;
		}
	if(nper==0) nper=0xffffffff;
	printf("\n����������� ������� � ��ࠬ��ࠬ�:");
	printf("\n��ਮ�\t\t\t\t%.1f ���",per);
	printf("\n�����\t\t\t\t%.3f ��",1000000/per);
	printf("\n���⥫쭮��� ������\t\t%.1f ���",dlit);
	printf("\n����������\t\t\t%.3f",dlit/per);
	printf("\n��᫮ �����ᮢ\t\t\t%lu",nper);
	printf("\n�த����⥫쭮��� �����樨\t%.2f �",per*nper/1000000);
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
	printf("\n\n����쭮 ᣥ���஢���:");
	printf("\n��ਮ�\t\t\t\t%.1f ���",prod*1000000/i);
	printf("\n�����\t\t\t\t%.3f ��",i/prod);
	printf("\n�த����⥫쭮��� �����樨\t%.2f �",prod);
	printf("\n�⪫������ �� ���������\t\t%.3f %",((prod*1000000/per/nper)-1)*100);
	printf("\n������ ��ꥬ ���� � �����\t\t");
	scanf("%lf",&vol);
	printf("\n����祭 ��室:\t\t\t%f �/�",vol*1000000/dlit/nper);
	}
}



