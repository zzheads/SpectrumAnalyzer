/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_CHECK     
* Describe  : проверка на наличие в директории файлов
* File      : C:\ALEX\WORK\AS_CHECK.C        
* Last edit : 14.08.93 01.02.16
* Remarks   : 
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <io.h>
#include "as_ftime.h"
#include "as_list.h"
#include "as_check.h"

void check (asList far *list,int dx)
/***********
* Describe : отмечает имеющиеся в директории файлы
* Params   : asList far *list - список имен записей
*          : int dx           - сколько из списка проверять
* Return   : void
* Call     : 
***********/
{
 int i,j,last;
 static char filename[15];
 static char fname[15];

 if (list==NULL)
  return;
 for (i=0;((i<dx)&&(strcmp((list[i]).name,"LAST")));i++)
 {
  strcpy (filename,(char *)(strchr((list[i]).name,'.')+2));
  for (j=7;((j>=0)&&(filename[j]==' '));j--)
   ;
  last = j;
  for (j = last;j>=0;j--)                      /*                             */
  {                                            /*                             */
   if (filename[j]!=' ')                       /* заменяем пробелы '_'        */
    fname[j]=filename[j];                      /*                             */
   else                                        /*                             */
    fname[j]='_';                              /*                             */
  }                                            /*                             */
  fname[last+1]=0;
  strcat (fname,".spe");

  if (access(fname,0)==0)
  {
   (list[i]).mark=1;
   (list[i]).name[0]='*';
  }
  else
  {
   (list[i]).mark=0;
   if ((list[i]).name[0]=='*')
    (list[i]).name[0]=' ';
  }
 }
}