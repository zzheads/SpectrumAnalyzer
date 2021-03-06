/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : DB_EXCH
* Describe  : �㭪樨 p����� (������) � 䠩���� �� dBASE
* File      : W:\WORK\DBF\DB_EXCH.C
* Last edit : 18.08.93 00.44.45
* Remarks   : �p��� dBASE 䠩�� �. � "�������p �p���" N 1'91 ��p.72
*****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include "db_exch.h"

int readfield (FILE *from,dbfField *f)
/***********
* Describe : ������ �p����� p����� � dBASE 䠩����,
*          : ���뢠�� � f �㦭�� ���� � ��p������ �p���
* Params   : FILE *from  - dBASE 䠩�, ��p�� "rb"
*          : dbfField *f - ��p�� �맮��� ᮤ�p��� ��� �㦭��� ����
* Return   : int         - ��� �訡��
* Call     :
***********/
{
 DBF_HEADER header;
 DBF_DESCRIPTOR far *descr = NULL;
 int full_length,rec_length,i,descr_num,found=0,need;
 unsigned long offset=0;
 char term_byte;

 strupr(f->field_name);                    /* ��� ���� �� � ��p孥� p�����p� */
 rewind(from);                                           /* � ��砫� 䠩��   */
 fread (&header,sizeof(header),sizeof(char),from);       /* �⠥� ��������� */
 full_length = header.full_length;                       /* ������ �����     */
 rec_length  = header.record_length;                     /* ����� �����     */
 descr_num = (full_length-sizeof(DBF_HEADER))/sizeof(DBF_DESCRIPTOR);
 descr = farcalloc ((unsigned long) descr_num,(unsigned long)sizeof(DBF_DESCRIPTOR));
 if (descr==NULL)
  return (eNoMemory);                               /* ��� �����          */
 fread (descr,sizeof(DBF_DESCRIPTOR),(size_t)descr_num,from); /* �⠥� ���p���p�  */
 fread (&term_byte,1,sizeof(char),from);                 /* ����-p������⥫� */
 if (term_byte!='\r')                     /* ������ ���� '\r' � dBASE 䠩��  */
 {
  farfree (descr);
  return (eFormFile);                   /* ��������� �p��� 䠩��        */
 }

 fread (&term_byte,1,sizeof(char),from);  /* ����-p������⥫� � dbu �� 1 �������*/
 if (term_byte!=' '&&term_byte!='*')    offset++;  /* � dBASE �� ��⪠ 㤠�����*/
												   /* ����� '*' ��� �஡�� */
 for (i=0;(i<descr_num)&&(!found);i++)            /* �饬 �㦭�� ����        */
 {
  found = (!strcmp(f->field_name,descr[i].field_name));
  if (!found)
   offset+=descr[i].field_length;                /* ᬥ饭�� �� �㦭��� ���� */
  else
   need = i;                                     /* ����p �᪮���� ����      */
 }
 if (!found)
 {
  farfree (descr);
  return (eNotFound);                                  /* ⠪��� ���� ��� */
 }

 fseek(from,offset,SEEK_CUR);               /* ᬥ頥��� �� �㦭��� ����    */

 f->field_type       = descr[need].field_type;   /* ⨯ ����                 */
 f->field_length     = descr[need].field_length; /* ����� ����               */
 f->field_dec_points = descr[need].dec_points;   /* �᫮ ��p ��᫥ �窨   */
 f->rec_num          = header.records_num;       /* �᫮ ����ᥩ � ��       */
 f->rec = (char far **) farcalloc((unsigned long)f->rec_num,(unsigned long)sizeof(char *));
 for (i=0;i<f->rec_num;i++)
 {                                       /* ������ ��� ����                  */
  f->rec[i] = (char *) calloc((size_t)f->field_length+1,sizeof(char));
  if (f->rec[i]==NULL)
  {
   for (i--;i>=0;i--)
	free (f->rec[i]);                    /* �᢮������� p���� �뤥������     */
   return (eNoMemory);                 /* ��� �����                       */
  }
  fread((f->rec)[i],(size_t)f->field_length,1,from); /* �⠥� �� �����             */
  (f->rec)[i][f->field_length] = '\0';      /* ����� ��p���                 */
  fseek(from,(long)rec_length-(f->field_length),SEEK_CUR);
 }                                           /* ᬥ頥��� �� ᫥�. �����    */
 farfree (descr);
 return (eOk);                                         /* �᪮��� ���� � f */
}

