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
float dt;
long i;

gettime(&ts);
for(i=0;i<10000000;i++)
       {outp(0x378+2,0x1);
	outp(0x378+2,0x0);};
gettime(&te);
dt=diftime(ts,te);
printf("\nAll Time is: %.2f\t\tOne sucle is: %f mks",dt,dt*1000000/i);
getch();
 }


