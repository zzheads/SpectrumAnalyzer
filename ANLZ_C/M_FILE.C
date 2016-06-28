/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_FILE
* Describe  : меню FILE пpогpаммы СпектpоАнализатоp ПР90
* File      : C:\ALEX\WORK\M_FILE.C
* Last edit : 30.06.93 00.08.05
* Remarks   :
*****************************************************************************/

#include <dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <graphics.h>
#include <ctype.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_list.h"
#include "as_check.h"
#include "as_choos.h"
#include "as_fndir.h"
#include "as_graph.h"
#include "as_3d.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_quit.h"
#include "as_main.h"
#include "db_exch.h"
#include "db_graph.h"
#include "as_wind.h"
#include "as_diag.h"
#include "m_file.h"

int REC_TYPE;		// Тип прибора, подключенного для обмена
								// 0 - ПР90, 1 - ПР90.59М
extern int help_context;
extern asChoice mnStd[2];
extern asHotKey hotkeys[];
extern FTIME intime;

#ifdef ENGLISH

asChoice mnFile[]     = {{" Receive      ",receive    ,1},
			 {" Change dir   ",changedir  ,1},
			 {" Change time  ",changetime ,1},
			 {" OS shell     ",osshell    ,1},
			 {" Quit         ",beforequit ,1},
			 {NULL             ,NULL       ,0}};

#else

asChoice mnFile[]     = {{" Прием данных ",receive    ,1},
												 {" Директория   ",changedir  ,1},
												 {" Уст. времени ",changetime ,1},
												 {" Временный вых ",osshell    ,1},
												 {" Выход        ",beforequit ,1},
												 {NULL             ,NULL       ,0}};

#endif

void file (void)
/***********
* Describe : пункт главного меню FILE
* Params   : void
* Return   : void
* Call     : подменю
***********/
{
 int was_c;
 was_c = help_context;
 help_context = CONTEXT_FILE;

#ifdef ENGLISH
 submenu ("  File  ",0,0,mnFile);
#else
 submenu ("  Файл  ",0,0,mnFile);
#endif

 help_context = was_c;
}

void saving (char *fname)
{
 setcolor (BLACK);
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2," %12s ",fname);
}

void receive (void)
/***********
* Describe : пpием инфоpмации из пpибоpа и запись ее в файлы (специфициpуя
*          : диpектоpию-агpегат/цех/завод и вpемя записи)
* Params   : void
* Return   : void
* Call     : message,readmemory,setframe,gprintfxy,killframe,statusline,
*          : grlist,ftimecpy,ftimeinc,save
***********/
{
 extern asChoice mnRec_type[];
 asRecPtr huge *rec = NULL;
 asHeadPtr hdr = NULL;
 asFile    file;
 asList    *list = NULL;
 int i,j,user,remains,was_context;
 char tmp[20];
 char *path[] = {"                                 ",
				 "                                 ",
				 "                                 ",
				 "                                 ",
				 NULL};

 was_context = help_context;
 help_context = CONTEXT_RECEIVE;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[4].able=0;
 hotkeys[5].able=0;
 hotkeys[7].able=0;

 strcpy (msgReceive[2]+20,mnRec_type[REC_TYPE].text); 	// Копирование типа прибора в массив меню

 i = message (" Warning: ",getmaxx()/6,getmaxy()/6,msgReceive,mnStd);
 if ((i==0)||(i==-1))
	return;

 rec = (asRecPtr huge *) readmemory (&hdr,drawprogress);

#ifdef ENGLISH
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
		 getmaxy()/4+YBUTTONSIZE*5," Received information: ");
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,
		" Volume information: %6.3f Kbytes ",(float)(hdr->volume)/512);
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*3,
		" Number of records:  %u records  ",(uint)(hdr->numrec)-1);
#else
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
		 getmaxy()/4+YBUTTONSIZE*5," Данные приняты. ");
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,
		" Объем данных:       %6.3f кбайт ",(float)(hdr->volume)/512);
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*3,
		" Число записей:      %u шт ",(uint)(hdr->numrec)-1);
