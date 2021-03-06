/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CHOOS
* Describe  :
* File      : C:\ALEX\WORK\AS_CHOOS.H
* Last edit : 04.07.93 00.06.20
* Remarks   :
*****************************************************************************/

FTIME chooseftime (int x1,int y1,char *fname);
/***********
* Describe :
* Params   : int x1
*          : int y1
*          : char *fname
* Return   : FTIME
* Call     :
***********/

FTIME far *chooseftimes (int x1,int y1,char *fname,int *num);
/***********
* Describe : �롮p ��᪮�쪨� ����ᥩ (ᯨ᪠) �� �p�����
* Params   : int x1      - ��㤠 
*          : int y1      - �뢮����
*          : char *fname - ��� 䠩��
*          : int *num    - �᫮ ��p����� �p����
* Return   : FTIME       - ���ᨢ ��p����� �p����
* Call     : 
***********/

char *choosefile (int x1,int y1,char *wc,char *result);
/***********
* Describe : ����� ��� 䠩�� � ⥪. ��४�ਨ �� 蠡����
* Params   : int x1    - ��㤠
*          : int y1    - �뢮����
*          : char *wc  - 蠡��� ��� ���᪠
* Return   : char *    - ��� 䠩��, ��࠭���� ���짮��⥫��
* Call     : setframe, killframe, setbutton
***********/

int numrecinfile (char *fname);
/***********
* Describe : �᫮ ����ᥩ � 䠩��
* Params   : char *fname - ��� 䠩��
* Return   : int         - �᫮ ����ᥩ
* Call     :
***********/

asList far *getfilelist (char *fname);
/***********
* Describe : ���ᨢ ��p���p, ����뢠��� ᮤ�p����� 䠩��
* Params   : char *fname  - ��� 䠩��
* Return   : asList       - ���ᨢ
* Call     :
***********/