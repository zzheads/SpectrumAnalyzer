/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : _FNDIR
* Describe  : Заголовок _fndir
* File      : C:\ALEX\ANLZR\_FNDIR.H
* Last edit : 30.05.93 00.58.33
* Remarks   :
*****************************************************************************/

#define MAX_WILDCARD_LEN 31      /* максимальная длина шаблона */
#define DYN_ARRAY_SIZE   50      /* начальный размер массива структур ffblk */
#define INC_ARRAY_SIZE   25      /* приращение массива */

struct ffblk* fn_edir (char wildcard[][MAX_WILDCARD_LEN],int num_wildcard,
                       struct ffblk *files,int *num_files);
/***********
* Describe : Функция поиска файлов по заданным шаблонам.
* Params   : char wildcard[][]   - массив шаблонов длиной не более MAX_WILDCARD_LEN
*          : int num_wildcard    - число шаблонов
*          : struct ffblk *files - указатель на массив структур ffblk (см. dos.h)
*          : int *num_files      - число найденных по шаблону файлов
* Return   : struct ffblk*       - указатель на массив структур ffblk
* Call     :
***********/

asList far *getdir (char *wc,int *numfiles);
/***********
* Describe : выдать список файлов в диpектоpии
* Params   : char *wc      - шаблон для поиска
*          : int *numfiles - число найденных файлов
* Return   : asList far *  - массив стpуктуp
* Call     : fn_edir
***********/