#endif

 getch ();
 killframe ();

 list = (asList *) calloc (hdr->numrec,sizeof(asList));

 for (i=0;i<hdr->numrec-1;i++)
 {
  strcpy(list[i].name,"  ");
  strcpy(tmp,itoa(i+1,tmp,10));
  strcat(list[i].name,tmp);
	strcat(list[i].name,". ");
  strcat(list[i].name,(char *)rec[i]->name);
 }
 strcpy(list[hdr->numrec-1].name,"LAST");
 remains = hdr->numrec-1;

 while (remains>0)
 {
	statusline((char *)slReceive);

#ifdef ENGLISH																			/* выводим список записей */
	user = grlist2(" Mark records to save: ",list,XSPACE,YDIGIT,4,10);
#else
	user = grlist2(" Отметьте записи для запоминания: ",list,XSPACE,YDIGIT,4,10);
#endif

	if (user==menuEscape)
	{
	 i=message(" Verify: ",getmaxx()/3,getmaxy()/4,msgEscape,mnStd);
	 if (i==1)
	remains=0;
	 continue;
	}

	if (user==menuChDir)	 continue;

			/* напоминание перед записью о времени и директории */
	changetime ();
	getcwd(&path[1][1],MAX_LEN);		/* получаем имя текущей директории */
#ifdef ENGLISH
	j=message(" Write to Directory: ",getmaxx()/4,getmaxy()/4,path,mnStd);
#else
	j=message(" Запись в директорию: ",getmaxx()/4,getmaxy()/4,path,mnStd);
#endif
	if(j != 1)	 continue;		// если нужно возвращаемся к выбору записей

	reportdir (getmaxx(),YBUTTONSIZE);

	setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
		getmaxy()/4+YBUTTONSIZE*4," Saving file ");
	for (i=0;i<hdr->numrec-1;i++)
	 if ((list[i].mark)&&(rec[i]!=NULL))
	 {
	strcpy(rec[i]->name,(char *)memchr(list[i].name,'.',10)+2);
	file.record=rec[i];
	ftimecpy (&(file.ftime),&intime);
	ftimeinc (&(file.ftime),i);
	file.ftime.time.ti_hund = (unsigned char)REC_TYPE;     /* номер версии записывается на место*/
																												 /* сотых долей секунды 1 байт */
	if(REC_TYPE==1)		// перевод формата ПР90.59М в формат ПР90
		{
		j = file.record->kofch;
		file.record->kofch = file.record->res5;
		file.record->res5 = j;
		}
	save (&file,saving);

	strcpy(tmp,itoa(i+1,tmp,10));
		strcat(tmp,".( saved )");
		strcpy(list[i].name,"  ");
    strcat(list[i].name,tmp);
	(list[i]).name[0]=1;

    list[i].mark=0;
    remains--;
   }
  killframe ();
 }

 if (list!=NULL)
  free ((asList *) list);
 if (hdr!=NULL)
	farfree ((byte far *) hdr);
 if (rec!=NULL)
	farfree ((asRecPtr *) rec);

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[4].able=1;
 hotkeys[5].able=1;
 hotkeys[7].able=1;

 help_context = was_context;
 statusline((char *)slMain);
}

/*void changedir (void)     старая версия неинтерактивная */
/***********
* Describe : смена текущей диpектоpии
* Params   : void
* Return   : void
* Call     :
***********/
/*{
 static char path[MAX_LEN];
 int err,j;
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_CHDIR;

 getcwd(path,MAX_LEN);
 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
	   getmaxy()/4+YBUTTONSIZE*4," Enter full path to dir: ");
 err=editstring (getmaxx()/4+XDIGIT,getmaxy()/4+YBUTTONSIZE*2,MAX_LEN,path);
 killframe ();

 if (err)
 {
  help_context = was_c;
  return;
 }

 for (j=0;path[j]!='\0';j++)
  if (path[j]==':')
   setdisk (tolower(path[j-1])-'a');

 err = chdir (path);
 if (err)
  message(" Error: ",getmaxx()/4,getmaxy()/4,msgChDir,mnStd);
 else
  reportdir (getmaxx(),YBUTTONSIZE);
 help_context = was_c;
} */


