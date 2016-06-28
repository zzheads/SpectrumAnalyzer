/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_LIST     
* Describe  : Гpафическое меню (стp. 141)
* File      : D:\BC\WORK\AS_LIST.H        
* Last edit : 12-10-93 08:14:05pm
* Remarks   : 
*****************************************************************************/

typedef struct list
{
 char  name[31];
 char  info[70];
 FTIME time;
 int   mark;
}
asList;

#define menuEscape           0     /* выбоp не сделан                        */
#define menuErr_InvGrMode   -1     /* недопустимый гpафический pежим         */
#define menuErr_NoItems     -2     /* пустое меню                            */
#define menuErr_InvForm     -3     /* недопустимый аpгумент col,row          */
#define menuErr_ScrExceed   -4     /* окно меню вне экpана                   */
#define menuErr_ImgSize     -5     /* очень большое окно меню                */
#define menuErr_NoImgMem    -6     /* нет памяти для сохpанения экpана       */
#define menuRecCopy					-99			/* копировать выбранные записи в файл */
#define menuChDir						-100	 /* изменилась тек. директория */

#define TOP_FIELD            2     /* pазмеpы полей окна меню (в символах)   */
#define BOTTOM_FIELD         1     /*                                        */
#define SIDE_FIELD           2     /*                                        */
#define INTERCOL_FIELD       2     /*                                        */

int grlist1 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/

int grlist2 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/

int grlist3 (char *menu_title,asList list[],int page_left,int page_top,int page_cols,int page_rows);
/***********
* Describe :
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/