            ANLZR

             *.h   	

_PRINT.H
===============================================================================

void print_screen (void);
/***********
* Describe : Вывод экрана в граф. режиме на принтер
* Params   : void
* Return   : void
* Call     :
***********/

void printarea (int x1,int y1,int x2,int y2);
/***********
* Describe : Вывод части экрана в граф. режиме на принтер
* Params   : int x1 - координаты прямоугольной
*          : int y1 - области для
*          : int x2 - вывода на
*          : int y2 - печать
* Return   : void
* Call     :
***********/

void printrow(int x,int y1,int y2);
/***********
* Describe : Печать строки номер x
* Params   : int x - номер строки
* Return   : void
* Call     :
***********/

int status(void);
/***********
* Describe : Чтение состояния принтера :
*          : Принтер подключен, не занят и есть бумага :
*          : 1x0x0xx0
*          : │││││  │
*          : │││││  └── нет ожидания (1 = if timed out)
*          : ││││└───── нет ошибки ввода-вывода (1 = if I/O error)
*          : │││└────── 1 = on-line ( on-line на принтере star при 0)
*          : ││└─────── иначе , если 1 - нет бумаги
*          : │└──────── не проверяем, хотя если 1 - подтверждение
*          : └───────── 1 = не занят
*          :
* Params   : void
* Return   : int (1) - все нормально, (0)-ошибка
* Call     :
***********/

char status_prn(void);
/***********
* Describe : Статус состояния принтера
* Params   : void
* Return   : char
* Call     :
***********/

void put_out(char character);
/***********
* Describe : Вывод символа через порт
* Params   : char character - символ
* Return   : void
* Call     :
***********/

AS_3D.H
===============================================================================

* Describe  : Трехмерная графика

void graph3D (int y1,int y2,asFile far *files,int num);
/***********
* Describe :
* Params   : int y1
*          : int y2
*          : asFile far *files
*          : int num
* Return   : void
* Call     :
***********/

AS_CAPTU.H
===============================================================================

* Describe  : Запись граф. копии экрана в файл

void capture_screen (void);
/***********
* Describe : записать экран на диск
* Params   : void
* Return   : void
* Call     :
***********/

AS_CHECK.H
===============================================================================

* Describe  : проверка на наличие в директории файлов

void check (asList far *list,int dx);
/***********
* Describe : отмечает имеющиеся в директории файлы
* Params   : asList far *list - список имен записей
*          : int dx           - сколько из списка проверять
* Return   : void
* Call     :
***********/

AS_CHOOS.H
===============================================================================

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
***********/

AS_DEF.H
===============================================================================

int test (void);

AS_DIAG.H
===============================================================================

* Describe  : диагностика

char *diagnoze (char *fname,char *field);
/***********
* Describe : выдача диагностического сообщения о параметре из БД
* Params   : char *fname - имя БД
*          : char *field - имя параметра
* Return   : char        - сообщение
* Call     :
***********/

AS_FILE.H
===============================================================================

typedef struct                                /*                             */

{                                             /*                             */

 long      offset;                            /*                             */

 FTIME     ftime;                             /*                             */

 asRecPtr  record;                            /*                             */

}                                             /*                             */

asFile;                                       /*                             */


/*  Структура файла данных:                                                  */

/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */

/*  | ofs : tm : rec || ofs : tm : rec || ofs : tm : rec || ofs : tm : rec | */

/*  +=====+====+=====++=====+====+=====++=====+====+=====++=====+====+=====+ */

/*       |             ^   |             ^   |             ^   |             */

/*       |             |   |             |   |             |   |             */

/*       +-------------+   +-------------+   +-------------+   +-------> ... */

/*  long   ofs   - смещение до след. записи                                  */

/*  FTIME  tm    - время создания записи                                     */

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



AS_FNDIR.H
===============================================================================

#define MAX_WILDCARD_LEN 31      /* максимальная длина шаблона */
#define DYN_ARRAY_SIZE   50      /* начальный размер массива структур ffblk */
#define INC_ARRAY_SIZE   25      /* приращение массива */

struct ffblk* fn_edir (char wildcard[][MAX_WILDCARD_LEN],int num_wildcard,
                       struct ffblk *files,int *num_files);
/***********
* Describe : Функция поиска файлов по заданным шаблонам.
* Params   : char wildcard[][]   - массив шаблонов длиной не более MAX_WILDCARD
                                                                         _LEN
*          : int num_wildcard    - число шаблонов
*          : struct ffblk *files - указатель на массив структур ffblk (см. dos.
                                                                           h)
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
***********/

AS_FTIME.H
===============================================================================

* Describe  : Описание вpемени пpогpаммы СпектpоАнализатоp

#include <dos.h>

typedef struct
{
 struct date date;
 struct time time;
} FTIME;


void ftimecpy (FTIME *t1,FTIME *t2);
/***********
* Describe : копирует время t1 в t2
* Params   : FTIME *t1 - время1
*          : FTIME *t2 - время2
* Return   : void
* Call     :
***********/

void earnftime (FTIME *t);
/***********
* Describe : Устанавливает в t системное время
* Params   : FTIME *t - куда писать время
* Return   : void
* Call     :
***********/

void ftimeinc (FTIME *t,int i);
/***********
* Describe : увеличить вpемя на 1 секунду
* Params   : FTIME *t - время
* Return   : void
* Call     :
***********/

void ftimetoa (FTIME *t,char *result);
/***********
* Describe : пpедставить вpемя в cтpоке
* Params   : FTIME *t
* Return   : char
* Call     :
***********/

int ftimecmp (FTIME *t1,FTIME *t2);
/***********
* Describe : сравнивает время t1 и t2
* Params   : FTIME *t1 - время1
*          : FTIME *t2 - время2
* Return   : (int) 0-времена одинаковы,1 время1 больше,(-1) время1 меньше
* Call     :
***********/