void changedir (void)     /* новая версия интерактивная */
/***********
* Describe : смена текущей диpектоpии
* Params   : void
* Return   : void
* Call     :
***********/
{
 struct ffblk ffblk;
 struct viewporttype prev_viewport;
 asList far *listdirs=NULL;
 int i,j,k,done,err,user,numdirs,was_c,numdisks;
 static char path[MAX_LEN];
 extern char *longpathmes[];
 char tmp[64];

 was_c = help_context;
 help_context = CONTEXT_CHDIR;

 while(user!=menuEscape)
	 {
	 numdirs=0;
	 done = findfirst("*",&ffblk,FA_DIREC);   /* подсчитываем в numdirs число */
	 while(!done)                               /* директорий */
	 {numdirs++;
	 done = findnext(&ffblk);
	 }
//	 numdirs--;				// уменьшаем число директорий на 1, т.е. свое имя "."
//	 numdisks=setdisk(-1);			/* в numdisks число доступных дисков */
																// начинаем просмотр дисков с диска С
	 for (numdisks=3; !(getcurdir(numdisks,tmp)); numdisks++);
	 numdisks--;								 /* выделяем память под список директорий */
	 listdirs = (asList far *) farcalloc((unsigned long)numdirs+numdisks+2,(unsigned long)sizeof(asList));
	 if (listdirs==NULL)
	 {
	 closegraph ();
	 printf ("\n Error reading directory: no enough memory. ");
	 exit (-1);
	 }

	 findfirst("*",&ffblk,FA_DIREC);   /* заполняем список директориями */
	 for(i=0;i<numdirs;i++)
		{
		if(strcmp(ffblk.ff_name,".") != 0)		// пропускаем имя "."
			{for(j=0;j<i;j++)		// упорядочиваем имена по возрастанию
				{if (strcmp(ffblk.ff_name,listdirs[j].name) < 0)
					{
					for(k=i-1;k>=j;k--)		strcpy(listdirs[k+1].name,listdirs[k].name);
					break;
					}
				}
			strcpy(listdirs[j].name,ffblk.ff_name);
			}
		else            // пропускаем имя "."
			{numdirs--;
			i--;
			}
		findnext(&ffblk);
		}

										 /* и именами доступных дисков */
	 for(i=0;i<numdisks;i++)
	 {listdirs[numdirs+i].name[0] = (char)(i+0x41);
	 listdirs[numdirs+i].name[1] = ':';
	 listdirs[numdirs+i].name[2] = '\0';
	 }

	 strcpy(listdirs[numdirs+numdisks+1].name,"LAST");
#ifdef ENGLISH
	 strcpy(listdirs[numdirs+numdisks].name,"Create Dir");
					/* выводим окно  */
	 user=grlist3(" Change directory: ",listdirs,20,YBUTTONSIZE,4,10);
#else
	 strcpy(listdirs[numdirs+numdisks].name,"Создать Дир.");
					/* выводим окно  */
	 user=grlist3(" Выбор директории: ",listdirs,20,YBUTTONSIZE,4,10);
#endif
	 if (user!=menuEscape)
	 {
	 if(user<=numdirs)
	   {
	   chdir(listdirs[user-1].name);  /* изменяем директорию */
	   if(getcwd(path,MAX_LEN)==NULL)  /* если имя текущего каталога длиннее MAX_LEN */
		 {chdir("..");  					/* возвращаемся */
		 message(" Error: ",getmaxx()/4,getmaxy()/4,longpathmes,mnStd);
		 }
	   }
	 else
	   {
	   if(user==(numdirs+numdisks+1))		/* создаем директорию */
		 {
		 getcwd(path,MAX_LEN);  /* получаем имя текущего каталога (с диском) */
									/* и редактируем в окне */
#ifdef ENGLISH
		 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
					getmaxy()/4+YBUTTONSIZE*4," Enter full path for Create Directory: ");
#else
		 setframe (20,YBUTTONSIZE*2,30+XBUTTONSIZE*3,
					YBUTTONSIZE*6," Введите полное имя создаваемой директории: ");
#endif
		 err=editstring (20+XDIGIT,YBUTTONSIZE*4,MAX_LEN,path);
		 killframe ();
		 if (err)
		   {help_context = was_c;
		   return;
		   }
		 err = mkdir(path);
		 if(err)	message(" Error: ",getmaxx()/4,getmaxy()/4,msgMkDir,mnStd);
		 }
		 else	setdisk(user-numdirs-1); /* переходим на другой диск */
	   }
	 }
	 farfree (listdirs);
	 getviewsettings (&prev_viewport);		// запоминаем параметры пред.окна
	 setviewport(0,0,getmaxx(),getmaxy(),1);	// уст. начало от полн.экрана
	 reportdir (getmaxx(),YBUTTONSIZE);	// выводим имя директории
						// и восстанавливаем параметры пред. окна
	 setviewport(prev_viewport.left,prev_viewport.top,
											prev_viewport.right,prev_viewport.bottom,prev_viewport.clip);
   }
 help_context = was_c;
}


void changetime (void)
/***********
* Describe : смена текущего внутpеннего вpемени (котоpое используется
*          : пpи записи в файлы пpинятой инфоpмации)
* Params   : void
* Return   : void
* Call     :
***********/
{
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_CHTIME;
 editftime (getmaxx()/4,getmaxy()/4,&intime);
 reporttime (getmaxx()-XBUTTONSIZE,0);
 help_context = was_c;
}

void osshell (void)
/***********
* Describe : вpеменный выход в ДОС
* Params   : void
* Return   : void
* Call     :
***********/
{
 char cmdln [50];
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_OSSHELL;
 closegraph ();
#ifdef ENGLISH
 printf ("\nFor return to Spectrum Analyzer type EXIT ... ");
#else
 printf ("\nДля возврата в программу введите команду EXIT ... ");
#endif
 strcpy (cmdln,searchpath("command.com"));
 system (cmdln);
 help_context = was_c;
 main (1,(char **)NULL);
}
