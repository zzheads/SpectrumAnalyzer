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
unsigned pout=0x378,pin=0x379;
int stat,prstat,nper=0,buf[1000],ibuf=0;
float dt,prolong,qlong,lic,flow,k_press;
unsigned long i,j,front,count=0,timeo=1000,timec=20000,n_flow,min=20000000,max=0;

		/* �������� ������: */
prolong=0;	/* �-� �ய���.㢥��祭�� �६.������ � % */
qlong=0;	/* �-� �����.㢥��祭�� �६.������ � % */
lic=0.4;        /* ��窠 � % �� ����.��室� */
flow=15;	/* ��砫쭮� ����.��室� � % �� ���� */
n_flow=50;	/* �᫮ 横��� ��� ���.��।����� ��室� */
k_press=25;	/* �-� ������� �६.������ ��-�� ������� �������� � % */

outp(pout,0);	/* ᭠砫� ����뢠�� ���㭪� */
prolong/=100;
qlong/=296600;
lic/=100;
flow/=100;
k_press/=100;
front=50;	/* 140 ��� �� �ॡ���, �� ����.᪮����� � ����㧪�� �㤥� */
		/* ࠡ���� ���ࠢ��쭮, ���஡����� ��� ����প� � �᫨ */
		/* �� ��ଠ�쭮 ���� �� �⪫����, �.�. �ॡ���� ����⭮ ���!! */

stat=prstat=inp(pin)&0x20;
while (1)			/* ���� 1.686 mks, ����� */
begin:	{stat=inp(pin)&0x20;    /* 1�� = 593 横��, 5�� = 2966 横��� */
	count++;		/* ���騢��� ���稪 横��� */
	if(count==2000000)	/* �����⥫� ��⠭����� */
		{printf("\n\n���⥫쭮��� �����ᮢ � �।���� %f - %f ms\n",(float)min/593,(float)max/593);
		return;
		}
	if(stat!=prstat)  	/* �᫨ �஢��� �� ���������, 横� 1.6 ��� */
		{if(count<front) continue;    /* �᫨ �ॡ���, ������� ���� ᭮�� */
		prstat=stat;	/* ���������� ���ﭨ� ���㭪� */
		if(stat!=0)	/* �᫨ ���� �室�. ������ �� ����⨥ */
			{			/* ����ন���� ��� */
			timec=count;	/* ��������� �६� ������.��� */
			i=lic*timec;
			count=0;
			while (count<i)
				{stat=inp(pin)&0x20;
				count++;
				}
			outp(pout,0x20);	/* � ���뢠�� ���㭪� */
			}
		else     	/* �᫨ ������� �� �����⨥ ���㭪� */
			{	/* ����ন���� �� */
			flow=flow-flow/n_flow+(double)timeo/(timeo+timec)/n_flow;
			if(count<min)	min=count;
			else	{if(count>max)	max=count;}
			/* �।��� �������� �� n_flow 横���*/
/*			buf[ibuf]=count;
			ibuf++;
			if(ibuf==1000)
				{for(ibuf=0;ibuf<250;ibuf++)
				printf("\t%d",buf[ibuf]);
				return;
				}	*/
/*			i=count*(prolong+count*qlong); */	/*�᫮ 横��� ����প� */
			i=count*(prolong+count*qlong+k_press*flow);
			timeo=count+i; /* ���������� �६� �����.��� */
			count=0;
			while (count<i)
				{stat=inp(pin)&0x20;
				count++;          /* �᫨ �� �� ����뫨, � */
/*				if(stat!=prstat)  /* ��� ����� ���뢠�� */
  /*					{
					prstat=stat;
					count=0;
					goto begin; /* ���� � ��砫� 横�� */
/*					}	*/
				}
			outp(pout,0);	/* � ����뢠�� ���㭪� */
			}
		count=0;	/* ����塞 ���稪 横��� */
		}
	}
}


