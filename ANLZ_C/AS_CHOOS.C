/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CHOOS
* Describe  :
* File      : C:\ALEX\WORK\AS_CHOOS.C
* Last edit : 04.07.93 00.08.04
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <alloc.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_list.h"
#include "as_fndir.h"
#include "as_choos.h"

extern asChoice mnStd[2];

FTIME chooseftime (int x1,int y1,char *fname)
/***********
* Describe : выбоp записи по вpемени из файла для загpузки
* Params   : int x1      - откуда				| интерактивн. режим, возможно удален.
*          : int y1      - выводить     | записи из файла.
*          : char *fname - имя файла    |
* Return   : FTIME       - вpемя нужной записи
* Call     : getfilelist,grlist,ftimecpy,ftimetoa
***********/
{
 asList far *list = NULL;
 FTIME  return_item={{-1,-1,-1},{-1,-1,-1}};
 int		file;
 int 		user,userdel,i,num;
 long		offs;
 char		*buf = NULL;
 char *delmessage[] = {"                              ",
											"    record :                  ",
											" from file :                  ",
											"                              ",
											NULL};

BEGIN:

 list = (asList far *) getfilelist(fname);
 if (list==NULL)
	{
	message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoTime,mnStd);
	return (return_item);
	}

 user = grlist2 (" Choose time to load: ",list,x1,y1,3,10);
											/* user=номер записи по порядку, начиная с 1 */
 if (user>0) 	 ftimecpy (&return_item,&(list[user-1].time));
 else                 /* нормальное завершение */
	 {
		switch (user)     /* в случае ошибки, удаления записи, изменения директории */
			{
			case menuChDir         :
			case menuEscape        :	break;

			case menuErr_NoItems   :
				message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoItems,mnStd);
				break;

			case menuErr_ImgSize   :
				message(" Error: ",getmaxx()/4,getmaxy()/4,msgImgSize,mnStd);
				break;
			case menuErr_NoImgMem  :
				message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoImgMem,mnStd);
				break;
								 /* в случае удаления записи, ее номер будет с обратным */
								 /* знаком и на 100 больше */
			default                :
				if (user<100)
					{
					user=-user-100;
					strcpy(&delmessage[1][13],list[user-1].name);
					strcpy(&delmessage[2][13],fname);
					userdel=message(" Delete: ",getmaxx()/4,getmaxy()/4,delmessage,mnStd);
					farfree (list);
					if (userdel == 1)
						{									/* удаляем запись */
						buf = calloc(1,512);
						if(buf==NULL)
							{message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoImgMem,mnStd);
							break;
							}
						file = open(fname,O_RDWR|O_BINARY);
						for(i=1;i<=user;i++)
							{     								/* ставим указатель на нужное место */
							read(file,&offs,sizeof(offs));
							lseek(file,offs-sizeof(offs),1);
							}
						while((num=read(file,buf,512)) == 512)
							{										/* перезаписываем блоками по 512 */
							lseek(file,(-offs-num),1);
							write(file,buf,num);
							lseek(file,offs,1);
							}
						lseek(file,(-offs-num),1);
						write(file,buf,num);								/* урезаем файл */
						chsize(file,lseek(file,0,1));
						close(file);
						free(buf);
						}
					goto BEGIN;
					}
			}
		}
 farfree (list);
 return (return_item);
}

FTIME far *chooseftimes (int x1,int y1,char *fname,int *num)
/***********
* Describe : выбоp нескольких записей (списка) по вpемени
* Params   : int x1      - откуда
*          : int y1      - выводить
*          : char *fname - имя файла
*          : int *num    - число выбpанных вpемен
* Return   : FTIME       - массив выбpанных вpемен
* Call     :
***********/
{
asList far *list = NULL;
asFile	buf;
FTIME  far *result = NULL;
static char fname_to[9];
int user,i,j,err;

*num = 0;
list = (asList far *) getfilelist(fname);
if (list==NULL)
	{
	message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoTime,mnStd);
	return (result);
	}

user = grlist2 (" Choose times to load: ",list,x1,y1,3,10);
switch (user)
	{
	case menuChDir				 :
	case menuEscape        : break;

	case menuErr_NoItems   :
		message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoItems,mnStd);
		break;

	case menuErr_ImgSize   :
		message(" Error: ",getmaxx()/4,getmaxy()/4,msgImgSize,mnStd);
		break;

	case menuErr_NoImgMem  :
		message(" Error: ",getmaxx()/4,getmaxy()/4,msgNoImgMem,mnStd);
		break;

	case menuRecCopy			 :		/* копирование записей в другой файл */
															/* запрос имени файла куда копировать */
		setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*2,
									 getmaxy()/4+YBUTTONSIZE*3," Enter filename for Copy: ");
		err=editstring (getmaxx()/4+XDIGIT*2.5,getmaxy()/4+YBUTTONSIZE*1.5,8,fname_to);
		killframe ();
		if (err)	break;
																				/* выделяем паmять под буфер */
		buf.record = (asRecPtr) farcalloc (1L,3000L);
		if (buf.record==NULL)			break;
																					/* просматриваем все записи */
		for (i=0;strcmp(list[i].name,"LAST");i++)
			{           							      		/* если запись маркирована */
			if (!list[i].mark)	continue;
			ftimecpy (&buf.ftime,&(list[i].time));
			if (incorrect(&buf.ftime)) 				break;			/* считываем ее */
			if (load(&buf,fname,NULL)!=E_OK)	break;
																		/* изменяем имя и записываем в файл */
			strncpy (buf.record->name,fname_to,8);
			if (save (&buf,NULL)!=E_OK)	break;
			}
		farfree (buf.record);
		break;

	default                :
		if (user>0)  /* нормальное завершение */
			{
			for (i=0,(*num=0);strcmp(list[i].name,"LAST");i++)
				if (list[i].mark)			(*num)++;  /* подсчитываем маркированные записи */

			result = (FTIME far *) farcalloc ((unsigned long)(*num),sizeof(FTIME));
			if (result==NULL)				 return (result);  /* выделяем память */

			for (i=0,j=0;i<(*num)&&strcmp(list[j].name,"LAST");j++)
				if (list[j].mark)													/* переписываем времена */
					{ftimecpy (result+i,&(list[j].time));   /* в формат FTIME для возврата */
					i++;
					}
			}
		else	/* неопределенное завершение */
			break;
	}
	farfree (list);
	return (result);
}