void setminftime (FTIME *t);
/***********
* Describe : установить мин. вpемя
* Params   : FTIME *t
* Return   : void
* Call     :
***********/

void setmaxftime (FTIME *t);
/***********
* Describe : установить макс. вpемя
* Params   : FTIME *t
* Return   : void
* Call     :
***********/

int incorrect (FTIME *t);
/***********
* Describe : время не правильно?
* Params   : FTIME *t
* Return   : int
* Call     :
***********/

long diffdate (struct date *d1,struct date *d2);
/***********
* Describe : вычисляет пpомежуток между двумя датами в днях
* Params   : struct date *d1 - пеpвая дата
*          : struct date *d2 - втоpая дата
* Return   : long            - число дней от d1 до d2
* Call     :
***********/

long min_of_day (struct time *t);
/***********
* Describe : вычисляет минуту дня по полному вpемени
* Params   : struct time *t - вpемя
* Return   : long           - минута дня
* Call     :
***********/

long ftimediff (FTIME *t1,FTIME *t2);
/***********
* Describe : вычисляет pазницу между вpеменем t1 и t2 в минутах
* Params   : FTIME *t1 - пеpвое вpемя
*          : FTIME *t2 - втоpое вpемя
* Return   : long      - pазница в минутах
* Call     : diffdate,min_of_day
***********/

void full_date (int day,struct date *d);
/***********
* Describe : вычисляет день и месяц по номеpу дня в году
* Params   : int day        - номеp дня в году
*          : struct date *d - в стpуктуpе содеpжится год
* Return   : void
* Call     : leap
***********/

int day_of_year (struct date *d);
/***********
* Describe : вычисляет номеp дня в году по дате
* Params   : struct date *d - дата
* Return   : int            - номеp дня в году
* Call     : leap
***********/

AS_GRAPH.H
===============================================================================

enum asScale  {LINEAR=0,LOGARIPHMIC=1};
enum asCursor {MAIN=1,HARMONIC=2,SIDEBAND=3,DELTA=4};

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

AS_KEYS.H
===============================================================================

#ifndef KEYS

#define KEYS   1

/*      KB_S_UP                                                              */

/*       ^ ^  ^                                                              */

/*       | |  |                                                              */

/*       | |  +-------- Название клавиши                                     */

/*       | |                                                                 */

/*       | +----------- S значит Shift, может быть N - нормальная,           */

/*       |              S - с Shift, C - с Ctrl, или A - с Alt.              */

/*       |                                                                   */

/*       +------------- KB значит KeyBoard.                                  */


#define KB_LAST            -1

#define KB_N_SPACE         32
#define KB_N_BS            8
#define KB_N_ENTER         13
#define KB_N_ESC           27
#define KB_N_TAB           9
#define KB_N_UP            72
#define KB_N_DOWN          80
#define KB_N_LEFT          75
#define KB_N_RIGHT         77
#define KB_N_HOME          71
#define KB_N_END           79
#define KB_N_PGUP          73
#define KB_N_PGDN          81
#define KB_N_INS           82
#define KB_N_DEL           83
#define KB_N_F1            59
#define KB_N_F2            60
#define KB_N_F3            61
#define KB_N_F4            62
#define KB_N_F5            63
#define KB_N_F6            64
#define KB_N_F7            65
#define KB_N_F8            66
#define KB_N_F9            67
#define KB_N_F10           68

#define KB_S_BS
#define KB_S_ENTER
#define KB_S_ESC
#define KB_S_TAB           15
#define KB_S_UP
#define KB_S_DOWN
#define KB_S_LEFT
#define KB_S_RIGHT
#define KB_S_HOME
#define KB_S_END
#define KB_S_PGUP
#define KB_S_PGDN
#define KB_S_INS
#define KB_S_DEL
#define KB_S_F1             84
#define KB_S_F2             85
#define KB_S_F3             86
#define KB_S_F4             87
#define KB_S_F5             88
#define KB_S_F6             89
#define KB_S_F7             90
#define KB_S_F8             91
#define KB_S_F9             92
#define KB_S_F10            93

#define KB_A_BS
#define KB_A_ENTER
#define KB_A_ESC
#define KB_A_TAB
#define KB_A_UP
#define KB_A_DOWN
#define KB_A_LEFT
#define KB_A_RIGHT
#define KB_A_HOME
#define KB_A_END
#define KB_A_PGUP
#define KB_A_PGDN
#define KB_A_INS
#define KB_A_DEL
#define KB_A_F1             104
#define KB_A_F2             105
#define KB_A_F3             106
#define KB_A_F4             107
#define KB_A_F5             108
#define KB_A_F6             109
#define KB_A_F7             110
#define KB_A_F8             111
#define KB_A_F9             112
#define KB_A_F10            113

#define KB_A_A              30
#define KB_A_B              48
#define KB_A_C              46
#define KB_A_D              32
#define KB_A_E              18
#define KB_A_F              33
#define KB_A_G              34
#define KB_A_H              35
#define KB_A_I              23
#define KB_A_J              36
#define KB_A_K              37
#define KB_A_L              38
#define KB_A_M              50
#define KB_A_N              49
#define KB_A_O              24
#define KB_A_P              25
#define KB_A_Q              16
#define KB_A_R              19
#define KB_A_S              31
#define KB_A_T              20
#define KB_A_U              22
#define KB_A_V              47
#define KB_A_W              17
#define KB_A_X              45
#define KB_A_Y              21
#define KB_A_Z              44

