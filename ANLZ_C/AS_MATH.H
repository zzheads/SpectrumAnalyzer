/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MATH
* Describe  : математические функции программы СпектроАнализатор
* File      : C:\ALEX\WORK\AS_MATH.H
* Last edit : 14.08.93 00.33.55
* Remarks   : разработка А.В.Папина и А.Э.Головнева
*****************************************************************************/

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