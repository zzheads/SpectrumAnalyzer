#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_list.h"
#include "as_choos.h"
#include "db_exch.h"
#include "as_diag.h"
#include "as_graph.h"
#include "db_graph.h"
#include "as_3d.h"
#include "as_msg.h"
#include "as_def.h"
#include "as_main.h"
#include "m_help.h"
#include "m_analyz.h"

extern int help_context;
#ifdef ENGLISH
asChoice mnAnalyze[]  = {{" Trend        ",trend   ,1},
												 {" Record       ",loadbytime,1},
												 {" Cascade      ",cascade     ,1},
												 {NULL            ,NULL      ,0}};
#else
asChoice mnAnalyze[]  = {{" Тренды       ",trend   ,1},
												 {" Записи       ",loadbytime,1},
												 {" Каскады      ",cascade     ,1},
												 {NULL            ,NULL      ,0}};
#endif

void analyze (void)
{
 int was_c;

 was_c = help_context;
 help_context = CONTEXT_ANALYZE;

#ifdef ENGLISH
 submenu ("  Analyze  ",XBUTTONSIZE,0,mnAnalyze);
#else
 submenu ("  Просмотр ",XBUTTONSIZE,0,mnAnalyze);
#endif
 help_context = was_c;
}

void trend (void)
/***********
* Describe : пpосмотp паpаметpов агpегата из БД по агpегату
* Params   : void
* Return   : void
* Call     : choosefile,getfieldnames,submenu,statusline,draw_parameters
***********/
{
 char fname [30],*field_name,*str_diagnoze;
 char **fields=NULL;
 int was_c,num,choosed,i;
 asList far *fields_list=NULL;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_CONTROL;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[6].able=0;

 if (test()==0)
  destroy_program();                        
                                            /* выбираем имя файла fname */
 while (statusline((char *)slChFile),choosefile(XSPACE,YSPACE*3,"*.dbf",fname)!=NULL)
 {
  fields = getfieldnames (fname,'N',&num);
	if (num==0)
	{
	 return;                   /* нет полей numeric в базе данных */
	}

	fields_list = (asList far *) farcalloc (num+1,sizeof(asList));
	for (i=0;i<num;i++)
	{
	 str_diagnoze = diagnoze(fname,fields[i]);	// проводим анализ таблицы
	 strncpy (fields_list[i].name,str_diagnoze+18,2);	// копируем метку )если есть
	 strcat (fields_list[i].name,fields[i]);	// имя поля
	 strncat (fields_list[i].name,"           ",11-strlen(fields_list[i].name));
	 strncat (fields_list[i].name,str_diagnoze,18);
	 strcpy (fields_list[i].info,str_diagnoze+18);	// заполняем строку-подсказку
   fields_list[i].mark=0;
  }
  strcpy(fields_list[num].name,"LAST");

  do
  {
	 statusline((char *)slChField);
#ifdef ENGLISH
	 choosed = grlist3(" Parameters Warning  Danger    Parameters Warning  Danger    Parameters Warning  Danger    ",
							fields_list,XSPACE,YBUTTONSIZE,3,10);
#else
	 choosed = grlist3(" Параметр   Предупр  Опасно    Параметр   Предупр  Опасно    Параметр   Предупр  Опасно    ",
							fields_list,XSPACE,YBUTTONSIZE,3,10);
#endif
	 if (choosed<1)
		break;
	 field_name = fields[choosed-1];
	 statusline((char *)sldbGraph);
   draw_parameters (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,fname,field_name);
  }
  while (choosed>=1);
  if (fields_list!=NULL)
   farfree (fields_list);

  for (i=0;i<num;i++)
   free(fields[i]);
  free(fields);
 }

 help_context = was_c;
 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[6].able=1;

 statusline((char *)slMain);
 return;
}

void loading (char *fname)
{
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,"\" %12s \"",fname);
}