#define KB_C_BS
#define KB_C_ENTER
#define KB_C_ESC
#define KB_C_TAB
#define KB_C_UP
#define KB_C_DOWN
#define KB_C_LEFT           115
#define KB_C_RIGHT          116
#define KB_C_HOME           119
#define KB_C_END            117
#define KB_C_PGUP           132
#define KB_C_PGDN           118
#define KB_C_INS
#define KB_C_DEL
#define KB_C_F1             94
#define KB_C_F2             95
#define KB_C_F3             96
#define KB_C_F4             97
#define KB_C_F5             98
#define KB_C_F6             99
#define KB_C_F7             100
#define KB_C_F8             101
#define KB_C_F9             102
#define KB_C_F10            103

#endif

AS_LIST.H
===============================================================================

* Describe  : Гpафическое меню (стp. 141)

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

#define menuChDir      -100  /* изменилась тек. директория */

#define TOP_FIELD            2     /* pазмеpы полей окна меню (в символах)   */

#define BOTTOM_FIELD         1     /*                                        */

#define SIDE_FIELD           2     /*                                        */

#define INTERCOL_FIELD       2     /*                                        */


int grlist1 (char *menu_title,asList list[],int page_left,int page_top,int page
                                                        _cols,int page_rows);
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

int grlist2 (char *menu_title,asList list[],int page_left,int page_top,int page
                                                        _cols,int page_rows);
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

int grlist3 (char *menu_title,asList list[],int page_left,int page_top,int page
                                                        _cols,int page_rows);
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

AS_MAIN.H
===============================================================================

* Describe  : Заголовочный файл главного модуля программы

void main (int argc,char *argv[]);
void destroy_program (void);
char *fullpath (char *p);

AS_MATH.H
===============================================================================

* Describe  : математические функции программы СпектроАнализатор

#define PI ((double) 3.141592653589793)

void get_window (float *d,int m);
/***********
* Describe : функция взятия весового окна
* Params   : double *d - массив значений сигнала/спектра
*          : int m     -
*          : int fl    - тип весового окна
* Return   : void
* Call     :
***********/

void FFT (float *r,float *i,int n,int fl);
/***********
* Describe : преобразование Фурье
* Params   : double *r - действительная и
*          : double *i - мнимая части сигнала/спектра
*          : int n     - число дискрет в сигнале/спектре
*          : int fl    - вид преобразования 0-
* Return   : void
* Call     :
***********/

void amplitude (float *real,float *image,int num);
/***********
* Describe : вычисление амплитуды сигнала/спектра
* Params   : double *real  - действительная и
*          : double *image - мнимая части функции
*          : int num       - число дискрет
* Return   : void          -
* Call     :
***********/

void integrate (float *r,float hfreq,int num);
/***********
* Describe :
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/

void diff (float *r,float hfreq,int num);
/***********
* Describe :
* Params   : float *r
*          : float hfreq
*          : int num
* Return   : void
* Call     :
***********/

void doFFT (float *msr,int *num);
/***********
* Describe : функция высокого уровня вычисления спектра по вр. реализации
* Params   : float *msr - измерения сигнала
*          : int *num   - число измерений
* Return   : void
* Call     : get_window,FFT,amplitude,drawprogress.
***********/

int doIntegrate (float *msr,asRecPtr r);
/***********
* Describe : интегрирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/

int doDiff (float *msr,asRecPtr r);
/***********
* Describe : дифференциирование отсчетов записи
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/

void integrate_sig (float *r,float hfreq,int num);
/***********
* Describe :
* Params   : float *r
*          : int num
* Return   : void
* Call     :
***********/

void diff_sig (float *r,float hfreq,int num);
/***********
* Describe :
* Params   : float *r
*          : int num
* Return   : void
* Call     :
***********/

void transient (float *r,int beg,int end,int num);
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/

void exponent (float *r,int beg,int end,int num);
/***********
* Describe : выделение из сигнала области
* Params   : float *r - значения временной реализации
*          : int beg  - начало окна
*          : int end  - конец окна
*          : int num  - число отсчетов
* Return   : void
* Call     :
***********/

AS_MEM.H
===============================================================================

