/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_LIST     
* Describe  : �p���᪮� ���� (��p. 141)
* File      : D:\BC\WORK\AS_LIST.H        
* Last edit : 12-10-93 08:14:05pm
* Remarks   : 
*****************************************************************************/

typedef struct list
{
 char  name[31];
 char  info[70];
 FTIME time;
 int   mark;
}
asList;

#define menuEscape           0     /* �롮p �� ᤥ���                        */
#define menuErr_InvGrMode   -1     /* �������⨬� �p���᪨� p����         */
#define menuErr_NoItems     -2     /* ���⮥ ����                            */
#define menuErr_InvForm     -3     /* �������⨬� �p�㬥�� col,row          */
#define menuErr_ScrExceed   -4     /* ���� ���� ��� ��p���                   */
#define menuErr_ImgSize     -5     /* �祭� ����讥 ���� ����                */
#define menuErr_NoImgMem    -6     /* ��� ����� ��� ��p������ ��p���       */
#define menuRecCopy					-99			/* ����஢��� ��࠭�� ����� � 䠩� */
#define menuChDir						-100	 /* ���������� ⥪. ��४��� */

#define TOP_FIELD            2     /* p����p� ����� ���� ���� (� ᨬ�����)   */
#define BOTTOM_FIELD         1     /*                                        */
#define SIDE_FIELD           2     /*                                        */
#define INTERCOL_FIELD       2     /*                                        */

int grlist1 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - ��������� ����
*          : asList list[]          - ᯨ᮪ ������⮢
*          : int page_left          - ���. �p���� ���� ����
*          : int page_top           - ��p��� �p���� ���� ����
*          : int page_cols          - �⮫�殢 � ���� ����
*          : int page_rows          - ��p�� � ���� ����
* Return   : int                    - �᫨ > 0 � ����p ��p������ �������
* Call     :                          ���� �. ���� ���� �訡��
***********/

int grlist2 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - ��������� ����
*          : asList list[]          - ᯨ᮪ ������⮢
*          : int page_left          - ���. �p���� ���� ����
*          : int page_top           - ��p��� �p���� ���� ����
*          : int page_cols          - �⮫�殢 � ���� ����
*          : int page_rows          - ��p�� � ���� ����
* Return   : int                    - �᫨ > 0 � ����p ��p������ �������
* Call     :                          ���� �. ���� ���� �訡��
***********/

int grlist3 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - ��������� ����
*          : asList list[]          - ᯨ᮪ ������⮢
*          : int page_left          - ���. �p���� ���� ����
*          : int page_top           - ��p��� �p���� ���� ����
*          : int page_cols          - �⮫�殢 � ���� ����
*          : int page_rows          - ��p�� � ���� ����
* Return   : int                    - �᫨ > 0 � ����p ��p������ �������
* Call     :                          ���� �. ���� ���� �訡��
***********/