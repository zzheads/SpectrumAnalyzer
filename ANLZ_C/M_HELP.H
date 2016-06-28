/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : M_HELP     
* Describe  : Контекстная помощь для пpогpаммы СпектpоАнализатоp
* File      : C:\ALEX\WORK\M_HELP.H        
* Last edit : 02.07.93 02.31.23
* Remarks   :
*****************************************************************************/

#define CONTEXT_ABOUT          5					// числа должны соответствовать
#define CONTEXT_FILE           6          // номерам страниц в файле помощи
 #define CONTEXT_CONTROL       13         // нумерация страниц с 0
 #define CONTEXT_RECEIVE       7
// #define CONTEXT_CHDRIVE
 #define CONTEXT_CHDIR         8
 #define CONTEXT_CHTIME        9
 #define CONTEXT_OSSHELL       10
 #define CONTEXT_QUIT          11
#define CONTEXT_ANALYZE        12
 #define CONTEXT_TRENDS        15
 #define CONTEXT_RECORD        14
// #define CONTEXT_CASCADE       15
#define CONTEXT_OPTIONS        16
 #define CONTEXT_CURSORTYPE    17
 #define CONTEXT_WINDOWTYPE    18
 #define CONTEXT_3DPARAMS      19
 #define CONTEXT_DATAPORT      21
 #define CONTEXT_TABLES				 22
 #define CONTEXT_GRAPHMODE     23
 #define CONTEXT_GRAF          24
 #define CONTEXT_PRINT         27
 #define CONTEXT_QUALITY       28
 #define CONTEXT_HEIGHT        29
 #define CONTEXT_PORT          31
 #define CONTEXT_EXPORT        32
 #define CONTEXT_HELP          33
 #define CONTEXT_CONTEXT       34
 #define CONTEXT_DATAREC_TYPE  20
// #define CONTEXT_TOPIC


void help (void);      // Тематическая помощь. Показывает меню, состоящее
/***********						  из первых 30 символов первой строки каждой страницы
* Describe :							текстового файла anlzr.hlp (т.е. названия темы).
* Return   : void         После выбора темы показывается вся страница в окне
* Call     :              с заголовком из полной первой строки листа (то же,
***********/            //что функция контекстной помощи см.ниже.

void contexthelp (void);
/***********                                         текстовый файл anlzr.hlp
* Describe : контекстная помощь                      разбивается на страницы
* Params   : void использует пеpеменную help_context обычным кодом разделителем
* Return   : void                                    страниц 12. Кажд. страница-
* Call     : message, next, previous                 отдельн. тема. Заголовок -первая
***********/                                      // строка страницы. Размер страницы
																									// неограничен, обеспечен скроллинг.
