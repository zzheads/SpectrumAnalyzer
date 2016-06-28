/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_FILE
* Describe  : Работа с файлами
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
* Describe : Чтение записи из файла по времени в file (или с равным временем,
*          : или первую с большим, или последнюю запись в файле)
* Params   : asFile *file - куда читать
*          : char *fname  - имя файла для чтения
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
* Call     :
***********/
{
 FILE *fout=NULL;                             /*                             */
 FTIME fa;                                    /*                             */
 long size;
                                              /*                             */
 ftimecpy (&fa,&(file->ftime));               /* нужное время сохраняем в fa */
 fout = fopen(fname,"rb");                    /*                             */
 if (fout==NULL)                              /*                             */
  return (E_READ);                            /* ошибка чтения               */
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем, какой файл читаем */
   func (fname);                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  fread(&(file->offset),sizeof(long),1,fout); /* читаем запись               */
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
* Describe : Запись записи в файл, упорядочивает записи в файле по времени
* Params   : asFile *file - что записывать, там же содержится и имя файла
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
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
  if (file->record->name[i]!=' ')             /* заменяем пробелы '_'        */
   fname[i]=file->record->name[i];            /*                             */
  else                                        /*                             */
	 fname[i]='_';                              /*                             */
 }                                            /*                             */
 fname[8]=0;
 strcat (fname,".spe");                       /* расширение - "spe"          */
 fout = fopen(fname,"a+b");                   /*                             */
 if (fout==NULL)                              /*                             */
 {
  printf ("\n Cant write to %s",fname);
  return (E_WRITE);                           /* ошибка записи файла         */
 }
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем какой файл пишем   */
   func (fname);                              /*                             */
 pos = setfile (fout,&(file->ftime));         /* писать с этой позиции       */
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
	fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ситываем остаток в буфер,   */
  fseek (fout,pos,SEEK_SET);                  /* обрезаем файл               */
 }
 file->offset = sizeof(long)+sizeof(FTIME)+file->record->number+49;
 fwrite(&(file->offset),sizeof(long),1,fout); /* записываем нашу запись      */
 fwrite(&(file->ftime),sizeof(FTIME),1,fout); /* в конец файла               */
 fwrite((byte *)file->record,sizeof(byte),(size_t)(file->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* дописываем остаток          */
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
  return (E_WRITE);                           /* ошибка записи файла         */
 else                                         /*                             */
  if (func!=NULL)                             /* сообщаем какой файл удаляем */
   func (fname);                              /*                             */
 pos1 = setfile (fout,&t);                    /* удалять с этой позиции      */
 ftimeinc (&t,1);
 pos2 = setfile (fout,&t);
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos2,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos2),fout);
 }
 chsize ( fileno(fout),pos1 );                /* ситываем остаток в буфер,   */
 fseek (fout,pos1,SEEK_SET);                  /* обрезаем файл               */
 if ((len-pos2)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos2),fout);
  farfree (buffer);                           /* дописываем остаток          */
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
  return (E_WRITE);                           /* ошибка записи файла         */
 pos = setfile (fout,&(file_to->ftime));      /* писать с этой позиции       */
 len = filelength ( fileno(fout) );           /* длина файла                 */
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  buffer = (byte far *) farcalloc (len-pos,sizeof(byte));
  if (buffer==NULL)                           /*                             */
   return (E_NOMEM);                          /* нет памяти                  */
  fread (buffer,sizeof(byte),(size_t)(len-pos),fout);
  chsize ( fileno(fout),pos );                /* ситываем остаток в буфер,   */
  fseek (fout,pos,SEEK_SET);                  /* обрезаем файл               */
 }
 file_from->offset = sizeof(long)+sizeof(FTIME)+file_from->record->number+49;
 fwrite(&(file_from->offset),sizeof(long),1,fout); /* записываем нашу запись      */
 fwrite(&(file_from->ftime),sizeof(FTIME),1,fout); /* в конец файла               */
 fwrite((byte *)file_from->record,sizeof(byte),(size_t)(file_from->record->number)+49,fout);
 if ((len-pos)>0)                             /*                             */
 {                                            /*                             */
  fwrite(buffer,sizeof(byte),(size_t)(len-pos),fout);
  farfree (buffer);                           /* дописываем остаток          */
 }                                            /*                             */
 fclose (fout);                               /*                             */
 return (E_OK);                               /*                             */
}                                             /*                             */

long setfile (FILE *f,FTIME *t)
/***********
* Describe : Установить указатель в файле на начало первой записи
*          : с большей датой, чем данная
* Params   : FILE *f  - файл
*          : FTIME *t - дата
* Return   : long     - смещение от начала файла до нужного места
* Call     :
***********/
{                                             /*                             */
 long ofs=0,now=0;                            /*                             */
 FTIME tmp;                                   /*                             */
                                              /*                             */
 do                                           /*                             */
 {                                            /*                             */
  now = ftell (f);                            /* текущий указатель в файле   */
  if (fread (&ofs,sizeof(long),1,f)!=1)       /* считываем записи из файла   */
   break;                                     /* пока не кончится файл       */
  if (fread (&tmp,sizeof(FTIME),1,f)!=1)      /* или не совпадут фремена     */
   break;                                     /* или считаное время больше   */
  if (fseek (f,ofs-sizeof(long)-sizeof(FTIME),SEEK_CUR)!=0)
   break;                                     /* искомого времени            */
 }                                            /*                             */
 while (! ( feof(f) || (ftimecmp(&tmp,t) >= 0)/*                             */ ) );
 fseek (f,now,SEEK_SET);                      /*                             */
 return now;                                  /*                             */
}                                             /*                             */

