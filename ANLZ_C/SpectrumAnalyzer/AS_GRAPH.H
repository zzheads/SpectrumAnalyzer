/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_GRAPH     
* Describe  : 
* File      : D:\BC\WORK\AS_GRAPH.H        
* Last edit : 12-10-93 08:09:41pm
* Remarks   : 
*****************************************************************************/

enum asScale  {LINEAR=0,LOGARIPHMIC=1};
enum asCursor {HARMONIC=1,SIDEBAND=2,DELTA=3};

#define SCALE(x)  ( yscale ? ( x>step ? 20*log10(x) : 20*log10(step) ) : x )
#define YINSIDE(y) ( y>ylow ? ylow : ( y<yhigh ? yhigh : y ) )
#define XINSIDE(x) ( x>ylow ? xlow : ( x<xhigh ? xhigh : x ) )

#define MIN_DELTA  (1)
#define MAX_DELTA  (50)

int graphwindow (int y1,int y2,asFile far *file);
/***********
* Describe : показ графика функции из файла с указанным временем
* Params   : int y1           - координаты верха
*          : int y2           - и низа окна
*          : asFile far *file - файл с установленным временем
* Return   : int
* Call     :
***********/

void infowindow (int x1,int y1,asRecPtr rec);
/***********
* Describe : информация о записи
* Params   : int x1       - координаты
*          : int y1       - окна
*          : asRecPtr rec - запись
* Return   : void
* Call     :
***********/

void editfloat (double *,char *);
/***********
* Describe : редактирование числа
* Params   : double * - указатель на число
*          : char *   - ед. измерения
* Return   : void
* Call     :
***********/

void editfloat2 (double *,double *);
/***********
* Describe : редактирование двух чисел
* Params   : double * - числа
*          : double * -
* Return   : void
* Call     :
***********/

void closeall (void);