/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : FINDBAD     
* Describe  : �ਢ離� � ���⮯�������� BAD-�����஢ �� ��᪥
* File      : C:\ALEX\WORK\FINDBAD.C
* Last edit : 17.07.94 00.19.49
* Remarks   : �.��ࡠ���, "���� �� ����஢����", ���. 18-20.
*****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <io.h>
#include "as_main.h"
#include "as_def.h"

static int bad_secs (int *,int *);
static void comp (int *);

#define MAX_BADSECS   100

void comp (int *true)
{
 int BadSecs[MAX_BADSECS],BadBytes[MAX_BADSECS],bads=0;
 int n_BadSecs[MAX_BADSECS],n_BadBytes[MAX_BADSECS],n_bads=0,i;
 FILE *out;

 for (i=0;i<MAX_BADSECS;i++)
 {
  BadSecs [i] = 0;
  BadBytes[i] = 0;
  n_BadSecs [i] = 0;
  n_BadBytes[i] = 0;
 }
 *true = (int)0;
 n_bads = bad_secs (n_BadSecs,n_BadBytes);
 out = fopen (fullpath("as_info.nfo"),"rb");
 if (out==NULL)
  return;
 fread (&bads   ,sizeof(int),1          ,out);
 fread (BadSecs ,sizeof(int),MAX_BADSECS,out);
 fread (BadBytes,sizeof(int),MAX_BADSECS,out);
 fclose (out);
 if (bads!=n_bads)
  return;
 for (i=0;i<MAX_BADSECS;i++)
  if ((BadSecs[i]!=n_BadSecs[i])||(BadBytes[i]!=n_BadBytes[i]))
   return;
 *true = (int)1;
 return;
}

int bad_secs (int *BadSecs,int *BadBytes)
{
/* ����� ��� �⥭�� ᥪ�஢                                                */
 unsigned char buffer[512],buf[512];
 int tg,SecNum,ByteNum,i,j;

/* ����� ��ࢮ�� ᥪ�� FAT � �᫮ ᥪ�஢ � FAT                          */
 int i_Fat1Sec,i_FatSecs;
 int i_LoopOffs,i_Ind;
 int sw_fat; /* 䫠� ⨯� FAT                                                 */
 int cur_disk;

/* ��।������ ⥪�饣� ��᪠                                                */
 cur_disk=getdisk();

/* ��।������ ⨯� FAT                                                      */
 absread (cur_disk,1,0,buf);
 i_FatSecs = *((int *)(buf+0x16));
 if (i_FatSecs * 341 / 4096)
  sw_fat = 1; /* 16-��⮢�� FAT                                               */
 else
  sw_fat = 0; /* 12-��⮢�� FAT                                               */

/* ��।������ ����� ��砫쭮�� ᥪ�� FAT                                 */
 i_Fat1Sec = * ((int *)(buf+0x0e));

/* 横� ���᪠ BAD-������                                                  */
 tg=0;
 for (i=0;i<i_FatSecs;i++)
 {
 /* �⥭�� ��।���� ᥪ�� FAT                                            */
  absread (cur_disk,1,i_Fat1Sec+i,buffer);
  if (sw_fat==1)
   for (j=0;j<256;j++)
   {
    if ((buffer[j*2]==0xf7)&&(buffer[j*2+1]==0xff))
    {
     SecNum = i;      /* BAD-������ ������                                   */
     ByteNum = j*2;
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
   }
  else
  {
   i_LoopOffs = i % 3;  /* � 12-� ��⮢�� FAT ������� �������� �������� ��  */
                        /* �࠭��� ����� ᥪ�ࠬ�, ���⮬� �� �� ᥪ��    */
                        /* ��稭����� � ������ �������, ��। ��� �����      */
                        /* �ᯮ�������� 1 ��� 2 ���� �� �।��饩 ����     */
                        /* ������⮢                                          */
   for (j=0;j<170;j++)
   {
    i_Ind = j*3+i_LoopOffs;
    if ((buffer[i_Ind]==0xf7)&&((buffer[i_Ind+1]&0x0f)==0x0f))
    {
     SecNum = i;
     ByteNum = i_Ind; /* BAD-������ ������                                   */
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
    if (((buffer[i_Ind+1]&0xf0)==0x70)&&(buffer[i_Ind+2]==0xff))
    {
     SecNum = i;
     ByteNum = i_Ind; /* BAD-������ ������                                   */
     BadSecs[tg] = SecNum;
     BadBytes[tg] = ByteNum;
     tg++;
     continue;
    }
   }
  }
 }
 return (tg);
}

int test (void)
{
 int flag_true=0;

 comp (&flag_true);
 return (1);
}