char **getfieldnames (char *fname,char field_type,int *num)
/***********
* Describe : 
*          : 
* Params   : fname - ��� 䠩�� .dbf
*          : 
* Return   : ���ᨢ 㪠��⥫�� �� ��ப�, ᮤ�ঠ騥 ����� ����� (��� NULL)
*            � ������� ��६ num - � ᮮ�. � �᫮� �����
* Call     :
***********/
{
 DBF_HEADER header;
 DBF_DESCRIPTOR far *descr = NULL;
 int full_length,i,descr_num,number_found[1024];
 char **result;
 FILE *from;

 if ((from = fopen(fname,"rb"))==NULL)
  return (NULL);

 fread (&header,sizeof(header),sizeof(char),from);       /* �⠥� ��������� */
 full_length = header.full_length;                       /* ������ �����     */
 descr_num = (full_length-sizeof(DBF_HEADER))/sizeof(DBF_DESCRIPTOR);

 descr = farcalloc ((unsigned long) descr_num,sizeof(DBF_DESCRIPTOR));
 if (descr==NULL)
  return (NULL);                                      /* ��� �����          */
 fread (descr,descr_num,sizeof(DBF_DESCRIPTOR),from); /* �⠥� ���p���p�  */

 for (i=0,(*num)=0;i<descr_num;i++)                            /* �饬 �㦭�� ����        */
 {
  if (descr[i].field_type==field_type)
  {
   number_found[(*num)]=i;
   (*num)++;  /* �᫮ ��������� ����� */
  }
 }

 if (!(*num))
 {
  farfree (descr);
  return (NULL);                                  /* ⠪��� ���� ��� */
 }

 result = (char **) calloc ((*num),sizeof(char *));
 for (i=0;i<(*num);i++)
 {
  result[i] = (char *) calloc (12,sizeof(char));
  strcpy (result[i],descr[number_found[i]].field_name);
 }

 farfree (descr);
 fclose (from);
 return (result);
}

struct date far *read_dbf_date (FILE *db,char *name,long *num)
/***********
* Describe : ���� ���� �� ��
* Params   : FILE *db   - ��㤠 ����
*          : char *name - ��� ����-����
*          : long *num  - �᫮ ��⠭��� ���
* Return   : struct date far * - ���ᨢ ��� �� ��
* Call     : readfield
***********/
{
 dbfField tmp;
 struct date far *result = NULL;
 int i,j;
 char y_str[5],m_str[3],d_str[3];

 strcpy (tmp.field_name,name);     /* ��⮢�� � ����� ���� name             */
 if (readfield (db,&tmp)!=0)       /* �᫨ ��� ⠪��� ���� ��� �訡��        */
  return (NULL);
 if (tmp.field_type!='D')          /* �᫨ ���� ⨯� �� ���                 */
  return (NULL);

 *num = tmp.rec_num;               /*  �᫮ �p��⠭��� ����ᥩ             */
 result = (struct date far *) farcalloc(*num,sizeof(struct date));
 if (result==NULL)
  return (NULL);                   /* ��� ����� ��� ���ᨢ ���              */
 for (i=0;i<*num;i++)
 {
  for (j=0;j<4;j++)                /* ���뢠��:                             */
   y_str[j] = tmp.rec[i][j];       /*            � y_str - ��p������         */
  y_str[4] = '\0';                 /*            �p���⠢����� ����          */
  for (j=0;j<2;j++)                /*                                        */
   m_str[j] = tmp.rec[i][4+j];     /*            � m_str - ��p������         */
  m_str[2] = '\0';                 /*            �p���⠢����� �����        */
  for (j=0;j<2;j++)                /*                                        */
   d_str[j] = tmp.rec[i][6+j];     /*            � d_str - ��p������         */
  d_str[2] = '\0';                 /*            �p���⠢����� ���           */
  result[i].da_year = atoi (y_str);
  result[i].da_mon  = atoi (m_str);  /* ��p������ � �p��� ����              */
  result[i].da_day  = atoi (d_str);
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);                  /* ����p�頥� ���ᨢ ���                  */
}

