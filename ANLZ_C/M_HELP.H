/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : M_HELP     
* Describe  : ���⥪�⭠� ������ ��� �p��p���� �����p���������p
* File      : C:\ALEX\WORK\M_HELP.H        
* Last edit : 02.07.93 02.31.23
* Remarks   :
*****************************************************************************/

#define CONTEXT_ABOUT          5					// �᫠ ������ ᮮ⢥��⢮����
#define CONTEXT_FILE           6          // ����ࠬ ��࠭�� � 䠩�� �����
 #define CONTEXT_CONTROL       13         // �㬥��� ��࠭�� � 0
 #define CONTEXT_RECEIVE       7
// #define CONTEXT_CHDRIVE
 #define CONTEXT_CHDIR         8
 #define CONTEXT_CHTIME        9
 #define CONTEXT_OSSHELL       10
 #define CONTEXT_QUIT          11
#define CONTEXT_ANALYZE        12
 #define CONTEXT_TRENDS        15
 #define CONTEXT_RECORD        14
// #define CONTEXT_CASCADE       15
#define CONTEXT_OPTIONS        16
 #define CONTEXT_CURSORTYPE    17
 #define CONTEXT_WINDOWTYPE    18
 #define CONTEXT_3DPARAMS      19
 #define CONTEXT_DATAPORT      21
 #define CONTEXT_TABLES				 22
 #define CONTEXT_GRAPHMODE     23
 #define CONTEXT_GRAF          24
 #define CONTEXT_PRINT         27
 #define CONTEXT_QUALITY       28
 #define CONTEXT_HEIGHT        29
 #define CONTEXT_PORT          31
 #define CONTEXT_EXPORT        32
 #define CONTEXT_HELP          33
 #define CONTEXT_CONTEXT       34
 #define CONTEXT_DATAREC_TYPE  20
// #define CONTEXT_TOPIC


void help (void);      // ������᪠� ������. �����뢠�� ����, ����饥
/***********						  �� ����� 30 ᨬ����� ��ࢮ� ��ப� ������ ��࠭���
* Describe :							⥪�⮢��� 䠩�� anlzr.hlp (�.�. �������� ⥬�).
* Return   : void         ��᫥ �롮� ⥬� �����뢠���� ��� ��࠭�� � ����
* Call     :              � ���������� �� ������ ��ࢮ� ��ப� ���� (� ��,
***********/            //�� �㭪�� ���⥪�⭮� ����� �.����.

void contexthelp (void);
/***********                                         ⥪�⮢� 䠩� anlzr.hlp
* Describe : ���⥪�⭠� ������                      ࠧ�������� �� ��࠭���
* Params   : void �ᯮ���� ��p������� help_context ����� ����� ࠧ����⥫��
* Return   : void                                    ��࠭�� 12. ����. ��࠭��-
* Call     : message, next, previous                 �⤥��. ⥬�. ��������� -��ࢠ�
***********/                                      // ��ப� ��࠭���. ������ ��࠭���
																									// ����࠭�祭, ���ᯥ祭 �஫����.
