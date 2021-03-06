/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : _FNDIR
* Describe  : �㭪�� ���᪠ 䠩��� �� ������� 蠡�����.
* File      : C:\ALEX\WORK\GRAPH\_FNDIR.H
* Last edit : 30.05.93 00.50.20
* Remarks   :
*****************************************************************************/

#include <io.h>
#include <fcntl.h>
#include <graphics.h>
#include <stdio.h>
#include <dir.h>
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include "as_ftime.h"
#include "as_list.h"
#include "as_fndir.h"

struct ffblk* fn_edir (char wildcard[][MAX_WILDCARD_LEN],int num_wildcard,
                       struct ffblk *files,int *num_files)
/***********
* Describe : �㭪�� ���᪠ 䠩��� �� ������� 蠡�����.
* Params   : char wildcard[][]   - ���ᨢ 蠡����� ������ �� ����� MAX_WILDCARD_LEN
*          : int num_wildcard    - �᫮ 蠡�����
*          : struct ffblk *files - 㪠��⥫� �� ���ᨢ ������� ffblk (�. dos.h)
*          : int *num_files      - �᫮ ��������� �� 蠡���� 䠩���
* Return   : struct ffblk*       - 㪠��⥫� �� ���ᨢ ������� ffblk
* Call     :
***********/
{
 int n=DYN_ARRAY_SIZE;
 unsigned int blk_size=sizeof(struct ffblk);
 int i,j,k,nomatch,not_unique,found;
 struct ffblk *ptr;

 files=(struct ffblk *) calloc(n,blk_size);
 if (files==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }
 ptr=(struct ffblk *) calloc(n,blk_size);
 if (ptr==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }
 *num_files=0;
 for (i=0;i<num_wildcard;i++)
 {
  nomatch=findfirst(*(wildcard+i),ptr,0);
  while (!nomatch)
  {
   not_unique = 0;
   if (*num_files>0)
    for (j=0;(j<*num_files)&&(!not_unique);j++)
     if (!strcmp(ptr->ff_name,(files+j)->ff_name))
      not_unique=1;
   if (!not_unique)
   {
    (*num_files)++;
    if (*num_files>n)
    {
     n+=INC_ARRAY_SIZE;
     files=(struct ffblk *) realloc(files,n*blk_size);
     if (files==NULL)
     {
      closegraph ();
      printf ("\n Error reading directory: no enough memory. ");
      exit (-1);
     }
    }
    if (*num_files>1)
    {
     found = 0;
     k=*num_files-1;
     for (j=0;(j<k)&&(!found);j++)
     {
      if (strcmp(ptr->ff_name,(files+j)->ff_name)<0)
      {
       found = 1;
       memmove((files+j+1),(files+j),(k-j)*blk_size);
       *(files+j)=*ptr;
      }
     }
     if (!found)
      *(files+k)=*ptr;
    }
    else
     *files=*ptr;
   }
   nomatch=findnext(ptr);
  }
 }
 free(ptr);
 return files;
}

asList far *getdir (char *wc,int *numfiles)
/***********
* Describe : �뤠�� ᯨ᮪ 䠩��� � ��p���p��
* Params   : char *wc      - 蠡��� ��� ���᪠
*          : int *numfiles - �᫮ ��������� 䠩���
* Return   : asList far *  - ���ᨢ ��p���p
* Call     : fn_edir
***********/
{
 struct ffblk *files=NULL;
 asList far *result=NULL;
 int i;
 char tmp[10];

 files = (struct ffblk *)fn_edir (wc,1,files,numfiles);

 result = (asList far *) farcalloc (*numfiles+1,sizeof(asList));
 if (result==NULL)
 {
  closegraph ();
  printf ("\n Error reading directory: no enough memory. ");
  exit (-1);
 }

 for (i=0;i<*numfiles;i++)
 {
  strcpy (result[i].name,files[i].ff_name);
					  /* �ନ�㥬 ��ப� ���ଠ樨 � 䠩�� ����, �६� */
  strcpy (result[i].info,ltoa(files[i].ff_fsize,tmp,10));
  strcat (result[i].info," byt  ");
  strcat (result[i].info,itoa(files[i].ff_fdate & 0x1f,tmp,10));
  strcat (result[i].info,"/");
  strcat (result[i].info,itoa((files[i].ff_fdate & 0x1e0)>>5,tmp,10));
  strcat (result[i].info,"/");
  strcat (result[i].info,itoa(((files[i].ff_fdate & 0xfe00)>>9)+80,tmp,10));
  strcat (result[i].info,"  ");
  strcat (result[i].info,itoa((files[i].ff_ftime & 0xf800)>>11,tmp,10));
  strcat (result[i].info,":");
  strcat (result[i].info,itoa((files[i].ff_ftime & 0x7e0)>>5,tmp,10));

 }
 strcpy (result[*numfiles].name,"LAST");
 farfree (files);
 return (asList far *)(result);
}