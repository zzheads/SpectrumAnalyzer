/*****************************************************************************
*           :
*           :     Copyright (C) 1992 by Alexey V. Papin.
*           :
* Name      : AS_FILE
* Describe  : заголовок файла as_file.c
* File      : C:\ALEX\WORK\AS_FILE.H
* Last edit : 27.12.92 00.30.12pm
* Remarks   :
*****************************************************************************/

typedef struct         /* В asFile.FTIME.time.ti_hung на место неиспользу- */
{                      /* емого байта сотых долей секунды записывается    */
 long      offset;     /* идентификатор формата записи. Для ПР-90 он = 1  */
 FTIME     ftime;      /* но в программе расшифровки он не анализируется  */
 asRecPtr  record;     /* Это надо будет делать, если в файлы .spe будут  */
}                      /* записываться записи от других приборов. */
asFile;                /*                             */

/*  Структура файла данных:                                                  */
/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */
/*  | ofs : tm : rec || ofs : tm : rec || ofs : tm : rec || ofs : tm : rec | */
/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */
/*       |             ^   |             ^   |             ^   |             */
/*       |             |   |             |   |             |   |             */
/*       +-------------+   +-------------+   +-------------+   +-------> ... */
/*  long   ofs   - смещение до след. записи 4 байта                          */
/*  FTIME  tm    - время создания записи 8 байт(определена в AS_FTIME.H) как:*/

/*                 typedef struct			struct date {		стандартная структура	 */
/*									 {               		int da_year;      current year 				*/
/*									 struct	date;      	char da_day;      day of the month 		*/
/*									 struct	time;       char da_mon;      month (1 = Jan) 		*/
/*									 } FTIME            }; 																		*/
/*                                                    стандартная структура   */
/*																		struct time {                           */
/*																		unsigned char  ti_min;      minutes 		*/
/*																	  unsigned char  ti_hour;     hours 			*/
/*  							маркер формата ->  	unsigned char  ti_hund;     hundredths of seconds */
/*  							записи см.выше			unsigned char  ti_sec;      seconds 		*/
/*																		 };                                     */
/*																																						*/
/*  asRecord rec - запись (переменной длины)                                 */
/*                 длина записи = 49 + число отсчетов (байт)                 */
/*                                 |                                         */
/*                                 +--- размер постоянного заголовка записи  */

int load (asFile *file,char *fname,void (*func)(char *));
/***********
* Describe : Чтение записи из файла по времени в file (или с равным временем,
*          : или первую с большим, или последнюю запись в файле)
* Params   : asFile *file - куда читать
*          : char *fname  - имя файла для чтения
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
* Call     :
***********/

int save (asFile *file,void (*func)(char *));
/***********
* Describe : Запись записи в файл, упорядочивает записи в файле по времени
* Params   : asFile *file - что записывать
*          : void (*func) - указ. на функцию сообщения
* Return   : int          - код ошибки
* Call     :
***********/

int delete (char *fname,FTIME t,void (*func)(char *));

int copy (asFile *file_from,asFile *file_to,char *from,char *to);

long setfile (FILE *f,FTIME *t);
/***********
* Describe : Установить указатель в файле на начало первой записи
*          : с большей датой, чем данная
* Params   : FILE *f  - файл
*          : FTIME *t - дата
* Return   : long     - смещение от начала файла до нужного места
* Call     :
***********/

