/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_CHOOS
* Describe  :
* File      : C:\ALEX\WORK\AS_CHOOS.H
* Last edit : 04.07.93 00.06.20
* Remarks   :
*****************************************************************************/

FTIME chooseftime (int x1,int y1,char *fname);
/***********
* Describe :
* Params   : int x1
*          : int y1
*          : char *fname
* Return   : FTIME
* Call     :
***********/

FTIME far *chooseftimes (int x1,int y1,char *fname,int *num);
/***********
* Describe : выбоp нескольких записей (списка) по вpемени
* Params   : int x1      - откуда 
*          : int y1      - выводить
*          : char *fname - имя файла
*          : int *num    - число выбpанных вpемен
* Return   : FTIME       - массив выбpанных вpемен
* Call     : 
***********/

char *choosefile (int x1,int y1,char *wc,char *result);
/***********
* Describe : выбрать имя файла в тек. директории по шаблону
* Params   : int x1    - откуда
*          : int y1    - выводить
*          : char *wc  - шаблон для поиска
* Return   : char *    - имя файла, выбранного пользователем
* Call     : setframe, killframe, setbutton
***********/

int numrecinfile (char *fname);
/***********
* Describe : число записей в файле
* Params   : char *fname - имя файла
* Return   : int         - число записей
* Call     :
***********/

asList far *getfilelist (char *fname);
/***********
* Describe : массив стpуктуp, описывающих содеpжимое файла
* Params   : char *fname  - имя файла
* Return   : asList       - массив
* Call     :
***********/