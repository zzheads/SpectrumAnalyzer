/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : DB_EXCH     
* Describe  : �㭪樨 p����� (������) � 䠩���� �� dBASE
* File      : W:\WORK\DBF\DB_EXCH.H        
* Last edit : 18.08.93 00.47.14
* Remarks   : �p��� dBASE 䠩�� �. � "�������p �p���" N 1'91 ��p.72
*****************************************************************************/

#ifndef dbfERRORS
#define dbfERRORS
 #define  eOk          (0)
 #define  eNotFound   (-1)
 #define  eFormFile   (-2)
 #define  eNoMemory   (-3)
#endif

/*        ��p���p�  䠩��  DBF                                              */
/*                                                                           */
/* ==========================   ===============================   ========== */
/* # ��������� # ���p���p #   # ���p���p # ��p�. # ������ #   # ������ # */
/* #   䠩��   #            #   #            # ����  #        #   #        # */
/* ==========================...===============================...========== */
/* |  32 ���� |  32 ����  |   |  32 ����  | 1 ����|        |   |        | */
/* |<--------->|<---------->|   |<---------->|<2�!!->|<------>|   |<------>| */
/* � ����� ���. ���� ("*" ��� �஡��), - ���� ����⪨ �� 㤠�����          */
/*                                                                           */
/*        ��p���p� ��������� 䠩�� DBF (����� 32 ����).                    */
/*                                                                           */
/* 1      �㫥��� ���������� ����                      1                   */
/* 2      ��� ��᫥���� ����䨪�樨 (������)            3                   */
/* 5      ��᫮ ����ᥩ                                  4                   */
/* 9      ������ ����� ��������� (������ ���p���p�)   2                   */
/* 11     ����� �����                                   2                   */
/* 13     ��p���p��p�����                                2                   */
/* 15     ���� ����p��� �p�����樨                       1                   */
/* 16     ��p���p��p�����                                13                  */
/* 29     ���� MDX                                       1                   */
/* 30     ��p���p��p�����                                3                   */
/*                                                                           */
/*                                                                           */
/*        ��p���p� �㫥���� ��������筮�� ����                             */
/*                                                                           */
/* ���     ���祭��                                                          */
/* 7 --->  ������祭�� 䠩�� DBT                                             */
/* 6 -+->  ���� SQL                                                          */
/* 5 -+    (⮫쪮 � dBASE IV)                                               */
/* 4 --->  ???                                                               */
/* 3 --->  ���� DBT (� dBASE IV)                                             */
/* 2 -+                                                                      */
/* 1 -+->  ����p ��pᨨ                                                      */
/* 0 -+                                                                      */
/*                                                                           */
/*                                                                           */
/*        ��p���p� ���p���p� (����� 32 ����)                             */
/*                                                                           */
/* 1      ��� ����                        11                                 */
/* 12     ��� ����                        1                                  */
/* 13     ��p���p��p�����                 4                                  */
/* 17     ����� ����                      1                                  */
/* 18     ��᫮ �������� p��p冷�       1                                  */
/* 19     ��p���p��p�����                 13                                 */
/* 32     ������祭�� ⥣� MDX            1                                  */
/*                                                                           */

typedef struct null_byte
{
 unsigned version     : 3;
 unsigned dbt_file    : 1;
 unsigned unknown     : 1;
 unsigned flag_sql    : 2;
 unsigned dbt_connect : 1;
}
NULL_BYTE;

typedef union hdr_byte
{
 char       a;
 NULL_BYTE  b;
}
HDR_BYTE;

typedef struct dbf_header
{
 HDR_BYTE first_byte                 ;  /* 1  */
 char     last_edit               [3];  /* 2  */
 unsigned long    records_num        ;  /* 5  */
 unsigned int     full_length        ;  /* 9  */
 unsigned int     record_length      ;  /* 11 */
 char     reserved1               [2];  /* 13 */
 char     flag_delay_transaction     ;  /* 15 */
 char     reserved2              [13];  /* 16 */
 char     flag_mdx                   ;  /* 29 */
 char     reserved3               [3];  /* 30 */
}
DBF_HEADER;

typedef struct dbf_descriptor
{
 char field_name  [11];
 char field_type      ;
 char reserved1    [4];
 char field_length    ;
 char dec_points      ;
 char reserved2   [13];
 char teg_mdx         ;
}
DBF_DESCRIPTOR;

typedef struct dbffield
{
 char field_name      [12];
 char field_type          ;
 int  field_length        ;
 int  field_dec_points    ;
 unsigned long rec_num    ;
 char far **rec           ;
}
dbfField;

int readfield (FILE *from,dbfField *f);
/***********
* Describe : ������ �p����� p����� � dBASE 䠩����,
*          : ���뢠�� � f �㦭�� ���� � ��p������ �p���
* Params   : FILE *from  - dBASE 䠩�, ��p�� "rb"
*          : dbfField *f - ��p�� �맮��� ᮤ�p��� ��� �㦭��� ����
* Return   : int         - ��� �訡��
* Call     : 
***********/

struct date far *read_dbf_date (FILE *db,char *name,long *num);
/***********
* Describe : ���� ���� �� ��
* Params   : FILE *db   - ��㤠 ����
*          : char *name - ��� ����-���� 
*          : long *num  - �᫮ ��⠭��� ���
* Return   : struct date far * - ���ᨢ ��� �� ��
* Call     : readfield
***********/

float far *read_dbf_number (FILE *db,char *name,long *num);
/***********
* Describe : ���뢠�� �� �� �᫮�� (Numeric) ����
* Params   : FILE *db   - ��㤠 ���뢠��
*          : char *name - ��� 稫����� ����
*          : long *num  - �᫮ �p�⠭��� ����ᥩ
* Return   : float far * - ���ᨢ �ᥫ �� ��
* Call     : readfield
***********/

char far **read_dbf_string (FILE *db,char *name,long *num);
/***********
* Describe : ���뢠�� �� �� �᫮�� (Numeric) ����
* Params   : FILE *db   - ��㤠 ���뢠��
*          : char *name - ��� 稫����� ����
*          : long *num  - �᫮ �p�⠭��� ����ᥩ
* Return   : float far * - ���ᨢ �ᥫ �� ��
* Call     : readfield
***********/

char far *read_dbf_logical (FILE *db,char *name,long *num);

char **getfieldnames (char *,char,int *);