/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : _FNDIR
* Describe  : ��������� _fndir
* File      : C:\ALEX\ANLZR\_FNDIR.H
* Last edit : 30.05.93 00.58.33
* Remarks   :
*****************************************************************************/

#define MAX_WILDCARD_LEN 31      /* ���ᨬ��쭠� ����� 蠡���� */
#define DYN_ARRAY_SIZE   50      /* ��砫�� ࠧ��� ���ᨢ� ������� ffblk */
#define INC_ARRAY_SIZE   25      /* ���饭�� ���ᨢ� */

struct ffblk* fn_edir (char wildcard[][MAX_WILDCARD_LEN],int num_wildcard,
                       struct ffblk *files,int *num_files);
/***********
* Describe : �㭪�� ���᪠ 䠩��� �� ������� 蠡�����.
* Params   : char wildcard[][]   - ���ᨢ 蠡����� ������ �� ����� MAX_WILDCARD_LEN
*          : int num_wildcard    - �᫮ 蠡�����
*          : struct ffblk *files - 㪠��⥫� �� ���ᨢ ������� ffblk (�. dos.h)
*          : int *num_files      - �᫮ ��������� �� 蠡���� 䠩���
* Return   : struct ffblk*       - 㪠��⥫� �� ���ᨢ ������� ffblk
* Call     :
***********/

asList far *getdir (char *wc,int *numfiles);
/***********
* Describe : �뤠�� ᯨ᮪ 䠩��� � ��p���p��
* Params   : char *wc      - 蠡��� ��� ���᪠
*          : int *numfiles - �᫮ ��������� 䠩���
* Return   : asList far *  - ���ᨢ ��p���p
* Call     : fn_edir
***********/