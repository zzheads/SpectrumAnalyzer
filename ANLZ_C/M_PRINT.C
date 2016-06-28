/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed
*           :       without special permission of author. 
*           : 
* Name      : M_PRINT
* Describe  :
* File      : C:\ALEX\WORK\M_PRINT.C
* Last edit : 30.06.93 01.05.35
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <stdio.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_msg.h"
#include "m_help.h"
#include "m_print.h"

extern int help_context;
int port,mode,margin;

#ifdef ENGLISH
asChoice mnPrint[]    = {{"  Length     ",quality ,1},
                         {"  Height     ",height  ,1},
                         {"  Margin     ",setmargin  ,1},
                         {"  Port       ",setport ,1},
                         {NULL           ,NULL    ,0}};

asChoice mnQual[]    = {{"  8dot 270mm  ",NULL,1},
                        {"  8dot 225mm  ",NULL,1},
                        {"  8dot 205mm Q",NULL,1},
                        {"  8dot 180mm Q",NULL,1},
                        {"  8dot 135mm  ",NULL,1},
                        {"  8dot 135mm Q",NULL,1},
                        {"  8dot  70mm  ",NULL,1},
                        {" 24dot 270mm  ",NULL,1},
                        {" 24dot 180mm  ",NULL,1},
                        {" 24dot 135mm  ",NULL,1},
                        {" 24dot  90mm  ",NULL,1},
                        {" 24dot  45mm  ",NULL,1},
                        {NULL            ,NULL,0}};

asChoice mnHeig[]    = {{"  Single      ",NULL,1},
												{"  Double      ",NULL,1},
												{NULL            ,NULL,0}};
#else
asChoice mnPrint[]    = {{"  Длина      ",quality ,1},
												 {"  Высота     ",height  ,1},
												 {"  Отступ     ",setmargin  ,1},
												 {"  Порт       ",setport ,1},
												 {NULL           ,NULL    ,0}};

asChoice mnQual[]    = {{"  8игл 270mm  ",NULL,1},
												{"  8игл 225mm  ",NULL,1},
												{"  8игл 205mm Q",NULL,1},
												{"  8игл 180mm Q",NULL,1},
												{"  8игл 135mm  ",NULL,1},
												{"  8игл 135mm Q",NULL,1},
												{"  8игл  70mm  ",NULL,1},
												{" 24игл 270mm  ",NULL,1},
												{" 24игл 180mm  ",NULL,1},
												{" 24игл 135mm  ",NULL,1},
												{" 24игл  90mm  ",NULL,1},
												{" 24игл  45mm  ",NULL,1},
												{NULL            ,NULL,0}};

asChoice mnHeig[]    = {{"  Одинарная   ",NULL,1},
												{"  Двойная     ",NULL,1},
												{NULL            ,NULL,0}};
#endif

asChoice mnPort[]    = {{"  LPT1   ",NULL,1},
												{"  LPT2   ",NULL,1},
												{"  LPT3   ",NULL,1},
												{NULL       ,NULL,0}};

void print (void)
{
 int was_c;
 extern asHotKey hotkeys[];
 was_c = help_context;
 help_context = CONTEXT_PRINT;

 hotkeys[1].able=0;
 hotkeys[2].able=0;
 hotkeys[3].able=0;
 hotkeys[4].able=0;
 hotkeys[5].able=0;
 hotkeys[6].able=0;
 hotkeys[7].able=0;
 hotkeys[8].able=0;

#ifdef ENGLISH
 submenu (" Print ",(int)XBUTTONSIZE*3,0,mnPrint);
#else
 submenu (" Принтер ",(int)XBUTTONSIZE*3,0,mnPrint);
#endif

 hotkeys[1].able=1;
 hotkeys[2].able=1;
 hotkeys[3].able=1;
 hotkeys[4].able=1;
 hotkeys[5].able=1;
 hotkeys[6].able=1;
 hotkeys[7].able=1;
 hotkeys[8].able=1;

 help_context = was_c;
}

void quality (void)     /* установка длины печати, мл байт mode */
{
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_QUALITY;

#ifdef ENGLISH
 j=submenu (" Length  ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE,mnQual);
#else
 j=submenu (" Длина   ",(int)XBUTTONSIZE*3,YSPACE+YBUTTONSIZE,mnQual);
#endif
 mode=mode&0xf00;
 switch(j)
  {case   0:        mode+=0; break;
  case    1:        mode+=5; break;
  case    2:        mode+=4; break;
  case    3:        mode+=6; break;
  case    4:        mode+=1; break;
  case    5:        mode+=2; break;
  case    6:        mode+=3; break;
  case    7:        mode+=32; break;
  case    8:        mode+=38; break;
  case    9:        mode+=33; break;
  case    10:       mode+=39; break;
  case    11:       mode+=40; break;
  default:          mode+=1;
  }
 help_context = was_c;
}

void height (void)      /* установка высоты печати 0-single, 1-double */
{                       /* код в ст. байте mode                       */
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_HEIGHT;

#ifdef ENGLISH
 j=submenu (" Height  ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*2,mnHeig);
#else
 j=submenu (" Высота  ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*2,mnHeig);
#endif
 if(j>=0)	mode=(mode&0xff)+(j<<8);
 help_context = was_c;
}

void setport (void)
{
 int j,was_c;
 was_c = help_context;
 help_context = CONTEXT_PORT;

#ifdef ENGLISH
 j=submenu (" Output to: ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*3,mnPort);
#else
 j=submenu (" Вывод на: ",(int)XBUTTONSIZE*4,YSPACE+YBUTTONSIZE*3,mnPort);
#endif
 if (j>-1)
	port=j;
 help_context = was_c;
}

void setmargin (void)
{
 int j,was_c;
 double x;

 was_c = help_context;
 help_context = CONTEXT_PORT;
 x=margin;
#ifdef ENGLISH
 editfloat(&x,"space");
#else
 editfloat(&x,"знак");
#endif
 if (x>=0)        margin=x;
 help_context = was_c;
}