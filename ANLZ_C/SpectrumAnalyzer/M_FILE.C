/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_FILE
* Describe  : ���� FILE �p��p���� �����p���������p ��90
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

int REC_TYPE;		// ��� �ਡ��, ������祭���� ��� ������
								// 0 - ��90, 1 - ��90.59�
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

asChoice mnFile[]     = {{" �ਥ� ������ ",receive    ,1},
												 {" ��४���   ",changedir  ,1},
												 {" ���. �६��� ",changetime ,1},
												 {" �६���� ��� ",osshell    ,1},
												 {" ��室        ",beforequit ,1},
												 {NULL             ,NULL       ,0}};

#endif

void file (void)
/***********
* Describe : �㭪� �������� ���� FILE
* Params   : void
* Return   : void
* Call     : �������
***********/
{
 int was_c;
 was_c = help_context;
 help_context = CONTEXT_FILE;

#ifdef ENGLISH
 submenu ("  File  ",0,0,mnFile);
#else
 submenu ("  ����  ",0,0,mnFile);
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
* Describe : �p��� ���p��樨 �� �p���p� � ������ �� � 䠩�� (ᯥ���p��
*          : ��p���p��-��p����/��/����� � �p��� �����)
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

 strcpy (msgReceive[2]+20,mnRec_type[REC_TYPE].text); 	// ����஢���� ⨯� �ਡ�� � ���ᨢ ����

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
		 getmaxy()/4+YBUTTONSIZE*5," ����� �ਭ���. ");
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,
		" ��ꥬ ������:       %6.3f ����� ",(float)(hdr->volume)/512);
 gprintfxy (getmaxx()/4+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*3,
		" ��᫮ ����ᥩ:      %u �� ",(uint)(hdr->numrec)-1);
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

#ifdef ENGLISH																			/* �뢮��� ᯨ᮪ ����ᥩ */
	user = grlist2(" Mark records to save: ",list,XSPACE,YDIGIT,4,10);
#else
	user = grlist2(" �⬥��� ����� ��� �����������: ",list,XSPACE,YDIGIT,4,10);
#endif

	if (user==menuEscape)
	{
	 i=message(" Verify: ",getmaxx()/3,getmaxy()/4,msgEscape,mnStd);
	 if (i==1)
	remains=0;
	 continue;
	}

	if (user==menuChDir)	 continue;

			/* ����������� ��। ������� � �६��� � ��४�ਨ */
	changetime ();
	getcwd(&path[1][1],MAX_LEN);		/* ����砥� ��� ⥪�饩 ��४�ਨ */
#ifdef ENGLISH
	j=message(" Write to Directory: ",getmaxx()/4,getmaxy()/4,path,mnStd);
#else
	j=message(" ������ � ��४���: ",getmaxx()/4,getmaxy()/4,path,mnStd);
#endif
	if(j != 1)	 continue;		// �᫨ �㦭� �����頥��� � �롮�� ����ᥩ

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
	file.ftime.time.ti_hund = (unsigned char)REC_TYPE;     /* ����� ���ᨨ �����뢠���� �� ����*/
																												 /* ���� ����� ᥪ㭤� 1 ���� */
	if(REC_TYPE==1)		// ��ॢ�� �ଠ� ��90.59� � �ଠ� ��90
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

/*void changedir (void)     ���� ����� �����ࠪ⨢��� */
/***********
* Describe : ᬥ�� ⥪�饩 ��p���p��
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


void changedir (void)     /* ����� ����� ���ࠪ⨢��� */
/***********
* Describe : ᬥ�� ⥪�饩 ��p���p��
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
	 done = findfirst("*",&ffblk,FA_DIREC);   /* ������뢠�� � numdirs �᫮ */
	 while(!done)                               /* ��४�਩ */
	 {numdirs++;
	 done = findnext(&ffblk);
	 }
