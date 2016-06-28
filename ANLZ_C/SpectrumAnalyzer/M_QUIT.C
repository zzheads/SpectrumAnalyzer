/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : M_QUIT     
* Describe  : 
* File      : C:\ALEX\WORK\M_QUIT.C        
* Last edit : 02.07.93 02.45.48
* Remarks   : 
*****************************************************************************/

#include <stdio.h>
#include <graphics.h>
#include <process.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "m_help.h"
#include "m_quit.h"

extern int help_context;

/*     asChoice mnQuit[] = {{"     Yes     ",quit ,1},
                     {"     No      ",NULL ,1},
                     {NULL           ,NULL ,0}};        */


void beforequit (void)
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/
{
 int i;

 for (i=0;i<30;i++)
  killframe ();
 closegraph ();
 exit (0);
}

/*void beforequit (void)
***********                           исключено
* Describe :
* Params   : void
* Return   : void
* Call     :
***********
{
 int tmp;
 tmp = help_context;
 help_context = CONTEXT_QUIT;
 submenu (" Are You sure? ",XBUTTONSIZE*5,0,mnQuit);
 help_context = tmp;
}                                                         */