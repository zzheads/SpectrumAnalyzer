/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_OTHER     
* Describe  : 
* File      : C:\ALEX\WORK\AS_OTHER.C        
* Last edit : 14.08.93 01.08.49
* Remarks   : 
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "as_mem.h"
#include "as_ftime.h"
#include "as_file.h"
#include "as_wind.h"
#include "as_list.h"
#include "as_choos.h"
#include "as_graph.h"
#include "as_msg.h"
#include "m_help.h"
#include "as_other.h"

static void loading (char *fname)
{
 gprintfxy (getmaxx()/3+XBUTTONSIZE/2,getmaxy()/4+YBUTTONSIZE*2,"\" %12s \"",fname);
}

void loadname (char *fname)
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/
{
 asRecPtr  rec;
 asFile    file;

 rec = (asRecPtr) farcalloc ((unsigned long)3000,(unsigned long)sizeof(byte));
 if (rec==NULL)
 {
  printf ("\n No enough memory in LOADBYTIME!");
  abort ();
 }
 file.record = rec;
 file.ftime = chooseftime (5,5,fname);
 if (incorrect(&(file.ftime)))
  return;

 setframe (getmaxx()/3,getmaxy()/4,getmaxx()/3+XBUTTONSIZE*2,
           getmaxy()/4+YBUTTONSIZE*4," Loading file ");
 if (load(&file,fname,loading)!=E_OK)
 {
  printf ("\nProblem in function LOAD.");
  abort ();
 }
 killframe();

 statusline((char *)slGraph);

 graphwindow (YBUTTONSIZE,getmaxy()-YBUTTONSIZE,&file);

 farfree ((asRecPtr)rec);
 statusline((char *)slMain);
}