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

void main()
{
struct  time ts,te;
int byte=0x20;	/* �뢮���� ���� */
unsigned pout=0x378,fric;	/* pin=0x379 */
double dt,dt_per,prod,vol;
unsigned long i,i_max=1000000,j,paus,dlit,per,nper=1000;

printf("\n\n�ணࠬ��,- ���������� (�� 500 ��) ������� TTL �����ᮢ.");
printf("\nGolovnev A. 20.11.03 y. glav@interdacom.ru tel.(8442) 697198");
printf("\n\n���ꥬ ���� LPT1 ���� 378h, ���⠪� ࠧꥬ�- 2.");

while(1)
	{printf("\n\n������ ��室�� ����� 楫묨 �᫠�� (��� ��室� ������ �� �㫨):\n");
	printf("\n��ਮ�\t���.���\t��᫮ ���\n");
	printf("(���)\t(���)\t(0-�����뢭�)\n");
	scanf("%lu%lu%lu",&per,&dlit,&nper);
	if(dlit==0) break;
	if(dlit>=per)
		{printf("\n���⥫쭮��� �� ������ �ॢ���� ��ਮ�!! ������ ����.");
		continue;
		}
	paus=per-dlit;
	if(nper==0) nper=0xffffffff;
	printf("\n����������� ������� � ��ࠬ��ࠬ�:");
	printf("\n��ਮ�\t\t\t\t%lu ���",per);
	printf("\n�����\t\t\t\t%.3f ��",1000/(float)per);
	printf("\n���⥫쭮��� ������\t\t%lu ���",dlit);
	printf("\n����������\t\t\t%.3f",(float)dlit/(float)per);
	printf("\n��᫮ �����ᮢ\t\t\t%lu",nper);
	printf("\n�த����⥫쭮��� �����樨\t%.2f �",(float)per*nper/1000);
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
	printf("\n\n����쭮 ᣥ���஢���:");
	printf("\n��ਮ�\t\t\t\t%.1f ���",prod*1000/i);
	printf("\n�����\t\t\t\t%.3f ��",i/prod);
	printf("\n�த����⥫쭮��� �����樨\t%.2f �",prod);
	printf("\n�訡�� \t\t\t\t%.3f %",(prod/((float)per*nper/1000)-1)*100);
	printf("\n������ ��ꥬ ���� � �����\t\t");
	scanf("%lf",&vol);
	printf("\n����祭 ��室:\t\t\t%f �/�",vol*1000/dlit/nper);
	}
}



