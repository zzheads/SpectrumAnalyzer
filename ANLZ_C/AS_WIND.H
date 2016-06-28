/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_WIND
* Describe  : Заголовок функций графического интерфейса
* File      : C:\ALEX\WORK\GRAPH\AS_WIND.H
* Last edit : 03.01.93 10.18.35
* Remarks   :
*****************************************************************************/

typedef void (*PF_V_V)(void);        /*  указатель на функцию void (void)    */
typedef struct choice                /*                                      */
        {                            /*                                      */
         char            *text;      /*  текст для вывода                    */
         PF_V_V          action;     /*  действие при выборе                 */
         int             able;       /*  может ли быть выбрано               */
        }                            /*                                      */
				asChoice;                    /*                                      */
                                     /*                                      */
typedef struct hotkey                /*                                      */
        {                            /*                                      */
         char      key;              /*                                      */
         PF_V_V    action;           /*                                      */
         int       able;             /*                                      */
        }                            /*                                      */
        asHotKey;                    /*                                      */

#define MAX_LEN       30
#define C_EDIT_INK    WHITE
#define C_EDIT_PAPER  BLACK
#define XLETSZ        textwidth("_")

#undef max
#undef min

#define max(A,B)  ((A)>(B)?(A):(B))  /*                                      */
#define min(A,B)  ((A)<(B)?(A):(B))  /*                                      */
                                     /*                                      */
#define BUFLEN       200             /*  размер буфера формат. вывода текста */
#define XSPACE       2               /*  зазор между объектом и рамкой по X  */
#define YSPACE       1               /*  зазор между объектом и рамкой по Y  */
#define YBUTTONSIZE (textheight("X")+4*YSPACE+1)  /*  размеры кнопок по осям X */
#define XBUTTONSIZE (textwidth("    Cancel    ")+4*XSPACE)  /*  и Y          */
#define OK           1               /*                                      */
#define CANCEL       0               /*                                      */
                                     /*                                      */
#define C_F_PAPER    WHITE           /*  цвет фона в setframe                */
#define C_F_INK      BLACK           /*  цвет краски в setframe              */
#define C_B_PAPER    BLACK           /*  цвет фона в setbutton               */
#define C_B_INK      WHITE           /*  цвет краски в setbutton             */

#define XDIGIT       textwidth ("8888")
#define YDIGIT       textheight ("8888")


void gprintfxy (int x,int y,char *fmt,...);
/***********
* Describe : Форматированный вывод в графике
* Params   : int x     - откуда
*          : int y     - выводить
*          : char *fmt - строка формата (см. printf)
*          : ...       - параметры для вывода
* Return   : void
* Call     :
***********/

void textfon (int x,int y,char *fmt,...);
/***********
* Describe : "Включение" форматированного текста
* Params   : int x     - откуда
*          : int y     - выводить
*          : char *fmt - строка формата (см. printf)
*          : ...       - параметры для вывода
* Return   : void
* Call     :
***********/

void textfoff (void);
/***********
* Describe : "Выключение" текста, выведенного последней функ. textfon
* Return   : void
* Call     :
***********/

void setframe (int x1,int y1,int x2,int y2,char *title);
/***********
* Describe : Выводит фрейм (рамка,заголовок)
* Params   : int x1      -  координаты левого
*          : int y1      -  верхнего угла
*          : int x2      -  координаты правого
*          : int y2      -  нижнего угла
*          : char *title -  заголовок
* Return   : void
* Call     :
***********/

void killframe (void);
/***********
* Describe : Уничтожант последний выведенный фрейм
* Params   : void
* Return   : void
* Call     :
***********/

void setbutton (int x1,int y2,char *buttontext,int active,int inverse);
/***********
* Describe : Выводит кнопку
* Params   : int x1           -  координаты левого
*          : int y2           -  верхнего угла
*          : char *buttontext -  текст кнопки
*          : int active       -  флаг активности
* Return   : void
* Call     :
***********/

void mainmenu (int y,asChoice *ch);
/***********
* Describe : главное горизонтальное меню программы
* Params   : int y        - высота откуда выводить
*          : asChoice *ch - массив структур asChoice
* Return   : void
* Call     : setbutton
***********/

int submenu (char *title,int x1,int y1,asChoice *ch);
/***********
* Describe : вертикальное подменю
* Params   : char *title  - заголовок
*          : int x1       - левый
*          : int y1       - верхний угол
*          : asChoice *ch - массив альтернатив
* Return   : void
* Call     : setframe,killframe,setbutton
***********/

void reportmemory (int x1,int y1);
/***********
* Describe : сообщить о свободной памяти
* Params   : int x1 - откуда
*          : int y1 - выводить
* Return   : void
* Call     :
***********/

void reporttime (int x1,int y1);
/***********
* Describe : сообщить о дате и времени
* Params   : int x1 - откуда
*          : int y1 - выводить
* Return   : void
* Call     :
***********/

void drawprogress (double pc);
/***********
* Describe : Рисует процент выполнения операции
* Params   : float pc - процент (всегда меньше 1.0)
* Return   : void
* Call     : setframe, killframe
***********/

void statusline (char *text);
/***********
* Describe : строка статуса внизу экрана и сообщение о свободной памяти
* Params   : char *text   - сообщение в строке статуса
* Return   : void
* Call     : reportmemory
***********/

int message (char *title,int x1,int y1,char *text[],asChoice ch[2]);
/***********
* Describe : сообщение и 2 кнопки
* Params   : char *title    - заголовок
*          : int x1         - откуда
*          : int y1         - выводить
*          : char *text[]   - текст
*          : asChoice ch[2] - кнопки
* Return   : int
* Call     : setframe, killframe
***********/

int editstring (int x1,int y1,int len,char *str);
/***********
* Describe : ввод стpоки
* Params   : int x1
*          : int y1
*          : int len
*          : char *str
* Return   : int
* Call     : 
***********/

int editint (int x1,int y1,int digit);
/***********
* Describe : ввод целого числа
* Params   : int x1
*          : int y1
*          : int digit
* Return   : int
* Call     : 
***********/

int editftime (int x1,int y1,FTIME *t);
/***********
* Describe : pедактиpовать вpемя
* Params   : int x1
*          : int y1
*          : FTIME *t
* Return   : int
* Call     : 
***********/

void reportdir (int x2,int y1);
/***********
* Describe : сообщить о текущей директории
* Params   : int x2 - откуда
*          : int y1 - выводить
* Return   : void
* Call     : gprintfxy
***********/