float far *read_dbf_number (FILE *db,char *name,long *num)
/***********
* Describe : ���뢠�� �� �� �᫮�� (Numeric) ����
* Params   : FILE *db   - ��㤠 ���뢠��
*          : char *name - ��� 稫����� ����
*          : long *num  - �᫮ �p�⠭��� ����ᥩ
* Return   : float far * - ���ᨢ �ᥫ �� ��
* Call     : readfield
***********/
{
 dbfField tmp;
 float far *result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* ��⮢�� � ����� ���� name             */
 if (readfield (db,&tmp)!=0)      /* �᫨ ��� ⠪��� ���� ��� �訡��        */
  return (NULL);
 if (tmp.field_type!='N')         /* �᫨ ���� �� �᫮���� ⨯�            */
  return (NULL);

 *num = tmp.rec_num;              /* �᫮ �p��⠭��� ����ᥩ               */
 result = (float far *) farcalloc(*num,sizeof(float));
 if (result==NULL)
  return (NULL);                  /* ��� ����� ��� ���ᨢ �ᥫ             */
 for (i=0;i<*num;i++)
 {
  result[i] = atof (tmp.rec[i]);  /* ��p������ ��p��� � �p��� float         */
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);                 /* ����p�頥� ���ᨢ �ᥫ �� ��           */
}

char far **read_dbf_string (FILE *db,char *name,long *num)
{
 dbfField tmp;
 char far **result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* ��⮢�� � ����� ���� name             */
 if (readfield (db,&tmp)!=0)      /* �᫨ ��� ⠪��� ���� ��� �訡��        */
  return (NULL);
 if (tmp.field_type!='C')
  return (NULL);

 *num = tmp.rec_num;              /* �᫮ �p��⠭��� ����ᥩ               */
 result = (char far **) farcalloc(*num,sizeof(char *));
 if (result==NULL)
  return (NULL);
 for (i=0;i<*num;i++)
 {
  if ((strlen(tmp.rec[i])==0)||tmp.rec[i]==NULL)
   continue;
  result[i] = calloc (tmp.field_length+1,sizeof(char));
  if (result==NULL)
   return (NULL);
  strcpy (result[i],tmp.rec[i]);
 }
 for (i=0;i<*num;i++)
  free (tmp.rec[i]);
 farfree (tmp.rec);
 return (result);
}

char far *read_dbf_logical (FILE *db,char *name,long *num)
{
 dbfField tmp;
 char far *result = NULL;
 int i;

 strcpy (tmp.field_name,name);    /* ��⮢�� � ����� ���� name             */
 if (readfield (db,&tmp)!=0)      /* �᫨ ��� ⠪��� ���� ��� �訡��        */
  return (NULL);
 if (tmp.field_type!='L')
  return (NULL);

 *num = tmp.rec_num;              /* �᫮ �p��⠭��� ����ᥩ               */
 result = (char far *) farcalloc(*num,sizeof(char));
 if (result==NULL)
  return (NULL);
 for (i=0;i<*num;i++)
 {
  result[i] = ((tmp.rec[i][0]=='Y')||(tmp.rec[i][0]=='T'));
  free (tmp.rec[i]);
 }
 farfree (tmp.rec);
 return (result);
}
