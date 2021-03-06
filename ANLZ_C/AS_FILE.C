/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_FILE
* Describe  : ����� � 䠩����
* File      : C:\ALEX\WORK\AS_FILE.C
* Last edit : 07.01.93 01.46.25
* Remarks   :
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <alloc.h>
#include <string.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"

int load (asFile *file,char *fname,void (*func)(char *))
/***********
* Describe : �⥭�� ����� �� 䠩�� �� �६��� � file (��� � ࠢ�� �६����,
*          : ��� ����� � ����訬, ��� ��᫥���� ������ � 䠩��)
* Params   : asFile *file - �㤠 ����
*          : char *fname  - ��� 䠩�� ��� �⥭��
*          : void (*func) - 㪠�. �� �㭪�� ᮮ�饭��
* Return   : int          - ��� �訡��
* Call     :
***********/
{
 FILE *fout=NULL;                             /*                             */
 FTIME fa;                                    /*                             */
 long size;
                                              /*                             */
 ftimecpy (&fa,&(file->ftime));               /* �㦭�� �६� ��࠭塞 � fa */
 fout = fopen(fname,"rb");                    /*                             */
 if (fout==NULL)                              /*                             */
  return (E_READ);                            /* �訡�� �⥭��               */
 else                                         /*                             */
  if (func!=NULL)                             /* ᮮ�頥�, ����� 䠩� �⠥� */
   func (fname);                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  fread(&(file->offset),sizeof(long),1,fout); /* �⠥� ������               */
  fread(&(file->ftime),sizeof(FTIME),1,fout); /*                             */
  size = file->offset-sizeof(long)-sizeof(FTIME);
  fread((byte *)file->record,sizeof(byte),(size_t)size,fout);
 }
 while (! ( feof(fout) || (ftimecmp(&(file->ftime),&fa)>=0) ) );
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}

int save (asFile *file,void (*func)(char *))
/***********
* Describe : ������ ����� � 䠩�, 㯮�冷稢��� ����� � 䠩�� �� �६���
* Params   : asFile *file - �� �����뢠��, ⠬ �� ᮤ�ন��� � ��� 䠩��
*          : void (*func) - 㪠�. �� �㭪�� ᮮ�饭��
* Return   : int          - ��� �訡��
* Call     :
***********/
{
 char fname[20]="        ";                   /*                             */
 FILE *fout=NULL;                             /*                             */
 int i;                                       /*                             */
 long pos,len;                                /*                             */
 byte far *buffer=NULL;                       /*                             */
 struct ftime t;
                                              /*                             */
 for (i=7;((i>=0)&&(file->record->name[i]==' '));i--)
  ;
 for (;i>=0;i--)                              /*                             */
 {                                            /*                             */
  if (file->record->name[i]!=' ')             /* �����塞 �஡��� '_'        */
   fname[i]=file->record->name[i];            /*                             */
  else                                        /*                             */
	 fname[i]='_';                              /*                             */
 }                                            /*                             */
 fname[8]=0;
 strcat (fname,".spe");                       /* ���७�� - "spe"          */
 fout = fopen(fname,"a+b");                   /*                             */
 if (fout==NULL)                              /*                             */
 {
  printf ("\n Cant write to %s",fname);
  return (E_WRITE);                           /* �訡�� ����� 䠩��         */
 }
 else                                         /*                             */
  if (func!=NULL)                             /* ᮮ�頥� ����� 䠩� ��襬   */
   func (fname);                              /*                             */
 pos = setfile (fout,&(file->ftime));         /* ����� � �⮩ ����樨       */
 len = filelength ( fileno(fout) );           /* ����� 䠩��                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* ��� �����                  */
	fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ��뢠�� ���⮪ � ����,   */
  fseek (fout,pos,SEEK_SET);                  /* ��१��� 䠩�               */
 }
 file->offset = sizeof(long)+sizeof(FTIME)+file->record->number+49;
 fwrite(&(file->offset),sizeof(long),1,fout); /* �����뢠�� ���� ������      */
 fwrite(&(file->ftime),sizeof(FTIME),1,fout); /* � ����� 䠩��               */
 fwrite((byte *)file->record,sizeof(byte),(size_t)(file->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* �����뢠�� ���⮪          */
 }                                            /*                             */
 t.ft_year  = (unsigned) (file->ftime.date.da_year-1980);
 t.ft_month = (unsigned) (file->ftime.date.da_mon);
 t.ft_day   = (unsigned) (file->ftime.date.da_day);
 if (setftime (fileno(fout),&t)!=0)
 {
  printf ("\n Error in setting file date/time");
  abort ();
 }
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

int delete (char *fname,FTIME t,void (*func)(char *))
{
 FILE *fout=NULL;                             /*                             */
 long pos1,pos2,len;                          /*                             */
 byte far *buffer=NULL;                       /*                             */
                                              /*                             */
 fout = fopen(fname,"a+b");                   /*                             */
 if (fout==NULL)                              /*                             */
  return (E_WRITE);                           /* �訡�� ����� 䠩��         */
 else                                         /*                             */
  if (func!=NULL)                             /* ᮮ�頥� ����� 䠩� 㤠�塞 */
   func (fname);                              /*                             */
 pos1 = setfile (fout,&t);                    /* 㤠���� � �⮩ ����樨      */
 ftimeinc (&t,1);
 pos2 = setfile (fout,&t);
 len = filelength ( fileno(fout) );           /* ����� 䠩��                 */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos2,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* ��� �����                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos2),fout);
 }
 chsize ( fileno(fout),pos1 );                /* ��뢠�� ���⮪ � ����,   */
 fseek (fout,pos1,SEEK_SET);                  /* ��१��� 䠩�               */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos2),fout);
  farfree (buffer);                           /* �����뢠�� ���⮪          */
 }                                            /*                             */
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

