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
	 /* ��᫥������� �ࠢ����� �����ᮢ ��⥬� ����᪠ Primera */

void main()
 {
struct  time ts,te;
unsigned pout=0x378,pin=0x379;
int stat,prstat,buf[15000][2],ibuf=0,i,test;
unsigned long front,count=0;
float dt,sum_count,count_pms;
FILE *file;

front=50;	/* 140 ��� �� �ॡ���, �� ����.᪮����� � ����㧪�� �㤥� */
		/* ࠡ���� ���ࠢ��쭮, ���஡����� ��� ����প� � �᫨ */
		/* �� ��ଠ�쭮 ���� �� �⪫����, �.�. �ॡ���� ����⭮ ���!! */

stat=prstat=inp(pin)&0x20;
while(1)
	{
	gettime(&ts);		/* ��������� �६� ��砫� */
	while(ibuf<15000)	/* ���� ������ ����� 100 � �� 3000 ��/���*/
		{while ((stat=inp(pin)&0x20)==prstat)
			{count++; /* ���� 1.686 mks, ����� */
/* ��⮢�� ��ப�, ���� *//*if(count==1000)	{if(test==0) test=stat=0x20; else test=stat=0; break;}*/
			}    /* 1�� = 593 横��, 5�� = 2966 横��� */
		/*if(count<front) continue;    /* �᫨ �ॡ���, ������� ���� ᭮�� */
		prstat=stat;	/* ���������� ���ﭨ� ���㭪� */
		if(stat==0)	/* �᫨ ������� �� �����⨥ */
			{buf[ibuf][0]=count;
			}
		else	/* �᫨ ������� �� ����⨥ */
			{buf[ibuf][1]=count;
			ibuf++; /* �।��騩 横� �����祭 */
			}
				/* ��������� �६� */
		count=0;	/* ����塞 ���稪 横��� */
		}
	gettime(&te);		/* ��������� �६� ����砭�� */
	dt=diftime(ts,te);	/* �६� �ᥩ ����� ᥪ */
	sum_count=0;
	for(i=0;i<ibuf;i++)	sum_count+=(float)buf[i][0]+buf[i][1]; /* ��饥 �᫮ 横��� �� ������ */
	count_pms=sum_count/dt/1000;	/* �᫮ 横��� �� �� */
	file=fopen("gig1.txt","a+");
	fprintf(file,"\nbegin, Record len=%.2f s, Number cucles= %d, Cucle per ms= %.2f",dt,ibuf,count_pms);
	for(i=0;i<ibuf;i++)
		fprintf(file,"\n%.2f\t%.2f",(float)buf[i][0]/count_pms,(float)buf[i][1]/count_pms);
	fclose(file);
	ibuf=0;
	stat=prstat=inp(pin)&0x20;
	}
}