void loadbytime (void)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 asFile    file;
 char fname [30],timestr[30];
 int was_c,res,num,select;
 asList far *spes=NULL;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_RECORD;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[6].able=0;

 if (test()==0)
	destroy_program();

 file.record = (asRecPtr) farcalloc ((unsigned long)3000,(unsigned long)sizeof(byte));
 if (file.record==NULL)
 {
  printf ("\n No enough memory in LOADBYTIME!");
  abort ();
 }

 while (statusline((char *)slChFile),choosefile(XSPACE,YSPACE*3,"*.spe",fname)!=NULL)
 {
  spes = getfilelist (fname);
  for (num=0;strcmp(spes[num].name,"LAST");num++)
   ;
  do
  {
   statusline((char *)slChFtime);
   file.ftime=chooseftime(5,5,fname);

	 LOAD_GRAPH:
   if (incorrect(&(file.ftime)))
   {
    /* printf ("\n Incorrect ftime"); */
    break;
   }

   for (select=0;ftimecmp(&(spes[select].time),&(file.ftime))!=0;select++)
    ;
   if ((select<0)||(select>=num))
   {
    printf ("\nProblem in function LOAD. #1");
    abort ();
   }

   setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
	     getmaxy()/4+YBUTTONSIZE*4," Loading file ");
   if (load(&file,fname,loading)!=E_OK)
   {
		printf ("\nProblem in function LOAD. #2");
    abort ();
   }
   killframe();
	 statusline((char *)slGraph);
	 res=graphwindow (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,&file);
   if (res!=0)
   {
    select += res;
    if (select<0)
     select = 0;
    if (select>=num)
     select = num-1;
    ftimecpy (&(file.ftime),&(spes[select].time));
    goto LOAD_GRAPH;
   }
  }
  while (!incorrect(&(file.ftime)));
  if (spes!=NULL)
	 farfree (spes);
 }
 if (file.record!=NULL)
  farfree ((asRecPtr) file.record);

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[6].able=1;
 help_context = was_c;
 statusline((char *)slMain);
}

void cascade (void)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 FTIME  far * ftimes;
 asFile far * files;
 char fname [30];
 int was_c,num,i,j;
 int hfreq,type,scale,number,yunit;
 extern asHotKey hotkeys[];

 was_c = help_context;
 help_context = CONTEXT_TRENDS;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[6].able=0;


 if (test()==0)
  destroy_program();

 while (statusline((char *)slChFile),choosefile(XSPACE,YSPACE*3,"*.spe",fname)!=NULL)
    {
		while(statusline((char *)slChFtimes),((ftimes = chooseftimes (5,5,fname,&num))!=NULL&&num>1))
       {
       files = (asFile far *) farcalloc ((unsigned long)num,
				  (unsigned long)sizeof(asFile));
       if (files==NULL)
          {
          printf ("\n No enough memory in TREND!");
          abort();
          }

       for (i=0;i<num;i++)
		  {
          (files[i]).record = (asRecPtr) farcalloc (1L,3000L);
          if ((files[i]).record==NULL)
							{
              printf ("\n No enough memory in TREND for records!");
              abort();
              }
          ftimecpy (&((files[i]).ftime),&(ftimes[i]));
          if (incorrect(&((files[i]).ftime)))
              break;
          if (load(&(files[i]),fname,NULL)!=E_OK)
              {
              printf ("\nProblem in function TREND of loading!");
              abort ();
              }

          if (i==0)
              {
			  hfreq  = files[0].record->hfreq;
			  yunit  = files[0].record->yunit;
			  type   = files[0].record->type;
			  scale  = files[0].record->scale;
				number = files[0].record->number;
			  }
		  else
			   if (hfreq !=files[i].record->hfreq  ||
			   yunit !=files[i].record->yunit  ||
               type  !=files[i].record->type   ||
               scale !=files[i].record->scale  ||
               number!=files[i].record->number)
                   {
                   num--;
                   for (j=i;j<num;j++)
                      ftimecpy (&((files[j]).ftime),&(ftimes[j+1]));
                   i--;
                   continue;
                   }
             drawprogress ((double)i/num);
             }
             drawprogress ((double)1.00);
             farfree (ftimes);

          if (num>1)
                {
                statusline((char *)sl3DGraph);
                graph3D (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,files,num);
                }

          for (i=0;i<num;i++)
                 farfree ((files[i]).record);
          farfree (files);
          }
     }
 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[6].able=1;

 help_context = was_c;
 statusline((char *)slMain);
}