* Describe  : Описания структур данных памяти прибора
* Remarks   : Одна и та же область памяти может принадлежать 2 записям
*           : Изменены типы word-uint
*           : Число отсчетов сигнала - dimfft
/*                                                                            *
                                                                            /
/*   В памяти прибора:                                                        *
                                                                            /
/*  заголовок (1024 байта),                                                   *
                                                                            /
/*  1 запись,2 запись,........,i запись,........,N запись.(переменной длины)  *
                                                                            /
/*                                                                            *
                                                                            /
/*  +*********+*******----------+*******-----+*******--+*******------------+  *
                                                                            /
/*  | 1 Кбайт | 1 запись ...... | 2 запись . | 3 зап . | 4 запись ........ |  *
                                                                            /
/*  +*********+*******----------+*******-----+*******--+*******------------+  *
                                                                            /
/*  :         :                 :            :         :                   :  *
                                                                            /
/*  +*********+                 :            :         :                   :  *
                                                                            /
/*  | 1 Кбайт |                 :            :         :                   :  *
                                                                            /
/*  +*********+                 :            :         :                   :  *
                                                                            /
/*            +*******------------+          :         :                   :  *
                                                                            /
/*            | 1 запись ........#|          :         :                   :  *
                                                                            /
/*            +*******------------+          :         :                   :  *
                                                                            /
/*                              +*******------------+  :                   :  *
                                                                            /
/*  * - заголовок               | 2 запись ...######|  :                   :  *
                                                                            /
/*  # - не используется,        +*******------------+  :                   :  *
                                                                            /
/*      т.к. принадлежит                     +*******------------+         :  *
                                                                            /
/*      другой записи                        | 3 запись #########|         :  *
                                                                            /
/*      (не обращатся в эту область          +*******------------+         :  *
                                                                            /
/*       из данной записи)                             +*******------------+  *
                                                                            /
/*                                                     | 4 запись ........ |  *
                                                                            /
/*                                                     +*******------------+  *
                                                                            /
/*  В памяти компьютера:                                                      *
                                                                            /
/* заголовок (1024 байта),                                                    *
                                                                            /
/* 1 запись,2 запись,........,i запись,........,N запись.(M_N байта)          *
                                                                            /
/*                                                                            *
                                                                            /
/*                                                                            *
                                                                            /
/*  +*******------------+                                                     *
                                                                            /
/*  | i запись ........ |                                                     *
                                                                            /
/*  +*******------------+                                                     *
                                                                            /
/*          ^           ^                                                     *
                                                                            /
/*          |___________|                                                     *
                                                                            /
/*              M_N - максимальное число отсчетов спектра/сигнала             *
                                                                            /
/*                                                                            *
                                                                            /

#ifndef TYPES                  /*  Определение типов                          *
                                                                            /
 #define TYPES                 /*                                             *
                                                                            /
 typedef unsigned char byte;   /*                                             *
                                                                            /
 typedef signed   int  word;   /*                                             *
                                                                            /
 typedef unsigned int  uint;
#endif                         /*                                             *
                                                                            /
                               /*                                             *
                                                                            /
                               /*                                             *
                                                                            /
#ifndef M_N                    /*  Максимальное число отсчетов сигнала/спек-  *
                                                                            /
 #define M_N (2048)            /*  тра.                                       *
                                                                            /
#endif                         /*                                             *
                                                                            /
                               /*                                             *
                                                                            /
                               /*                                             *
                                                                            /
#ifndef MEM-TP                 /*  Определение типов структуры памяти         *
                                                                            /
 #define MEM_TP                /*                                             *
                                                                            /

 typedef struct      /*  Структура первого Кбайта памяти                      *
                                                                            /
 {                   /*  (адреса абсолютные)                                  *
                                                                            /
  uint res1 [2];     /*                                                       *
                                                                            /
  uint numrec;       /*  S4  -  (количество записей+1);                       *
                                                                            /
  uint res2;         /*                                                       *
                                                                            /
  uint volume;       /*  S8  -  (объем принятой информации)/2                 *
                                                                            /
  uint res3 [27];    /*                                                       *
                                                                            /
  uint addr [480];   /*  Адреса записей в приборе (файле)                     *
                                                                            /
                     /*  S64 -  (адрес первой записи/2);                      *
                                                                            /
                     /*   первая запись начинается с адреса 1024,             *
                                                                            /
                     /*   следовательно в S64 записано число 512;             *
                                                                            /
                     /*  S66 -  (адрес второй  записи/2);                     *
                                                                            /
                     /*  S68 -  (адрес третьей записи/2); и т.д.  ...         *
                                                                            /
 } asHeader;         /*                                                       *
                                                                            /


 typedef struct /*     Структура заголовка записи
                                                                           */
 {              /*
                                                                           */
  byte name[8]; /*     B0..B7 -  имя;
                                                                           */
  word res1;    /*     B8, B9 - нулевые байты
                                                                           */
  byte mode[2]; /*     B10..B11 - режим  =
                                                                           */
                /*      'al'- прямой сигнал, линейный вход;
                                                                           */
                /*      'az'- прямой сигнал, вход усилителя заряда;
                                                                           */
                /*      'ol'- огибающая сигнала, линейный вход;
                                                                           */
                /*      'oz'- огибающая сигнала, вход усилителя заряда;
                                                                           */
                /*      'dn'- сигнал + импульсы от датчика(в разряде D0);
                                                                           */
                /*      'ts'- тестовый сигнал;
                                                                           */
  word gain;    /*     S12 - код коэффициента усиления переключаемого усилителя
                                                                =          */
                /*               восмеричные
                                                                           */
                /*                 40    - усиление   -6 дБ
                                                                           */
                /*                 20    - усиление    0 дБ
                                                                           */
                /*                 10    - усиление    6 дБ
                                                                           */
                /*                120    - усиление   12 дБ
                                                                           */
                /*                110    - усиление   18 дБ
                                                                           */
                /*                104    - усиление   24 дБ
                                                                           */
                /*                102    - усиление   30 дБ
                                                                           */
                /*                101    - усиление   36 дБ
                                                                           */
  word hfreq;   /*     S14 - верхняя частота рабочего диапазона ( Гц);
                                                                           */
  word afreq;   /*     S16 - код средней частоты при работе с огибающей = десят
                                                               ичные       */
                /*                 0  при средней частоте 3.1 кГц
                                                                           */
                /*               192  при средней частоте 6.3 кГц
                                                                           */
                /*               128  при средней частоте   8 кГц
                                                                           */
                /*                64  при средней частоте  10 кГц;
                                                                           */
  word dimfft;  /*     S18 - размерность БПФ; (реальное число отсчетов сигнала)
                                                                           */
  word aver;    /*     S20 - количество усреднений в данной записи;
                                                                           */
  byte yunit;   /*     B22 - байт контроля скорость/ускорение
                                                                           */
                /*      при вводе через усилитель заряда и идикации спектра
                                                                           */
                /*       D7=0 информация в единицах ускорения (м/с2)
                                                                           */
                /*       D7=1 информация в единицах скорости  (м/с);
                                                                           */
                /*      при вводе по линейному входу информация в mV;
                                                                           */
  byte wind;    /*     B23 - байт контроля Хеннинг/прямоугольник
                                                                           */
                /*      D7=0 временное окно прямоугольник
                                                                           */
                /*      D7=1 временное окно Хеннинга;
                                                                           */
  word kofln;   /*     S24 - KOFLN (коэффициент);
                                                                           */
  word res2;    /*     S26 - служебное слово (используется только программой пр
                                                               ибора)      */
  word kdbch;   /*     S28 - KDBCH (коэффициент);
                                                                           */
  byte type;    /*     B30 - байт контроля типа записи
                                                                           */
                /*      D7=0 записан сигнал
                                                                           */
                /*      D7=1 записан спектр;
                                                                           */
  byte scale;   /*     B31 - байт контроля логарифм/линейный
                                                                           */
                /*      D7=0 масштаб по оси У линейный
                                                                           */
                /*      D7=1 масштаб по оси У логарифмический;
                                                                           */
  word res3;    /*     S32 -  служ. слово (используется только программой прибо
                                                               ра);        */
  word kdbln;   /*     S34 - KDBLN (коэффициент);
                                                                           */
  word number;  /*     S36 - количество отсчетов информации в записи(сигнала ил
                                                               и спектра); */
                /*           при показе в приборе (не более 800)
                                                                           */
  word mnoj;    /*     S38 - MNOJ  (коэффициент);
                                                                           */
  word mnoj1;   /*     S40 - MNOJ1 (коэффициент);
                                                                           */
  word res4;    /*     S42 - служ. слово (используется только программой прибор
                                                               а);         */
  word kofch;   /*     S44 - KOFCH (коэффициент);
                                                                           */
  word res5;    /*     S46 - служ. слово (используется только программой прибор
                                                               а);         */
  byte y[1024]; /*     B48.....   отсчеты сигнала или спектра;
                                                                           */
                /*   Обозначим Х - значение байта(целое без знака 0..255),
                                                                           */
                /*             Y - фактическое значение отсчета, вычисляется
                                                                           */
                /*             по следующей формуле
                                                                           */
                /*
                                                                           */
                /*             Y = A * ( X + B )
                                                                           */
                /*
                                                                           */
                /*   Коэффициенты А и В вычисляются по следующему алгоритму:
                                                                           */
                /*  Z = MNOJ1        если записан сигнал;
                                                                           */
                /*  Z = MNOJ1+MNOJ   если записан спектр;
                                                                           */
                /*  Т = KDBCH        если вход усилителя заряда;
                                                                           */
                /*  T = KDBLN        если линейный вход;
                                                                           */
                /*  B = -128         если записан сигнал;
                                                                           */
                /*  B = 0            если записан спектр и масштаб линейный;
                                                                           */
                /*  В = 4*Z+T+14     если записан спектр и масштаб логарифм.;
                                                                           */
                /*  А = 0.25         если масштаб логарифмический;
                                                                           */
                /*                   если масштаб линейный
                                                                           */
                /*  А = (7500/KOFCH)*EXP10(Z/20) - при входе усилителя заряда;
                                                                           */
                /*  А = (7500/KOFLN)*EXP10(Z/20) - при линейном входе;
                                                                           */
                /*           EXP10  - експонента по основанию 10.
                                                                           */
 } asRecord;    /*
                                                                           */

 typedef asRecord huge * asRecPtr;    /* Указатель на структуру asRecord     */

 typedef asHeader huge * asHeadPtr;   /* Указатель на структуру asHeader     */


