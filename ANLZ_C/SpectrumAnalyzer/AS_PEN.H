/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_PEN     
* Describe  : ����� � ��p�p��
* File      : C:\ALEX\WORK\GRAPH\AS_PEN.C        
* Last edit : 10.09.93 02.05.46
* Remarks   : 
*****************************************************************************/

#define SCURNUMBER  16 // ��६���� ��� ᯥ�.����� ( max �� 16 ���)

void initcursor (char *cursor_name);
/***********
* Describe : ���樠����p�� ��p�p: ���p㦠�� � ������ �� 䠩�� ��p�� ��p�p�
* Params   : char *cursor_name - ��� 䠩�� � ��p���� ��p�p�
* Return   : void
* Call     : 
***********/

void killcursor (void);
/***********
* Describe : 㭨�⮦��� p���� �뢥����� ��p�p (����⠭�������� ����p������)
* Params   : void
* Return   : void
* Call     : 
***********/

void setcursor (int x1,int y1);
/***********
* Describe : ��⠭�������� ��p�p
* Params   : int x1 - ��� ��������
*          : int y1 - ��p�p
* Return   : void
* Call     : 
***********/

void closecursor (void);
/***********
* Describe : �����稢��� p����� � ��p�p��
* Params   : void
* Return   : void
* Call     : 
***********/