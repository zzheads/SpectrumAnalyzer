/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : _PRINT
* Describe  :
* File      : D:\TC\WORK\_PRINT.H
* Last edit : 12-21-93 09:10:07pm
* Remarks   :
*****************************************************************************/


void print_screen (void);
/***********
* Describe : �뢮� ��࠭� � ���. ०��� �� �ਭ��
* Params   : void
* Return   : void
* Call     :
***********/

void printarea (int x1,int y1,int x2,int y2);
/***********
* Describe : �뢮� ��� ��࠭� � ���. ०��� �� �ਭ��
* Params   : int x1 - ���न���� ��אַ㣮�쭮� 
*          : int y1 - ������ ���
*          : int x2 - �뢮�� ��
*          : int y2 - �����
* Return   : void
* Call     : 
***********/

void printrow(int x,int y1,int y2);
/***********
* Describe : ����� ��ப� ����� x
* Params   : int x - ����� ��ப�
* Return   : void
* Call     :
***********/

int status(void);
/***********
* Describe : �⥭�� ���ﭨ� �ਭ�� :
*          : �ਭ�� ������祭, �� ����� � ���� �㬠�� :
*          : 1x0x0xx0
*          : �����  �
*          : �����  ��� ��� �������� (1 = if timed out)
*          : ���������� ��� �訡�� �����-�뢮�� (1 = if I/O error)
*          : ���������� 1 = on-line ( on-line �� �ਭ�� star �� 0)
*          : ���������� ���� , �᫨ 1 - ��� �㬠��
*          : ���������� �� �஢��塞, ��� �᫨ 1 - ���⢥ত����
*          : ���������� 1 = �� �����
*          :
* Params   : void
* Return   : int (1) - �� ��ଠ�쭮, (0)-�訡��
* Call     :
***********/

char status_prn(void);
/***********
* Describe : ����� ���ﭨ� �ਭ��
* Params   : void
* Return   : char
* Call     :
***********/

void put_out(char character);
/***********
* Describe : �뢮� ᨬ���� �१ ����
* Params   : char character - ᨬ���
* Return   : void
* Call     :
***********/