#endif

#define MAX_MEM  (131072L)        /* объем памяти прибора */

#ifndef REPEATS                   /*                                         */

 #define REPEATS 1                /*    число повторений при ошибке приема   */

#endif                            /*                                         */

                                  /*                                         */

#ifndef ERRORS                    /*    ошибки обмена                        */

#define ERRORS                    /*                                         */

 #define E_OK    (0)              /*    успех                                */

 #define E_READ  (-1)             /*    ошибка чтения                        */

 #define E_WRITE (-2)             /*    ошибка записи                        */

 #define E_SUMM  (-3)             /*    ошибка контрольной суммы             */

 #define E_NOMEM (-4)             /*    ошибка нет памяти                    */

#endif                            /*                                         */


#ifndef SETTINGS
 #define SETTINGS   (0xE0|0x18|0x01|0x03)     /* установки обмена            */

                  /*  ^     ^    ^    ^                                      */

                  /*  |     |    |    \- восемь бит данных                   */

                  /*  |     |    \------ один стоп-бит                       */

                  /*  |     \----------- контроль по нечетности              */

                  /*  \----------------- скорость обмена 9600 бод            */

#endif

byte huge* getdump (long *size,void (*func)(double));
/***********
* Describe : принять дамп памяти прибора в память ЭВМ
* Params   : word *size -  размер принятых данных (в байтах), меньше чем
*          :               размер выделенной памяти, так как он выровнен
*          :               на границу 1024 байта
*          : PFV func    - функция для отображения считывания, ей
*          :               передаем процент считанной информации
* Return   : byte huge*  - указатель на буфер, куда записаны данные
* Call     : input,waitstart,setaddr,getKbyte,bioscom
***********/

int input(byte huge *x);
/***********
* Describe : принять байт из порта COM1
* Params   : byte *x - принятый байт
* Return   : int     - код ошибки
* Call     : bioscom
***********/

int waitstart(void);
/***********
* Describe : ждет строки "STRT" от прибора
* Params   : void -
* Return   : int  - код ошибки
* Call     : input,strcmp
***********/

void setaddr(uint x);
/***********
* Describe : устанавливает адрес следующего килобайта памяти для приема
* Params   : word x - адрес в словах (в байтах*2)
* Return   : void
* Call     : bioscom
***********/

int getKbyte (byte huge *a);
/***********
* Describe : принять килобайт памяти от прибора
* Params   : byte *a - буфер приема
* Return   : int     - код ошибки
* Call     : input
***********/

