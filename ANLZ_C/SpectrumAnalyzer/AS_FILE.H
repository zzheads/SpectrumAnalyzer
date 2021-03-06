/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : AS_FILE
* Describe  : ��������� 䠩�� as_file.c
* File      : C:\ALEX\WORK\AS_FILE.H
* Last edit : 27.12.92 00.30.12pm
* Remarks   :
*****************************************************************************/

typedef struct         /* � asFile.FTIME.time.ti_hung �� ���� ���ᯮ���- */
{                      /* ����� ���� ���� ����� ᥪ㭤� �����뢠����    */
 long      offset;     /* �����䨪��� �ଠ� �����. ��� ��-90 �� = 1  */
 FTIME     ftime;      /* �� � �ணࠬ�� ����஢�� �� �� ������������  */
 asRecPtr  record;     /* �� ���� �㤥� ������, �᫨ � 䠩�� .spe ����  */
}                      /* �����뢠���� ����� �� ��㣨� �ਡ�஢. */
asFile;                /*                             */

/*  ������� 䠩�� ������:                                                  */
/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */
/*  | ofs : tm : rec || ofs : tm : rec || ofs : tm : rec || ofs : tm : rec | */
/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */
/*       |             ^   |             ^   |             ^   |             */
/*       |             |   |             |   |             |   |             */
/*       +-------------+   +-------------+   +-------------+   +-------> ... */
/*  long   ofs   - ᬥ饭�� �� ᫥�. ����� 4 ����                          */
/*  FTIME  tm    - �६� ᮧ����� ����� 8 ����(��।����� � AS_FTIME.H) ���:*/

/*                 typedef struct			struct date {		�⠭���⭠� �������	 */
/*									 {               		int da_year;      current year 				*/
/*									 struct	date;      	char da_day;      day of the month 		*/
/*									 struct	time;       char da_mon;      month (1 = Jan) 		*/
/*									 } FTIME            }; 																		*/
/*                                                    �⠭���⭠� �������   */
/*																		struct time {                           */
/*																		unsigned char  ti_min;      minutes 		*/
/*																	  unsigned char  ti_hour;     hours 			*/
/*  							��થ� �ଠ� ->  	unsigned char  ti_hund;     hundredths of seconds */
/*  							����� �.���			unsigned char  ti_sec;      seconds 		*/
/*																		 };                                     */
/*																																						*/
/*  asRecord rec - ������ (��६����� �����)                                 */
/*                 ����� ����� = 49 + �᫮ ����⮢ (����)                 */
/*                                 |                                         */
/*                                 +--- ࠧ��� ����ﭭ��� ��������� �����  */

int load (asFile *file,char *fname,void (*func)(char *));
/***********
* Describe : �⥭�� ����� �� 䠩�� �� �६��� � file (��� � ࠢ�� �६����,
*          : ��� ����� � ����訬, ��� ��᫥���� ������ � 䠩��)
* Params   : asFile *file - �㤠 ����
*          : char *fname  - ��� 䠩�� ��� �⥭��
*          : void (*func) - 㪠�. �� �㭪�� ᮮ�饭��
* Return   : int          - ��� �訡��
* Call     :
***********/

int save (asFile *file,void (*func)(char *));
/***********
* Describe : ������ ����� � 䠩�, 㯮�冷稢��� ����� � 䠩�� �� �६���
* Params   : asFile *file - �� �����뢠��
*          : void (*func) - 㪠�. �� �㭪�� ᮮ�饭��
* Return   : int          - ��� �訡��
* Call     :
***********/

int delete (char *fname,FTIME t,void (*func)(char *));

int copy (asFile *file_from,asFile *file_to,char *from,char *to);

long setfile (FILE *f,FTIME *t);
/***********
* Describe : ��⠭����� 㪠��⥫� � 䠩�� �� ��砫� ��ࢮ� �����
*          : � ����襩 ��⮩, 祬 ������
* Params   : FILE *f  - 䠩�
*          : FTIME *t - ���
* Return   : long     - ᬥ饭�� �� ��砫� 䠩�� �� �㦭��� ����
* Call     :
***********/

