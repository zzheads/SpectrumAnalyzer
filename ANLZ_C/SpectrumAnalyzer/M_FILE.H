/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_FILE
* Describe  :
* File      : C:\ALEX\WORK\M_FILE.H
* Last edit : 30.06.93 01.09.11
* Remarks   :
*****************************************************************************/

void file (void);
/***********
* Describe : �㭪� �������� ���� FILE
* Params   : void
* Return   : void
* Call     : �������
***********/

void receive (void);
/***********
* Describe : �p��� ���p��樨 �� �p���p� � ������ �� � 䠩�� (ᯥ���p��
*          : ��p���p��-��p����/��/����� � �p��� �����)
* Params   : void
* Return   : void
* Call     : message,readmemory,setframe,gprintfxy,killframe,statusline,
*          : grlist,ftimecpy,ftimeinc,save
***********/

void changedir (void);
/***********
* Describe : ᬥ�� ⥪�饩 ��p���p��
* Params   : void
* Return   : void
* Call     : 
***********/

void changetime (void);
/***********
* Describe : ᬥ�� ⥪�饣� ����p������ �p����� (���p�� �ᯮ������
*          : �p� ����� � 䠩�� �p���⮩ ���p��樨)
* Params   : void
* Return   : void
* Call     : 
***********/

void osshell (void);
/***********
* Describe : �p������ ��室 � ���
* Params   : void
* Return   : void
* Call     : 
***********/