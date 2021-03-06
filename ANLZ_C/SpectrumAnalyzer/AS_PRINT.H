/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_PRINT
* Describe  :
* File      : C:\ALEX\WORK\PRINT\AS_PRINT.H
* Last edit : 21.01.94 00.49.31
* Remarks   :
*****************************************************************************/

void init_print (int mode,int num);
/***********
* Describe : ���樠������ �ਭ�� � ��⠭���� ��. ०��� ����
* Params   : int mode - ०��:
*          :          0 - �����ୠ� ���⭮���, 60 �/�.
*          :          1 - ������� ���⭮���, 120 �/�.
*          :          2 - ������� ���⭮���, 120 �/�. (�᪮७��)
*          :          3 - ��⢥७��� ���⭮���, 240 �/�.
*          : int num  - �᫮ ���� ��� ����
* Return   : void
* Call     :
***********/

char *status_msg (int i);
/***********
* Describe : ��ப� - ᮮ�饭�� � ���ﭨ� �ਭ��
* Params   : int i - ���� ���ﭨ�
* Return   : char
* Call     :
***********/

int printc (char ch);
/***********
* Describe : ����� ᨬ����
* Params   : char ch - ᨬ��� ��� ����
* Return   : int     - ���ﭨ� �ਭ��
* Call     : 
***********/

int printw (char *w);
/***********
* Describe : ����� ᫮��
* Params   : char ch - ᫮�� ��� ����
* Return   : int     - ���ﭨ� �ਭ��
* Call     :
***********/

void print_screen (void);