//	 numdirs--;				// 㬥��蠥� �᫮ ��४�਩ �� 1, �.�. ᢮� ��� "."
//	 numdisks=setdisk(-1);			/* � numdisks �᫮ ����㯭�� ��᪮� */
																// ��稭��� ��ᬮ�� ��᪮� � ��᪠ �
	 for (numdisks=3; !(getcurdir(numdisks,tmp)); numdisks++);
	 numdisks--;								 /* �뤥�塞 ������ ��� ᯨ᮪ ��४�਩ */
	 listdirs = (asList far *) farcalloc((unsigned long)numdirs+numdisks+2,(unsigned long)sizeof(asList));
	 if (listdirs==NULL)
	 {
	 closegraph ();
	 printf ("\n Error reading directory: no enough memory. ");
	 exit (-1);
	 }

	 findfirst("*",&ffblk,FA_DIREC);   /* ������塞 ᯨ᮪ ��४��ﬨ */
	 for(i=0;i<numdirs;i++)
		{
		if(strcmp(ffblk.ff_name,".") != 0)		// �ய�᪠�� ��� "."
			{for(j=0;j<i;j++)		// 㯮�冷稢��� ����� �� �����⠭��
				{if (strcmp(ffblk.ff_name,listdirs[j].name) < 0)
					{
					for(k=i-1;k>=j;k--)		strcpy(listdirs[k+1].name,listdirs[k].name);
					break;
					}
				}
			strcpy(listdirs[j].name,ffblk.ff_name);
			}
		else            // �ய�᪠�� ��� "."
			{numdirs--;
			i--;
			}
		findnext(&ffblk);
		}

										 /* � ������� ����㯭�� ��᪮� */
	 for(i=0;i<numdisks;i++)
	 {listdirs[numdirs+i].name[0] = (char)(i+0x41);
	 listdirs[numdirs+i].name[1] = ':';
	 listdirs[numdirs+i].name[2] = '\0';
	 }

	 strcpy(listdirs[numdirs+numdisks+1].name,"LAST");
#ifdef ENGLISH
	 strcpy(listdirs[numdirs+numdisks].name,"Create Dir");
					/* �뢮��� ����  */
	 user=grlist3(" Change directory: ",listdirs,20,YBUTTONSIZE,4,10);
#else
	 strcpy(listdirs[numdirs+numdisks].name,"������� ���.");
					/* �뢮��� ����  */
	 user=grlist3(" �롮� ��४�ਨ: ",listdirs,20,YBUTTONSIZE,4,10);
#endif
	 if (user!=menuEscape)
	 {
	 if(user<=numdirs)
	   {
	   chdir(listdirs[user-1].name);  /* �����塞 ��४��� */
	   if(getcwd(path,MAX_LEN)==NULL)  /* �᫨ ��� ⥪�饣� ��⠫��� ������� MAX_LEN */
		 {chdir("..");  					/* �����頥��� */
		 message(" Error: ",getmaxx()/4,getmaxy()/4,longpathmes,mnStd);
		 }
	   }
	 else
	   {
	   if(user==(numdirs+numdisks+1))		/* ᮧ���� ��४��� */
		 {
		 getcwd(path,MAX_LEN);  /* ����砥� ��� ⥪�饣� ��⠫��� (� ��᪮�) */
									/* � ।����㥬 � ���� */
#ifdef ENGLISH
		 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
					getmaxy()/4+YBUTTONSIZE*4," Enter full path for Create Directory: ");
#else
		 setframe (20,YBUTTONSIZE*2,30+XBUTTONSIZE*3,
					YBUTTONSIZE*6," ������ ������ ��� ᮧ�������� ��४�ਨ: ");
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
		 else	setdisk(user-numdirs-1); /* ���室�� �� ��㣮� ��� */
	   }
	 }
	 farfree (listdirs);
	 getviewsettings (&prev_viewport);		// ���������� ��ࠬ���� �।.����
	 setviewport(0,0,getmaxx(),getmaxy(),1);	// ���. ��砫� �� ����.��࠭�
	 reportdir (getmaxx(),YBUTTONSIZE);	// �뢮��� ��� ��४�ਨ
						// � ����⠭�������� ��ࠬ���� �।. ����
	 setviewport(prev_viewport.left,prev_viewport.top,
											prev_viewport.right,prev_viewport.bottom,prev_viewport.clip);
   }
 help_context = was_c;
}


void changetime (void)
/***********
* Describe : ᬥ�� ⥪�饣� ����p������ �p����� (���p�� �ᯮ������
*          : �p� ����� � 䠩�� �p���⮩ ���p��樨)
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
* Describe : �p������ ��室 � ���
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
 printf ("\n��� ������ � �ணࠬ�� ������ ������� EXIT ... ");
#endif
 strcpy (cmdln,searchpath("command.com"));
 system (cmdln);
 help_context = was_c;
 main (1,(char **)NULL);
}