int copy (asFile *file_from,asFile *file_to,char *from,char *to)
{
 FILE *fout=NULL;                             /*                             */
 long pos,len;                                /*                             */
 byte far *buffer=NULL;                       /*                             */

 load (file_from,from,NULL);
 fout = fopen(to,"a+b");                      /*                             */
 if (fout==NULL)                              /*                             */
  return (E_WRITE);                           /* �訡�� ����� 䠩��         */
 pos = setfile (fout,&(file_to->ftime));      /* ����� � �⮩ ����樨       */
 len = filelength ( fileno(fout) );           /* ����� 䠩��                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* ��� �����                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ��뢠�� ���⮪ � ����,   */
  fseek (fout,pos,SEEK_SET);                  /* ��१��� 䠩�               */
 }
 file_from->offset = sizeof(long)+sizeof(FTIME)+file_from->record->number+49;
 fwrite(&(file_from->offset),sizeof(long),1,fout); /* �����뢠�� ���� ������      */
 fwrite(&(file_from->ftime),sizeof(FTIME),1,fout); /* � ����� 䠩��               */
 fwrite((byte *)file_from->record,sizeof(byte),(size_t)(file_from->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* �����뢠�� ���⮪          */
 }                                            /*                             */
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

long setfile (FILE *f,FTIME *t)
/***********
* Describe : ��⠭����� 㪠��⥫� � 䠩�� �� ��砫� ��ࢮ� �����
*          : � ����襩 ��⮩, 祬 ������
* Params   : FILE *f  - 䠩�
*          : FTIME *t - ���
* Return   : long     - ᬥ饭�� �� ��砫� 䠩�� �� �㦭��� ����
* Call     :
***********/
{                                             /*                             */
 long ofs=0,now=0;                            /*                             */
 FTIME tmp;                                   /*                             */
                                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  now = ftell (f);                            /* ⥪�騩 㪠��⥫� � 䠩��   */
  if (fread (&ofs,sizeof(long),1,f)!=1)       /* ���뢠�� ����� �� 䠩��   */
   break;                                     /* ���� �� ������� 䠩�       */
  if (fread (&tmp,sizeof(FTIME),1,f)!=1)      /* ��� �� ᮢ����� �६���     */
   break;                                     /* ��� ��⠭�� �६� �����   */
  if (fseek (f,ofs-sizeof(long)-sizeof(FTIME),SEEK_CUR)!=0)
   break;                                     /* �᪮���� �६���            */
 }                                            /*                             */
 while (! ( feof(f) || (ftimecmp(&tmp,t) >= 0)/*                             */ ) );
 fseek (f,now,SEEK_SET);                      /*                             */
 return now;                                  /*                             */
}                                             /*                             */

