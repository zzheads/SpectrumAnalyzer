/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_ANALYZ
* Describe  :
* File      : C:\ALEX\WORK\M_ANALYZ.H
* Last edit : 30.06.93 01.07.58
* Remarks   :
*****************************************************************************/

void analyze (void);

void trend (void);
/***********
* Describe : �p�ᬮ�p ��p����p�� ��p���� �� �� �� ��p�����
* Params   : void
* Return   : void
* Call     : choosefile,getfieldnames,submenu,statusline,draw_parameters
***********/

void loading (char *fname);
/***********
* Describe :
* Params   : char *fname
* Return   : void
* Call     :
***********/


void loadbytime (void);
/***********
* Describe : ���p㧪� ����� �� 䠩�� .spe �� ����� � �p����� �����
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/

void cascade (void);
