/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_CAPTU     
* Describe  : Запись граф. копии экрана в файл
* File      : C:\ALEX\WORK\AS_CAPTU.C        
* Last edit : 14.08.93 00.59.52
* Remarks   : 
*****************************************************************************/

#include <stdio.h>
#include <alloc.h>
#include <graphics.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_captu.h"

void abort (void);

void capture_screen (void)
/***********
* Describe : записать экран на диск
* Params   : void
* Return   : void
* Call     : 
***********/
{
 extern int p_x1,p_y1,p_x2,p_y2;
 char far *buffer=NULL,fname[30]="capture.img";
 unsigned int volume;
 FILE *fout;

 volume = imagesize (p_x1,p_y1,p_x2,p_y2);
 buffer = (char far *) farcalloc (volume,sizeof(char));
 if (buffer==NULL)
 {
  printf (" No enough memory in function CAPTURE_SCREEN.");
  abort ();
 }
 printf ("\07");
 getimage (p_x1,p_y1,p_x2,p_y2,buffer);
 printf ("\07");

 setframe (getmaxx()/4,getmaxy()/4,getmaxx()/4+XBUTTONSIZE*3,
           getmaxy()/4+YBUTTONSIZE*4," Enter file name: ");
 editstring (getmaxx()/4+XDIGIT,getmaxy()/4+YBUTTONSIZE*2,30,fname);
 killframe ();

 fout = fopen (fname,"wb");
 if (fout==NULL)
 {
  printf (" Problem in function CAPTURE_SCREEN. Disk full?");
  abort ();
 }
 fwrite ((char *)buffer,volume,sizeof(char),fout);
 fclose (fout);

 farfree (buffer);
 printf ("\07");
 return;
}