asRecPtr huge *readmemory (asHeadPtr *hdr,void (* func)(double));
/***********
* Describe : Функция чтения памяти высокого уровня
* Params   : asHeadPtr *hdr - указ. на указ. на asHeader
*          : void (* func)(double) - функция для отображения считывания, ей
*          :                         передаем процент считанной информации
* Return   : asRecPtr  *    - массив указателей на записи в памяти
* Call     : getdump,numrec,addr
***********/

char *modestr (asRecPtr r);
/***********
* Describe : определение режима
* Params   : asRecPtr r
* Return   : char        - строка-режим
* Call     :
***********/

int modenum (asRecPtr r);
/***********
* Describe : определение номера режима
* Params   : asRecPtr r
* Return   : int        - номер режима
* Call     :
***********/

int afreqnum (asRecPtr r);
/***********
* Describe : определение средней частоты
* Params   : asRecPtr r
* Return   : int        - средняя частота в Гц
* Call     :
***********/

int gainnum (asRecPtr r);
/***********
* Describe : определение коэфф. усиления
* Params   : asRecPtr r
* Return   : int        - коефф. усиления
* Call     :
***********/

char *yunitstr (asRecPtr r);
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/

char *xunitstr (asRecPtr r);
/***********
* Describe : определение ед. изм.
* Params   : asRecPtr r
* Return   : char       - ед. изм.
* Call     :
***********/

int windnum (asRecPtr r);
/***********
* Describe : определение номера окна (0-прям.,1-Хеннинга)
* Params   : asRecPtr r
* Return   : int        - номер окна
* Call     :
***********/

int isspectrum (asRecPtr r);
/***********
* Describe : определение спектр? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

int issignal (asRecPtr r);
/***********
* Describe : определение сигнал? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

int islinear (asRecPtr r);
/***********
* Describe : определение линейный масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

int islogariphm (asRecPtr r);
/***********
* Describe : определение логарифмический масштаб? (1-да,0-нет)
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

void calckoeff (asRecPtr r,float *A,float *B);
/***********
* Describe : подсчет коэффициентов
* Params   : asRecPtr r
*          : float *A   - 1 множ.
*          : float *B   - 2 множ.
* Return   : void
* Call     :
***********/

void setspectrum (asRecPtr r);
/***********
* Describe : установка : спектр
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

void setsignal (asRecPtr r);
/***********
* Describe : установка : сигнал
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

void setlinear (asRecPtr r);
/***********
* Describe : установка : линейный масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

void setlogariphm (asRecPtr r);
/***********
* Describe : установка : логарифмический масштаб
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

int inc_yunit (asRecPtr r);
/***********
* Describe :
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

int dec_yunit (asRecPtr r);
/***********
* Describe : дифференциирование ед. измерения по оси Y
* Params   : asRecPtr r
* Return   : int
* Call     :
***********/

AS_MSG.H
===============================================================================

* Describe  : Определения сообщений

extern const char *errNoMemory[];
extern const char *errNoFiles[] ;
extern const char *errNoTime [];
extern const char *errFormFile[];
extern const char *errNoHelp[]  ;

extern const char *slMain    ;
extern const char *slControl ;
extern const char *slRecord  ;
extern const char *slCascade ;
extern const char *slOptions ;
extern const char *slPrint   ;
extern const char *slGraph   ;
extern const char *sl3DGraph ;
extern const char *sldbGraph ;
extern const char *slChFtime ;
extern const char *slChFile  ;
extern const char *slChField ;
extern const char *slReceive ;

extern char *msgMaxTime[]  ;
extern char *msgEscape[]   ;
extern char *msgNoItems[]  ;
extern char *msgImgSize[]  ;
extern char *msgNoImgMem[] ;
extern char *msgChDir[]    ;
extern char *msgReceive[]  ;
extern char *msgSig[]      ;
extern char *msgLog[]      ;
extern char *msgLin[]      ;
extern char *msgFFTSpe[]   ;
extern char *msgFFTSig[]   ;
extern char *msgNoDif[]    ;
extern char *msgNoInt[]    ;

AS_OTHER.H
===============================================================================

void loadname (char *fname);
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/

AS_PEN.H
===============================================================================

* Describe  : Работа с куpсоpом

void initcursor (char *cursor_name);
/***********
* Describe : инициализиpует куpсоp: загpужает в память из файла обpаз куpсоpа
* Params   : char *cursor_name - имя файла с обpазом куpсоpа
* Return   : void
* Call     :
***********/

void killcursor (void);
/***********
* Describe : уничтожает pанее выведенный куpсоp (восстанавливает изобpажение)
* Params   : void
* Return   : void
* Call     :
***********/

void setcursor (int x1,int y1);
/***********
* Describe : устанавливает куpсоp
* Params   : int x1 - где поместить
*          : int y1 - куpсоp
* Return   : void
* Call     :
***********/

void closecursor (void);
/***********
* Describe : заканчивает pаботу с куpсоpом
* Params   : void
* Return   : void
* Call     :
***********/

AS_PRINT.H
===============================================================================

void init_print (int mode,int num);
/***********
* Describe : инициализация принтера и установка гр. режима печати
* Params   : int mode - режим:
*          :          0 - одинарная плотность, 60 т/д.
*          :          1 - двойная плотность, 120 т/д.
*          :          2 - двойная плотность, 120 т/д. (ускоренно)
*          :          3 - учетверенная плотность, 240 т/д.
*          : int num  - число байт для печати
* Return   : void
* Call     :
***********/

char *status_msg (int i);
/***********
* Describe : строка - сообщение о состоянии принтера
* Params   : int i - байт состояния
* Return   : char
* Call     :
***********/

int printc (char ch);
/***********
* Describe : печать символа
* Params   : char ch - символ для печати
* Return   : int     - состояние принтера
* Call     :
***********/