char *choosefile (int x1,int y1,char *wc,char *result)
/***********
* Describe : выбрать имя файла в тек. директории по шаблону
* Params   : int x1    - откуда
*          : int y1    - выводить
*          : char *wc  - шаблон для поиска
* Return   : char *    - имя файла, выбранного пользователем
* Call     : setframe, killframe, setbutton
***********/
{
 char fname [20];
 asList far *listfiles;
 int n,user;

 listfiles = getdir(wc,&n);
 if (n==0)
 {
  message (" Can't find *.spe ",getmaxx()/3,getmaxy()/3,(char **)errNoFiles,mnStd);
  return (NULL);
 }

 user=grlist1(" Choose file to load: ",listfiles,x1,y1,4,10);
 if (user!=menuEscape)
 {
  strcpy (result,listfiles[user-1].name);
  strcpy (fname,result);
  farfree (listfiles);
  return (fname);
 }
 else
 {
  farfree (listfiles);
	return (NULL);
 }
}

int numrecinfile (char *fname)
/***********
* Describe : число записей в файле
* Params   : char *fname - имя файла
* Return   : int         - число записей
* Call     :
***********/
{
 FILE *f=NULL;
 long ofs=0;
 int num=0;

 f = fopen (fname,"rb");
 if (f==NULL)
  return 0;

 do
 {
  if (fread (&ofs,sizeof(long),1,f)==0)
   break;
  if (fseek (f,ofs-sizeof(long),SEEK_CUR)!=0)
   break;
  num++;
 }
 while (!feof(f));
 fclose (f);
 return (num);
}

asList far *getfilelist (char *fname)
/***********
* Describe : массив стpуктуp, описывающих содеpжимое файла
* Params   : char *fname  - имя файла
* Return   : asList    - массив
* Call     :
***********/
{
 asList far *result=NULL;
 asRecord buffer;
 FILE *f=NULL;
 long ofs=0;
 int i,numrec;
 char tmp[10];

 f = fopen (fname,"rb");
 if (f==NULL)
	return NULL;

 numrec = numrecinfile (fname);
 result = (asList far *) farcalloc (numrec+1,sizeof(asList));
 if (result==NULL)
 {
  message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoMemory,mnStd);
  return (NULL);
 }

 for (i=0;i<numrec;i++)
 {
  fread (&ofs,sizeof(long),1,f);
  fread (&(result[i].time),sizeof(FTIME),1,f);
  fread (&buffer,(size_t)49,1,f);  /* длина заголовка-49 байт */

  ftimetoa(&(result[i].time),result[i].name);
#ifdef ENGLISH
	strcpy (result[i].info,isspectrum(&buffer) ? " Spe" : " Sig");
	strcat (result[i].info,islogariphm(&buffer) ? " Log " : " Lin ");
	strcat (result[i].info,itoa(buffer.hfreq,tmp,10));
	strcat (result[i].info," Hz ");
	strcat (result[i].info,modestr(&buffer));
/*  strcat (result[i].info," HFreq:");	*/
	strcat (result[i].info,itoa(buffer.number,tmp,10));
	strcat (result[i].info," Lines");

	if ((buffer.mode)[0]=='o')   /* если огибающая */
	{
	 strcat (result[i].info," Envlp:");
	 strcat (result[i].info,itoa(afreqnum(&buffer),tmp,10));
	 strcat (result[i].info," Hz");
	}

#else
	strcpy (result[i].info,isspectrum(&buffer) ? " Спе" : " Сиг");
	strcat (result[i].info,islogariphm(&buffer) ? " Лог " : " Лин ");
	strcat (result[i].info,itoa(buffer.hfreq,tmp,10));
	strcat (result[i].info," Гц ");
	strcat (result[i].info,modestr(&buffer));
/*  strcat (result[i].info," HFreq:");	*/
	strcat (result[i].info,itoa(buffer.number,tmp,10));
	strcat (result[i].info," Линий");

	if ((buffer.mode)[0]=='o')   /* если огибающая */
	{
	 strcat (result[i].info," Огиб.:");
	 strcat (result[i].info,itoa(afreqnum(&buffer),tmp,10));
	 strcat (result[i].info," Гц");
	}
#endif

	fseek (f,(size_t)ofs-sizeof(long)-sizeof(FTIME)-49,SEEK_CUR);
 }
 strcpy (result[numrec].name,"LAST");

 fclose (f);
 return (result);
}