/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MATH
* Describe  : ��⥬���᪨� �㭪樨 �ணࠬ�� �����஀��������
* File      : C:\ALEX\WORK\AS_MATH.H
* Last edit : 14.08.93 00.33.55
* Remarks   : ࠧࠡ�⪠ �.�.������ � �.�.���������
*****************************************************************************/

#define PI ((double) 3.141592653589793)

void get_window (float *d,int m);
/***********
* Describe : �㭪�� ����� ��ᮢ��� ����
* Params   : double *d - ���ᨢ ���祭�� ᨣ����/ᯥ���
*          : int m     -
*          : int fl    - ⨯ ��ᮢ��� ����
* Return   : void
* Call     :
***********/

void FFT (float *r,float *i,int n,int fl);
/***********
* Describe : �८�ࠧ������ ����
* Params   : double *r - ����⢨⥫쭠� �
*          : double *i - ������ ��� ᨣ����/ᯥ���
*          : int n     - �᫮ ����� � ᨣ����/ᯥ���
*          : int fl    - ��� �८�ࠧ������ 0-
* Return   : void
* Call     :
***********/

void amplitude (float *real,float *image,int num);
/***********
* Describe : ���᫥��� �������� ᨣ����/ᯥ���
* Params   : double *real  - ����⢨⥫쭠� �
*          : double *image - ������ ��� �㭪樨
*          : int num       - �᫮ �����
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
* Describe : �㭪�� ��᮪��� �஢�� ���᫥��� ᯥ��� �� ��. ॠ����樨
* Params   : float *msr - ����७�� ᨣ����
*          : int *num   - �᫮ ����७��
* Return   : void
* Call     : get_window,FFT,amplitude,drawprogress.
***********/

int doIntegrate (float *msr,asRecPtr r);
/***********
* Describe : ��⥣�஢���� ����⮢ �����
* Params   : float *msr
*          : asRecPtr r
* Return   : int
* Call     :
***********/

int doDiff (float *msr,asRecPtr r);
/***********
* Describe : ����७樨஢���� ����⮢ �����
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
* Describe : �뤥����� �� ᨣ���� ������
* Params   : float *r - ���祭�� �६����� ॠ����樨
*          : int beg  - ��砫� ����
*          : int end  - ����� ����
*          : int num  - �᫮ ����⮢
* Return   : void
* Call     : 
***********/

void exponent (float *r,int beg,int end,int num);
/***********
* Describe : �뤥����� �� ᨣ���� ������
* Params   : float *r - ���祭�� �६����� ॠ����樨
*          : int beg  - ��砫� ����
*          : int end  - ����� ����
*          : int num  - �᫮ ����⮢
* Return   : void
* Call     : 
***********/