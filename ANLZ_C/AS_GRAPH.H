/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_GRAPH     
* Describe  : 
* File      : D:\BC\WORK\AS_GRAPH.H        
* Last edit : 12-10-93 08:09:41pm
* Remarks   : 
*****************************************************************************/

enum asScale  {LINEAR=0,LOGARIPHMIC=1};
enum asCursor {HARMONIC=1,SIDEBAND=2,DELTA=3};

#define SCALE(x)  ( yscale ? ( x>step ? 20*log10(x) : 20*log10(step) ) : x )
#define YINSIDE(y) ( y>ylow ? ylow : ( y<yhigh ? yhigh : y ) )
#define XINSIDE(x) ( x>ylow ? xlow : ( x<xhigh ? xhigh : x ) )

#define MIN_DELTA  (1)
#define MAX_DELTA  (50)

int graphwindow (int y1,int y2,asFile far *file);
/***********
* Describe : ����� ��䨪� �㭪樨 �� 䠩�� � 㪠����� �६����
* Params   : int y1           - ���न���� ����
*          : int y2           - � ���� ����
*          : asFile far *file - 䠩� � ��⠭������� �६����
* Return   : int
* Call     :
***********/

void infowindow (int x1,int y1,asRecPtr rec);
/***********
* Describe : ���ଠ�� � �����
* Params   : int x1       - ���न����
*          : int y1       - ����
*          : asRecPtr rec - ������
* Return   : void
* Call     :
***********/

void editfloat (double *,char *);
/***********
* Describe : ।���஢���� �᫠
* Params   : double * - 㪠��⥫� �� �᫮
*          : char *   - ��. ����७��
* Return   : void
* Call     :
***********/

void editfloat2 (double *,double *);
/***********
* Describe : ।���஢���� ���� �ᥫ
* Params   : double * - �᫠
*          : double * -
* Return   : void
* Call     :
***********/

void closeall (void);