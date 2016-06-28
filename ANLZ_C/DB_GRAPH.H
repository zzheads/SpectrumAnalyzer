/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : DB_GRAPH
* Describe  :
* File      : C:\ALEX\WORK\DB_GRAPH.H
* Last edit : 08.04.93 00.46.48
* Remarks   :
*****************************************************************************/

/*#define max(A,B)       ((A)>(B)?(A):(B))
#define min(A,B)       ((A)<(B)?(A):(B))*/
#define SIDE           textwidth("############")
#define TOP            textheight("#")*4
#define BOTTOM         textheight("#")*4
#define YSPCE          3
#define XSPCE          5
#define XDIGIT         textwidth ("8888")
#define YDIGIT         textheight ("8888")

enum GRAPHICS {GRAPH,XSPE_LINE,YSPE_LINE,ZSPE_LINE};


void draw_parameters (int y1,int y2,char *db,char *field_name);
/***********
* Describe : строит график,
* Params   : int y1
*          : int y2
*          : char *db
*          : char *field_name
* Return   : void
* Call     :
***********/

void loadspe (int left,char *fname,int dir_num,asList far *dir_spes,struct date *now);
/***********
* Describe : загружает ближайший по дате к курсору спектр (x,y,z)
* Params   : int left              - 1-слева,0-справа
*          : char *fname           - имя файла спектра
*          : int dir_num           - число
*          : asList far *dir_spes  - список спектров данного направления
*          : struct date *now      - время на которое указ. курсор
* Return   : void
* Call     : graphwindow,loading,diffdate,load,statusline,setframe,killframe
***********/

char *spefile (char *dbfile,char *field,char dir);
/***********
* Describe : имя .spe-файла по имени файла БД
* Params   : char *dbfile - название БД
*          : char *field  - название поля
*          : char dir     - направление x,y,z
* Return   : char
* Call     :
***********/