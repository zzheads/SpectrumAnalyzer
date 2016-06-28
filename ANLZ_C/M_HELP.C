/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : M_HELP     
* Describe  : Контекстная помощь для пpогpаммы СпектpоАнализатоp
* File      : C:\ALEX\WORK\M_HELP.C        
* Last edit : 02.07.93 02.28.55
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "as_main.h"
#include "m_help.h"
#include "as_list.h"

extern char far pathto_as[];
extern asChoice mnStd[2];
extern int help_context;

#ifdef ENGLISH
asChoice mnUpDn[2] = {{"    Next    ",NULL    ,1},
											{"  Previous  ",NULL		,1}};
#else
asChoice mnUpDn[2] = {{" Продолжить ",NULL    ,1},
											{" Вернуться  ",NULL		,1}};
#endif

#define STRLEN   70                    /* число символов в стpоке help'а    */
#define STRNUM   13                     /* число стpок help'а                */

void help (void)       // Тематическая помощь. Показывает меню, состоящее
/***********						  из первых 30 символов первой строки каждой страницы
* Describe :							текстового файла anlzr.hlp (т.е. названия темы).
* Return   : void         После выбора темы показывается вся страница в окне
* Call     :              с заголовком из полной первой строки листа (то же,
***********/            //что функция контекстной помощи см.ниже.
{
int was_c, pagescount=0, user;
asList far *list=NULL;
FILE *file;
char buf[STRLEN];

was_c = help_context;
help_context = CONTEXT_HELP;

file = fopen (fullpath ("anlzr.hlp"),"rt");
if (file==NULL)
	{
	message (" Error: ",100,70,(char **)errNoHelp,mnStd);
	return;
	}
															/* считаем количество страниц */
while(fgets (buf,sizeof(buf),file)!=NULL)
	if (buf[0] == 12)	pagescount++;
																 /* выделяем память для списка заголовков */
list = (asList far *) farcalloc(pagescount+2,sizeof(asList));
if (list==NULL)
	{
	message (" Error: ",getmaxx()/4,getmaxy()/4,(char **)errNoMemory,mnStd);
	fclose (file);
	return;
	}

strcpy (list[pagescount+1].name,"LAST");		/* заполняем список заголовками */
pagescount = 0;
rewind (file);
buf[0] = 12;
do
	{if (buf[0] == 12)
		{if (fgets (list[pagescount].name,sizeof(list[pagescount].name),file)==NULL)	break;
		pagescount++;
		}
	}
while(fgets (buf,sizeof(buf),file)!=NULL);
fclose (file);
										/* выводим список для выбора темы */
while ((user = grlist1 (" Для подробного описания выберите тему: ",list,10,YBUTTONSIZE,2,10)) >0)
	{
	help_context = user-1;
	contexthelp();
	help_context = CONTEXT_HELP;
	}

farfree (list);
help_context = was_c;
}



void contexthelp (void)
/***********                                         текстовый файл anlzr.hlp
* Describe : контекстная помощь                      разбивается на страницы
* Params   : void использует пеpеменную help_context обычным кодом разделителем
* Return   : void                                    страниц 12. Кажд. страница-
* Call     : message, next, previous                 отдельн. тема. Заголовок -первая
***********/                                      // строка страницы. Размер страницы
{                                                 // неограничен, обеспечен скроллинг.
FILE *file=NULL;
char buftitle[STRLEN], buf[STRNUM][STRLEN], *pointbuf[STRNUM+1];
int i, was_c, user, screen_num;
extern int russian_font, Y_SIZEFONT;
long	begin_pos, end_pos;


was_c = help_context;
help_context = CONTEXT_CONTEXT;
for (i=0;i<STRNUM;i++)	pointbuf[i] = buf[i];
pointbuf[STRNUM] = NULL;

//settextstyle (SMALL_FONT,HORIZ_DIR,0);
setusercharsize (12,8,Y_SIZEFONT,8);
																						 /* открываем файл */
file = fopen(fullpath("anlzr.hlp"),"rt");
if (file==NULL)
	{
	message (" Error: ",100,70,(char **)errNoHelp,mnStd);
	return;
	}

i=0;									/* Пропускаем was_c страниц текста */
while (i<was_c)       /* считая разделители страниц код 12 */
	{if(fgets (buf[0],STRLEN,file)==NULL)	break;
	if (buf[0][0] == 12)	i++;
	}
										 /* считываем заголовок */
if(fgets (buftitle,STRLEN,file)==NULL)	return;

user = 0;
begin_pos = ftell (file);	/* запоминаем положение указателя в файле */
end_pos = -1L;
screen_num = 0;            /* счетчик экранов от начала выводимой страницы */

while (user != -1)     /* до нажатия ESC */
	{										 /* считываем STRNUM строк из файла */
	for (i=0,screen_num++;i<STRNUM;i++)   /* либо меньше, если его конец, или конец страницы */
		{                       /* в этом случае устанавливаем end_flag = 1 */
		if((fgets (buf[i],STRLEN,file)==NULL) || (buf[i][0] == 12))
			{end_pos = ftell(file);
			break;
			}
		}
														/* оставшиеся строки делаем пустыми */
	for (;i<STRNUM;i++)		buf[i][0]=NULL;

OUTTEXT:														/* выводим строки на экран */
	user=message(buftitle,0,0,pointbuf,mnUpDn);
	switch (user)
		{
		case 0	:							/* предыдущий экран */
			if(screen_num == 1)		goto	OUTTEXT;
			screen_num-=2;
			end_pos = -1L;
			fseek (file, begin_pos, 0);
			for (i=0;i<(screen_num*STRNUM);i++)
			fgets (buf[0],STRLEN,file);
			break;

		case 1	:							/* следующий экран */
			if (end_pos >= 0)		goto OUTTEXT;
		}
	}

//settextstyle (SMALL_FONT,HORIZ_DIR,4);
setusercharsize (1,1,Y_SIZEFONT,8);
fclose(file);
help_context = was_c;
}