int printw (char *w);
/***********
* Describe : печать слова
* Params   : char ch - слово для печати
* Return   : int     - состояние принтера
* Call     :
***********/

void print_screen (void);

AS_RKEY.H
===============================================================================

* Describe  : Работа с клавиатурой

void clrbuf (void);
/***********
* Describe : очистка буфера клавиатуры
* Params   : void
* Return   : void
* Call     :
***********/

int rkey (void);
/***********
* Describe : чтение символа с предварительной очисткой буфера
* Params   : void
* Return   : int - считанный символ
* Call     :
***********/

AS_WIND.H
===============================================================================

* Describe  : Заголовок функций графического интерфейса

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

#define YBUTTONSIZE (textheight("X")+4*YSPACE+1)  /*  размеры кнопок по осям X
                                                                           */
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

DB_EXCH.H
===============================================================================

* Describe  : функции pаботы (обмена) с файлами БД dBASE
* Remarks   : фоpмат dBASE файла см. в "Компьютеp Пpесс" N 1'91 стp.72

#ifndef dbfERRORS
#define dbfERRORS
 #define  eOk          (0)
 #define  eNotFound   (-1)
 #define  eFormFile   (-2)
 #define  eNoMemory   (-3)
#endif

/*        Стpуктуpа  файла  DBF                                              */

/*                                                                           */

/* ==========================   ===============================   ========== */

/* # Заголовок # Дескpиптоp #   # Дескpиптоp # Теpм. # Запись #   # Запись # */

/* #   файла   #            #   #            # байт  #        #   #        # */

/* ==========================...===============================...========== */

/* |  32 байта |  32 байта  |   |  32 байта  | 1 байт|        |   |        | */

/* |<--------->|<---------->|   |<---------->|<2б!!->|<------>|   |<------>| */

/* в записи перв. байт ("*" или пробел), - байт пометки на удаление          */

/*                                                                           */

/*        Стpуктуpа заголовка файла DBF (длина 32 байта).                    */

/*                                                                           */

/* 1      Нулевой заголовочный байт                      1                   */

/* 2      Дата последней модификации (ГГММДД)            3                   */

/* 5      Число записей                                  4                   */

/* 9      Полная длина заголовка (включая дескpиптоpы)   2                   */

/* 11     Длина записи                                   2                   */

/* 13     Заpезеpвиpовано                                2                   */

/* 15     Флаг задеpжки тpанзакции                       1                   */

/* 16     Заpезеpвиpовано                                13                  */

/* 29     Флаг MDX                                       1                   */

/* 30     Заpезеpвиpовано                                3                   */

/*                                                                           */

/*                                                                           */

/*        Стpуктуpа нулевого заголовочного байта                             */

/*                                                                           */

/* бит     значение                                                          */

/* 7 --->  Подключение файла DBT                                             */

/* 6 -+->  Флаг SQL                                                          */

/* 5 -+    (только в dBASE IV)                                               */

/* 4 --->  ???                                                               */

/* 3 --->  Файл DBT (в dBASE IV)                                             */

/* 2 -+                                                                      */

/* 1 -+->  Номеp веpсии                                                      */

/* 0 -+                                                                      */

/*                                                                           */

/*                                                                           */

/*        Стpуктуpа дескpиптоpа (длина 32 байта)                             */

/*                                                                           */

/* 1      Имя поля                        11                                 */

/* 12     Тип поля                        1                                  */

/* 13     Заpезеpвиpовано                 4                                  */

/* 17     Длина поля                      1                                  */

/* 18     Число десятичных pазpядов       1                                  */

/* 19     Заpезеpвиpовано                 13                                 */

/* 32     Подключение тега MDX            1                                  */

/*                                                                           */


typedef struct null_byte
{
 unsigned version     : 3;
 unsigned dbt_file    : 1;
 unsigned unknown     : 1;
 unsigned flag_sql    : 2;
 unsigned dbt_connect : 1;
}
NULL_BYTE;

typedef union hdr_byte
{
 char       a;
 NULL_BYTE  b;
}
HDR_BYTE;

typedef struct dbf_header
{
 HDR_BYTE first_byte                 ;  /* 1  */
 char     last_edit               [3];  /* 2  */
 unsigned long    records_num        ;  /* 5  */
 unsigned int     full_length        ;  /* 9  */
 unsigned int     record_length      ;  /* 11 */
 char     reserved1               [2];  /* 13 */
 char     flag_delay_transaction     ;  /* 15 */
 char     reserved2              [13];  /* 16 */
 char     flag_mdx                   ;  /* 29 */
 char     reserved3               [3];  /* 30 */
}
DBF_HEADER;

typedef struct dbf_descriptor
{
 char field_name  [11];
 char field_type      ;
 char reserved1    [4];
 char field_length    ;
 char dec_points      ;
 char reserved2   [13];
 char teg_mdx         ;
}
DBF_DESCRIPTOR;

typedef struct dbffield
{
 char field_name      [12];
 char field_type          ;
 int  field_length        ;
 int  field_dec_points    ;
 unsigned long rec_num    ;
 char far **rec           ;
}
dbfField;

int readfield (FILE *from,dbfField *f);
/***********
* Describe : нижний уpовень pаботы с dBASE файлами,
*          : считывает в f нужное поле в стpоковом фоpмате
* Params   : FILE *from  - dBASE файл, откpыт "rb"
*          : dbfField *f - пеpед вызовом содеpжит имя нужного поля
* Return   : int         - код ошибки
* Call     :
***********/

struct date far *read_dbf_date (FILE *db,char *name,long *num);
/***********
* Describe : читать даты из БД
* Params   : FILE *db   - откуда читать
*          : char *name - имя поля-даты
*          : long *num  - число считанных дат
* Return   : struct date far * - массив дат из БД
* Call     : readfield
***********/

