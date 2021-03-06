/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_WIND
* Describe  : ��������� �㭪権 ����᪮�� ����䥩�
* File      : C:\ALEX\WORK\GRAPH\AS_WIND.H
* Last edit : 03.01.93 10.18.35
* Remarks   :
*****************************************************************************/

typedef void (*PF_V_V)(void);        /*  㪠��⥫� �� �㭪�� void (void)    */
typedef struct choice                /*                                      */
        {                            /*                                      */
         char            *text;      /*  ⥪�� ��� �뢮��                    */
         PF_V_V          action;     /*  ����⢨� �� �롮�                 */
         int             able;       /*  ����� �� ���� ��࠭�               */
        }                            /*                                      */
				asChoice;                    /*                                      */
                                     /*                                      */
typedef struct hotkey                /*                                      */
        {                            /*                                      */
         char      key;              /*                                      */
         PF_V_V    action;           /*                                      */
         int       able;             /*                                      */
        }                            /*                                      */
        asHotKey;                    /*                                      */

#define MAX_LEN       30
#define C_EDIT_INK    WHITE
#define C_EDIT_PAPER  BLACK
#define XLETSZ        textwidth("_")

#undef max
#undef min

#define max(A,B)  ((A)>(B)?(A):(B))  /*                                      */
#define min(A,B)  ((A)<(B)?(A):(B))  /*                                      */
                                     /*                                      */
#define BUFLEN       200             /*  ࠧ��� ���� �ଠ�. �뢮�� ⥪�� */
#define XSPACE       2               /*  ����� ����� ��ꥪ⮬ � ࠬ��� �� X  */
#define YSPACE       1               /*  ����� ����� ��ꥪ⮬ � ࠬ��� �� Y  */
#define YBUTTONSIZE (textheight("X")+4*YSPACE+1)  /*  ࠧ���� ������ �� ��� X */
#define XBUTTONSIZE (textwidth("    Cancel    ")+4*XSPACE)  /*  � Y          */
#define OK           1               /*                                      */
#define CANCEL       0               /*                                      */
                                     /*                                      */
#define C_F_PAPER    WHITE           /*  梥� 䮭� � setframe                */
#define C_F_INK      BLACK           /*  梥� ��᪨ � setframe              */
#define C_B_PAPER    BLACK           /*  梥� 䮭� � setbutton               */
#define C_B_INK      WHITE           /*  梥� ��᪨ � setbutton             */

#define XDIGIT       textwidth ("8888")
#define YDIGIT       textheight ("8888")


void gprintfxy (int x,int y,char *fmt,...);
/***********
* Describe : ��ଠ�஢���� �뢮� � ��䨪�
* Params   : int x     - ��㤠
*          : int y     - �뢮����
*          : char *fmt - ��ப� �ଠ� (�. printf)
*          : ...       - ��ࠬ���� ��� �뢮��
* Return   : void
* Call     :
***********/

void textfon (int x,int y,char *fmt,...);
/***********
* Describe : "����祭��" �ଠ�஢������ ⥪��
* Params   : int x     - ��㤠
*          : int y     - �뢮����
*          : char *fmt - ��ப� �ଠ� (�. printf)
*          : ...       - ��ࠬ���� ��� �뢮��
* Return   : void
* Call     :
***********/

void textfoff (void);
/***********
* Describe : "�몫�祭��" ⥪��, �뢥������� ��᫥���� �㭪. textfon
* Return   : void
* Call     :
***********/

void setframe (int x1,int y1,int x2,int y2,char *title);
/***********
* Describe : �뢮��� �३� (ࠬ��,���������)
* Params   : int x1      -  ���न���� ������
*          : int y1      -  ���孥�� 㣫�
*          : int x2      -  ���न���� �ࠢ���
*          : int y2      -  ������� 㣫�
*          : char *title -  ���������
* Return   : void
* Call     :
***********/

void killframe (void);
/***********
* Describe : ����⮦��� ��᫥���� �뢥����� �३�
* Params   : void
* Return   : void
* Call     :
***********/

void setbutton (int x1,int y2,char *buttontext,int active,int inverse);
/***********
* Describe : �뢮��� ������
* Params   : int x1           -  ���न���� ������
*          : int y2           -  ���孥�� 㣫�
*          : char *buttontext -  ⥪�� ������
*          : int active       -  䫠� ��⨢����
* Return   : void
* Call     :
***********/

void mainmenu (int y,asChoice *ch);
/***********
* Describe : ������� ��ਧ��⠫쭮� ���� �ணࠬ��
* Params   : int y        - ���� ��㤠 �뢮����
*          : asChoice *ch - ���ᨢ ������� asChoice
* Return   : void
* Call     : setbutton
***********/

int submenu (char *title,int x1,int y1,asChoice *ch);
/***********
* Describe : ���⨪��쭮� �������
* Params   : char *title  - ���������
*          : int x1       - ����
*          : int y1       - ���孨� 㣮�
*          : asChoice *ch - ���ᨢ ����ୠ⨢
* Return   : void
* Call     : setframe,killframe,setbutton
***********/

void reportmemory (int x1,int y1);
/***********
* Describe : ᮮ���� � ᢮������ �����
* Params   : int x1 - ��㤠
*          : int y1 - �뢮����
* Return   : void
* Call     :
***********/

void reporttime (int x1,int y1);
/***********
* Describe : ᮮ���� � ��� � �६���
* Params   : int x1 - ��㤠
*          : int y1 - �뢮����
* Return   : void
* Call     :
***********/

void drawprogress (double pc);
/***********
* Describe : ����� ��業� �믮������ ����樨
* Params   : float pc - ��業� (�ᥣ�� ����� 1.0)
* Return   : void
* Call     : setframe, killframe
***********/

void statusline (char *text);
/***********
* Describe : ��ப� ����� ����� ��࠭� � ᮮ�饭�� � ᢮������ �����
* Params   : char *text   - ᮮ�饭�� � ��ப� �����
* Return   : void
* Call     : reportmemory
***********/

int message (char *title,int x1,int y1,char *text[],asChoice ch[2]);
/***********
* Describe : ᮮ�饭�� � 2 ������
* Params   : char *title    - ���������
*          : int x1         - ��㤠
*          : int y1         - �뢮����
*          : char *text[]   - ⥪��
*          : asChoice ch[2] - ������
* Return   : int
* Call     : setframe, killframe
***********/

int editstring (int x1,int y1,int len,char *str);
/***********
* Describe : ���� ��p���
* Params   : int x1
*          : int y1
*          : int len
*          : char *str
* Return   : int
* Call     : 
***********/

int editint (int x1,int y1,int digit);
/***********
* Describe : ���� 楫��� �᫠
* Params   : int x1
*          : int y1
*          : int digit
* Return   : int
* Call     : 
***********/

int editftime (int x1,int y1,FTIME *t);
/***********
* Describe : p�����p����� �p���
* Params   : int x1
*          : int y1
*          : FTIME *t
* Return   : int
* Call     : 
***********/

void reportdir (int x2,int y1);
/***********
* Describe : ᮮ���� � ⥪�饩 ��४�ਨ
* Params   : int x2 - ��㤠
*          : int y1 - �뢮����
* Return   : void
* Call     : gprintfxy
***********/