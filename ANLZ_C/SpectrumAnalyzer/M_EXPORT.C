/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_EXPORT.C
* Describe  : Экспорт из формата .spe в текстовый формат
* File      :
* Last edit :
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <math.h>

#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_keys.h"
#include "as_rkey.h"
#include "as_wind.h"
#include "as_pen.h"
#include "as_msg.h"
#include "as_main.h"
#include "as_math.h"
#include "as_graph.h"
#include "as_3d.h"
#include "as_list.h"
#include "as_choos.h"
#include "db_exch.h"
#include "as_diag.h"
#include "db_graph.h"
#include "as_def.h"
#include "m_help.h"
#include "m_analyz.h"

extern int help_context;

extern char pathto_as[];
extern int p_x1,p_x2,p_y1,p_y2;
extern asRecPtr p_rec;
extern asHotKey hotkeys[];


void export (void)
/***********
* Describe : загpузка записей из файла .spe, пометка по имени и вpемени
*	   : записи и конвертация в текст. файл .spt выбранных записей
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 FTIME  far * ftimes;
 asFile far * files;
 FILE   *file_spt;
 char fname [30],fname_spt[30],xunit[10],yunit[10],tmp[30];
 int was_c,num,i,j;
 int hfreq,type,scale,number,err,afreq,yunits;
 float far  *a,*b,maxx;
 double KOEFY=1.0;
 extern asHotKey hotkeys[];
 char *msg[] ={"       ESC = Cancel",
							 NULL};
 char *nulmsg[] ={"",
				 NULL};

#ifdef ENGLISH
asChoice errbutton[] = {{"      O'K     ",NULL ,1},
												 {"    RETURN    ",NULL ,1},
												 {NULL            ,NULL ,0}};
asChoice button[] = {{"  Overvrite   ",NULL ,1},
										 {"     Add      ",NULL ,1},
										 {NULL            ,NULL ,0}};
#else
asChoice errbutton[] = {{"  Продолжить  ",NULL ,1},
												 {"  Вернуться  ",NULL ,1},
												 {NULL            ,NULL ,0}};
asChoice button[] = {{"  Переписать  ",NULL ,1},
										 {"   Добавить   ",NULL ,1},
										 {NULL            ,NULL ,0}};
#endif

 was_c = help_context;
 help_context = CONTEXT_EXPORT;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[6].able=0;


 if (test()==0)
	destroy_program();
			/* меню выбора файла */
 while(statusline((char *)slChFile),choosefile(XSPACE,YSPACE*3,"*.spe",fname)!=NULL)
		{                   /* меню выбора записей */
		while(statusline((char *)slChFtime),((ftimes = chooseftimes (5,5,fname,&num))!=NULL&&num>0))
			 { /* выделение памяти под массив записей размера asFile и числом num */
			 files = (asFile far *) farcalloc ((unsigned long)num,
				  (unsigned long)sizeof(asFile));
       if (files==NULL)
		{
		message (" No enough MEMORY: ",getmaxx()/4,getmaxy()/4,(char **)errNoMemory,errbutton);
	  continue;
		}

			 for (i=0;i<num;i++)
		{
		(files[i]).record = (asRecPtr) farcalloc (1L,3000L);
		if ((files[i]).record==NULL)
				{
				message (" No enough MEMORY: ",getmaxx()/4,getmaxy()/4,(char **)errNoMemory,errbutton);
				for(j=0;j<i;j++)	farfree((files[j]).record);
				farfree(files);
				continue;
				}
		ftimecpy (&((files[i]).ftime),&(ftimes[i]));
		if (incorrect(&((files[i]).ftime)))
				break;
		if (load(&(files[i]),fname,NULL)!=E_OK)  /* чтение записей из файл */
				{
				message (" Load file ERROR: ",getmaxx()/4,getmaxy()/4,(char **)nulmsg,errbutton);
				for(j=0;j<num;j++)	farfree((files[j]).record);
				farfree(files);
				continue;
				}

		if (i==0)	/* запоминаем формат 0 записи */
				{
				hfreq  = files[0].record->hfreq;
				type   = files[0].record->type;
				scale  = files[0].record->scale;
				number = files[0].record->number;
	      afreq  = files[0].record->afreq;
	      yunits = files[0].record->yunit;
				}
	  else	       /* контролируем работу только с записями одного формата */
	       if (hfreq !=files[i].record->hfreq  ||
	       type  !=files[i].record->type   ||
	       scale !=files[i].record->scale  ||
	       number!=files[i].record->number ||
				 afreq !=files[i].record->afreq  ||
	       yunits!=files[i].record->yunit)
		   {		/* убираем запись не в формате */
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
/****************************** вставка ************************/

    strcpy(fname_spt,fname);    /* открываем файл приемник .spt */
    fname_spt[strlen(fname_spt)-1]='T';
    setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
       getmaxy()/4+YBUTTONSIZE*4," Export to text file:    ");
    err=editstring (getmaxx()/4+XDIGIT,getmaxy()/4+YBUTTONSIZE*2,MAX_LEN,fname_spt);
		killframe ();
    if (err)
	{
	for(j=0;j<num;j++)	farfree((files[j]).record);
	farfree(files);
	continue;
	}
    file_spt=fopen(fname_spt,"rt");     /* проверяем наличие файла */
    i=1;
		if(file_spt!=NULL)
#ifdef ENGLISH
	i=message (" File is present: ",getmaxx()/4,getmaxy()/4,(char **)msg,button);
#else
	i=message (" Такой файл уже существует: ",getmaxx()/4,getmaxy()/4,(char **)msg,button);
#endif
		fclose(file_spt);
		if(i==-1)
	{                       /* Отменить перезапись файла */
	for (i=0;i<num;i++)
	farfree ((files[i]).record);
	farfree (files);
	continue;
	}
		if(i==1)
	{                       /* Перезаписать файл */
	file_spt=fopen(fname_spt,"wt");
	if(file_spt==NULL)
			{
			 message (" File open ERROR: ",getmaxx()/4,getmaxy()/4,(char **)nulmsg,errbutton);
			 for(j=0;j<num;j++)	farfree((files[j]).record);
			 farfree(files);
	     continue;
			}
	}
		if(i==0)
	{                       /* Дописать в файл */
	file_spt=fopen(fname_spt,"at");
	if(file_spt==NULL)
	    {
			 message (" File open ERROR: ",getmaxx()/4,getmaxy()/4,(char **)nulmsg,errbutton);
	     for(j=0;j<num;j++)	farfree((files[j]).record);
	     farfree(files);
	     continue;
	    }
	}

	a= (float far *) farcalloc ((unsigned long)num,  /* выделяем место */
		  (unsigned long)sizeof(float));         /* под массивы */
	b= (float far *) farcalloc ((unsigned long)num,  /* коэффициентов */
			(unsigned long)sizeof(float));
	if(a==NULL||b==NULL)    abort();
				/* вычисляем коэффициенты пересчета во float */
	for(i=0;i<num;i++)        calckoeff (files[i].record,a+i,b+i);
	strcpy (xunit,xunitstr(files[0].record));
	strcpy (yunit,yunitstr(files[0].record));
	if (!strcmp(yunit," mm/s "))  KOEFY = (float) 1000;
	if (!strcmp(yunit," m/ss "))  KOEFY = (float) 1.0;
	if (!strcmp(yunit," mkm "))   KOEFY = (float) 1000000.0;
	maxx = isspectrum(files[0].record) ? (float)files[0].record->hfreq:
						(float)number/(files[0].record->hfreq*2.56);

					 /* Записываем в файл общ.параметры */
	strcpy (tmp,modestr(files[0].record));
	fprintf (file_spt,"\n Mode_____________ %s\n",tmp);
	fprintf (file_spt," High Frequency___ %d Hz\n",hfreq);
	if ((modenum(files[0].record)==2)||(modenum(files[0].record)==3))
			fprintf (file_spt," Average Frequency %d Hz\n",afreqnum(files[0].record));
	fprintf (file_spt," FFT Dimension____ %d\n",files[0].record->dimfft);
	fprintf (file_spt," Counts Number____ %d\n",number);
	fprintf (file_spt," Function Type____ %s%s\n",isspectrum(files[0].record) ? "Spectrum " : "Signal ",yunit);

						/* Записыв.парам.каждой записи */
	fprintf (file_spt,"\nAverage     ");   /* длина 12 позиций */
	for(i=0;i<num;i++)
			fprintf (file_spt,"%11d ",files[i].record->aver);
	fprintf (file_spt,"\nWindow      ");
	for(i=0;i<num;i++)
			fprintf (file_spt,"%11s ",windnum(files[i].record) ? "Hanning" : "Rectang");
	fprintf (file_spt,"\nDate        ");
	for(i=0;i<num;i++)
			{
			ftimetoa (&(files[i].ftime),tmp);
			fprintf (file_spt,"%11.9s ",tmp);
			}
	fprintf (file_spt,"\nTime        ");
	for(i=0;i<num;i++)
			{
			ftimetoa (&(files[i].ftime),tmp);
			fprintf (file_spt,"%11.8s ",tmp+9);
			}
	fprintf (file_spt,"\n\n%s\n",xunit);
						 /* Записываем значения сигнала */
	for(j=0;j<number;j++)
		{
		fprintf (file_spt,"\n%11.5f ",(float)j*maxx/number);
		for (i=0;i<num;i++)
			{
			if (islinear(files[0].record))
				fprintf (file_spt,"%11.5f ",a[i]*((byte)((files[i]).record->y)[j]+b[i])*KOEFY);
			else
				fprintf (file_spt,"%11.5f ",pow(10.0,a[i]*((byte)((files[i]).record->y)[j]+b[i])/20)*KOEFY);
			}
		}
	if(ferror(file_spt)!=0) 	/* проверяем на ошибку при записи */
			message (" File write ERROR: ",getmaxx()/4,getmaxy()/4,(char **)nulmsg,errbutton);

	fclose(file_spt);
/******************************конец вставки********************/
	farfree (a);
	farfree (b);
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