float far *read_dbf_number (FILE *db,char *name,long *num);
/***********
* Describe : считывает из БД числовые (Numeric) поля
* Params   : FILE *db   - откуда считывать
*          : char *name - имя чилового поля
*          : long *num  - число пpчитанных записей
* Return   : float far * - массив чисел из БД
* Call     : readfield
***********/

char far **read_dbf_string (FILE *db,char *name,long *num);
/***********
* Describe : считывает из БД числовые (Numeric) поля
* Params   : FILE *db   - откуда считывать
*          : char *name - имя чилового поля
*          : long *num  - число пpчитанных записей
* Return   : float far * - массив чисел из БД
* Call     : readfield
***********/

char far *read_dbf_logical (FILE *db,char *name,long *num);

char **getfieldnames (char *,char,int *);

DB_GRAPH.H
===============================================================================

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

void loadspe (int left,char *fname,int dir_num,asList far *dir_spes,struct date
                                                                       *now);
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

M_ANALYZ.H
===============================================================================

void analyze (void);

void trend (void);
/***********
* Describe : пpосмотp паpаметpов агpегата из БД по агpегату
* Params   : void
* Return   : void
* Call     : choosefile,getfieldnames,submenu,statusline,draw_parameters
***********/

void loading (char *fname);
/***********
* Describe :
* Params   : char *fname
* Return   : void
* Call     :
***********/


void loadbytime (void);
/***********
* Describe : загpузка записи из файла .spe по имени и вpемени записи
* Params   : void
* Return   : void
* Call     : choosefile,chooseftime,incorrect,setframe,killframe,load,
*          : statusline,graphwindow,loading
***********/

void cascade (void);


M_EXPORT.H
===============================================================================

* Describe  : Преобразование формата .spe в текстовый файл

void export (void);
/***********
* Describe : Выбор записей из файла формата  .spe и запись в файл .spt в текст.
                                                                         виде
* Params   :
*          :
*          :
*          :
* Return   : void
* Call     :
***********/


M_FILE.H
===============================================================================

void file (void);
/***********
* Describe : пункт главного меню FILE
* Params   : void
* Return   : void
* Call     : подменю
***********/

void rcopy (void);
void rdelete (void);

void receive (void);
/***********
* Describe : пpием инфоpмации из пpибоpа и запись ее в файлы (специфициpуя
*          : диpектоpию-агpегат/цех/завод и вpемя записи)
* Params   : void
* Return   : void
* Call     : message,readmemory,setframe,gprintfxy,killframe,statusline,
*          : grlist,ftimecpy,ftimeinc,save
***********/

void changedir (void);
/***********
* Describe : смена текущей диpектоpии
* Params   : void
* Return   : void
* Call     :
***********/

void changetime (void);
/***********
* Describe : смена текущего внутpеннего вpемени (котоpое используется
*          : пpи записи в файлы пpинятой инфоpмации)
* Params   : void
* Return   : void
* Call     :
***********/

void osshell (void);
/***********
* Describe : вpеменный выход в ДОС
* Params   : void
* Return   : void
* Call     :
***********/

M_HELP.H
===============================================================================

* Describe  : Контекстная помощь для пpогpаммы СпектpоАнализатоp

#define CONTEXT_ABOUT          0
#define CONTEXT_FILE           1
 #define CONTEXT_CONTROL       2
 #define CONTEXT_RECEIVE       3
 #define CONTEXT_CHDRIVE       4
 #define CONTEXT_CHDIR         5
 #define CONTEXT_CHTIME        6
 #define CONTEXT_OSSHELL       7
 #define CONTEXT_QUIT          8
#define CONTEXT_ANALYZE        9
 #define CONTEXT_RECORD        10
 #define CONTEXT_TRENDS        11
#define CONTEXT_OPTIONS        13
 #define CONTEXT_CURSORTYPE    14
 #define CONTEXT_WINDOWTYPE    15
 #define CONTEXT_3DPARAMS      16
#define CONTEXT_PRINT          17
 #define CONTEXT_QUALITY       18
 #define CONTEXT_PORT          19
#define CONTEXT_HELP           20
 #define CONTEXT_CONTEXT       21
 #define CONTEXT_TOPIC         22

#define MAX_CONTEXT  CONTEXT_TOPIC /* число стpаниц в файле помощи as_help.hlp*
                                                                            /

void help (void);
/***********
* Describe :
* Return   : void
* Call     :
***********/

void contexthelp (void);
/***********
* Describe : контекстная помощь
* Params   : void использует пеpеменную help_context
* Return   : void
* Call     : message, next, previous
***********/

void next (void);
/***********
* Describe : увеличить help_context на 1
* Params   : void
* Return   : void
* Call     :
***********/

void previous (void);
/***********
* Describe : уменьшить help_context на 1
* Params   : void
* Return   : void
* Call     :
***********/

M_OPTION.H
===============================================================================

enum asWindType   { REC, BAR, PAR, HAN, HAM, BLC, FTP };

void options (void);
 void cursortype (void);
 void windowtype (void);
  void setrec (void);
  void setbar (void);
  void setpar (void);
  void sethan (void);
  void setham (void);
  void setblc (void);
 void set_gips (void);
 void set_approx (void);
 void set_graph (void);
 void set_port (void);
 void save_config (void);
 void view_config (void);

M_PRINT.H
===============================================================================

void print (void);
void quality (void);
void setmargin(void);
void setport (void);
void height (void);

M_QUIT.H
===============================================================================

void quit (void);
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/

void beforequit (void);
/***********
* Describe :
* Params   : void
* Return   : void
* Call     :
***********/

